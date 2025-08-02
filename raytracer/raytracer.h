#pragma once

#include <image.h>
#include <options/camera_options.h>
#include <options/render_options.h>

#include "screen.h"

#include <filesystem>

#include <scene.h>
#include <ray.h>
#include <geometry.h>
#include <postprocessor.h>
#include <pixel_calculator.h>

#define UNUSED(x) (void)(x)

std::vector<std::vector<Vector>> Raytrace(const Scene& scene, const CameraOptions& camera_options,
                                          const RenderOptions& render_options) {

    std::vector<std::vector<Vector>> preprocessed_pixels(
        camera_options.screen_width, std::vector<Vector>(camera_options.screen_height));

    auto screen = Screen(camera_options);
    for (int x = 0; x < camera_options.screen_width; ++x) {
        for (int y = 0; y < camera_options.screen_height; ++y) {
            auto ray = Ray{camera_options.look_from, screen.GetPointRay(x, y)};
            auto color = CalculateRay(ray, scene, render_options);
            preprocessed_pixels[x][y] = color;
        }
    }

    return preprocessed_pixels;
}

void RenderImage(Image* image, const Scene& scene, const CameraOptions& camera_options,
                 const RenderOptions& render_options) {

    auto preprocessed_pixels = Raytrace(scene, camera_options, render_options);
    std::vector<std::vector<RGB>> pixels;

    if (render_options.mode == RenderMode::kFull) {
        pixels = PostProcess(preprocessed_pixels);
    } else if (render_options.mode == RenderMode::kNormal) {
        pixels = PostProcessNormal(preprocessed_pixels);
    } else if (render_options.mode == RenderMode::kDepth) {
        pixels = PostProcessDepth(preprocessed_pixels);
    } else {
        throw std::runtime_error("Unknown render mode");
    }

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            image->SetPixel(pixels[x][y], y, x);
        }
    }
}

Image Render(const std::filesystem::path& path, const CameraOptions& camera_options,
             const RenderOptions& render_options) {

    auto scene = ReadScene(path);
    Image image(camera_options.screen_width, camera_options.screen_height);
    RenderImage(&image, scene, camera_options, render_options);
    return image;
}

// hello