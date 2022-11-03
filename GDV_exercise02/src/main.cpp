/*
    Framework for GDV exercises, based on NanoGUI.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <fstream>
#include <iostream>
#include <memory>
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <nanogui/texture.h>
#include <sstream>
#include <string>
#include <vector>

#include "camera.h"
#include "mesh.h"
#include "meshcanvas.h"
#include "nanogui/vector.h"
#include "raytracer_view.h"

#include "exercise02.h"

using namespace nanogui;

class GDVApplication final : public Screen {
public:
    GDVApplication(const Vector2i& size)
        : Screen{size,
                 "GDV 2022/23",
                 /* resizable */ true,
                 /* fullscreen */ false,
                 /* depth_buffer */ true,
                 /* stencil_buffer */ true,
                 /* float_buffer */ false,
                 /* gl_major */ 4,
                 /* gl_minor */ 1}
    {
        inc_ref();

        m_leftCanvas = new MeshCanvas(this);
        // m_rightCanvas = new MeshCanvas(this);
        m_rightCanvas = new RayTracerView(this);

        Mesh mesh;
        mesh.loadOBJ("../meshes/gdv.obj");
        // mesh.loadOBJ("../meshes/bunny.obj");
        m_leftCanvas->uploadMesh(mesh);

        {
            FormHelper gui{this};
            m_display_controls = new MeshDisplayControls{gui, {10, 10}};
            m_leftCanvas->setCanvasControls(m_display_controls);
            m_camera_controls = new CameraControls{gui, {400, 10}};
            m_leftCanvas->setCameraControls(m_camera_controls);
            m_rightCanvas->setCameraControls(m_camera_controls);
            m_exercise_controls = new Exercise02Controls{gui, {700, 10}, m_rightCanvas};
        }

        // set up Raytracer
        std::unique_ptr<Scene> scene = std::make_unique<Scene>();
        scene->addMesh(std::move(mesh));
        m_rightCanvas->setScene(std::move(scene));

        resize_event(framebuffer_size());
        perform_layout();
        set_visible(true);
    }

    bool resize_event(const Vector2i& size) override
    {
        const Vector2i canvasSize = {(size.x() - 1) / 2, size.y()};

        m_rightCanvas->set_position({0, 0});
        m_leftCanvas->set_size(canvasSize);
        m_rightCanvas->set_position(size - canvasSize);
        m_rightCanvas->set_size(canvasSize);

        Screen::resize_event(size);
        return true;
    }

private:
    ref<MeshCanvas> m_leftCanvas;
    ref<RayTracerView> m_rightCanvas;
    ref<MeshDisplayControls> m_display_controls;
    ref<CameraControls> m_camera_controls;
    ref<Exercise02Controls> m_exercise_controls;
};

int main(int /* argc */, char** /* argv */)
{
    try {
        init();

        /* scoped variables */ {
            ref<GDVApplication> app = new GDVApplication(Vector2i(1024, 768));
            app->dec_ref();
            app->draw_all();
            app->set_visible(true);
            mainloop(1 / 60.f * 1000);
        }

        shutdown();
    }
    catch (const std::exception& e) {
        std::string error_msg = std::string("Caught a fatal error: ") + std::string(e.what());
#if defined(_WIN32)
        MessageBoxA(nullptr, error_msg.c_str(), NULL, MB_ICONERROR | MB_OK);
#else
        std::cerr << error_msg << std::endl;
#endif
        return -1;
    }
    catch (...) {
        std::cerr << "Caught an unknown error!" << std::endl;
    }

    return 0;
}
