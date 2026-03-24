#include "game.h"
#include "board.h"
#include "ships.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>
#include <wctype.h>

static GameState current_state = STATE_PLACING;

void set_game_state(GameState new_state) {
    current_state = new_state;
}

GameState get_game_state(void) {
    return current_state;
}

// Функция для вывода статистики
static void print_statistics(const Player* player, const Player* computer) {
    wprintf(L"\n%s========== СТАТИСТИКА ИГРЫ ==========%s\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"\n%s--- %ls ---%s\n", COLOR_GREEN, player->name, COLOR_RESET);
    wprintf(L"  Попаданий: %s%d%s\n", COLOR_CYAN, player->hits, COLOR_RESET);
    wprintf(L"  Промахов:  %s%d%s\n", COLOR_WHITE, player->misses, COLOR_RESET);
    wprintf(L"  Уничтожено кораблей: %s%d%s\n", COLOR_RED, player->kills, COLOR_RESET);
    wprintf(L"  Точность: %s%.1f%%%s\n", COLOR_YELLOW, 
            player->hits + player->misses > 0 ? 
            (float)player->hits / (player->hits + player->misses) * 100 : 0, 
            COLOR_RESET);
    
    wprintf(L"\n%s--- %ls ---%s\n", COLOR_RED, computer->name, COLOR_RESET);
    wprintf(L"  Попаданий: %s%d%s\n", COLOR_CYAN, computer->hits, COLOR_RESET);
    wprintf(L"  Промахов:  %s%d%s\n", COLOR_WHITE, computer->misses, COLOR_RESET);
    wprintf(L"  Уничтожено кораблей: %s%d%s\n", COLOR_RED, computer->kills, COLOR_RESET);
    wprintf(L"  Точность: %s%.1f%%%s\n", COLOR_YELLOW,
            computer->hits + computer->misses > 0 ?
            (float)computer->hits / (computer->hits + computer->misses) * 100 : 0,
            COLOR_RESET);
    
    wprintf(L"\n%s=====================================%s\n", COLOR_YELLOW, COLOR_RESET);
}

// Расстановка кораблей игроком
static void player_place_ships(Player* player, Board* board, Fleet* fleet) {
    wprintf(L"\n%s=== %ls, расставьте свои корабли ===%s\n",
            COLOR_YELLOW, player->name, COLOR_RESET);
    wprintf(L"%sУ вас есть:%s\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"  - 1 четырёхпалубный (4 клетки)\n");
    wprintf(L"  - 2 трёхпалубных (3 клетки)\n");
    wprintf(L"  - 3 двухпалубных (2 клетки)\n");
    wprintf(L"  - 4 однопалубных (1 клетка)\n");
    wprintf(L"Всего: 10 кораблей, 20 палуб\n\n");

    int ship_sizes[] = {4, 3, 3, 2, 2, 2, 1, 1, 1, 1};

    for (int i = 0; i < TOTAL_SHIPS; i++) {
        wchar_t input[20];
        wchar_t letter[10];
        int number;
        wchar_t orientation[10];
        bool horizontal;
        bool placed = false;

        wprintf(L"\n%sОсталось разместить: %d кораблей%s\n",
                COLOR_YELLOW, TOTAL_SHIPS - i, COLOR_RESET);

        while (!placed) {
            board_print(board, true, player->name);
            wprintf(L"%sРазместите %ls (размер %d):%s\n",
                   COLOR_YELLOW, fleet_get_ship_description(ship_sizes[i]),
                   ship_sizes[i], COLOR_RESET);
            wprintf(L"Формат: буква цифра ориентация (например: А1г, А10г, Е5в)\n");
            wprintf(L"(г - горизонтально, в - вертикально): ");

            wscanf(L" %ls", input);
            
            int len = wcslen(input);
            
            // Минимальная длина = 3 символа
            if (len < 3) {
                wprintf(L"%sНеверный формат! Используйте формат: А1г или А10г%s\n",
                       COLOR_RED, COLOR_RESET);
                continue;
            }
            
            // Извлекаем букву
            letter[0] = input[0];
            letter[1] = L'\0';
            
            // Определяем, где начинается ориентация
            int orientation_pos;
            if (len >= 4 && iswdigit(input[1]) && iswdigit(input[2])) {
                // Двузначное число (10)
                number = (input[1] - L'0') * 10 + (input[2] - L'0');
                orientation_pos = 3;
            } else if (len >= 3 && iswdigit(input[1])) {
                // Однозначное число (1-9)
                number = input[1] - L'0';
                orientation_pos = 2;
            } else {
                wprintf(L"%sНеверный формат! Используйте формат: А1г или А10г%s\n",
                       COLOR_RED, COLOR_RESET);
                continue;
            }
            
            // Извлекаем ориентацию
            if (orientation_pos < len) {
                orientation[0] = input[orientation_pos];
                orientation[1] = L'\0';
            } else {
                wprintf(L"%sНеверный формат! Не указана ориентация (г или в)%s\n",
                       COLOR_RED, COLOR_RESET);
                continue;
            }
            
            // Проверяем корректность числа
            if (number < 1 || number > 10) {
                wprintf(L"%sНеверное число! Используйте цифры 1-10.%s\n",
                       COLOR_RED, COLOR_RESET);
                continue;
            }

            int x = letter_to_index(letter[0]);
            int y = number - 1;

            horizontal = (orientation[0] == L'г' || orientation[0] == L'Г');

            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                if (board_can_place_ship(board, x, y, ship_sizes[i], horizontal)) {
                    fleet->ships[fleet->ship_count].x = x;
                    fleet->ships[fleet->ship_count].y = y;
                    fleet->ships[fleet->ship_count].size = ship_sizes[i];
                    fleet->ships[fleet->ship_count].horizontal = horizontal;
                    fleet->ships[fleet->ship_count].hits = 0;
                    fleet->ships[fleet->ship_count].is_alive = true;

                    board_place_ship(board, x, y, ship_sizes[i], horizontal);

                    fleet->ship_count++;
                    fleet->total_parts += ship_sizes[i];
                    placed = true;
                    wprintf(L"%sКорабль размещён!%s\n", COLOR_GREEN, COLOR_RESET);
                } else {
                    wprintf(L"%sНельзя разместить корабль здесь! Попробуйте снова.%s\n",
                           COLOR_RED, COLOR_RESET);
                }
            } else {
                wprintf(L"%sНеверные координаты! Буква А-К, цифра 1-10.%s\n",
                       COLOR_RED, COLOR_RESET);
            }
        }
    }
}

// Ход игрока
static void player_turn(Player* player, Board* enemy_board, Fleet* enemy_fleet,
                       Board* player_board) {
    wchar_t input[20];
    wchar_t letter[10];
    int number;
    GameShotResult result = SHOT_INVALID;
    wchar_t temp_message[256] = L"";
    static wchar_t last_message[256] = L"";

    wprintf(L"\n%s=== Ход %ls ===%s\n", COLOR_YELLOW, player->name, COLOR_RESET);
    
    board_print_side_by_side(player_board, enemy_board, true, L"Игрок", L"Противник", last_message);

    wprintf(L"%sОсталось палуб у противника: %d%s\n",
            COLOR_YELLOW, enemy_fleet->total_parts - enemy_fleet->destroyed_parts,
            COLOR_RESET);

    while (result == SHOT_INVALID) {
        wprintf(L"Введите координаты для выстрела (буква цифра, например: А5 или А10): ");
        wscanf(L" %ls", input);
        
        int len = wcslen(input);
        
        // Проверяем минимальную длину
        if (len < 2) {
            wprintf(L"%sНеверный формат! Используйте формат: А5 или А10%s\n",
                   COLOR_RED, COLOR_RESET);
            continue;
        }
        
        // Извлекаем букву
        letter[0] = input[0];
        letter[1] = L'\0';
        
        // Извлекаем число
        if (len >= 3 && iswdigit(input[1]) && iswdigit(input[2])) {
            // Двузначное число (10)
            number = (input[1] - L'0') * 10 + (input[2] - L'0');
        } else if (len >= 2 && iswdigit(input[1])) {
            // Однозначное число (1-9)
            number = input[1] - L'0';
        } else {
            wprintf(L"%sНеверный формат! Используйте формат: А5 или А10%s\n",
                   COLOR_RED, COLOR_RESET);
            continue;
        }
        
        // Проверяем корректность числа
        if (number < 1 || number > 10) {
            wprintf(L"%sНеверное число! Используйте цифры 1-10.%s\n",
                   COLOR_RED, COLOR_RESET);
            continue;
        }

        int x = letter_to_index(letter[0]);
        int y = number - 1;

        if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
            int temp_parts = enemy_fleet->total_parts - enemy_fleet->destroyed_parts;
            result = board_shoot(enemy_board, x, y, &temp_parts);

            switch (result) {
                case SHOT_MISS:
                    swprintf(temp_message, 256, L"%sМимо!%s", COLOR_WHITE, COLOR_RESET);
                    wprintf(L"%s\n", temp_message);
                    player->misses++;
                    break;
                case SHOT_HIT:
                    swprintf(temp_message, 256, L"%sПопадание!%s", COLOR_GREEN, COLOR_RESET);
                    wprintf(L"%s\n", temp_message);
                    player->hits++;
                    fleet_register_hit(enemy_fleet, x, y, &result, enemy_board);
                    if (result == SHOT_KILL) {
                        player->kills++;
                    }
                    break;
                case SHOT_KILL:
                    player->hits++;
                    player->kills++;
                    fleet_register_hit(enemy_fleet, x, y, &result, enemy_board);
                    break;
                case SHOT_INVALID:
                    wprintf(L"%sСюда уже стреляли! Попробуйте другие координаты.%s\n",
                           COLOR_RED, COLOR_RESET);
                    break;
            }
        } else {
            wprintf(L"%sНеверные координаты! Буква А-К, цифра 1-10.%s\n",
                   COLOR_RED, COLOR_RESET);
        }
    }
    
    wcscpy(last_message, temp_message);

    if (fleet_is_destroyed(enemy_fleet)) {
        wprintf(L"\n%s%s ПОБЕДИЛ!%s\n", COLOR_GREEN, player->name, COLOR_RESET);
        set_game_state(STATE_GAME_OVER);
    } else if (result == SHOT_HIT || result == SHOT_KILL) {
        wprintf(L"%sДополнительный ход!%s\n", COLOR_GREEN, COLOR_RESET);
        last_message[0] = L'\0';
    } else {
        set_game_state(STATE_COMPUTER_TURN);
    }
}

