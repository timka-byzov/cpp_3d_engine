#pragma once
#include <sphere.h>
#include <fstream>
#include <string>
#include <vector.h>
#include <light.h>
#include <tuple>
#include <sstream>
#include <vector>
#include <optional>

Sphere ReadSphere(std::istringstream& input) {
    double x, y, z, radius;
    input >> x >> y >> z >> radius;
    return Sphere(Vector(x, y, z), radius);
}

Light ReadLight(std::istringstream& input) {
    double x, y, z, r, g, b;
    input >> x >> y >> z >> r >> g >> b;
    return Light(Vector(x, y, z), Vector(r, g, b));
}

std::vector<std::tuple<std::optional<int>, std::optional<int>, std::optional<int>>> ReadF(
    std::istringstream& input) {

    std::vector<std::tuple<std::optional<int>, std::optional<int>, std::optional<int>>> result;

    std::string a, b, c;

    std::string s;
    while (input >> s) {
        std::istringstream iss{s};

        std::getline(iss, a, '/');
        std::getline(iss, b, '/');
        std::getline(iss, c, '/');

        std::optional<int> b_opt, c_opt;

        if (!b.empty()) {
            b_opt = std::stoi(b);
        }

        if (!c.empty()) {
            c_opt = std::stoi(c);
        }

        result.emplace_back(std::stoi(a), b_opt, c_opt);
    }

    return result;
}