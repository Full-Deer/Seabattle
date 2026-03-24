#include "board.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <locale.h>

static const wchar_t RUSSIAN_LETTERS[] = L"АБВГДЕЖЗИК";

int letter_to_index(wchar_t letter) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        if (RUSSIAN_LETTERS[i] == letter ||
            RUSSIAN_LETTERS[i] == (int)towupper(letter)) {
            return i;
        }
    }
    return -1;
}

wchar_t index_to_letter(int index) {
    if (index >= 0 && index < BOARD_SIZE) {
        return RUSSIAN_LETTERS[index];
    }
    return L'?';
}

// Получение цвета для клетки
const char* get_cell_color(wchar_t cell, bool is_player_ship) {
  (void)is_player_ship;
    if (cell == CELL_HIT || cell == CELL_DEAD) {
        return COLOR_RED;
    }
    return COLOR_WHITE;
}

// Инициализация поля
void board_init(Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            board->cells[i][j] = CELL_EMPTY;
            board->visible[i][j] = CELL_EMPTY;
        }
    }
}

// Вывод поля
void board_print(const Board* board, bool show_ships, const wchar_t* player_name) {
    wprintf(L"\n=== Поле %ls ===\n", player_name);
    wprintf(L"   ");

    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L" %lc ", index_to_letter(i));
    }
    wprintf(L"\n");

    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L"%2d ", i + 1);

        for (int j = 0; j < BOARD_SIZE; j++) {
            wchar_t cell;
            if (show_ships) {
                cell = board->cells[i][j];
            } else {
                cell = board->visible[i][j];
            }

            const char* color = get_cell_color(cell, show_ships);
            wprintf(L"%s %lc %s", color, cell, COLOR_RESET);
        }
        wprintf(L"\n");
    }
    wprintf(L"\n");
}

// Вывод только своего поля
void board_print_player_only(const Board* player_board, const wchar_t* message) {
    
    
    if (message != NULL && wcslen(message) > 0) {
        wprintf(L"%s\n", message);
    }
    
    // Выводим заголовок
    wprintf(L"\n%s=== ВАШЕ ПОЛЕ ===%s\n", COLOR_GREEN, COLOR_RESET);
    
    // Верхняя граница
    wprintf(L"   ");
    wprintf(L"%s", BG_BLUE);
    wprintf(L"┌");
    for (int i = 0; i < BOARD_SIZE * 3 + 1; i++) {
        wprintf(L"─");
    }
    wprintf(L"┐");
    wprintf(L"%s\n", BG_RESET);
    
    // Строка с буквами
    wprintf(L"   ");
    wprintf(L"%s│", BG_BLUE);
    wprintf(L"   ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L" %lc ", index_to_letter(i));
    }
    wprintf(L"  ");
    wprintf(L"│");
    wprintf(L"%s\n", BG_RESET);
    
    // Вывод строк поля
    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L"   ");
        wprintf(L"%s│%2d ", BG_BLUE, i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            wchar_t cell = player_board->visible[i][j];
            const char* color = get_cell_color(cell, true);
            
            // Каждая клетка занимает 3 символа: пробел + символ + пробел
            if (cell == CELL_EMPTY) {
                wprintf(L"%s ~ %s", BG_BLUE, BG_BLUE);
            } else {
                wprintf(L"%s %lc %s", color, cell, BG_BLUE);
            }
        }
        wprintf(L" │");
        wprintf(L"%s\n", BG_RESET);
    }
    
    // Нижняя граница
    wprintf(L"   ");
    wprintf(L"%s", BG_BLUE);
    wprintf(L"└");
    for (int i = 0; i < BOARD_SIZE * 3 + 1; i++) {
        wprintf(L"─");
    }
    wprintf(L"┘");
    wprintf(L"%s\n", BG_RESET);
    
    wprintf(L"\n");
}

