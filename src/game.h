#ifndef GAME_H
#define GAME_H

#include <stdbool.h>
#include <wchar.h>

// Состояния игры
typedef enum {
    STATE_PLACING,       // Расстановка кораблей
    STATE_PLAYER_TURN,   // Ход игрока
    STATE_COMPUTER_TURN, // Ход компьютера
    STATE_GAME_OVER,     // Игра окончена
    STATE_EXIT           // Выход
} GameState;

// Результаты выстрела
typedef enum {
    SHOT_MISS,      // Мимо
    SHOT_HIT,       // Попадание
    SHOT_KILL,      // Убил
    SHOT_INVALID    // Неверный выстрел
} GameShotResult;

// Структура игрока
typedef struct {
    wchar_t name[50];
    int score;
    bool is_computer;
    int hits;      // Количество попаданий
    int misses;    // Количество промахов
    int kills;     // Количество уничтоженных кораблей
} Player;

// Главная функция игры
void game_loop(void);

// Управление состоянием
void set_game_state(GameState new_state);
GameState get_game_state(void);

#endif
