#ifndef MESHCANVAS_H
#define MESHCANVAS_H

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

#include "mesh.h"

using namespace nanogui;

/// A class to display a 3D mesh
class MeshCanvas final : public Canvas {
public:
    MeshCanvas(Widget* parent);

    void uploadMesh(const Mesh& mesh);

    virtual void draw_contents() override;

    void set_foreground_color(const Color& fg_color) { if (m_shader) m_shader->set_uniform("base_color", fg_color); }

    void set_wireframe(bool wireframe)
    {
        this->wireframe = wireframe;
        if (wireframe)
            render_pass()->set_cull_mode(RenderPass::CullMode::Disabled);
        else
            render_pass()->set_cull_mode(RenderPass::CullMode::Back);
    }

    void set_shade_normal(bool shade_normal)
    {
        this->shadeNormal = shade_normal;
    }

    void set_rotate(bool rotate)
    {
        this->rotate = rotate;
        if (rotate)
            lastTime = static_cast<float>(glfwGetTime());
    }
    void set_auto_scale(bool auto_scale) { this->auto_scale = auto_scale; }
    void set_auto_center(bool auto_center) { this->auto_center = auto_center; }
    void set_show_axes(bool show_coords) { this->show_axes = show_coords; }

private:
    bool wireframe{false};
    bool shadeNormal{false};
    bool rotate{true};
    bool auto_scale{true};
    bool auto_center{true};
    bool show_axes{true};
    std::vector<std::pair<size_t, size_t>> smoothGroups;
    ref<Shader> m_shader;
    ref<Shader> m_coordShader;
    Mesh m_coordMesh;
    size_t numTriangles{0};
    AABB aabb{};
    float time{0.0f};
    float lastTime{0.0f};
};

/// Some controls for the GUI
class MeshCanvasControls final : public Object {
public:
    MeshCanvasControls(FormHelper& gui, Vector2i pos) {
        controlWindow = gui.add_window(pos, "Display options");
        gui.add_variable<Color>(
            "Foreground",
            [&](const Color& c) -> void {
                fg_color = c;
                for (auto& canvas : canvasObjects)
                    canvas->set_foreground_color(c);
            },
            [&]() -> Color { return fg_color; });
        gui.add_variable<Color>(
            "Background",
            [&](const Color& c) -> void {
                bg_color = c;
                for (auto& canvas : canvasObjects)
                    canvas->set_background_color(c);
            },
            [&]() -> Color { return bg_color; });
        gui.add_variable<bool>(
            "Wireframe",
            [&](const bool& b) -> void {
                wireframe = b;
                for (auto& canvas : canvasObjects)
                    canvas->set_wireframe(b);
            },
            [&]() -> bool { return wireframe; });
        gui.add_variable<bool>(
                    "Normals",
                    [&](const bool& b) -> void {
            normals = b;
            for (auto& canvas : canvasObjects)
                canvas->set_shade_normal(b);
        },
        [&]() -> bool { return normals; });
        gui.add_variable<bool>(
                    "Rotate",
                    [&](const bool& b) -> void {
            rotate = b;
            for (auto& canvas : canvasObjects)
                canvas->set_rotate(b);
        },
        [&]() -> bool { return rotate; });
        gui.add_variable<bool>(
                    "Auto Scale",
                    [&](const bool& b) -> void {
            auto_scale = b;
            for (auto& canvas : canvasObjects)
                canvas->set_auto_scale(b);
        },
        [&]() -> bool { return auto_scale; });
        gui.add_variable<bool>(
                    "Auto Center",
                    [&](const bool& b) -> void {
            auto_center = b;
            for (auto& canvas : canvasObjects)
                canvas->set_auto_center(b);
        },
        [&]() -> bool { return auto_center; });
        gui.add_variable<bool>(
                    "Show Axes",
                    [&](const bool& b) -> void {
            show_axes = b;
            for (auto& canvas : canvasObjects)
                canvas->set_show_axes(b);
        },
        [&]() -> bool { return show_axes; });
    }

    void addMeshCanvas(ref<MeshCanvas>& canvas) {
        canvasObjects.push_back(canvas);
        canvas->set_foreground_color(fg_color);
        canvas->set_background_color(bg_color);
        canvas->set_wireframe(wireframe);
        canvas->set_shade_normal(normals);
        canvas->set_rotate(rotate);
        canvas->set_auto_center(auto_center);
        canvas->set_auto_scale(auto_scale);
    }

private:
    ref<Window> controlWindow;
    std::vector<ref<MeshCanvas>> canvasObjects;

    // GUI parameters
    Color fg_color{165 / 255.0f, 30 / 255.0f, 55 / 255.0f, 1.f};
    Color bg_color{180 / 255.0f, 160 / 255.0f, 105 / 255.0f, 1.f};
    bool wireframe{false};
    bool normals{false};
    bool rotate{true};
    bool auto_scale{false};
    bool auto_center{false};
    bool show_axes{true};
};

#endif // MESHCANVAS_H