// Вывод двух полей рядом
void board_print_side_by_side(const Board* player_board, const Board* enemy_board,
                              bool show_player_ships, const wchar_t* player_name,
                              const wchar_t* enemy_name, const wchar_t* message) {

   if (enemy_board == NULL) {
      board_print_player_only(player_board, message);
       return;
    }
    
    system("clear");
    
    // Выводим сообщение о предыдущем ходе
    if (message != NULL && wcslen(message) > 0) {
        wprintf(L"%s\n", message);
    }
    
    // Выводим заголовки
    wprintf(L"\n%s=== ВАШЕ ПОЛЕ %ls ===%20s=== ПОЛЕ ПРОТИВНИКА %ls ===%s\n",
            COLOR_GREEN, player_name, "", enemy_name, COLOR_RESET);
    
    // Левое поле - верхняя граница
    wprintf(L"   ");
    wprintf(L"%s", BG_BLUE);
    wprintf(L"┌");
    for (int i = 0; i < BOARD_SIZE * 3 + 4; i++) {
        wprintf(L"─");
    }
    wprintf(L"┐");
    wprintf(L"%s", BG_RESET);
    
    wprintf(L"    ");
    
    // Правое поле - верхняя граница
    wprintf(L"%s", BG_BLUE);
    wprintf(L"┌");
    for (int i = 0; i < BOARD_SIZE * 3 + 4; i++) {
        wprintf(L"─");
    }
    wprintf(L"┐");
    wprintf(L"%s\n", BG_RESET);
    
    // Строка с буквами
    wprintf(L"   ");
    wprintf(L"%s│", BG_BLUE);
    wprintf(L"   ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L" %lc ", index_to_letter(i));
    }
    wprintf(L" ");
    wprintf(L"│");
    wprintf(L"%s", BG_RESET);
    
    wprintf(L"    ");
    
    wprintf(L"%s│", BG_BLUE);
    wprintf(L"   ");
    for (int i = 0; i < BOARD_SIZE; i++) {
        wprintf(L" %lc ", index_to_letter(i));
    }
    wprintf(L" ");
    wprintf(L"│");
    wprintf(L"%s\n", BG_RESET);
    
    // Вывод строк полей
    for (int i = 0; i < BOARD_SIZE; i++) {
        // Левое поле
        wprintf(L"   ");
        wprintf(L"%s│%2d ", BG_BLUE, i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            wchar_t cell;
            if (show_player_ships) {
                cell = player_board->cells[i][j];
            } else {
                cell = player_board->visible[i][j];
            }
            const char* color = get_cell_color(cell, true);
            
            // Каждая клетка занимает 3 символа
            if (cell == CELL_EMPTY) {
                wprintf(L"%s ~ %s", BG_BLUE, BG_BLUE);
            } else {
                wprintf(L"%s %lc %s", color, cell, BG_BLUE);
            }
        }
        wprintf(L" │");
        wprintf(L"%s", BG_RESET);
        
        // Разделитель
        wprintf(L"    ");
        
        // Правое поле
        wprintf(L"%s│%2d ", BG_BLUE, i + 1);
        for (int j = 0; j < BOARD_SIZE; j++) {
            wchar_t cell = enemy_board->visible[i][j];
            const char* color = get_cell_color(cell, false);
            
            // Каждая клетка занимает 3 символа
            if (cell == CELL_EMPTY) {
                wprintf(L"%s ~ %s", BG_BLUE, BG_BLUE);
            } else {
                wprintf(L"%s %lc %s", color, cell, BG_BLUE);
            }
        }
        wprintf(L" │");
        wprintf(L"%s\n", BG_RESET);
    }
    
    // Левое поле - нижняя граница
    wprintf(L"   ");
    wprintf(L"%s", BG_BLUE);
    wprintf(L"└");
    for (int i = 0; i < BOARD_SIZE * 3 + 4; i++) {
        wprintf(L"─");
    }
    wprintf(L"┘");
    wprintf(L"%s", BG_RESET);
    
    wprintf(L"    ");
    
    // Правое поле - нижняя граница
    wprintf(L"%s", BG_BLUE);
    wprintf(L"└");
    for (int i = 0; i < BOARD_SIZE * 3 + 4; i++) {
        wprintf(L"─");
    }
    wprintf(L"┘");
    wprintf(L"%s\n", BG_RESET);
    
    wprintf(L"\n");
}

