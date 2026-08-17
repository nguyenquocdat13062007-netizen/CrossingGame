#define NOMINMAX // Prevent Windows.h min/max macros from conflicting with std::min / std::max

#include "CPeople.h"
#include <algorithm> // Required for std::min and std::max

using namespace std;
using namespace sf;

// ================================================================
// CPEOPLE Implementation
// ================================================================

CPEOPLE::CPEOPLE()
    : mX(SCREEN_WIDTH / 2), mY(START_Y),
      mAlive(true), mFinished(false),
      mDir(DIR_UP),
      mAnimUp(0.12f), mAnimLeft(0.12f), mAnimRight(0.12f) {
    loadAssets();
}

CPEOPLE::CPEOPLE(int startX, int startY)
    : mX(startX), mY(startY),
      mAlive(true), mFinished(false),
      mDir(DIR_UP),
      mAnimUp(0.12f), mAnimLeft(0.12f), mAnimRight(0.12f) {
    loadAssets();
}

void CPEOPLE::loadAssets() {
    // Load 6 vertical running frames
    vector<string> upFrames = {
        "Assets/images/player/run-up-1.png",
        "Assets/images/player/run-up-2.png",
        "Assets/images/player/run-up-3.png",
        "Assets/images/player/run-up-4.png",
        "Assets/images/player/run-up-5.png",
        "Assets/images/player/run-up-6.png"
    };
    mAnimUp.loadAssets(upFrames);

    // Load lateral sprites
    mAnimLeft.loadAssets({"Assets/images/player/run-left.png"});
    mAnimRight.loadAssets({"Assets/images/player/run-right.png"});
}

void CPEOPLE::updateAnim(float dt) {
    if (!mAlive) return;

    if (mDir == DIR_LEFT)        mAnimLeft.update(dt);
    else if (mDir == DIR_RIGHT)  mAnimRight.update(dt);
    else                         mAnimUp.update(dt);
}

// ----------------------------------------------------------------
// Directional Movement & Step Animation Progression
// ----------------------------------------------------------------

void CPEOPLE::Up(int step) {
    if (!mAlive || mFinished) return;
    mY = max(0, mY - step); // Upper screen boundary
    mDir = DIR_UP;
    mAnimUp.nextFrame();    // Step forward in 6-frame run sequence
    checkFinish();
}

void CPEOPLE::Down(int step) {
    if (!mAlive || mFinished) return;
    mY = min(START_Y, mY + step); // Bottom screen boundary
    mDir = DIR_DOWN;
    mAnimUp.nextFrame();          // Step backward using run sequence
}

void CPEOPLE::Left(int step) {
    if (!mAlive || mFinished) return;
    mX = max(0, mX - step); // Left screen boundary
    mDir = DIR_LEFT;
    mAnimLeft.nextFrame();
}

void CPEOPLE::Right(int step) {
    if (!mAlive || mFinished) return;
    mX = min(SCREEN_WIDTH - 1, mX + step); // Right screen boundary
    mDir = DIR_RIGHT;
    mAnimRight.nextFrame();
}

// ----------------------------------------------------------------
// Rendering Active Direction Sprite
// ----------------------------------------------------------------

void CPEOPLE::Draw(RenderWindow& window, Font& font) {
    float px = CellToPixel(mX);
    float py = CellToPixel(mY);

    // Select active animation based on movement direction
    AnimatedSprite* activeAnim = &mAnimUp;
    if (mDir == DIR_LEFT)       activeAnim = &mAnimLeft;
    else if (mDir == DIR_RIGHT) activeAnim = &mAnimRight;

    if (activeAnim->isLoaded()) {
        activeAnim->draw(window, px, py, 1, 1, false);
    }
    else {
        // Fallback text "Y" if images fail to load
        Text text(font);
        text.setString("Y");
        text.setCharacterSize(18);
        text.setFillColor(mAlive ? COLOR_PLAYER : Color::Red);
        text.setPosition(Vector2f(px + 6.f, py + 2.f));
        window.draw(text);
    }
}

// ----------------------------------------------------------------
// Collisions & State Logic
// ----------------------------------------------------------------

bool CPEOPLE::isImpact(const CVEHICLE* v) const {
    if (!v || !mAlive) return false;
    if (v->isStopped()) return false; // Safe when vehicle is stopped at traffic light
    return v->isImpact(mX, mY);
}

bool CPEOPLE::isImpact(const CANIMAL* a) const {
    if (!a || !mAlive) return false;
    if (mY != a->getY()) return false;
    return (mX >= a->getX() && mX < a->getX() + a->getWidth());
}

bool CPEOPLE::checkFinish() {
    if (mY <= FINISH_Y) {
        mFinished = true;
        return true;
    }
    return false;
}

void CPEOPLE::Reset(int startX, int startY) {
    mX = startX;
    mY = startY;
    mAlive = true;
    mFinished = false;
    mDir = DIR_UP;

    mAnimUp.reset();
    mAnimLeft.reset();
    mAnimRight.reset();
}

void CPEOPLE::Die() {
    mAlive = false;
}
