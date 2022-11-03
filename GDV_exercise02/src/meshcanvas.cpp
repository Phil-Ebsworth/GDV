#include "meshcanvas.h"

#include <iostream>

using namespace nanogui;

MeshCanvas::MeshCanvas(Widget* parent) : Canvas{parent}
{
    static const std::string vertex_shader = R"(
#version 330

uniform mat4 mvp;
uniform mat4 mv;

in vec3 position;
in vec3 normal;
out vec3 ws_normal;
flat out vec3 ws_normal_flat;

void main() {
    gl_Position = mvp * vec4(position, 1.0);
    ws_normal = transpose(inverse(mat3(mv)))*normal;
    ws_normal_flat = ws_normal;
}
)";
    static const std::string fragment_shader = R"(
#version 330

uniform vec4 base_color;
uniform bool shade_flat;
uniform bool shade_normal;

in vec3 ws_normal;
flat in vec3 ws_normal_flat;
out vec4 color;

void main() {
    vec3 normal = normalize(shade_flat ? ws_normal_flat : ws_normal);
    if (shade_normal)
        color = vec4(normal*0.5+vec3(0.5), 1.0);
    else
        color = base_color*normal.z;
}
)";
    shader = new Shader(render_pass(), "mesh_shader", vertex_shader, fragment_shader);
    shader->set_uniform("base_color", Color{0.8f, 0.8f, 0.8f, 1.0f});

    // coordinate axes
    axesShader = new Shader(render_pass(), "coord_shader", vertex_shader, fragment_shader);
    m_coordMesh.loadOBJ("../meshes/Axis.obj");
    axesShader->set_buffer("indices", VariableType::UInt32, {m_coordMesh.getFaces().size() * 3},
                           m_coordMesh.getFaces().data());
    axesShader->set_buffer("position", VariableType::Float32, {m_coordMesh.getVertices().size(), 3},
                           m_coordMesh.getVertices().data());
    axesShader->set_buffer("normal", VariableType::Float32, {m_coordMesh.getNormals().size(), 3},
                           m_coordMesh.getNormals().data());
}

void MeshCanvas::uploadMesh(const Mesh& mesh)
{
    shader->set_buffer("indices", VariableType::UInt32, {mesh.getFaces().size() * 3},
                       mesh.getFaces().data());
    shader->set_buffer("position", VariableType::Float32, {mesh.getVertices().size(), 3},
                       mesh.getVertices().data());
    shader->set_buffer("normal", VariableType::Float32, {mesh.getNormals().size(), 3},
                       mesh.getNormals().data());

    numTriangles = mesh.getFaces().size();
    aabb = mesh.getBounds();
    smoothGroups = mesh.getSmoothGroups();
}

void MeshCanvas::draw_contents()
{
    if (!numTriangles)
        return;

    const MeshDisplayParameters& displayParams =
        meshDisplayControls ? meshDisplayControls->getDisplayParameters() : MeshDisplayParameters{};

    const float prev = lastTime;
    lastTime = static_cast<float>(glfwGetTime());
    if (displayParams.rotateMesh)
        meshTime += lastTime - prev;

    Matrix4f model = Matrix4f::rotate({0.0f, 1.0f, 0.0f}, meshTime);

    const CameraParameters& cameraParams =
        cameraControls ? cameraControls->getCameraParameters(m_size) : CameraParameters{};
    const CameraFrame cameraFrame = cameraParams.computeFrame();

    Matrix4f view;
    view.m[0] = {cameraFrame.right.x, cameraFrame.up.x, -cameraFrame.dir.x, 0.0f};
    view.m[1] = {cameraFrame.right.y, cameraFrame.up.y, -cameraFrame.dir.y, 0.0f};
    view.m[2] = {cameraFrame.right.z, cameraFrame.up.z, -cameraFrame.dir.z, 0.0f};
    view.m[3] = {-dot(cameraParams.pos, cameraFrame.right), -dot(cameraParams.pos, cameraFrame.up),
                 dot(cameraParams.pos, cameraFrame.dir), 1.0f};

    if (displayParams.autoScale) {
        const float scale = (displayParams.autoCenter)
                              ? 2.0f / aabb.extents().maxComponent()
                              : 1.0f / max(abs(aabb.min), abs(aabb.max)).maxComponent();
        view = view * Matrix4f::scale(Vector3f{scale});
    }
    if (displayParams.autoCenter)
        view = view * Matrix4f::translate(-aabb.center());

    Matrix4f proj;
    if (cameraParams.isPerspective)
        proj = Matrix4f::perspective(cameraParams.perspective.fov * degToRad, 0.1f, 20.f,
                                     cameraParams.aspect);
    else
        proj = Matrix4f::ortho(cameraParams.orthographic.left, cameraParams.orthographic.right,
                               cameraParams.orthographic.top, cameraParams.orthographic.bottom,
                               0.1f, 20.f);

    const Matrix4f mvp = proj * view * model;

    set_background_color(displayParams.bgColor);

    if (displayParams.showAxes) {
        axesShader->set_uniform("mvp", proj * view);
        axesShader->set_uniform("mv", view);

        axesShader->set_uniform("shade_flat", true);
        axesShader->set_uniform("shade_normal", displayParams.normals);
        axesShader->set_uniform("base_color", Color{1.0f} - background_color());

        axesShader->begin();
        axesShader->draw_array(Shader::PrimitiveType::Triangle, 0,
                               m_coordMesh.getFaces().size() * 3, true);
        axesShader->end();
    }

    shader->set_uniform("mvp", mvp);
    shader->set_uniform("mv", model);

    shader->set_uniform("base_color", displayParams.fgColor);
    shader->set_uniform("shade_flat", true);
    shader->set_uniform("shade_normal", displayParams.normals);
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    if (displayParams.wireframe) {
        render_pass()->set_cull_mode(nanogui::RenderPass::CullMode::Disabled);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else
        render_pass()->set_cull_mode(nanogui::RenderPass::CullMode::Back);

    shader->begin();

    // draw flat parts
    {
        size_t pos = 0;
        for (auto [start, end] : smoothGroups) {
            if (start > pos)
                shader->draw_array(Shader::PrimitiveType::Triangle, pos * 3, (start - pos) * 3,
                                   true);
            pos = end;
        }
        shader->draw_array(Shader::PrimitiveType::Triangle, pos * 3, (numTriangles - pos) * 3,
                           true);
    }
    // draw smooth parts
    if (smoothGroups.size()) {
        shader->end();
        shader->set_uniform("shade_flat", false);
        shader->begin();
        for (auto [start, end] : smoothGroups)
            shader->draw_array(Shader::PrimitiveType::Triangle, start * 3, (end - start) * 3, true);
    }

    shader->end();

    if (displayParams.wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