// Проверка возможности размещения корабля
bool board_can_place_ship(const Board* board, int x, int y, int size, bool horizontal) {
    // Проверка границ
    if (horizontal) {
        if (x + size > BOARD_SIZE) return false;
    } else {
        if (y + size > BOARD_SIZE) return false;
    }

    // Проверяем все клетки, которые займет корабль
    for (int i = 0; i < size; i++) {
        int check_x = horizontal ? x + i : x;
        int check_y = horizontal ? y : y + i;

        // Проверяем саму клетку
        if (board->cells[check_y][check_x] != CELL_EMPTY) {
            return false;
        }

        // Проверяем все соседние клетки (включая диагональные)
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int nx = check_x + dx;
                int ny = check_y + dy;

                if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE) {
                    if (horizontal) {
                        if (!(ny == y && nx >= x && nx < x + size)) {
                            if (board->cells[ny][nx] == CELL_SHIP ||
                                board->cells[ny][nx] == CELL_HIT ||
                                board->cells[ny][nx] == CELL_DEAD) {
                                return false;
                            }
                        }
                    } else {
                        if (!(nx == x && ny >= y && ny < y + size)) {
                            if (board->cells[ny][nx] == CELL_SHIP ||
                                board->cells[ny][nx] == CELL_HIT ||
                                board->cells[ny][nx] == CELL_DEAD) {
                                return false;
                            }
                        }
                    }
                }
            }
        }
    }
    return true;
}

// Размещение корабля
void board_place_ship(Board* board, int x, int y, int size, bool horizontal) {
    for (int i = 0; i < size; i++) {
        if (horizontal) {
            board->cells[y][x + i] = CELL_SHIP;
        } else {
            board->cells[y + i][x] = CELL_SHIP;
        }
    }
}

// Выстрел по координатам
GameShotResult board_shoot(Board* board, int x, int y, int* ship_parts_left) {
    if (x < 0 || x >= BOARD_SIZE || y < 0 || y >= BOARD_SIZE) {
        return SHOT_INVALID;
    }

    if (board->visible[y][x] == CELL_MISS ||
        board->visible[y][x] == CELL_HIT ||
        board->visible[y][x] == CELL_DEAD) {
        return SHOT_INVALID;
    }

    wchar_t cell = board->cells[y][x];

    if (cell == CELL_EMPTY) {
        board->cells[y][x] = CELL_MISS;
        board->visible[y][x] = CELL_MISS;
        return SHOT_MISS;
    } else if (cell == CELL_SHIP || cell == CELL_HIT) {
        board->cells[y][x] = CELL_HIT;
        board->visible[y][x] = CELL_HIT;
        (*ship_parts_left)--;
        return SHOT_HIT;
    }

    return SHOT_INVALID;
}

// Отметка уничтоженного корабля и точек вокруг
void board_mark_dead_ship(Board* board, int x, int y, int size, bool horizontal) {

    // Сначала помечаем все клетки корабля как убитые
    for (int i = 0; i < size; i++) {
        int ship_x = horizontal ? x + i : x;
        int ship_y = horizontal ? y : y + i;
        board->cells[ship_y][ship_x] = CELL_DEAD;
        board->visible[ship_y][ship_x] = CELL_DEAD;
    }

    // Помечаем соседние клетки
    for (int i = 0; i < size; i++) {
        int ship_x = horizontal ? x + i : x;
        int ship_y = horizontal ? y : y + i;

        // Проверяем все 8 направлений вокруг каждой палубы
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                // Пропускаем саму клетку корабля
                if (dx == 0 && dy == 0) continue;

                int mark_x = ship_x + dx;
                int mark_y = ship_y + dy;

                // Проверяем границы
                if (mark_x >= 0 && mark_x < BOARD_SIZE &&
                    mark_y >= 0 && mark_y < BOARD_SIZE) {

                    // Проверяем, что это не клетка другой части этого же корабля
                    bool is_other_part = false;
                    if (horizontal) {
                        if (mark_y == y && mark_x >= x && mark_x < x + size) {
                            is_other_part = true;
                        }
                    } else {
                        if (mark_x == x && mark_y >= y && mark_y < y + size) {
                            is_other_part = true;
                        }
                    }

                    // Если это не другая часть корабля и клетка пустая
                    if (!is_other_part && board->cells[mark_y][mark_x] == CELL_EMPTY) {
                        board->cells[mark_y][mark_x] = CELL_MISS;
                        board->visible[mark_y][mark_x] = CELL_MISS;
                    }
                }
            }
        }
    }
}

// Проверка, все ли корабли убиты
bool board_all_ships_dead(const Board* board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board->cells[i][j] == CELL_SHIP) {
                return false;
            }
        }
    }
    return true;
}

size_t board_get_size(void) {
    Board b;
    return sizeof(b);
}
