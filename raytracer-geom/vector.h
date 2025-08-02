#pragma once

#include <array>
#include <cstddef>
#include <cmath>

class Vector {
public:
    Vector() : data_{0, 0, 0} {};
    Vector(double x, double y, double z) : data_{x, y, z} {};

    double& operator[](size_t ind) {
        return data_[ind];
    };
    double operator[](size_t ind) const {
        return data_[ind];
    };

    friend double Length(const Vector& v);

    void Normalize() {
        double len = Length(*this);
        data_[0] /= len;
        data_[1] /= len;
        data_[2] /= len;
    }

    Vector Normalized() {
        Vector res = *this;
        res.Normalize();
        return res;
    }

    bool operator==(const Vector& other) const {
        return data_[0] == other.data_[0] && data_[1] == other.data_[1] &&
               data_[2] == other.data_[2];
    }

    bool operator!=(const Vector& other) const {
        return !(*this == other);
    }

    Vector operator+(const Vector& other) const {
        return {data_[0] + other.data_[0], data_[1] + other.data_[1], data_[2] + other.data_[2]};
    }

    Vector& operator+=(const Vector& other) {
        data_[0] += other.data_[0];
        data_[1] += other.data_[1];
        data_[2] += other.data_[2];
        return *this;
    }

    Vector operator+(double k) const {
        return {data_[0] + k, data_[1] + k, data_[2] + k};
    }

    Vector operator-(const Vector& other) const {
        return {data_[0] - other.data_[0], data_[1] - other.data_[1], data_[2] - other.data_[2]};
    }

    Vector operator*(double k) const {
        return {data_[0] * k, data_[1] * k, data_[2] * k};
    }

    Vector operator/(double k) const {
        return *this * (1 / k);
    }

    Vector operator/(const Vector& other) const {
        return {data_[0] / other.data_[0], data_[1] / other.data_[1], data_[2] / other.data_[2]};
    }

    Vector operator*(const Vector& other) const {
        return {data_[0] * other.data_[0], data_[1] * other.data_[1], data_[2] * other.data_[2]};
    }

    friend Vector operator*(double k, const Vector& v);
    friend Vector operator+(double k, const Vector& v);

private:
    std::array<double, 3> data_;
};

Vector operator*(double k, const Vector& v) {
    return v * k;
}

Vector operator+(double k, const Vector& v) {
    return v + k;
}

double DotProduct(const Vector& a, const Vector& b) {
    return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

Vector CrossProduct(const Vector& a, const Vector& b) {
    return {a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]};
}

double Length(const Vector& v) {
    return std::sqrt(DotProduct(v, v));
}
