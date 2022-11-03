#ifndef MESHCANVAS_H
#define MESHCANVAS_H

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

#include <cmath>

#include "camera.h"
#include "mesh.h"

struct MeshDisplayParameters {
    nanogui::Color fgColor{165 / 255.0f, 30 / 255.0f, 55 / 255.0f, 1.f};
    nanogui::Color bgColor{180 / 255.0f, 160 / 255.0f, 105 / 255.0f, 1.f};
    bool wireframe{false};
    bool normals{true};
    bool rotateMesh{false};
    bool autoScale{false};
    bool autoCenter{false};
    bool showAxes{false};
};

class MeshDisplayControls;
class CameraControls;

/// A class to display a 3D mesh
class MeshCanvas final : public nanogui::Canvas {
public:
    MeshCanvas(nanogui::Widget* parent);

    void uploadMesh(const Mesh& mesh);

    virtual void draw_contents() override;

    void setCanvasControls(const nanogui::ref<MeshDisplayControls> controls)
    {
        meshDisplayControls = controls;
    }
    void setCameraControls(const nanogui::ref<CameraControls> controls)
    {
        cameraControls = controls;
    }

private:
    std::vector<std::pair<size_t, size_t>> smoothGroups;
    nanogui::ref<nanogui::Shader> shader;
    nanogui::ref<nanogui::Shader> axesShader;
    Mesh m_coordMesh;
    size_t numTriangles{0};
    AABB aabb{};
    mutable float meshTime{0.0f};
    mutable float lastTime{0.0f};

    nanogui::ref<MeshDisplayControls> meshDisplayControls;
    nanogui::ref<CameraControls> cameraControls;
};

/// Some controls for the GUI
class MeshDisplayControls final : public nanogui::Object {
public:
    MeshDisplayControls(nanogui::FormHelper& gui, nanogui::Vector2i pos)
    {
        using nanogui::Color;

        controlWindow = gui.add_window(pos, "Display options");
        gui.add_variable<Color>("Foreground", params.fgColor);
        gui.add_variable<Color>("Background", params.bgColor);
        gui.add_variable<bool>("Wireframe", params.wireframe);
        gui.add_variable<bool>("Normals", params.normals);
        gui.add_variable<bool>("Rotate Mesh", params.rotateMesh);
        gui.add_variable<bool>("Auto Scale", params.autoScale);
        gui.add_variable<bool>("Auto Center", params.autoCenter);
        gui.add_variable<bool>("Show Axes", params.showAxes);
    }

    const MeshDisplayParameters& getDisplayParameters() const { return params; }

private:
    MeshDisplayParameters params;
    nanogui::ref<nanogui::Window> controlWindow;
};

#endif // MESHCANVAS_H
