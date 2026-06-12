#include "game.h"
#include <chrono>

Game::Game(int width, int height, Topology topo)
    : W_(width), H_(height), topo_(topo),
      rng_(static_cast<unsigned>(
          std::chrono::steady_clock::now().time_since_epoch().count())) {
    // Стартовая змейка длиной 3, движется вправо.
    Vec2 start{W_ / 2, H_ / 2};
    snake_.push_back(start);
    snake_.push_back(Vec2{start.x - 1, start.y});
    snake_.push_back(Vec2{start.x - 2, start.y});

    dir_ = Vec2{1, 0};
    nextDir_ = dir_;
    spawnFood();
}

void Game::setNextDir(Vec2 d) {
    if (d.x == 0 && d.y == 0) return; // Запрещаем мгновенный разворот на 180 градусов.
    if (d.x == -dir_.x && d.y == -dir_.y) return;
    nextDir_ = d;
}

void Game::setTopology(Topology t) {
    topo_ = t;
}

bool Game::occupies(Vec2 p, bool ignoreTail) const {
    const std::size_t n = snake_.size();
    for (std::size_t i = 0; i < n; ++i) {
        if (ignoreTail && i + 1 == n) continue;  // хвост сейчас сдвинется
        if (snake_[i] == p) return true;
    }
    return false;
}

void Game::spawnFood() {
    std::uniform_int_distribution<int> dx(0, W_ - 1);
    std::uniform_int_distribution<int> dy(0, H_ - 1);
    Vec2 p;
    do {
        p = Vec2{dx(rng_), dy(rng_)};
    } while (occupies(p, /*ignoreTail=*/false));
    food_ = p;
}

void Game::step() {
    if (over_) return;

    dir_ = nextDir_;
    Vec2 cand = snake_.front() + dir_;

    // Обработка границ согласно текущей топологии.
    Transition tr = wrap(topo_, cand, dir_, W_, H_);
    if (!tr.alive) { // врезались в стену
        over_ = true;
        return;
    }
    Vec2 newHead = tr.pos;
    dir_ = tr.dir; // направление могло смениться при склейке
    nextDir_ = dir_;

    const bool grew = (newHead == food_);

    // Столкновение с собой: если не растём, хвост освобождает клетку.
    if (occupies(newHead, /*ignoreTail=*/!grew)) {
        over_ = true;
        return;
    }

    snake_.push_front(newHead);
    if (grew) {
        ++score_;
        spawnFood();
    } else {
        snake_.pop_back();
    }
}
