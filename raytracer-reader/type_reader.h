#pragma once
#include <sphere.h>
#include <fstream>
#include <string>
#include <vector.h>
#include <sstream>
#include <light.h>
#include <tuple>
#include <vector>

Vector ReadVector(std::istringstream& input) {
    double r, g, b;
    input >> r >> g >> b;
    return Vector(r, g, b);
}

double ReadDouble(std::istringstream& input) {
    double value;
    input >> value;
    return value;
}

std::string ReadString(std::istringstream& input) {
    std::string material_name;
    input >> material_name;
    return material_name;
}