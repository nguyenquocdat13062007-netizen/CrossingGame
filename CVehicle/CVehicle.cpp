#include "CVehicle.h"
#include <iostream>
using namespace std;
// =============================================
// CVEHICLE
// =============================================
CVEHICLE::CVEHICLE()
    : mX(0), mY(0), mSpeed(1), mDirection(1),
    mWidth(3), mColor(COLOR_CAR),
    mStopped(false), mStopTimer(0), mStopDuration(0) {
}

CVEHICLE::CVEHICLE(int x, int y, int speed, int dir, int color, int width)
    : mX(x), mY(y), mSpeed(speed), mDirection(dir),
    mWidth(width), mColor(color),
    mStopped(false), mStopTimer(0), mStopDuration(0) {
}

void CVEHICLE::Move(int screenWidth, int screenHeight) {
   
}

void CVEHICLE::Draw() {
    
}

void CVEHICLE::Clear() {
   
}

std::string CVEHICLE::GetShape() const {
    
}

void CVEHICLE::Stop(int duration_ms) {
   
}

void CVEHICLE::Resume() { }

void CVEHICLE::UpdateStopStatus() {
   
}

// =============================================
// CCAR
// =============================================
CCAR::CCAR() : CVEHICLE(0, 5, 1, 1, COLOR_CAR, 4) {}

CCAR::CCAR(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_CAR, 4) {
}

void CCAR::Draw() {
   
}

void CCAR::Clear() {  }

std::string CCAR::GetShape() const {
  
}

// =============================================
// CTRUCK
// =============================================
CTRUCK::CTRUCK() : CVEHICLE(0, 7, 1, -1, COLOR_TRUCK, 7) {}

CTRUCK::CTRUCK(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_TRUCK, 7) {
}

void CTRUCK::Draw() {
  
}

void CTRUCK::Clear() { }

std::string CTRUCK::GetShape() const {
    
}

// =============================================
// CTRAFFICLIGHT
// =============================================
CTRAFFICLIGHT::CTRAFFICLIGHT(int lane, int greenMs, int redMs)
    : mLane(lane), mState(GREEN),
    mTimer(GetCurrentTimeMs()),
    mGreenDuration(greenMs), mRedDuration(redMs) {
}

void CTRAFFICLIGHT::Update() {
   
}

void CTRAFFICLIGHT::Draw(int x, int y) {
   
}
