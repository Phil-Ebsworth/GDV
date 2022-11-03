#ifndef NORMAL_INTEGRATOR_H
#define NORMAL_INTEGRATOR_H

#include "integrator.h"
#include "ray.h"
#include "scene.h"

class NormalIntegrator : public Integrator {
public:
    virtual ~NormalIntegrator() override = default;

    nanogui::Color L(const Scene& scene, const Ray& ray) const override;
};

#endif // !NORMAL_INTEGRATOR_H
