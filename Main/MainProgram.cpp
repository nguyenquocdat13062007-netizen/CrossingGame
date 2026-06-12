// ============================================================
// CROSSING GAME - Bang qua duong
// Mon: Lap trinh Huong doi tuong - HCMUS
// Build: g++ -o CrossingGame main.cpp src/*.cpp -I include -lwinmm
// ============================================================
#include "Utils.h"
#include "CGame.h"
#include <conio.h>
#include <thread>
#include <iostream>
#include <string>
using namespace std;
// Bien toan cuc da khai bao trong Utils.cpp
// extern volatile bool IS_RUNNING;
// extern volatile char MOVING;

// ============================================================
// XU LY MENU CHINH
// Tra ve: 1=New, 2=Load, 3=Settings, 4=Quit
// ============================================================
int HandleMenu(CGAME& cg) {
    
}

// ============================================================
// XU LY LOAD GAME TU MENU
// ============================================================
bool HandleLoadFromMenu(CGAME& cg) {
   
}

// ============================================================
// GAME LOOP CHINH
// ============================================================
void RunGame(CGAME& cg) {

}

// ============================================================
// MAIN
// ============================================================
int main() {
    // Cai dat cua so console
    FixConsoleWindow();
    SetConsoleSize(SCREEN_WIDTH, SCREEN_HEIGHT);
    SetConsoleTitle(TEXT("Crossing Game - HCMUS OOP"));
    HideCursor();

    CGAME cg;

    // Vong lap menu chinh
    while (true) {
        int choice = HandleMenu(cg);

        if (choice == 1) {
            // New Game
            cg.startGame();
            RunGame(cg);
        }
        else if (choice == 2) {
            // Load Game
           
            }
            // Neu load that bai -> quay lai menu
        }
        else if (choice == 3) {
            // Settings (chua cai dat)
          
        }
        else if (choice == 4) {
            // Thoat
           
        }
    }

    ShowCursor();
    return 0;
}
