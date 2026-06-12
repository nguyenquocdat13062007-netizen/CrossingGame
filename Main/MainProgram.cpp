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
    while (true) {
        cg.showMenu();
        ShowCursor();
        char ch = _getch();

        if (ch == '1') return 1; // New Game
        if (ch == '2') return 2; // Load Game
        if (ch == '3') return 3; // Settings (chua lam)
        if (ch == '4') return 4; // Quit
        // Phim khac -> hien lai menu
    }
}

// ============================================================
// XU LY LOAD GAME TU MENU
// ============================================================
bool HandleLoadFromMenu(CGAME& cg) {
    system("cls");
    ShowCursor();
    GotoXY(10, 10);
    SetColor(COLOR_TITLE);
    std::cout << "Nhap duong dan file save (vd: save.dat): ";
    ResetColor();

    std::string path;
    std::getline(std::cin, path);

    if (cg.loadGame(path)) {
        GotoXY(10, 12);
        SetColor(10);
        std::cout << "Tai game thanh cong! Nhan phim bat ky...";
        ResetColor();
        _getch();
        return true;
    }
    else {
        GotoXY(10, 12);
        SetColor(12);
        std::cout << "Khong the doc file: " << path << ". Nhan phim bat ky...";
        ResetColor();
        _getch();
        return false;
    }
}

// ============================================================
// GAME LOOP CHINH
// ============================================================
void RunGame(CGAME& cg) {
    IS_RUNNING = true;
    MOVING = ' ';
    HideCursor();

    // Tao thread phu chay game loop
    std::thread t1(SubThread);

    while (true) {
        char temp = (char)toupper(_getch());

        if (!cg.getPeople().isDead()) {
            // === NGUOI DANG SONG ===
            if (temp == 27) {
                // ESC: thoat
                cg.exitGame((HANDLE)t1.native_handle());
                t1.join();
                return;
            }
            else if (temp == 'P') {
                // Tam dung
                cg.pauseGame((HANDLE)t1.native_handle());
                cg.showPauseMsg();
                // Doi P de tiep tuc
                while (_getch() != 'p' && _getch() != 'P') {}
                cg.resumeGame((HANDLE)t1.native_handle());
            }
            else if (temp == 'L') {
                // Save game
                cg.pauseGame((HANDLE)t1.native_handle());
                ShowCursor();
                GotoXY(2, SCREEN_HEIGHT - 2);
                SetColor(COLOR_TITLE);
                std::cout << "Nhap ten file luu (vd: save.dat): ";
                ResetColor();
                std::string path;
                std::getline(std::cin, path);
                if (cg.saveGame(path)) {
                    GotoXY(2, SCREEN_HEIGHT - 2);
                    SetColor(10);
                    std::cout << "Da luu! Nhan phim bat ky de tiep tuc...   ";
                    ResetColor();
                }
                else {
                    GotoXY(2, SCREEN_HEIGHT - 2);
                    SetColor(12);
                    std::cout << "Luu that bai! Nhan phim bat ky...         ";
                    ResetColor();
                }
                _getch();
                HideCursor();
                ClearPos(0, SCREEN_HEIGHT - 2, SCREEN_WIDTH);
                cg.resumeGame((HANDLE)t1.native_handle());
            }
            else if (temp == 'T') {
                // Load game
                cg.pauseGame((HANDLE)t1.native_handle());
                ShowCursor();
                GotoXY(2, SCREEN_HEIGHT - 2);
                SetColor(COLOR_TITLE);
                std::cout << "Nhap duong dan file save: ";
                ResetColor();
                std::string path;
                std::getline(std::cin, path);
                if (cg.loadGame(path)) {
                    GotoXY(2, SCREEN_HEIGHT - 2);
                    SetColor(10);
                    std::cout << "Tai game thanh cong! Tiep tuc...         ";
                    ResetColor();
                    Sleep(1000);
                }
                else {
                    GotoXY(2, SCREEN_HEIGHT - 2);
                    SetColor(12);
                    std::cout << "Khong tim thay file!                     ";
                    ResetColor();
                    Sleep(1000);
                }
                HideCursor();
                ClearPos(0, SCREEN_HEIGHT - 2, SCREEN_WIDTH);
                cg.resumeGame((HANDLE)t1.native_handle());
            }
            else if (temp == 'W' || temp == 'A' || temp == 'S' || temp == 'D') {
                // Di chuyen nguoi
                cg.resumeGame((HANDLE)t1.native_handle()); // Dam bao dang chay
                MOVING = temp;
            }
            // Kiem tra qua duong (finish) - xu ly o day
            if (cg.getPeople().isFinished()) {
                if (cg.getLevel() >= MAX_LEVEL) {
                    // Win game
                    cg.exitGame((HANDLE)t1.native_handle());
                    t1.join();
                    cg.showWin();
                    ShowCursor();
                    _getch();
                    return;
                }
                else {
                    // QUAN TRONG: pause SubThread truoc khi cls/redraw
                    // de tranh 2 thread cung ve -> man hinh vo
                    cg.pauseGame((HANDLE)t1.native_handle());
                    cg.nextLevel();
                    cg.resumeGame((HANDLE)t1.native_handle());
                }
            }
        }
        else {
            // === NGUOI DA CHET ===
            cg.showDeadMsg();
            if (temp == 'Y') {
                // Choi lai
                if (cg.getmLives() <= 0) {
                    // Het mang -> game over
                    cg.exitGame((HANDLE)t1.native_handle());
                    t1.join();
                    system("cls");
                    GotoXY(SCREEN_WIDTH / 2 - 10, SCREEN_HEIGHT / 2);
                    SetColor(12);
                    std::cout << "  GAME OVER! Final Score: " << cg.getScore();
                    ResetColor();
                    ShowCursor();
                    _getch();
                    return;
                }
                cg.pauseGame((HANDLE)t1.native_handle());
                cg.resetGame();
                cg.resumeGame((HANDLE)t1.native_handle());
            }
            else {
                // Thoat
                cg.exitGame((HANDLE)t1.native_handle());
                t1.join();
                return;
            }
        }
    }
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
            if (HandleLoadFromMenu(cg)) {
                system("cls");
                cg.DrawBorder();
                cg.DrawRoad();
                cg.drawGame();
                RunGame(cg);
            }
            // Neu load that bai -> quay lai menu
        }
        else if (choice == 3) {
            // Settings (chua cai dat)
            system("cls");
            GotoXY(SCREEN_WIDTH / 2 - 12, SCREEN_HEIGHT / 2);
            SetColor(7);
            std::cout << "Settings chua duoc cai dat. Nhan phim bat ky...";
            ResetColor();
            ShowCursor();
            _getch();
        }
        else if (choice == 4) {
            // Thoat
            system("cls");
            GotoXY(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2);
            SetColor(COLOR_TITLE);
            std::cout << "Cam on ban da choi! Tam biet!";
            ResetColor();
            Sleep(1000);
            break;
        }
    }

    ShowCursor();
    return 0;
}
