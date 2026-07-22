#pragma once

extern int screenWidth;
extern int screenHeight;

struct view_matrix_t {
    float matrix[4][4];

    float* operator[](int index) {
        return matrix[index];
    }

    const float* operator[](int index) const {
        return matrix[index];
    }
};

struct Vector3 {
    Vector3(float x = 0.f, float y = 0.f, float z = 0.f) noexcept;
    // Dizi işaretçisinden constructor (const float*)
    explicit Vector3(const float* data) noexcept : x(data[0]), y(data[1]), z(data[2]) {}
    Vector3 operator-(const Vector3& other) const noexcept;
    Vector3 operator+(const Vector3& other) const noexcept;
    Vector3 operator*(float factor) const noexcept;
    Vector3 operator/(float factor) const noexcept;
    Vector3 w2s(view_matrix_t matrix) const;
    float Distance(const Vector3& other) const noexcept;
    float DotProduct(const Vector3& other) const noexcept;
    Vector3 CrossProduct(const Vector3& other) const noexcept;
    Vector3 Normalize() const noexcept;

    float x, y, z;
};

struct Vector2 {
    float x, y;
    Vector2(float x = 0.f, float y = 0.f) : x(x), y(y) {}
    Vector2 operator+(const Vector2& other) const noexcept {
        return Vector2(x + other.x, y + other.y);
    }
    Vector2 operator-(const Vector2& other) const noexcept {
        return Vector2(x - other.x, y - other.y);
    }
    float Distance(const Vector2& other) const noexcept {
        float dx = x - other.x;
        float dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }
};
