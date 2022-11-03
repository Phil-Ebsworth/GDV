#ifndef EXERCISE02_H
#define EXERCISE02_H

#include <nanogui/nanogui.h>

#include "aabb.h"
#include "common.h"
#include "intersection.h"
#include "ray.h"
#include "raytracer_view.h"
#include "triangle.h"

class Exercise02 {
public:
    [[nodiscard]] static bool intersect(const AABB& aabb, const IntersectionRay& ray);
    [[nodiscard]] static Intersection intersect(const Triangle& triangle,
                                                const IntersectionRay& ray);

    [[nodiscard]] static Ray orthographic(const Point2D& pixel, const Point3D& origin,
                                          const CameraFrame& cameraFrame,
                                          const CameraParameters::Orthographic& orthographic);
};

class Exercise02Controls final : public nanogui::Object {
public:
    Exercise02Controls(nanogui::FormHelper& gui, nanogui::Vector2i pos,
                       const nanogui::ref<RayTracerView>& rayTracerView)
        : rayTracerView{rayTracerView}
    {
        controlWindow = gui.add_window(pos, "Exercise 2");
        gui.add_group("Ray Tracer");
        gui.add_button("(Re-)render", [&]() -> void { this->rayTracerView->restart(); });
        gui.add_button("Reset View", [&]() -> void { this->rayTracerView->reset(); });
    }

private:
    nanogui::ref<nanogui::Window> controlWindow;
    nanogui::ref<RayTracerView> rayTracerView;
};

#endif // EXERCISE02_H
