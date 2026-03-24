#ifndef SHIPS_H
#define SHIPS_H

#include "board.h"
#include <stdbool.h>
#include <wchar.h>

// Структура корабля
typedef struct {
    int x;              // Координата x (0-9)
    int y;              // Координата y (0-9)
    int size;           // Размер (1-4)
    bool horizontal;    // Ориентация
    int hits;           // Количество попаданий
    bool is_alive;      // Жив ли корабль
} Ship;

// Классическое количество кораблей
#define SHIPS_4DECK  1   // Четырёхпалубных: 1
#define SHIPS_3DECK  2   // Трёхпалубных:    2
#define SHIPS_2DECK  3   // Двухпалубных:    3
#define SHIPS_1DECK  4   // Однопалубных:    4
#define TOTAL_SHIPS  (SHIPS_4DECK + SHIPS_3DECK + SHIPS_2DECK + SHIPS_1DECK)  // Всего: 10

// Структура флота
typedef struct {
    Ship ships[TOTAL_SHIPS];
    int ship_count;
    int total_parts;
    int destroyed_parts; // Уничтожено палуб
} Fleet;


void fleet_init_player(Fleet* fleet);

void fleet_init_computer(Fleet* fleet, Board* board);

bool fleet_is_destroyed(const Fleet* fleet);

void fleet_register_hit(Fleet* fleet, int x, int y, GameShotResult* result, Board* board);

const wchar_t* fleet_get_ship_description(int size);

#endif
