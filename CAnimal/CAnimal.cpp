#include "CAnimal.h"

using namespace std;
using namespace sf;

// ================================================================
// CANIMAL Implementation
// ================================================================

CANIMAL::CANIMAL()
    : mX(0), mY(10), mSpeed(1), mDirection(1),
      mWidth(2), mColor(COLOR_BIRD), mAnim(0.2f) {
}

CANIMAL::CANIMAL(int x, int y, int speed, int dir, Color color, int width)
    : mX(x), mY(y), mSpeed(speed), mDirection(dir),
      mWidth(width), mColor(color), mAnim(0.2f) {
}

void CANIMAL::loadAssets(const vector<string>& files) {
    mAnim.loadAssets(files);
}

void CANIMAL::Move(int screenWidthCells) {
    mX += mDirection * mSpeed;
    mAnim.nextFrame(); // Advance animation frame on move

    // Screen wrap-around boundary logic
    if (mX >= screenWidthCells)        mX = -mWidth;          // Off right edge -> enter left edge
    else if (mX + mWidth <= 0)         mX = screenWidthCells; // Off left edge -> enter right edge
}

void CANIMAL::Draw(RenderWindow& window) {
    float px = CellToPixel(mX);
    float py = CellToPixel(mY);

    if (mAnim.isLoaded()) {
        bool flipX = (mDirection == -1); // Flip sprite when moving left
        mAnim.draw(window, px, py, mWidth, 1, flipX);
    }
    else {
        // Fallback shape if textures fail to load
        RectangleShape rect = MakeShape(mX, mY, mWidth, mColor);
        window.draw(rect);
    }
}

// ================================================================
// CBIRD Subclass (3-frame animation)
// ================================================================

CBIRD::CBIRD() 
    : CANIMAL(0, 12, 2, 1, COLOR_BIRD, 2) {
    loadAssets({
        "Assets/images/entities/bird1.png",
        "Assets/images/entities/bird2.png",
        "Assets/images/entities/bird3.png"
    });
}

CBIRD::CBIRD(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_BIRD, 2) {
    loadAssets({
        "Assets/images/entities/bird1.png",
        "Assets/images/entities/bird2.png",
        "Assets/images/entities/bird3.png"
    });
}

void CBIRD::Tell() {
    // Sound trigger hook
}

// ================================================================
// CROCK Subclass (3-frame animation)
// ================================================================

CROCK::CROCK() 
    : CANIMAL(0, 15, 1, -1, COLOR_DINO, 3) {
    loadAssets({
        "Assets/images/other/rock1.png",
        "Assets/images/other/rock2.png",
        "Assets/images/other/rock3.png"
    });
}

CROCK::CROCK(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_DINO, 3) {
    loadAssets({
        "Assets/images/other/rock1.png",
        "Assets/images/other/rock2.png",
        "Assets/images/other/rock3.png"
    });
}

void CROCK::Tell() {
    // Sound trigger hook
}
