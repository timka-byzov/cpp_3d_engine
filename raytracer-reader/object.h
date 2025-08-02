#pragma once

#include <triangle.h>
#include <material.h>
#include <sphere.h>
#include <vector.h>
#include <vector>
#include <optional>

struct Object {

    Object(Triangle& polygon, Material* material, std::vector<std::optional<Vector>>& normals)
        : polygon(polygon), material(material) {

        for (size_t i = 0; i < 3; ++i) {
            normals_[i] = normals[i];
        }
    }

    Triangle polygon;
    Material* material;

    const Vector* GetNormal(size_t index) const {
        return reinterpret_cast<const Vector*>(&normals_[index]);
        // TODO calculate normal if not present
    }

    bool NormalExists() const {
        return normals_[0].has_value() && normals_[1].has_value() && normals_[2].has_value();
    }

private:
    std::optional<Vector> normals_[3];
};

struct SphereObject {
    const Material* material = nullptr;
    Sphere sphere;
};
