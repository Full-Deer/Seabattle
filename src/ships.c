#include "ships.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

// Инициализация кораблей игрока
void fleet_init_player(Fleet* fleet) {
    fleet->ship_count = 0;
    fleet->total_parts = 0;
    fleet->destroyed_parts = 0;
}

// Получение описания корабля
const wchar_t* fleet_get_ship_description(int size) {
    switch(size) {
        case 4: return L"четырёхпалубный";
        case 3: return L"трёхпалубный";
        case 2: return L"двухпалубный";
        case 1: return L"однопалубный";
        default: return L"неизвестный";
    }
}

// Инициализация флота компьютера
void fleet_init_computer(Fleet* fleet, Board* board) {
    fleet->ship_count = 0;
    fleet->total_parts = 0;
    fleet->destroyed_parts = 0;

    // Сначала расставляем большие корабли
    int ship_sizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

    for (int i = 0; i < TOTAL_SHIPS; i++) {
        bool placed = false;
        int attempts = 0;

        while (!placed && attempts < 1000) {
            int x = rand() % BOARD_SIZE;
            int y = rand() % BOARD_SIZE;
            bool horizontal = rand() % 2;
            if (board_can_place_ship(board, x, y, ship_sizes[i], horizontal)) {
                // Сохраняем корабль
                fleet->ships[fleet->ship_count].x = x;
                fleet->ships[fleet->ship_count].y = y;
                fleet->ships[fleet->ship_count].size = ship_sizes[i];
                fleet->ships[fleet->ship_count].horizontal = horizontal;
                fleet->ships[fleet->ship_count].hits = 0;
                fleet->ships[fleet->ship_count].is_alive = true;

                // Размещаем на доске
                board_place_ship(board, x, y, ship_sizes[i], horizontal);

                fleet->ship_count++;
                fleet->total_parts += ship_sizes[i];
                placed = true;
            }
            attempts++;
        }
        if (!placed) {
            wprintf(L"Ошибка: не удалось разместить корабль %d\n", ship_sizes[i]);
        }
    }
}

// Проверка, все ли корабли уничтожены
bool fleet_is_destroyed(const Fleet* fleet) {
    return fleet->destroyed_parts >= fleet->total_parts;
}

// Регистрация попадания
void fleet_register_hit(Fleet* fleet, int x, int y, GameShotResult* result, Board* board) {
    // Сначала ищем, в какой корабль попали
    for (int i = 0; i < fleet->ship_count; i++) {
        Ship* ship = &fleet->ships[i];

        if (!ship->is_alive) continue;

        bool is_part = false;
        if (ship->horizontal) {
            // Горизонтальный корабль: y фиксирован, x меняется
            if (y == ship->y && x >= ship->x && x < ship->x + ship->size) {
                is_part = true;
            }
        } else {
            // Вертикальный корабль: x фиксирован, y меняется
            if (x == ship->x && y >= ship->y && y < ship->y + ship->size) {
                is_part = true;
            }
        }

        if (is_part) {
            // Нашли корабль, в который попали
            ship->hits++;
            fleet->destroyed_parts++;

            wprintf(L"Попадание в %ls! (часть %d/%d)\n",
                   fleet_get_ship_description(ship->size),
                   ship->hits, ship->size);

            if (ship->hits >= ship->size) {
                // Корабль уничтожен
                ship->is_alive = false;
                *result = SHOT_KILL;
                wprintf(L"Корабль (%ls) УНИЧТОЖЕН!\n",
                       fleet_get_ship_description(ship->size));
                // Отмечаем уничтоженный корабль на доске
                board_mark_dead_ship(board, ship->x, ship->y, ship->size, ship->horizontal);
            } else {
                *result = SHOT_HIT;
            }
            return;
        }
    }

    // Если не нашли корабль
    wprintf(L"Ошибка: попадание в неизвестный корабль!\n");
    *result = SHOT_HIT;
}
