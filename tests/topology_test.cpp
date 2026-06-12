#include "topology.h"
#include <cassert>
#include <cstdio>

// Тесты проверяют правила склейки краёв. Запускаются в любом окружении
// (без терминала), поэтому годятся для CI.
int main() {
    const int W = 10;
    const int H = 8;

    // ТОР: правый край -> левый, направление не меняется.
    {
        Transition t = wrap(Topology::Torus, Vec2{W, 3}, Vec2{1, 0}, W, H);
        assert(t.alive && t.pos.x == 0 && t.pos.y == 3);
        assert(t.dir.x == 1 && t.dir.y == 0);
    }
    // ТОР: низ -> верх.
    {
        Transition t = wrap(Topology::Torus, Vec2{4, H}, Vec2{0, 1}, W, H);
        assert(t.alive && t.pos.x == 4 && t.pos.y == 0);
    }

    // ПЛОСКОСТЬ: любой край — стена.
    {
        Transition t = wrap(Topology::Plane, Vec2{W, 3}, Vec2{1, 0}, W, H);
        assert(!t.alive);
    }

    // ЦИЛИНДР: бок заворачивается, верх/низ — стена.
    {
        Transition a = wrap(Topology::Cylinder, Vec2{W, 2}, Vec2{1, 0}, W, H);
        assert(a.alive && a.pos.x == 0 && a.pos.y == 2);
        Transition b = wrap(Topology::Cylinder, Vec2{5, H}, Vec2{0, 1}, W, H);
        assert(!b.alive);
    }

    // МЁБИУС: правый край -> левый с отражением по Y; верх/низ — стена.
    {
        Transition t = wrap(Topology::Mobius, Vec2{W, 1}, Vec2{1, 0}, W, H);
        assert(t.alive && t.pos.x == 0 && t.pos.y == (H - 1) - 1);
        Transition v = wrap(Topology::Mobius, Vec2{3, -1}, Vec2{0, -1}, W, H);
        assert(!v.alive);
    }

    // КЛЕЙН: бок с отражением, верх/низ — прямая склейка.
    {
        Transition s = wrap(Topology::Klein, Vec2{-1, 2}, Vec2{-1, 0}, W, H);
        assert(s.alive && s.pos.x == W - 1 && s.pos.y == (H - 1) - 2);
        Transition v = wrap(Topology::Klein, Vec2{4, H}, Vec2{0, 1}, W, H);
        assert(v.alive && v.pos.x == 4 && v.pos.y == 0);
    }

    // RP^2: оба края с отражением.
    {
        Transition h = wrap(Topology::Projective, Vec2{W, 2}, Vec2{1, 0}, W, H);
        assert(h.alive && h.pos.x == 0 && h.pos.y == (H - 1) - 2);
        Transition v = wrap(Topology::Projective, Vec2{3, H}, Vec2{0, 1}, W, H);
        assert(v.alive && v.pos.x == (W - 1) - 3 && v.pos.y == 0);
        // Скорость вертикальна (d.x == 0), вдоль-шовная компонента нулевая,
        // поэтому dir.x остаётся 0; значим именно перенос позиции выше.
        assert(v.dir.x == 0);
    }

    // Якобиан склейки: компонента скорости ВДОЛЬ шва меняет знак.
    // (В самой игре змейка ходит только по осям, поэтому на практике это не
    //  проявляется; проверяем математическую корректность отображения.)
    {
        Transition j = wrap(Topology::Klein, Vec2{W, 2}, Vec2{1, 1}, W, H);
        assert(j.dir.x == 1 && j.dir.y == -1);
    }

    std::printf("Все топологические тесты пройдены успешно.\n");
    return 0;
}