// Ход компьютера
static void computer_turn(Player* computer, Board* player_board, Fleet* player_fleet) {
    static int last_hit_x = -1, last_hit_y = -1;
    static int first_hit_x = -1, first_hit_y = -1;
    static int hunt_mode = 0;
    static int direction_x = 0, direction_y = 0;
    static int ship_parts_hit = 0;
    static wchar_t last_message[256] = L"";

    int x, y;
    GameShotResult result = SHOT_INVALID;
    int attempts = 0;
    wchar_t temp_message[256] = L"";

    const int dx[] = {1, -1, 0, 0};
    const int dy[] = {0, 0, 1, -1};

    board_print_player_only(player_board, last_message);

    while (result == SHOT_INVALID && attempts < 200) {
        attempts++;

        if (hunt_mode && first_hit_x >= 0) {
            bool found_valid = false;
            
            if (direction_x == 0 && direction_y == 0) {
                for (int i = 0; i < 4; i++) {
                    x = first_hit_x + dx[i];
                    y = first_hit_y + dy[i];
                    
                    if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                        if (player_board->visible[y][x] != CELL_MISS &&
                            player_board->visible[y][x] != CELL_HIT &&
                            player_board->visible[y][x] != CELL_DEAD) {
                            direction_x = dx[i];
                            direction_y = dy[i];
                            found_valid = true;
                            break;
                        }
                    }
                }
            } else {
                x = last_hit_x + direction_x;
                y = last_hit_y + direction_y;
                
                if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                    if (player_board->visible[y][x] != CELL_MISS &&
                        player_board->visible[y][x] != CELL_HIT &&
                        player_board->visible[y][x] != CELL_DEAD) {
                        found_valid = true;
                    }
                }
                
                if (!found_valid) {
                    int opp_x = first_hit_x - direction_x;
                    int opp_y = first_hit_y - direction_y;
                    
                    if (opp_x >= 0 && opp_x < BOARD_SIZE && opp_y >= 0 && opp_y < BOARD_SIZE) {
                        if (player_board->visible[opp_y][opp_x] != CELL_MISS &&
                            player_board->visible[opp_y][opp_x] != CELL_HIT &&
                            player_board->visible[opp_y][opp_x] != CELL_DEAD) {
                            x = opp_x;
                            y = opp_y;
                            found_valid = true;
                            direction_x = -direction_x;
                            direction_y = -direction_y;
                        }
                    }
                }
            }
            
            if (!found_valid) {
                hunt_mode = 0;
                direction_x = 0;
                direction_y = 0;
                first_hit_x = -1;
                first_hit_y = -1;
                last_hit_x = -1;
                last_hit_y = -1;
                ship_parts_hit = 0;
                continue;
            }
        } else {
            x = rand() % BOARD_SIZE;
            y = rand() % BOARD_SIZE;
        }
        
        if (player_board->visible[y][x] == CELL_MISS ||
            player_board->visible[y][x] == CELL_HIT ||
            player_board->visible[y][x] == CELL_DEAD) {
            result = SHOT_INVALID;
            continue;
        }
        
        int temp_parts = player_fleet->total_parts - player_fleet->destroyed_parts;
        result = board_shoot(player_board, x, y, &temp_parts);
    }

    if (attempts >= 200) {
        wprintf(L"Компьютер не может найти цель...\n");
        set_game_state(STATE_PLAYER_TURN);
        return;
    }

    wprintf(L"Компьютер стреляет в %lc%d - ", index_to_letter(x), y + 1);

    switch (result) {
        case SHOT_MISS:
            swprintf(temp_message, 256, L"%sМимо!%s", COLOR_WHITE, COLOR_RESET);
            wprintf(L"%s\n", temp_message);
            computer->misses++;
            
            if (hunt_mode && direction_x != 0) {
                direction_x = -direction_x;
                direction_y = -direction_y;
                last_hit_x = first_hit_x;
                last_hit_y = first_hit_y;
            } else if (hunt_mode) {
                hunt_mode = 0;
                direction_x = 0;
                direction_y = 0;
                first_hit_x = -1;
                first_hit_y = -1;
                last_hit_x = -1;
                last_hit_y = -1;
                ship_parts_hit = 0;
            }
            
            set_game_state(STATE_PLAYER_TURN);
            break;

        case SHOT_HIT:
            swprintf(temp_message, 256, L"%sПопадание в ваш корабль!%s", COLOR_RED, COLOR_RESET);
            wprintf(L"%s\n", temp_message);
            computer->hits++;
            
            if (!hunt_mode) {
                first_hit_x = x;
                first_hit_y = y;
                last_hit_x = x;
                last_hit_y = y;
                hunt_mode = 1;
                direction_x = 0;
                direction_y = 0;
                ship_parts_hit = 1;
            } else {
                ship_parts_hit++;
                last_hit_x = x;
                last_hit_y = y;
            }
            
            fleet_register_hit(player_fleet, x, y, &result, player_board);
            if (result == SHOT_KILL) {
                computer->kills++;
            }
            wprintf(L"%sКомпьютер продолжает атаку...%s\n", COLOR_RED, COLOR_RESET);
            break;

        case SHOT_KILL:
            swprintf(temp_message, 256, L"%sВаш корабль уничтожен!%s", COLOR_RED, COLOR_RESET);
            wprintf(L"%s\n", temp_message);
            computer->hits++;
            computer->kills++;
            
            fleet_register_hit(player_fleet, x, y, &result, player_board);
            
            hunt_mode = 0;
            direction_x = 0;
            direction_y = 0;
            first_hit_x = -1;
            first_hit_y = -1;
            last_hit_x = -1;
            last_hit_y = -1;
            ship_parts_hit = 0;
            
            wprintf(L"%sКомпьютер продолжает атаку...%s\n", COLOR_RED, COLOR_RESET);
            break;

        default:
            break;
    }
    
    if (result != SHOT_HIT && result != SHOT_KILL) {
        wcscpy(last_message, temp_message);
    } else {
        last_message[0] = L'\0';
    }

    if (fleet_is_destroyed(player_fleet)) {
        wprintf(L"\n%sКОМПЬЮТЕР ПОБЕДИЛ!%s\n", COLOR_RED, COLOR_RESET);
        set_game_state(STATE_GAME_OVER);
	return;
    }
}

