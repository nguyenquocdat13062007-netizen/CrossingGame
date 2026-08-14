#include "CAnimal.h"

// ================================================================
// CANIMAL
// ================================================================
CANIMAL::CANIMAL()
    : mX(0), mY(10), mSpeed(1), mDirection(1),
    mWidth(2), mColor(COLOR_BIRD), mAnim(0.2f) {
}

CANIMAL::CANIMAL(int x, int y, int speed, int dir, Color color, int width)
    : mX(x), mY(y), mSpeed(speed), mDirection(dir),
    mWidth(width), mColor(color), mAnim(0.2f) {
}

void CANIMAL::loadAssets(const std::string& frame1,
    const std::string& frame2) {
    vector<string> files;
    if (!frame1.empty()) files.push_back(frame1);
    if (!frame2.empty()) files.push_back(frame2);
    mAnim.loadAssets(files);
}

void CANIMAL::Move(int screenWidthCells) {
	
}

void CANIMAL::Draw(RenderWindow& window) {

}

// ================================================================
// CBIRD - Chim: nho, nhanh, 2 frame vo canh
// ================================================================
CBIRD::CBIRD() : CANIMAL(0, 12, 2, 1, COLOR_BIRD, 2) {}

CBIRD::CBIRD(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_BIRD, 2) {
}

void CBIRD::Tell() {
    // Mo rong am thanh sau
}

// ================================================================
// CROCK - Da lan: to, cham, thay the CDINOSAUR
// Width = 3 o luoi (giong CDINOSAUR cu)
// ================================================================
CROCK::CROCK() : CANIMAL(0, 15, 1, -1, COLOR_DINO, 3) {}

CROCK::CROCK(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_DINO, 3) {
}

void CROCK::Tell() {
    // Mo rong am thanh sau
}
