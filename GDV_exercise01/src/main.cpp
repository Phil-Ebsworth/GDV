/*
    src/example2.cpp -- C++ version of an example application that shows
    how to use the form helper class. For a Python implementation, see
    '../python/example2.py'.

    NanoGUI was developed by Wenzel Jakob <wenzel.jakob@epfl.ch>.
    The widget drawing code is based on the NanoVG demo application
    by Mikko Mononen.

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#include <fstream>
#include <iostream>
#include <nanogui/nanogui.h>
#include <nanogui/opengl.h>
#include <nanogui/texture.h>
#include <sstream>
#include <string>
#include <memory>
#include <vector>

#include "mesh.h"
#include "meshcanvas.h"

#include "exercise01.h"

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
        m_rightCanvas = new MeshCanvas(this);

        mesh.loadOBJ("../meshes/gdv.obj");
        //mesh.loadOBJ("../meshes/bunny.obj");
        m_leftCanvas->uploadMesh(mesh);

        {
            FormHelper gui{this};
            m_display_controls = new MeshCanvasControls{gui, {10, 10}};
            m_display_controls->addMeshCanvas(m_leftCanvas);
            m_display_controls->addMeshCanvas(m_rightCanvas);
            m_exercise_controls = new Exercise01Controls{gui, {400, 10}, mesh, m_rightCanvas};
        }

        resize_event(framebuffer_size());
        perform_layout();
        set_visible(true);
    }

    bool resize_event(const Vector2i& size) override
    {
        const Vector2i canvasSize = {(size.x()-1)/2, size.y()};

        m_rightCanvas->set_position({0, 0});
        m_leftCanvas->set_size(canvasSize);
        m_rightCanvas->set_position(size-canvasSize);
        m_rightCanvas->set_size(canvasSize);

        Screen::resize_event(size);
        return true;
    }

private:
    ref<MeshCanvas> m_leftCanvas;
    ref<MeshCanvas> m_rightCanvas;
    ref<MeshCanvasControls> m_display_controls;
    ref<Exercise01Controls> m_exercise_controls;

    Mesh mesh;
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