// Главный игровой цикл
void game_loop(void) {
    Player player = {.name = L"Игрок", .score = 0, .is_computer = false, .hits = 0, .misses = 0, .kills = 0};
    Player computer = {.name = L"Компьютер", .score = 0, .is_computer = true, .hits = 0, .misses = 0, .kills = 0};

    Board player_board;
    Board computer_board;
    board_init(&player_board);
    board_init(&computer_board);

    Fleet player_fleet;
    Fleet computer_fleet;
    fleet_init_player(&player_fleet);

    wprintf(L"\n%sИнформация о размерах структур:%s\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"  Board: %lu байт\n", (unsigned long)board_get_size());
    wprintf(L"  Fleet: %lu байт\n", (unsigned long)sizeof(Fleet));
    wprintf(L"  Player: %lu байт\n\n", (unsigned long)sizeof(Player));

    player_place_ships(&player, &player_board, &player_fleet);
    wprintf(L"\n%sКомпьютер расставляет свои корабли...%s\n", COLOR_YELLOW, COLOR_RESET);
    fleet_init_computer(&computer_fleet, &computer_board);

    wprintf(L"\n%sНАЧАЛО БИТВЫ!%s\n", COLOR_YELLOW, COLOR_RESET);
    wprintf(L"%sПервыми ходите вы!%s\n", COLOR_GREEN, COLOR_RESET);
    set_game_state(STATE_PLAYER_TURN);

    while (get_game_state() != STATE_GAME_OVER && get_game_state() != STATE_EXIT) {
        switch (get_game_state()) {
            case STATE_PLAYER_TURN:
                player_turn(&player, &computer_board, &computer_fleet, &player_board);
                break;

            case STATE_COMPUTER_TURN:
                computer_turn(&computer, &player_board, &player_fleet);
                break;

            case STATE_GAME_OVER:
                print_statistics(&player, &computer);
                
                wprintf(L"\n%sХотите сыграть ещё? (д/н): %s", 
                       COLOR_YELLOW, COLOR_RESET);
                wchar_t answer[10];
                wscanf(L" %ls", answer);
                if (answer[0] == L'д' || answer[0] == L'Д' || 
                    answer[0] == L'y' || answer[0] == L'Y') {
                    return;
                } else {
                    set_game_state(STATE_EXIT);
                }
                break;

            default:
                break;
        }
    }
}
