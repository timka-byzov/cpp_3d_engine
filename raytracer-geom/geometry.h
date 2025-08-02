#pragma once

#include <vector.h>
#include <sphere.h>
#include <intersection.h>
#include <triangle.h>
#include <ray.h>

#include <optional>

std::optional<Intersection> GetIntersection(const Ray& ray, const Sphere& sphere) {
    auto b = 2 * DotProduct(ray.GetDirection(), ray.GetOrigin() - sphere.GetCenter());
    auto vl = ray.GetOrigin() - sphere.GetCenter();

    auto squared_l = vl[0] * vl[0] + vl[1] * vl[1] + vl[2] * vl[2];

    auto c = squared_l - sphere.GetRadius() * sphere.GetRadius();
    auto d = b * b - 4 * c;

    if (d >= 0) {
        auto sqrt_d = std::sqrt(d);
        auto t1 = (-b + sqrt_d) / 2;
        auto t2 = (-b - sqrt_d) / 2;

        if (t1 < 0 && t2 < 0) {
            return std::nullopt;
        }

        auto t = t1;

        if (t1 > 0 && t2 > 0) {
            t = std::min(t1, t2);
        }

        if (t1 < 0 && t2 > 0) {
            t = t2;
        }

        auto pos = ray.GetOrigin() + ray.GetDirection() * t;

        auto norm = pos - sphere.GetCenter();

        if (DotProduct(norm, ray.GetDirection()) > 0) {
            norm = norm * -1;
        }

        return Intersection(pos, norm, t);
    }

    return std::nullopt;
}

Vector Reflect(const Vector& ray, const Vector& normal) {
    return ray - normal * 2 * DotProduct(ray, normal);
}

std::optional<Vector> Refract(const Vector& ray, const Vector& normal, double eta) {
    double cos = DotProduct(ray, normal);
    double k = 1 - eta * eta * (1 - cos * cos);

    if (k < 0) {
        return std::nullopt;
    }

    return ray * eta - normal * (eta * cos + std::sqrt(k));
}

Vector GetBarycentricCoords(const Triangle& triangle, const Vector& point) {
    Vector ab = triangle[1] - triangle[0];
    Vector ac = triangle[2] - triangle[0];
    Vector ap = point - triangle[0];

    double dot_abab = DotProduct(ab, ab);
    double dot_abac = DotProduct(ab, ac);
    double dot_acac = DotProduct(ac, ac);
    double dot_apab = DotProduct(ap, ab);
    double dot_apac = DotProduct(ap, ac);

    double denominator = dot_abab * dot_acac - dot_abac * dot_abac;

    double alpha = (dot_acac * dot_apab - dot_abac * dot_apac) / denominator;
    double beta = (dot_abab * dot_apac - dot_abac * dot_apab) / denominator;
    double gamma = 1 - alpha - beta;

    return {
        gamma,
        alpha,
        beta,
    };
}

std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {

    const double epsilon = 0.0000001;

    const Vector& vertex0 = triangle[0];
    const Vector& vertex1 = triangle[1];
    const Vector& vertex2 = triangle[2];
    const Vector& ray_origin = ray.GetOrigin();
    const Vector& ray_direction = ray.GetDirection();

    Vector edge1 = vertex1 - vertex0;
    Vector edge2 = vertex2 - vertex0;

    Vector h = CrossProduct(ray_direction, edge2);
    double a = DotProduct(edge1, h);

    if (a > -epsilon && a < epsilon) {
        return std::nullopt;  // Ray is parallel to the triangle.
    }

    double f = 1.0 / a;
    Vector s = ray_origin - vertex0;
    double u = f * DotProduct(s, h);

    if (u < 0.0 || u > 1.0) {
        return std::nullopt;
    }

    Vector q = CrossProduct(s, edge1);
    double v = f * DotProduct(ray_direction, q);

    if (v < 0.0 || u + v > 1.0) {
        return std::nullopt;
    }

    double t = f * DotProduct(edge2, q);

    if (t > epsilon) {
        Vector intersection_point = ray_origin + ray_direction * t;

        Vector normal = CrossProduct(edge1, edge2);

        if (DotProduct(normal, ray_direction) > 0) {
            normal = normal * -1;
        }

        return Intersection(intersection_point, normal, t);
    }

    return std::nullopt;  // No intersection in the ray direction.
}

// std::optional<Intersection> GetIntersection(const Ray& ray, const Triangle& triangle) {
//     Vector normal = CrossProduct(triangle[1] - triangle[0], triangle[2] - triangle[0]);
//     normal.Normalize();
//
//     double cos = DotProduct(normal, ray.GetDirection());
//
//     if (std::abs(cos) < 1e-6) {
//         return std::nullopt;
//     }
//
//     double t = DotProduct(normal, triangle[0] - ray.GetOrigin()) / cos;
//
//     if (t < 0) {
//         return std::nullopt;
//     }
//
//     Vector pos = ray.GetOrigin() + ray.GetDirection() * t;
//
//     Vector coords = GetBarycentricCoords(triangle, pos);
//
//     if (coords[0] >= 0 && coords[1] >= 0 && coords[2] >= 0) {
//
//         if (DotProduct(normal, ray.GetDirection()) > 0) {
//             normal = normal * -1;
//         }
//
//         return Intersection(pos, normal, t);
//     } else {
//         return std::nullopt;
//     }
// }