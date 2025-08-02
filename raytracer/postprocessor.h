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

std::vector<std::vector<Vector>> ToneMapping(std::vector<std::vector<Vector>>& pixels) {
    auto max = 0.;

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            for (size_t i = 0; i < 3; ++i) {
                max = std::max(max, pixels[x][y][i]);
            }
        }
    }

    if (max == 0) {
        return pixels;
    }  // full black image

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            auto v_in = pixels[x][y];
            auto v_out = v_in * (1. + v_in / (max * max)) / (1. + v_in);

            pixels[x][y] = v_out;
        }
    }

    return pixels;
}

int GetGammaColor(double color) {
    return std::pow(color, 1 / 2.2) * 255;
}

std::vector<std::vector<RGB>> GammaCorrection(std::vector<std::vector<Vector>>& pixels) {
    std::vector<std::vector<RGB>> result(pixels.size(), std::vector<RGB>(pixels[0].size()));

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            for (size_t i = 0; i < 3; ++i) {
                RGB rgb{GetGammaColor(pixels[x][y][0]), GetGammaColor(pixels[x][y][1]),
                        GetGammaColor(pixels[x][y][2])};
                result[x][y] = rgb;
            }
        }
    }

    return result;
}

std::vector<std::vector<RGB>> PostProcess(std::vector<std::vector<Vector>>& pixels) {

    auto tone_mapped = ToneMapping(pixels);
    return GammaCorrection(tone_mapped);
}

std::vector<std::vector<RGB>> PostProcessNormal(std::vector<std::vector<Vector>>& pixels) {

    auto get_color = [](double color) -> int { return static_cast<int>(color * 255); };

    std::vector<std::vector<RGB>> result(pixels.size(), std::vector<RGB>(pixels[0].size()));

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            for (size_t i = 0; i < 3; ++i) {
                RGB rgb{get_color(pixels[x][y][0]), get_color(pixels[x][y][1]),
                        get_color(pixels[x][y][2])};
                result[x][y] = rgb;
            }
        }
    }

    return result;
}

std::vector<std::vector<RGB>> PostProcessDepth(std::vector<std::vector<Vector>>& pixels) {
    std::vector<std::vector<RGB>> result(pixels.size(), std::vector<RGB>(pixels[0].size()));

    auto get_color = [](double color, double d) -> int {
        if (color == -1) {
            return 1 * 255;
        }
        return static_cast<int>(color / d * 255);
    };

    auto d = 0.;
    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            if (pixels[x][y][0] != -1) {
                d = std::max(d, pixels[x][y][0]);
            }
        }
    }

    for (size_t x = 0; x < pixels.size(); ++x) {
        for (size_t y = 0; y < pixels[0].size(); ++y) {
            RGB rgb{get_color(pixels[x][y][0], d), get_color(pixels[x][y][1], d),
                    get_color(pixels[x][y][2], d)};
            result[x][y] = rgb;
        }
    }

    return result;
}
