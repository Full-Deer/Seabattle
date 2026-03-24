#ifndef BOARD_H
#define BOARD_H

#include "game.h"
#include <stdbool.h>
#include <stddef.h>
#include <wchar.h>

#define BOARD_SIZE 10  // Размер поля 10x10

// Цвета ANSI
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_BOLD    "\033[1m"

// Фоновые цвета
#define BG_BLUE       "\033[44m"   // Голубой фон
#define BG_RESET      "\033[49m"   // Сброс фона

// Рамки
#define BORDER_TOP_LEFT     "┌"
#define BORDER_TOP_RIGHT    "┐"
#define BORDER_BOTTOM_LEFT  "└"
#define BORDER_BOTTOM_RIGHT "┘"
#define BORDER_HORIZONTAL   "─"
#define BORDER_VERTICAL     "│"

// Типы клеток
typedef enum {
    CELL_EMPTY = L'~',      // Вода
    CELL_SHIP = L'□',       // Корабль (пустой квадрат)
    CELL_MISS = L'•',       // Промах (точка)
    CELL_HIT = L'X',        // Попадание
    CELL_DEAD = L'█'        // Убитый корабль (заполненный квадрат)
} CellType;

// Структура поля
typedef struct {
    wchar_t cells[BOARD_SIZE][BOARD_SIZE];
    wchar_t visible[BOARD_SIZE][BOARD_SIZE];
} Board;

int letter_to_index(wchar_t letter);

wchar_t index_to_letter(int index);

void board_init(Board* board);

void board_print_side_by_side(const Board* player_board, const Board* enemy_board,
                              bool show_player_ships, const wchar_t* player_name,
                              const wchar_t* enemy_name, const wchar_t* message);

void board_print_player_only(const Board* player_board, const wchar_t* message);

void board_print(const Board* board, bool show_ships, const wchar_t* player_name);

bool board_can_place_ship(const Board* board, int x, int y, int size, bool horizontal);

void board_place_ship(Board* board, int x, int y, int size, bool horizontal);

GameShotResult board_shoot(Board* board, int x, int y, int* ship_parts_left);

void board_mark_dead_ship(Board* board, int x, int y, int size, bool horizontal);

bool board_all_ships_dead(const Board* board);

size_t board_get_size(void);

const char* get_cell_color(wchar_t cell, bool is_player_ship);

#endif
