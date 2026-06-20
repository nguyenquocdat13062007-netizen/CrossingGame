// ================================================================
// main.cpp - Dieu phoi chinh + SubThread
//
// Theo yeu cau do an:
//   - Bien toan cuc: IS_RUNNING, MOVING, cg (CGAME)
//   - SubThread() chay song song voi main()
//   - main(): xu ly phim, goi startGame(), exitGame(), pauseGame()...
//   - isImpact(getVehicle()), isImpact(getAnimal()) trong SubThread
//   - Tich hop dual-input: KeyPressed (1 lan) + isKeyPressed (giu phim)
// ================================================================

// ================================================================
// FIX: Include windows.h TRUOC moi header khac
//   - HANDLE: kieu du lieu Windows cho thread handle
//   - Sleep(): ham ngu cua Windows (khac std::this_thread::sleep_for)
//   - SuspendThread/ResumeThread: dung trong pauseGame/resumeGame
//   - WIN32_LEAN_AND_MEAN: giam bo thu vien Windows duoc include,
//     tranh conflict voi SFML (min/max macro)
// ================================================================
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CGame.h"
#include <SFML/Graphics.hpp>
#include <thread>

// ================================================================
// BIEN TOAN CUC (dinh nghia trong CGame.cpp, extern trong Utils.h)
// IS_RUNNING va MOVING duoc dung chung giua main va SubThread
// ================================================================
// IS_RUNNING va MOVING da duoc dinh nghia trong CGame.cpp

// ================================================================
// OBJECT GAME TOAN CUC - SubThread truy cap truc tiep
// ================================================================
CGAME cg; 

// ================================================================
// CONG THUC THOI GIAN
// ================================================================
const float UPDATE_INTERVAL = 0.10f;   // cap nhat xe/thu/den moi 100ms
const float MOVE_HOLD_DELAY = 0.35f;   // giu >= 350ms moi tinh la "giu phim"
const float MOVE_HOLD_INTERVAL = 0.15f;  // toc do lap lai khi giu (150ms/buoc)

// ================================================================
// SUBTHREAD - Chay song song voi main()
//
// Theo yeu cau do an (SubThread):
//   - Cap nhat vi tri nguoi theo MOVING
//   - Cap nhat xe (updatePosVehicle), thu (updatePosAnimal)
//   - Ve man hinh (drawGame)
//   - Kiem tra: isImpact(getVehicle()), isImpact(getAnimal()), isFinish()
//   - Sleep(100) -> ~10 FPS logic
//
// SFML: window & font phai truyen tu ngoai vao vi
//       SFML object khong thread-safe -> de main quan ly render
//       SubThread chi cap nhat logic, main render
// ================================================================
RenderWindow* g_window = nullptr;
Font* g_font = nullptr;

void SubThread() {
    
}

// ================================================================
// MAIN
// ================================================================
int main() {
    
}
