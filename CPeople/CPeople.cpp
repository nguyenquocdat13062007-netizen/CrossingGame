#define NOMINMAX // Prevent Windows.h min/max macros from conflicting with std::min / std::max

#include "CPeople.h"
#include <algorithm>

using namespace std;
using namespace sf;

// ================================================================
// CPEOPLE Implementation
// ================================================================

CPEOPLE::CPEOPLE()
    : mX(SCREEN_WIDTH / 2), mY(START_Y),
      mAlive(true), mFinished(false),
      mDir(DIR_UP),
      mAnimUp(0.12f), mAnimDown(0.12f), mAnimLeft(0.12f), mAnimRight(0.12f),
      mDeadLoaded(false) {
    loadAssets();
}

CPEOPLE::CPEOPLE(int startX, int startY)
    : mX(startX), mY(startY),
      mAlive(true), mFinished(false),
      mDir(DIR_UP),
      mAnimUp(0.12f), mAnimDown(0.12f), mAnimLeft(0.12f), mAnimRight(0.12f),
      mDeadLoaded(false) {
    loadAssets();
}

void CPEOPLE::loadAssets() {
    // 1. Load 6 vertical running frames (Up)
    vector<string> upFrames = {
        "Assets/images/player/run-up-1.png",
        "Assets/images/player/run-up-2.png",
        "Assets/images/player/run-up-3.png",
        "Assets/images/player/run-up-4.png",
        "Assets/images/player/run-up-5.png",
        "Assets/images/player/run-up-6.png"
    };
    mAnimUp.loadAssets(upFrames);

    // 2. Load run down
    mAnimDown.loadAssets({"Assets/images/player/run_down.png"});

    // 3. Load lateral sprites
    mAnimLeft.loadAssets({"Assets/images/player/run-left.png"});
    mAnimRight.loadAssets({"Assets/images/player/run-right.png"});

    // 4. Load dead sprite
    mDeadLoaded = mTexDead.loadFromFile("Assets/images/player/player_dead.png");
    if (mDeadLoaded) {
        mTexDead.setSmooth(false);
        mDeadSprite.emplace(mTexDead);
    }
}

void CPEOPLE::loadAssets(const std::string& /*frame1*/, const std::string& /*frame2*/) {
    loadAssets();
}

void CPEOPLE::updateAnim(float dt) {
    if (!mAlive) return;

    if (mDir == DIR_LEFT)        mAnimLeft.update(dt);
    else if (mDir == DIR_RIGHT)  mAnimRight.update(dt);
    else if (mDir == DIR_DOWN)   mAnimDown.update(dt);
    else                         mAnimUp.update(dt);
}

// ----------------------------------------------------------------
// Directional Movement & Step Animation Progression
// ----------------------------------------------------------------

void CPEOPLE::Up(int step) {
    if (!mAlive) return;
    mY = max(0, mY - step); // Upper screen boundary
    mDir = DIR_UP;
    mAnimUp.nextFrame();    // Step forward in 6-frame run sequence
}

void CPEOPLE::Down(int step) {
    if (!mAlive) return;
    mY = min(START_Y, mY + step); // Bottom screen boundary
    mDir = DIR_DOWN;
    mAnimDown.nextFrame();        // Step backward using run_down
}

void CPEOPLE::Left(int step) {
    if (!mAlive) return;
    mX = max(0, mX - step); // Left screen boundary
    mDir = DIR_LEFT;
    mAnimLeft.nextFrame();
}

void CPEOPLE::Right(int step) {
    if (!mAlive) return;
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

    if (!mAlive) {
        if (mDeadLoaded && mDeadSprite.has_value()) {
            FloatRect bounds = mDeadSprite->getLocalBounds();
            if (bounds.size.x > 0.f && bounds.size.y > 0.f) {
                mDeadSprite->setScale(Vector2f(
                    34.f / bounds.size.x,
                    34.f / bounds.size.y));
                mDeadSprite->setPosition(Vector2f(px - 5.f, py - 5.f));
                window.draw(*mDeadSprite);
                return;
            }
        }
        Text text(font, "X", 18);
        text.setFillColor(Color::Red);
        text.setPosition(Vector2f(px + 4.f, py + 2.f));
        window.draw(text);
        return;
    }

    // Select active animation based on movement direction
    AnimatedSprite* activeAnim = &mAnimUp;
    if (mDir == DIR_LEFT)       activeAnim = &mAnimLeft;
    else if (mDir == DIR_RIGHT) activeAnim = &mAnimRight;
    else if (mDir == DIR_DOWN)  activeAnim = &mAnimDown;

    if (activeAnim->isLoaded()) {
        activeAnim->drawExact(window, px - 3.f, py - 4.f, 30.f, 30.f, false);
    }
    else {
        // Fallback text "Y" if images fail to load
        Text text(font, "Y", 18);
        text.setFillColor(COLOR_PLAYER);
        text.setPosition(Vector2f(px + 4.f, py + 2.f));
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
    return checkFinish(FINISH_Y);
}

bool CPEOPLE::checkFinish(int finishY) {
    if (mY <= finishY) {
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
    mAnimDown.reset();
    mAnimLeft.reset();
    mAnimRight.reset();
}

void CPEOPLE::Die() {
    mAlive = false;
}
