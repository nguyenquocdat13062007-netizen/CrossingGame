#include "CVehicle.h"
using namespace std;
using namespace sf;

// ================================================================
// CVEHICLE
// ================================================================
CVEHICLE::CVEHICLE()
    : mX(0), mY(0), mSpeed(1), mDirection(1),
    mWidth(3), mColor(COLOR_CAR),
    mStopped(false), mStopDurationMs(0),
    mAnim(0.18f) {
}

CVEHICLE::CVEHICLE(int x, int y, int speed, int dir, Color color, int width)
    : mX(x), mY(y), mSpeed(speed), mDirection(dir),
    mWidth(width), mColor(color),
    mStopped(false), mStopDurationMs(0),
    mAnim(0.18f) {
}

void CVEHICLE::loadAssets(const std::string& frame1,
   
}

void CVEHICLE::Move(int screenWidthCells) {
    
}

void CVEHICLE::Draw(RenderWindow& window) {
    
}

void CVEHICLE::Stop(int duration_ms) {
    
}

void CVEHICLE::Resume() {
    
}

void CVEHICLE::UpdateStopStatus() {
    
}

// ================================================================
// CCAR - Xe hoi: width = 2 o luoi
// ================================================================
CCAR::CCAR() : CVEHICLE(0, 5, 1, 1, COLOR_CAR, 2) {}

CCAR::CCAR(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_CAR, 2) {
}

// ================================================================
// CTRUCK - Xe tai: width = 3 o luoi
// ================================================================
CTRUCK::CTRUCK() : CVEHICLE(0, 7, 1, -1, COLOR_TRUCK, 3) {}

CTRUCK::CTRUCK(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_TRUCK, 3) {
}

// ================================================================
// CTRAFFICLIGHT
// Fix SFML 3.0: mSprite la optional<Sprite>, phai dung -> thay vi .
// ================================================================
CTRAFFICLIGHT::CTRAFFICLIGHT(int laneY, int greenMs, int redMs)
    : mState(GREEN),
    mGreenDurationMs(greenMs),
    mRedDurationMs(redMs),
    mAssetsLoaded(false) {
    mClock.restart();
}

void CTRAFFICLIGHT::loadAssets(const std::string& stopImg,
    const std::string& goImg) {
    
}

void CTRAFFICLIGHT::Update() {
    
}

void CTRAFFICLIGHT::Draw(RenderWindow& window, int x, int y) {
    
   
}
