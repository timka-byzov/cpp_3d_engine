#pragma once

#include <vector.h>

class Triangle {
public:
    Triangle(const Vector& a, const Vector& b, const Vector& c) : a_(a), b_(b), c_(c) {
    }

    const Vector& operator[](size_t ind) const {
        if (ind == 0) {
            return a_;
        } else if (ind == 1) {
            return b_;
        } else {
            return c_;
        }
    }

    double Area() const {
        Vector ab = b_ - a_;
        Vector ac = c_ - a_;

        return Length(CrossProduct(ab, ac)) / 2;
    }

private:
    const Vector a_, b_, c_;
};
