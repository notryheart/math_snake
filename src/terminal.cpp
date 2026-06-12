#include "terminal.h"
#include <unistd.h>
#include <cstdio>
#include <string>

Terminal::Terminal() {
    tcgetattr(STDIN_FILENO, &orig_);
    termios raw = orig_;
    raw.c_lflag &= ~(ICANON | ECHO); // без буферизации строки и без эха
    raw.c_cc[VMIN]  = 0; // read() не блокируется...
    raw.c_cc[VTIME] = 0; // ...и возвращается мгновенно
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    std::printf("\033[?25l"); // спрятать курсор
    std::printf("\033[2J"); // очистить экран
    std::fflush(stdout);
}

Terminal::~Terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_);
    std::printf("\033[?25h"); // показать курсор
    std::printf("\033[0m\n"); // сбросить цвет
    std::fflush(stdout);
}

Terminal::Input Terminal::poll() {
    Input in;
    unsigned char buf[32];
    int n = static_cast<int>(read(STDIN_FILENO, buf, sizeof buf));

    for (int i = 0; i < n; ++i) {
        unsigned char c = buf[i];

        // Стрелки приходят как escape-последовательность: ESC '[' A/B/C/D.
        if (c == 27 && i + 2 < n && buf[i + 1] == '[') {
            switch (buf[i + 2]) {
                case 'A': in.dir = Vec2{0, -1}; break;  // вверх
                case 'B': in.dir = Vec2{0,  1}; break;  // вниз
                case 'C': in.dir = Vec2{1,  0}; break;  // вправо
                case 'D': in.dir = Vec2{-1, 0}; break;  // влево
            }
            i += 2;
            continue;
        }

        switch (c) {
            case 'w': case 'W': in.dir = Vec2{0, -1}; break;
            case 's': case 'S': in.dir = Vec2{0,  1}; break;
            case 'a': case 'A': in.dir = Vec2{-1, 0}; break;
            case 'd': case 'D': in.dir = Vec2{1,  0}; break;
            case 'q': case 'Q': in.quit    = true;    break;
            case 'r': case 'R': in.restart = true;    break;
            case '1': in.topology = 0; break;
            case '2': in.topology = 1; break;
            case '3': in.topology = 2; break;
            case '4': in.topology = 3; break;
            case '5': in.topology = 4; break;
            case '6': in.topology = 5; break;
        }
    }
    return in;
}

void Terminal::render(const Game& g) {
    const int W = g.width();
    const int H = g.height();

    std::string out;
    out.reserve(static_cast<std::size_t>((W + 8) * (H + 8)));

    out += "\033[H";  // курсор в левый верхний угол (без полной очистки)

    // --- HUD ----------------------------------------------------------------
    out += "  \033[1;36mЗМЕЙКА НА ТОПОЛОГИЧЕСКИХ ПОВЕРХНОСТЯХ\033[0m\033[K\n";
    out += "  Поверхность: \033[1;33m";
    out += topologyName(g.topology());
    out += "\033[0m   Счёт: \033[1;32m";
    out += std::to_string(g.score());
    out += "\033[0m\033[K\n";

    // --- Подготовка сетки символов -----------------------------------------
    std::string grid(static_cast<std::size_t>(W) * H, ' ');
    auto idx = [W](int x, int y) {
        return static_cast<std::size_t>(y) * W + x;
    };

    Vec2 f = g.food();
    grid[idx(f.x, f.y)] = '*';

    const auto& s = g.snake();
    for (std::size_t i = 0; i < s.size(); ++i) {
        grid[idx(s[i].x, s[i].y)] = (i == 0) ? '@' : 'o';
    }

    // --- Верхняя рамка ------------------------------------------------------
    out += "  ┌";
    for (int x = 0; x < W; ++x) out += "─";
    out += "┐\033[K\n";

    // --- Поле ---------------------------------------------------------------
    for (int y = 0; y < H; ++y) {
        out += "  │";
        for (int x = 0; x < W; ++x) {
            switch (grid[idx(x, y)]) {
                case '@': out += "\033[1;32m@\033[0m"; break;  // голова
                case 'o': out += "\033[32mo\033[0m";   break;  // тело
                case '*': out += "\033[1;31m*\033[0m"; break;  // еда
                default:  out += ' ';                  break;
            }
        }
        out += "│\033[K\n";
    }

    // --- Нижняя рамка -------------------------------------------------------
    out += "  └";
    for (int x = 0; x < W; ++x) out += "─";
    out += "┘\033[K\n";

    // --- Подсказки и статус -------------------------------------------------
    out += "  WASD/стрелки — движение | 1-6 — поверхность | R — рестарт | "
           "Q — выход\033[K\n";
    out += "  \033[2m1 Плоскость  2 Цилиндр  3 Тор  4 Мёбиус  5 Клейн  "
           "6 RP^2\033[0m\033[K\n";

    if (g.isOver()) {
        out += "  \033[1;31mИГРА ОКОНЧЕНА — нажмите R, чтобы начать заново."
               "\033[0m\033[K\n";
    } else {
        out += "\033[K\n";  // пустая строка (стираем возможный прежний текст)
    }

    std::fputs(out.c_str(), stdout);
    std::fflush(stdout);
}
