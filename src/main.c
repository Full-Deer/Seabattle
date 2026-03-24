#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <wchar.h>
#include <locale.h>
#include "game.h"

int main() {
    // Локаль для поддержки русского языка
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    srand(time(NULL));
    
    wchar_t choice[10];
    do {
        game_loop();
        
        wprintf(L"\nВыйти из программы? (д/н): ");
        wscanf(L" %ls", choice);
        
    } while (choice[0] != L'д' && choice[0] != L'Д' && 
             choice[0] != L'y' && choice[0] != L'Y');
    
    wprintf(L"Спасибо за игру! До свидания!\n");
    return 0;
}
