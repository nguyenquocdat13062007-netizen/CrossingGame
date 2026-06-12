#include "CGame.h"
#include <iostream>
#include <algorithm>
using namespace std;
// Bien toan cuc tro den object CGAME duy nhat — SubThread can truy cap
CGAME* g_pGame = nullptr;

// ============================================================
// SubThread — chay trong thread phu
// ============================================================
void SubThread() {
    while (IS_RUNNING) {
        if (g_pGame && !g_pGame->isPaused()) {

            if (g_pGame->getPeople().isAlive()) {
                g_pGame->updateTrafficLights();
                g_pGame->updatePosVehicle();
                g_pGame->updatePosAnimal();

                g_pGame->updatePosPeople(MOVING);
                MOVING = ' ';

                g_pGame->drawGame();

                g_pGame->checkCollision();
                g_pGame->checkFinish();
            }
        }
        Sleep(100); // ~10 FPS
    }
}

// ============================================================
// CONSTRUCTOR & DESTRUCTOR
// ============================================================
CGAME::CGAME()
    : mLevel(1), mScore(0), mLives(3), mPaused(false),
    mNumTrucks(0), mNumCars(0), mNumDinos(0), mNumBirds(0),
    mSubThread(nullptr)
{
   /*for (int i = 0; i < MAX_TRUCKS; i++) mTrucks[i] = nullptr;
    for (int i = 0; i < MAX_CARS; i++) mCars[i] = nullptr;
    for (int i = 0; i < MAX_DINOS; i++) mDinos[i] = nullptr;
    for (int i = 0; i < MAX_BIRDS; i++) mBirds[i] = nullptr;
    for (int i = 0; i < MAX_TRUCKS + MAX_CARS; i++) mLights[i] = nullptr;

    g_pGame = this;*/ // Tham khảo 
}

CGAME::~CGAME() {
   
}

// ============================================================
// GETTER
// ============================================================
int CGAME::getmLives() { }

// ============================================================
// TINH TOAN VI TRI LAN
// ============================================================
int CGAME::GetLaneY(int laneIndex) {
    
}

// ============================================================
// INIT LANES — Tao xe, thu, den theo cap do hien tai (mLevel)
// ============================================================
void CGAME::InitLanes() { // ---------------------------THAM KHẢO---------------------------
    // Xoa doi tuong cu
   /* for (int i = 0; i < MAX_TRUCKS; i++) { delete mTrucks[i]; mTrucks[i] = nullptr; }
    for (int i = 0; i < MAX_CARS; i++) { delete mCars[i];   mCars[i] = nullptr; }
    for (int i = 0; i < MAX_DINOS; i++) { delete mDinos[i];  mDinos[i] = nullptr; }
    for (int i = 0; i < MAX_BIRDS; i++) { delete mBirds[i];  mBirds[i] = nullptr; }
    for (int i = 0; i < MAX_TRUCKS + MAX_CARS; i++) {
		delete mLights[i]; mLights[i] = nullptr;
    }

    // So luong xe/thu tang theo level
    mNumTrucks = min(mLevel, MAX_TRUCKS);
    mNumCars = min(mLevel + 1, MAX_CARS);
    mNumDinos = min(mLevel / 2 + 1, MAX_DINOS);
    mNumBirds = min(mLevel, MAX_BIRDS);

    // Toc do tang theo level
    int baseSpeed = 1 + (mLevel - 1) / 2;

    // Xe tai — cac lan duoi
    for (int i = 0; i < mNumTrucks; i++) {
        int y = GetLaneY(i);
        int dir = (i % 2 == 0) ? 1 : -1;
        int x = (dir == 1) ? 0 : SCREEN_WIDTH - 7;
        mTrucks[i] = new CTRUCK(x, y, baseSpeed, dir);
        mLights[i] = new CTRAFFICLIGHT(y, 3000 - mLevel * 200, 2000);
    }

    // Xe hoi — cac lan giua
    for (int i = 0; i < mNumCars; i++) {
        int y = GetLaneY(mNumTrucks + i);
        int dir = (i % 2 == 0) ? -1 : 1;
        int x = (dir == 1) ? 0 : SCREEN_WIDTH - 4;
        mCars[i] = new CCAR(x, y, baseSpeed + 1, dir);
        mLights[mNumTrucks + i] = new CTRAFFICLIGHT(y, 2500 - mLevel * 150, 1500);
    }

    // Thu — cac lan tren
    for (int i = 0; i < mNumDinos; i++) {
        int y = GetLaneY(mNumTrucks + mNumCars + i);
        int dir = (i % 2 == 0) ? 1 : -1;
        int x = (dir == 1) ? 0 : SCREEN_WIDTH - 5;
        mDinos[i] = new CDINOSAUR(x, y, baseSpeed, dir);
    }
    for (int i = 0; i < mNumBirds; i++) {
        int y = GetLaneY(mNumTrucks + mNumCars + mNumDinos + i);
        int dir = (i % 2 == 0) ? -1 : 1;
        int x = (dir == 1) ? 0 : SCREEN_WIDTH - 4;
        mBirds[i] = new CBIRD(x, y, baseSpeed + 2, dir);
    }
}*/

// ============================================================
// INIT & START
// ============================================================
void CGAME::Init() {
   
}

void CGAME::startGame() {
   
}

// ============================================================
// VE MAN HINH
// ============================================================
void CGAME::DrawBorder() {
   
}

// ----------------------------------------------------------
// DrawRoad — SUA: them nen cham '.', vach ke lan, FINISH/START
// (truoc day chi ve 2 duong gach o ROAD_TOP/ROAD_BOTTOM -> man hinh trong)
// ----------------------------------------------------------
void CGAME::DrawRoad() {
   
}

void CGAME::drawGame() {
   
}

void CGAME::DrawStatus() {
  
}

// ============================================================
// CAP NHAT VI TRI
// ============================================================
void CGAME::updatePosPeople(char key) {
    
}

void CGAME::updatePosVehicle() {
    
}

void CGAME::updatePosAnimal() {
    
}

void CGAME::updateTrafficLights() {
   
}

// ============================================================
// KIEM TRA TRANG THAI
// ============================================================
bool CGAME::checkCollision() {
   
}

bool CGAME::checkFinish() {
   
}

// ============================================================
// RESET & NEXT LEVEL
// ============================================================
void CGAME::resetGame() {
    
}

void CGAME::nextLevel() {
   
}

// ============================================================
// PAUSE / RESUME / EXIT
// ============================================================
void CGAME::pauseGame(HANDLE hThread) {
  
}

void CGAME::resumeGame(HANDLE hThread) {
   
}

void CGAME::exitGame(HANDLE hThread) {

}

// ============================================================
// GETTER ARRAYS
// ============================================================
CVEHICLE** CGAME::getVehicles() {
    
}

CANIMAL** CGAME::getAnimals() {
   
}

int CGAME::getVehicleCount() const {}
int CGAME::getAnimalCount()  const {  }