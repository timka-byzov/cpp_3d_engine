#include <intersection.h>
#include <ray.h>
#include <scene.h>
#include <vector.h>
#include <object.h>
#include <geometry.h>
#include <optional>
#include <tuple>
#include <cmath>
#include <material.h>
#include <image.h>
#include <options/camera_options.h>
#include <options/render_options.h>

#define UNUSED(x) (void)(x)
constexpr double kEps = 1e-6;

Vector GetNormal(const Intersection& intersection, const Object& object) {
    if (!object.NormalExists()) {
        return intersection.GetNormal();
    } else {
        Vector normal = {0, 0, 0};
        Vector barycentric = GetBarycentricCoords(object.polygon, intersection.GetPosition());
        for (int i = 0; i != 3; ++i) {
            normal = normal + barycentric[i] * (*object.GetNormal(i));
        }
        return normal;
    }
}

Vector GetNormal(const Intersection& intersection, const SphereObject& object) {
    UNUSED(object);
    return intersection.GetNormal();
}

std::optional<std::tuple<Intersection, const Material*, Vector>> Intersect(const Ray& ray,
                                                                           const Scene& scene) {
    // find closest intersection

    std::optional<Intersection> closest_intersection = std::nullopt;
    const Material* material;
    Vector normal;

    for (const auto& obj : scene.GetObjects()) {
        auto intersection = GetIntersection(ray, obj.polygon);
        if (intersection && (!closest_intersection || intersection < closest_intersection)) {
            closest_intersection = intersection;
            material = obj.material;
            normal = GetNormal(intersection.value(), obj);
        }
    }

    for (const auto& obj : scene.GetSphereObjects()) {
        auto intersection = GetIntersection(ray, obj.sphere);
        if (intersection && (!closest_intersection || intersection < closest_intersection)) {
            closest_intersection = intersection;
            material = obj.material;
            normal = GetNormal(intersection.value(), obj);
        }
    }

    if (closest_intersection) {
        return std::make_tuple(closest_intersection.value(), material, normal.Normalized());
    } else {
        return std::nullopt;
    }
}

bool IsShadowed(const Ray& ray, const Scene& scene, double len) {
    auto intersection_info = Intersect(ray, scene);
    if (intersection_info) {
        const auto& [intersection, _, __] = intersection_info.value();
        return Length(intersection.GetPosition() - ray.GetOrigin()) < len + kEps;
    } else {
        return false;
    }
}

Vector CalculatePointLight(std::tuple<Intersection, const Material*, Vector> intersection_info,
                           const Scene& scene, const Ray& ray) {

    const auto& [intersection, material, norm] = intersection_info;

    Vector sum_light;
    for (auto& light : scene.GetLights()) {
        Vector pos = intersection.GetPosition() + kEps * norm;

        Vector light_dir = (light.position - pos).Normalized();

        if (IsShadowed(Ray{pos, light_dir}, scene, Length(light.position - pos))) {
            continue;
        }

        double cos = std::max(0., DotProduct(norm, light_dir));
        sum_light += cos * light.intensity * material->diffuse_color;

        Vector reflected_ray = Reflect(light_dir, norm);
        sum_light += pow(std::max(0., DotProduct(reflected_ray, ray.GetDirection())),
                         material->specular_exponent) *
                     light.intensity * material->specular_color;
    }

    return sum_light;
}

const Vector CalculateRay(const Ray& ray, const Scene& scene, const RenderOptions& render_options,
                          int depth = 0, bool inside = false) {

    if (depth == render_options.depth) {
        return {0, 0, 0};
    }

    auto intersection_info = Intersect(ray, scene);
    if (!intersection_info) {
        if (render_options.mode == RenderMode::kNormal) {
            return Vector{0, 0, 0};
        }
        if (render_options.mode == RenderMode::kDepth) {
            return Vector{-1, -1, -1};
        }
        return Vector{0, 0, 0};
    } else {

        const auto& [intersection, material, norm] = intersection_info.value();

        if (render_options.mode == RenderMode::kNormal) {
            return Vector{(norm[0] / 2 + 0.5), (norm[1] / 2 + 0.5), (norm[2] / 2 + 0.5)};
        }

        if (render_options.mode == RenderMode::kDepth) {
            return Vector{intersection.GetDistance(), intersection.GetDistance(),
                          intersection.GetDistance()};
        }

        Vector reflection;
        reflection = CalculateRay(
            Ray{intersection.GetPosition() + kEps * norm, Reflect(ray.GetDirection(), norm)}, scene,
            render_options, depth + 1, inside);

        Vector light = CalculatePointLight(intersection_info.value(), scene, ray);

        Vector refraction;

        if (material->albedo[2] > 0 and depth < render_options.depth) {
            double r = material->refraction_index;

            if (!inside) {
                r = 1 / r;
            }

            auto refract_dir = Refract(ray.GetDirection(), norm, r);
            if (refract_dir) {
                auto refracted_ray =
                    Ray{intersection.GetPosition() - kEps * norm, refract_dir.value()};

                auto alb = material->albedo[2];
                if (inside) {
                    alb = 1;
                }
                refraction =
                    alb * CalculateRay(refracted_ray, scene, render_options, depth + 1, !inside);
            }
        }

        return material->ambient_color + material->intensity + material->albedo[0] * light +
               material->albedo[1] * reflection + refraction;
    }
}
