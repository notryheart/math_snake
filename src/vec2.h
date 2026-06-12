#pragma once

// Простая целочисленная 2D-точка / вектор направления.
// Используется и для координат клетки, и для направления движения.
struct Vec2 {
    int x = 0;
    int y = 0;
};

inline bool operator==(const Vec2& a, const Vec2& b) {
    return a.x == b.x && a.y == b.y;
}
inline bool operator!=(const Vec2& a, const Vec2& b) {
    return !(a == b);
}
inline Vec2 operator+(const Vec2& a, const Vec2& b) {
    return Vec2{a.x + b.x, a.y + b.y};
}
