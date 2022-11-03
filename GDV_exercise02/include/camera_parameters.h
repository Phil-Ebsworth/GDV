#ifndef CAMERA_PARAMETERS_H
#define CAMERA_PARAMETERS_H

#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>

#include "common.h"

#include <cmath>

/**
 * @brief The CameraFrame class takes a view direction and an up-vector
 * to create an orthonormal coordinate system.
 */
struct CameraFrame {
    const Vector3D dir, right, up; // dir = -z, right = x, up = y
    CameraFrame(const Vector3D& dir, const Vector3D& up)
        : dir{normalize(dir)}, right(normalize(cross(dir, up))), up{cross(this->right, this->dir)}
    {
    }
};

/**
 * @brief The CameraParameters struct contains all parameters necessary to configure a camera.
 */
struct CameraParameters {
    /// 3D position of the camera
    Point3D pos{0.0f, 0.5f, 4.0f};
    /// 3D position of the target to look at
    Point3D target{0.0f, 0.0f, 0.0f};
    /// up vector
    Vector3D up{0.0f, 1.0f, 0.0f};

    struct Perspective {
        /// field of view in degrees
        float fov{30.0f};
    } perspective;

    struct Orthographic {
        float left{-1.0f};
        float right{1.0f};
        float top{-1.0f};
        float bottom{1.0f};
    } orthographic;

    /// apsect ratio (width / height)
    float aspect{1.0f};

    bool isPerspective{true};

    /// orthonormal camera frame
    CameraFrame computeFrame() const { return {target - pos, up}; }
    /// half of the width and height of the view frustum at depth 1
    Point2D halfViewSpan() const
    {
        float w = std::tan(perspective.fov * degToRad * 0.5f);
        return {w * aspect, w};
    }
};

/// Some more controls for the GUI
class CameraControls final : public nanogui::Object {
public:
    CameraControls(nanogui::FormHelper& gui, nanogui::Vector2i pos)
    {
        controlWindow = gui.add_window(pos, "Camera");
        gui.add_group("Position");
        gui.add_variable("x", params.pos.x)->set_spinnable(true);
        gui.add_variable("y", params.pos.y)->set_spinnable(true);
        gui.add_variable("z", params.pos.z)->set_spinnable(true);
        gui.add_variable("rotate", rotateCamera)->set_tooltip("rotate camera on the zx-plane");
        // gui.add_button("reset rotation", [&]() -> void { cameraTime = 0.0f; });
        gui.add_group("Target");
        gui.add_variable("x", params.target.x)->set_spinnable(true);
        gui.add_variable("y", params.target.y)->set_spinnable(true);
        gui.add_variable("z", params.target.z)->set_spinnable(true);
        gui.add_group("Up");
        gui.add_variable("x", params.up.x)->set_spinnable(true);
        gui.add_variable("y", params.up.y)->set_spinnable(true);
        gui.add_variable("z", params.up.z)->set_spinnable(true);
        gui.add_group("Perspective");
        gui.add_variable("fov", params.perspective.fov)->set_spinnable(true);
        gui.add_group("Orthographic");
        gui.add_variable("left", params.orthographic.left)->set_spinnable(true);
        gui.add_variable("right", params.orthographic.right)->set_spinnable(true);
        gui.add_variable("top", params.orthographic.top)->set_spinnable(true);
        gui.add_variable("bottom", params.orthographic.bottom)->set_spinnable(true);
        gui.add_group("Options");
        gui.add_variable("auto aspect", autoAdjustFOV)
            ->set_tooltip("auto-adjust field of view / top/bottom for narrow aspect ratios");
        gui.add_variable("perspective", params.isPerspective)
            ->set_tooltip("use the perspective camera");
    }

    CameraParameters getCameraParameters(nanogui::Vector2i res) const
    {
        CameraParameters result{params};
        const float prev = lastTime;
        lastTime = static_cast<float>(glfwGetTime());
        if (rotateCamera)
            cameraTime += lastTime - prev;
        const float sin = std::sin(cameraTime);
        const float cos = std::cos(cameraTime);
        result.pos.x = sin * params.pos.z + cos * params.pos.x;
        result.pos.z = cos * params.pos.z - sin * params.pos.x;

        result.aspect = static_cast<float>(res.x()) / static_cast<float>(res.y());
        // scale fov when the aspect ratio is weird
        if (autoAdjustFOV) {
            result.perspective.fov /= std::max(0.2f, std::min(1.0f, result.aspect));

            result.orthographic.top /= result.aspect;
            result.orthographic.bottom /= result.aspect;
        }
        return result;
    }

private:
    CameraParameters params;
    mutable float cameraTime{0.0f};
    mutable float lastTime{0.0f};
    bool autoAdjustFOV{true};
    bool rotateCamera{false};

    nanogui::ref<nanogui::Window> controlWindow;
};

#endif // CAMERA_PARAMETERS_H
