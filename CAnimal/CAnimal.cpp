#include "CAnimal.h"
#include <iostream>
using namespace std;
// =============================================
// CANIMAL
// =============================================
CANIMAL::CANIMAL()
    : mX(0), mY(10), mSpeed(1), mDirection(1),
    mWidth(4), mColor(COLOR_BIRD), mLastSoundTime(0) {
}

CANIMAL::CANIMAL(int x, int y, int speed, int dir, int color, int width)
    : mX(x), mY(y), mSpeed(speed), mDirection(dir),
    mWidth(width), mColor(color), mLastSoundTime(0) {
}

void CANIMAL::Move(int screenWidth, int screenHeight) {
   
}

void CANIMAL::Draw() {
   
}

void CANIMAL::Clear() {
   
}

std::string CANIMAL::GetShape() const {
    
}

// =============================================
// CBIRD
// =============================================
CBIRD::CBIRD() : CANIMAL(0, 12, 2, 1, COLOR_BIRD, 4) {}

CBIRD::CBIRD(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_BIRD, 4) {
}

void CBIRD::Draw() {
    if (mX + mWidth < 0 || mX >= SCREEN_WIDTH) return;
    GotoXY(mX, mY);
    SetColor(mColor);
    std::cout << GetShape();
    ResetColor();
}

void CBIRD::Clear() {  }

string CBIRD::GetShape() const {
    
}

void CBIRD::Tell() {
    
}

// =============================================
// CDINOSAUR
// =============================================
CDINOSAUR::CDINOSAUR() : CANIMAL(0, 15, 1, -1, COLOR_DINO, 5) {}

CDINOSAUR::CDINOSAUR(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_DINO, 5) {
}

void CDINOSAUR::Draw() {
   
}

void CDINOSAUR::Clear() {  }

string CDINOSAUR::GetShape() const {
}

void CDINOSAUR::Tell() {
   
}