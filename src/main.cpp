#include "game.h"
#include "terminal.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <thread>

namespace {

std::atomic<bool> g_running{true};

void onSigint(int) { g_running = false; }  // Ctrl-C: корректный выход

Topology toTopology(int i, Topology fallback) {
    switch (i) {
        case 0: return Topology::Plane;
        case 1: return Topology::Cylinder;
        case 2: return Topology::Torus;
        case 3: return Topology::Mobius;
        case 4: return Topology::Klein;
        case 5: return Topology::Projective;
        default: return fallback;
    }
}

}  // namespace

int main() {
    std::signal(SIGINT, onSigint);

    const int W = 36;
    const int H = 20;
    Topology topo = Topology::Torus;

    Game game(W, H, topo);
    Terminal term;  // RAII: terminal восстановится при выходе из main

    const auto tick = std::chrono::milliseconds(110);

    while (g_running) {
        Terminal::Input in = term.poll();

        if (in.quit) break;
        if (in.restart) game = Game(W, H, topo);
        if (in.topology >= 0) {
            topo = toTopology(in.topology, topo);
            game.setTopology(topo);
        }
        if (in.dir.x != 0 || in.dir.y != 0) game.setNextDir(in.dir);
        game.step(); // ничего не делает, если игра окончена
        term.render(game);
        std::this_thread::sleep_for(tick);
    }
    return 0;
}
