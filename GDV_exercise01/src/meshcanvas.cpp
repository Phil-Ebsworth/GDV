#include "meshcanvas.h"

#include <iostream>
#include <numbers>

MeshCanvas::MeshCanvas(Widget* parent) : Canvas{parent}
{
    static const std::string vertex_shader = R"(
#version 330

uniform mat4 mvp;
uniform mat4 model;

in vec3 position;
in vec3 normal;
out vec3 ws_pos;
out vec3 ws_normal;
flat out vec3 ws_normal_flat;

void main() {
    vec4 pos = mvp * vec4(position, 1.0);
    vec4 ws_pos_tmp = model * vec4(position, 1.0);
    ws_pos = ws_pos_tmp.xyz/ws_pos_tmp.w;
    gl_Position = pos;
    ws_normal = transpose(inverse(mat3(model))) * normal;
    ws_normal_flat = ws_normal;
}
)";
    static const std::string fragment_shader = R"(
#version 330

uniform vec4 base_color;
uniform vec3 camera_pos;
uniform bool shade_flat;
uniform bool shade_normal;

in vec3 ws_pos;
in vec3 ws_normal;
flat in vec3 ws_normal_flat;
out vec4 color;

void main() {
    vec3 cam_dir = normalize(camera_pos-ws_pos);
    vec3 normal = normalize(shade_flat ? ws_normal_flat : ws_normal);
    if (shade_normal)
        color = vec4(normal*0.5+vec3(0.5),1.0);
    else
        color = base_color*dot(cam_dir, normal);
}
)";
    m_shader = new Shader(render_pass(), "mesh_shader", vertex_shader, fragment_shader);
    m_shader->set_uniform("base_color", Color{0.8f, 0.8f, 0.8f, 1.0f});

    // coordinate axes
    m_coordShader = new Shader(render_pass(), "coord_shader", vertex_shader, fragment_shader);
    m_coordMesh.loadOBJ("../meshes/Axis.obj");
    m_coordShader->set_buffer("indices", VariableType::UInt32, {m_coordMesh.getFaces().size() * 3},
                         m_coordMesh.getFaces().data());
    m_coordShader->set_buffer("position", VariableType::Float32, {m_coordMesh.getVertices().size(), 3},
                         m_coordMesh.getVertices().data());
    m_coordShader->set_buffer("normal", VariableType::Float32, {m_coordMesh.getNormals().size(), 3},
                         m_coordMesh.getNormals().data());
}

void MeshCanvas::uploadMesh(const Mesh& mesh)
{
    m_shader->set_buffer("indices", VariableType::UInt32, {mesh.getFaces().size() * 3},
                         mesh.getFaces().data());
    m_shader->set_buffer("position", VariableType::Float32, {mesh.getVertices().size(), 3},
                         mesh.getVertices().data());
    m_shader->set_buffer("normal", VariableType::Float32, {mesh.getNormals().size(), 3},
                         mesh.getNormals().data());

    numTriangles = mesh.getFaces().size();
    aabb = mesh.getBounds();
    smoothGroups = mesh.getSmoothGroups();
}

void MeshCanvas::draw_contents()
{
    if (!numTriangles)
        return;

    if (rotate) {
        float prev = lastTime;
        lastTime = static_cast<float>(glfwGetTime());
        time += lastTime-prev;
    }

    Vector3f camera_pos{0, 0.5f, -4.0f};

    Matrix4f rotate = Matrix4f::rotate({0.0f, 1.0f, 0.0f}, time);

    const float scale = (auto_center) ? 2.0f / aabb.extents().maxComponent()
                                      : 1.0f / max(abs(aabb.min), abs(aabb.max)).maxComponent();
    const Vector3f translation = -(aabb.min + aabb.extents() * 0.5f);

    Matrix4f model = rotate;

    if (auto_scale)
        model = model * Matrix4f::scale(Vector3f{scale});
    if (auto_center)
        model = model * Matrix4f::translate(translation);

    Matrix4f view = Matrix4f::look_at(camera_pos, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});

    const float width = static_cast<float>(m_size.x());
    const float height = static_cast<float>(m_size.y());
    const float aspect = width / height;

    const float fov = 30.0f * std::numbers::pi_v<float> / 180.0f
                    / std::max(0.2f, std::min(1.0f, aspect));

    Matrix4f proj = Matrix4f::perspective(fov, 0.1f, 20.f, aspect);

    Matrix4f mvp = proj * view * model;

    if (show_axes) {
        m_coordShader->set_uniform("mvp", mvp);
        m_coordShader->set_uniform("model", rotate);
        m_coordShader->set_uniform("camera_pos", camera_pos);

        m_coordShader->set_uniform("shade_flat", true);
        m_coordShader->set_uniform("shade_normal", shadeNormal);

        m_coordShader->set_uniform("base_color", Color{1.0f}-background_color());

        m_coordShader->begin();
        m_coordShader->draw_array(Shader::PrimitiveType::Triangle, 0, m_coordMesh.getFaces().size()*3, true);
        m_coordShader->end();
    }

    m_shader->set_uniform("mvp", mvp);
    m_shader->set_uniform("model", model);
    m_shader->set_uniform("camera_pos", camera_pos);

    m_shader->set_uniform("shade_flat", true);
    m_shader->set_uniform("shade_normal", shadeNormal);
    glProvokingVertex(GL_FIRST_VERTEX_CONVENTION);

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    m_shader->begin();

    // draw flat parts
    {
        size_t pos = 0;
        for (auto [start, end] : smoothGroups) {
            if (start > pos)
                m_shader->draw_array(Shader::PrimitiveType::Triangle, pos*3, (start-pos)*3, true);
            pos = end;
        }
        m_shader->draw_array(Shader::PrimitiveType::Triangle, pos*3, (numTriangles-pos)*3, true);
    }
    // draw smooth parts
    if (smoothGroups.size()) {
        m_shader->end();
        m_shader->set_uniform("shade_flat", false);
        m_shader->begin();
        for (auto [start, end] : smoothGroups)
            m_shader->draw_array(Shader::PrimitiveType::Triangle, start*3, (end-start)*3, true);
    }

    m_shader->end();

    if (wireframe)
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}
