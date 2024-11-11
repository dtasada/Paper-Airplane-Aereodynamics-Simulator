#ifndef ENGINE_HEADER
#define ENGINE_HEADER

#include <raymath.h>

#include <string>

struct v3 {
    float x, y, z;

    v3(float x, float y, float z);
    v3(Vector3 v);
    v3(void);

    v3 operator+(const v3& other);
    v3 operator-(const v3& other);
    v3 operator*(const v3& other);
    v3 operator/(const v3& other);
    v3 operator+=(const v3& other);
    v3 operator-=(const v3& other);
    v3 operator*=(const v3& other);
    v3 operator/=(const v3& other);

    v3 operator-(const float& other);
    v3 operator+(const float& other);
    v3 operator*(const float& other);
    v3 operator/(const float& other);
    v3 operator*=(const float& other);
    v3 operator/=(const float& other);

    std::string to_string();
    const char* to_cstr();

    operator Vector3() const;
    operator float*() const;
};

struct Cell {
    v3 position;
    float density;
    float distanceSquared;  // Distance from the camera (squared)
};

#endif