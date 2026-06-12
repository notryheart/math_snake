#pragma once

#include "vec2.h"
#include "topology.h"
#include <deque>
#include <random>

// Игровое состояние и логика. От ввода/вывода ничего не знает.
class Game {
public:
    Game(int width, int height, Topology topo);

    void setNextDir(Vec2 d); // запрос смены направления
    void setTopology(Topology t); // сменить поверхность на лету
    void step(); // один тик логики

    bool isOver() const { return over_; }
    int  score() const { return score_; }
    int  width() const { return W_; }
    int  height() const { return H_; }
    Topology topology() const { return topo_; }
    const std::deque<Vec2>& snake() const { return snake_; }
    Vec2 food() const { return food_; }

private:
    void spawnFood();
    bool occupies(Vec2 p, bool ignoreTail) const;
    int W_;
    int H_;
    Topology topo_;
    std::deque<Vec2> snake_; // голова — front()
    Vec2 dir_; // текущее направление
    Vec2 nextDir_; // запрошенное направление (применяется в step)
    Vec2 food_{};
    int  score_ = 0;
    bool over_  = false;
    std::mt19937 rng_;
};
