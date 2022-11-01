#ifndef EXERCISE01_H
#define EXERCISE01_H

#include <nanogui/nanogui.h>

#include "mesh.h"
#include "meshcanvas.h"

#if __cpp_lib_math_constants >= 201907L
#include <numbers>
#endif

class Exercise01
{
public:
    [[nodiscard]] static Vertex scale(Vertex v, float sx, float sy, float sz);

    [[nodiscard]] static Vertex translate(Vertex v, float tx, float ty, float tz);

    [[nodiscard]] static Vertex rotateX(Vertex v, float angle);
    [[nodiscard]] static Vertex rotateY(Vertex v, float angle);
    [[nodiscard]] static Vertex rotateZ(Vertex v, float angle);
};

class Exercise01Controls final : public nanogui::Object
{
public:
    Exercise01Controls(nanogui::FormHelper& gui, nanogui::Vector2i pos,
                       const Mesh& mesh, nanogui::ref<MeshCanvas>& canvas)
        : mesh{mesh}, transformedMesh{mesh}, canvas{canvas}
    {
        controlWindow = gui.add_window({450, 10}, "Exercise 1");
        gui.add_group("Scaling");
        gui.add_variable("x", sx);
        gui.add_variable("y", sy);
        gui.add_variable("z", sz);
        gui.add_button("scale", [&]() -> void {scaleMesh();});
        gui.add_group("Translation");
        gui.add_variable("x", tx);
        gui.add_variable("y", ty);
        gui.add_variable("z", tz);
        gui.add_button("translate", [&]() -> void {translateMesh();});
        gui.add_group("Rotation");
        gui.add_variable("angle", angle);
        gui.add_button("rotate x", [&]() -> void {rotateMeshX();});
        gui.add_button("rotate y", [&]() -> void {rotateMeshY();});
        gui.add_button("rotate z", [&]() -> void {rotateMeshZ();});
        gui.add_group("");
        gui.add_button("reset", [&]() -> void {resetMesh();});

        canvas->uploadMesh(transformedMesh);
    }

    void scaleMesh() {
        for (auto& vertex : transformedMesh.getVertices())
            vertex = Exercise01::scale(vertex, sx, sy, sz);
        for (auto& normal : transformedMesh.getNormals())
            normal = Exercise01::scale(normal, 1.0f/sx, 1.0f/sy, 1.0f/sz);
        transformedMesh.updateBounds();
        canvas->uploadMesh(transformedMesh);
    }

    void translateMesh() {
        for (auto& vertex : transformedMesh.getVertices())
            vertex = Exercise01::translate(vertex, tx, ty, tz);
        transformedMesh.updateBounds();
        canvas->uploadMesh(transformedMesh);
    }

    void rotateMeshX() {
        for (auto& vertex : transformedMesh.getVertices())
            vertex = Exercise01::rotateX(vertex, angle*degToRad);
        for (auto& normal : transformedMesh.getNormals())
            normal = Exercise01::rotateX(normal, angle*degToRad);
        transformedMesh.updateBounds();
        canvas->uploadMesh(transformedMesh);
    }

    void rotateMeshY() {
        for (auto& vertex : transformedMesh.getVertices())
            vertex = Exercise01::rotateY(vertex, angle*degToRad);
        for (auto& normal : transformedMesh.getNormals())
            normal = Exercise01::rotateY(normal, angle*degToRad);
        transformedMesh.updateBounds();
        canvas->uploadMesh(transformedMesh);
    }

    void rotateMeshZ() {
        for (auto& vertex : transformedMesh.getVertices())
            vertex = Exercise01::rotateZ(vertex, angle*degToRad);
        for (auto& normal : transformedMesh.getNormals())
            normal = Exercise01::rotateZ(normal, angle*degToRad);
        transformedMesh.updateBounds();
        canvas->uploadMesh(transformedMesh);
    }

    void resetMesh() {
        transformedMesh = mesh;
        canvas->uploadMesh(transformedMesh);
    }

private:
    const Mesh& mesh;
    Mesh transformedMesh;
    nanogui::ref<nanogui::Window> controlWindow;
    nanogui::ref<MeshCanvas> canvas;

    // GUI parameters
    float sx{1.0f}, sy{1.0f}, sz{1.0f};
    float tx{0.0f}, ty{0.0f}, tz{0.0f};
    float angle{0.0f};

#if __cpp_lib_math_constants >= 201907L
    static constexpr float degToRad = std::numbers::pi_v<float>/180.0f;
#else
    static constexpr float degToRad = 3.14159265359f/180.0f;
#endif
};

#endif // EXERCISE01_H
