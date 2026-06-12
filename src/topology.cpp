#include "topology.h"

const char* topologyName(Topology t) {
    switch (t) {
        case Topology::Plane:      return "Плоскость (стены)";
        case Topology::Cylinder:   return "Цилиндр";
        case Topology::Torus:      return "Тор";
        case Topology::Mobius:     return "Лента Мёбиуса";
        case Topology::Klein:      return "Бутылка Клейна";
        case Topology::Projective: return "Проективная плоскость RP^2";
    }
    return "?";
}

Transition wrap(Topology t, Vec2 c, Vec2 d, int W, int H) {
    Transition r{c, d, /*alive=*/true};

    const bool outX = (c.x < 0 || c.x >= W);
    const bool outY = (c.y < 0 || c.y >= H);

    // Внутри поля — ничего не делаем.
    if (!outX && !outY) return r;

    // --- Горизонтальные края (выход влево/вправо) --------------------------
    // Здесь скорость заведомо горизонтальна (d.y == 0), вертикаль = 0.
    if (outX) {
        switch (t) {
            case Topology::Plane:
                r.alive = false;                       // стена
                break;

            case Topology::Cylinder:
            case Topology::Torus:
                r.pos.x = (c.x + W) % W;               // прямая склейка
                break;

            case Topology::Mobius:
            case Topology::Klein:
            case Topology::Projective:
                r.pos.x = (c.x + W) % W;               // склейка с переворотом
                r.pos.y = (H - 1) - c.y;               // отражение по вертикали
                r.dir.y = -d.y;                        // якобиан отображения*
                break;
        }
        return r;
    }

    // --- Вертикальные края (выход вверх/вниз) ------------------------------
    if (outY) {
        switch (t) {
            case Topology::Plane:
            case Topology::Cylinder:
            case Topology::Mobius:
                r.alive = false;                       // верх/низ — стены
                break;

            case Topology::Torus:
            case Topology::Klein:
                r.pos.y = (c.y + H) % H;               // прямая склейка
                break;

            case Topology::Projective:
                r.pos.y = (c.y + H) % H;               // склейка с переворотом
                r.pos.x = (W - 1) - c.x;               // отражение по горизонтали
                r.dir.x = -d.x;                        // якобиан отображения*
                break;
        }
        return r;
    }

    return r;
}

// * Поясн.: отображение склейки переворачивает компоненту скорости ВДОЛЬ шва.
//   При пересечении вертикального края скорость горизонтальна (d.y == 0),
//   поэтому r.dir.y = -d.y = 0 — изменения нет. Для движения строго по осям
//   переворот направления всегда тривиален; значимым остаётся перенос
//   позиции. Формула с якобианом diag(1,-1)/diag(-1,1) оставлена для
//   математической корректности (она «сработала» бы при диагональном ходе).
