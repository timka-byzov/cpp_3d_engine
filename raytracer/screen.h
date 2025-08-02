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

class Screen {
private:
    const CameraOptions& camera_options_;
    const double aspect_;

public:
    const double plane_width;
    const double plane_height;

    Screen(const CameraOptions& camera_options)
        : camera_options_(camera_options),
          aspect_(static_cast<double>(camera_options.screen_width) / camera_options.screen_height),
          plane_width(CalculatePlaneWidth()),
          plane_height(plane_width / aspect_),
          forward_((camera_options_.look_from - camera_options_.look_to).Normalized()),
          right_(CalculateRight().Normalized()),
          up_(CrossProduct(forward_, right_).Normalized())

    {
    }

    Vector GetPointRay(int i, int j) const {  // TODO: fix
        double scale = std::tan(camera_options_.fov / 2);
        double image_aspect_ratio =
            1.0 * camera_options_.screen_width / camera_options_.screen_height;

        double x = (2 * (i + 0.5) / camera_options_.screen_width - 1) * image_aspect_ratio * scale;
        double y = (2 * (j + 0.5) / camera_options_.screen_height - 1) * scale;
        Vector t = {x, -y, -1};
        t.Normalize();
        return (t[0] * right_ + t[1] * up_ + t[2] * forward_).Normalized();
    }

    const CameraOptions& GetCameraOptions() const {
        return camera_options_;
    }

private:
    const Vector bottom_left_local_;
    const Vector forward_;
    const Vector right_;
    const Vector up_;

    double CalculatePlaneWidth() {
        return 2 * std::tan(camera_options_.fov / 2.) * 1.;
    }

    Vector CalculateRight() {

        if (1 - std::fabs(forward_[1]) < 1e-5) {
            return Vector(1, 0, 0);
        }

        return CrossProduct(Vector(0, 1, 0), forward_).Normalized();
    }
};