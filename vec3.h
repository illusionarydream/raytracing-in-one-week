#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include "math_materials.h"
using std::sqrt;

class Vec3 {
   public:
    double e[3];

    Vec3() : e{0, 0, 0} {}
    Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }

    Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
    double operator[](int i) const { return e[i]; }
    double &operator[](int i) { return e[i]; }

    Vec3 &operator+=(const Vec3 &v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        return *this;
    }

    Vec3 &operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        return *this;
    }

    Vec3 &operator/=(double t) {
        return *this *= 1 / t;
    }

    double length() const {
        return sqrt(length_squared());
    }

    double length_squared() const {
        return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
    }

    bool near_zero() {
        return (fabs(e[0]) < min_double_error) && (fabs(e[1]) < min_double_error) && (fabs(e[2]) < min_double_error);
    }
};

// point3 is just an alias for vec3, but useful for geometric clarity in the code.
using Point3 = Vec3;

// Vector Utility Functions

inline std::ostream &operator<<(std::ostream &out, const Vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
    return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) {
    return t * v;
}

inline Vec3 operator/(Vec3 v, double t) {
    return (1 / t) * v;
}

inline double dot(const Vec3 &u, const Vec3 &v) {
    return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
    return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(Vec3 v) {
    return v / v.length();
}

static Vec3 random_vector() {
    return Vec3(random_double(), random_double(), random_double());
}

static Vec3 random_vector(double min, double max) {
    return Vec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

// 拒绝-接受采样法
inline Vec3 random_in_unit_sphere() {
    while (true) {
        auto new_vector = random_vector(-1.0, 1.0);
        if (new_vector.length_squared() < 1.0)
            return new_vector;
    }
}

inline Vec3 random_unit_vector() {
    return unit_vector(random_in_unit_sphere());
}

inline Vec3 random_unit_vector_in_hemisphere(const Vec3 &normal) {
    auto new_vec = random_unit_vector();
    if (dot(normal, new_vec) > 0.0)
        return new_vec;
    return -new_vec;
}

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) {
    return unit_vector(v - 2 * dot(v, n) * n);
}

// * etai_over_etat 入射介质折射率比折射介质折射率
inline Vec3 refraction(const Vec3 &v_in, const Vec3 &n, double etai_over_etat) {
    // auto cos_theta = fmin(dot(-v_in, n), 1.0);
    // vec3 r_out_perp = etai_over_etat * (v_in + cos_theta * n);
    // vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    // return r_out_perp + r_out_parallel;
    auto v_out_vertical = (v_in - dot(v_in, n) * n) * etai_over_etat;
    // 若发生全反射则直接返回反射向量
    if (v_out_vertical.length() >= 1.0 - min_double_error)
        return reflect(v_in, n);
    auto v_out_parrell = -n * sqrt(1 - v_out_vertical.length_squared());
    return v_out_parrell + v_out_vertical;
}

inline Vec3 random_in_unit_disk() {
    while (true) {
        auto p = Vec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0);
        if (p.length() <= 1.0)
            return p;
    }
}
#endif