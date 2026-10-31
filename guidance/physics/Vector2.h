#pragma once

//  physics/Vector2.h
//  2D vector math — shared by every subsystem in the simulation.

#include <cmath>
#include <string>
#include <sstream>

struct Vector2 {
    float x = 0.f;
    float y = 0.f;

    // Construction 
    constexpr Vector2() = default;
    constexpr Vector2(float x, float y) : x(x), y(y) {}

    // Arithmetic
    Vector2 operator+(const Vector2& o) const { return { x + o.x, y + o.y }; }
    Vector2 operator-(const Vector2& o) const { return { x - o.x, y - o.y }; }
    Vector2 operator*(float s)          const { return { x * s, y * s }; }
    Vector2 operator/(float s)          const { return { x / s, y / s }; }

    Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
    Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y; return *this; }

    Vector2& operator*=(float s) { 
        x *= s; 
        y *= s; 
        return *this; 
    }

    // Geometry 
    float lengthSq() const { return x * x + y * y; }
    float length()   const { return std::sqrt(lengthSq()); }

    float distanceTo(const Vector2& o) const { 
        return (*this - o).length(); 
    }

    Vector2 normalized() const {
        float len = length();
        return (len > 1e-6f) ? (*this / len) : Vector2{};
    }

    float dot(const Vector2& o) const { return x * o.x + y * o.y; }

    float angle() const { return std::atan2(y, x); }

    // Utilities 
    static Vector2 fromAngle(float radians, float magnitude = 1.f) {
        return { std::cos(radians) * magnitude,
                 std::sin(radians) * magnitude };
    }

    std::string toString() const {
        std::ostringstream ss;
        ss << "(" << x << ", " << y << ")";
        return ss.str();
    }
};

// Scalar * Vector
inline Vector2 operator*(float s, const Vector2& v) { return v * s; }
