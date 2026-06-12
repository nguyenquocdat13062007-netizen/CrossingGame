// ============================================================
// Menu.cpp — Implement cac man hinh thong bao cua CGAME
// showMenu, showPauseMsg, showDeadMsg, showLevelUp, showWin
// ============================================================
#include "CGame.h"
#include <iostream>

// ============================================================
// MENU CHINH
// ============================================================
void CGAME::showMenu() {
    system("cls");
    int cx = SCREEN_WIDTH / 2;

    SetColor(COLOR_TITLE);
    GotoXY(cx - 12, 4);  std::cout << "=========================";
    GotoXY(cx - 12, 5);  std::cout << "||   BANG QUA DUONG    ||";
    GotoXY(cx - 12, 6);  std::cout << "=========================";

    SetColor(COLOR_INFO);
    GotoXY(cx - 8, 10); std::cout << "1. New Game";
    GotoXY(cx - 8, 12); std::cout << "2. Load Game";
    GotoXY(cx - 8, 14); std::cout << "3. Settings";
    GotoXY(cx - 8, 16); std::cout << "4. Thoat";

    SetColor(COLOR_DEFAULT);
    GotoXY(cx - 8, 19); std::cout << "Chon (1-4): ";
    ResetColor();
}

// ============================================================
// THONG BAO TAM DUNG
// ============================================================
void CGAME::showPauseMsg() {
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;

    SetColor(COLOR_TITLE);
    GotoXY(cx - 15, cy);     std::cout << "============================";
    GotoXY(cx - 15, cy + 1); std::cout << "||      PAUSED          ||";
    GotoXY(cx - 15, cy + 2); std::cout << "||  Nhan P de tiep tuc   ||";
    GotoXY(cx - 15, cy + 3); std::cout << "============================";
    ResetColor();
}

// ============================================================
// THONG BAO CHET
// ============================================================
void CGAME::showDeadMsg() {
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;

    SetColor(12); // Do
    GotoXY(cx - 15, cy - 1); std::cout << "============================";
    GotoXY(cx - 15, cy);     std::cout << "||   BAN DA CHET!       ||";

    SetColor(COLOR_INFO);
    GotoXY(cx - 15, cy + 1);
    std::cout << "|| Mang con lai: " << mLives << "          ||";

    GotoXY(cx - 15, cy + 2);
    if (mLives > 0)
        std::cout << "|| [Y] Choi lai  [Khac] Thoat||";
    else
        std::cout << "|| Het mang! [Y] Game Over   ||";

    SetColor(12);
    GotoXY(cx - 15, cy + 3); std::cout << "============================";
    ResetColor();
}

// ============================================================
// THONG BAO LEN CAP
// ============================================================
void CGAME::showLevelUp() {
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;

    SetColor(10); // Xanh la
    GotoXY(cx - 13, cy);
    std::cout << "  *** LEVEL UP! --> Level " << mLevel << " ***";
    ResetColor();

    Sleep(1200);

    // Xoa thong bao
    GotoXY(cx - 13, cy);
    std::cout << "                              ";
}

// ============================================================
// THONG BAO THANG GAME (qua het MAX_LEVEL)
// ============================================================
void CGAME::showWin() {
    system("cls");
    int cx = SCREEN_WIDTH / 2;
    int cy = SCREEN_HEIGHT / 2;

    SetColor(COLOR_TITLE);
    GotoXY(cx - 16, cy - 2); std::cout << "================================";
    GotoXY(cx - 16, cy - 1); std::cout << "||  CHUC MUNG! BAN DA THANG!  ||";

    SetColor(COLOR_INFO);
    GotoXY(cx - 16, cy);
    std::cout << "||  Diem cuoi cung: " << mScore << "          ||";

    GotoXY(cx - 16, cy + 1); std::cout << "||  Nhan phim bat ky de thoat ||";

    SetColor(COLOR_TITLE);
    GotoXY(cx - 16, cy + 2); std::cout << "================================";
    ResetColor();
}