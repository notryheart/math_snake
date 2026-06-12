#pragma once

#include "game.h"
#include <termios.h>

// RAII-обёртка над терминалом (POSIX: Linux / macOS).
// В конструкторе переводит терминал в «сырой» неблокирующий режим и прячет
// курсор; в деструкторе аккуратно всё восстанавливает.
class Terminal {
public:
    Terminal();
    ~Terminal();

    Terminal(const Terminal&)            = delete;
    Terminal& operator=(const Terminal&) = delete;

    // Команды ввода за один кадр.
    struct Input {
        Vec2 dir{0, 0};      // запрошенное направление (0,0 — не нажато)
        bool quit     = false;
        bool restart  = false;
        int  topology = -1;  // 0..kTopologyCount-1, если игрок переключил
    };

    Input poll();                  // неблокирующее чтение клавиатуры
    void  render(const Game& g);   // отрисовка кадра

private:
    termios orig_{};               // исходные настройки терминала
};
