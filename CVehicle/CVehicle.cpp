#include "CVehicle.h"
using namespace std;
using namespace sf;

// ================================================================
// CVEHICLE
// ================================================================

CVEHICLE::CVEHICLE()
    : mX(0), mY(0), mSpeed(2), mMoveCounter(0), mDirection(1),
    mWidth(3), mColor(COLOR_CAR),
    mStopped(false), mStopDurationMs(0),
    mAnim(0.15f) {
}

CVEHICLE::CVEHICLE(int x, int y, int speed, int dir, Color color, int width)
    : mX(x), mY(y), mSpeed(speed > 0 ? speed : 2), mMoveCounter(0), mDirection(dir),
    mWidth(width), mColor(color),
    mStopped(false), mStopDurationMs(0),
    mAnim(0.15f) {
}

// ----------------------------------------------------------------
// loadAssets — Load danh sach frame anh vao AnimatedSprite
// ----------------------------------------------------------------
void CVEHICLE::loadAssets(const std::vector<std::string>& files) {
    mAnim.loadAssets(files);
}

void CVEHICLE::loadAssets(const std::string& frame1, const std::string& frame2) {
    vector<string> files;
    files.push_back(frame1);
    if (!frame2.empty()) files.push_back(frame2);
    mAnim.loadAssets(files);
}

// ----------------------------------------------------------------
// Move — Di chuyen xe, wrap khi ra khoi man hinh
// ----------------------------------------------------------------
void CVEHICLE::Move(int screenWidthCells) {
    if (mStopped) return;

    mMoveCounter++;
    if (mMoveCounter < mSpeed) {
        return;
    }
    mMoveCounter = 0;

    mX += mDirection;

    if (mX >= screenWidthCells)    mX = -mWidth;          // Ra phai -> vao trai
    else if (mX + mWidth <= 0)     mX = screenWidthCells; // Ra trai -> vao phai
}

// ----------------------------------------------------------------
// Draw — Ve xe len cua so SFML theo ti le goc dep mat
// ----------------------------------------------------------------
void CVEHICLE::Draw(RenderWindow& window) {
    float px = CellToPixel(mX);
    float py = CellToPixel(mY);

    if (mAnim.isLoaded()) {
        // Anh PNG goc quay mat sang TRAI (-1) -> can lat flipX khi di sang PHAI (+1)
        bool flipX = (mDirection == 1);
        mAnim.drawProportional(window, px, py, mWidth, flipX);
    }
    else {
        RectangleShape rect = MakeShape(mX, mY, mWidth, mColor);
        window.draw(rect);
    }
}

// ----------------------------------------------------------------
// isImpact — Kiem tra xe co dung nguoi tai o luoi (px, py) khong
// ----------------------------------------------------------------
bool CVEHICLE::isImpact(int px, int py) const {
    if (mY != py) return false;
    return (px >= mX && px < mX + mWidth);
}

// ----------------------------------------------------------------
// Stop — Dung xe trong duration_ms milliseconds
// ----------------------------------------------------------------
void CVEHICLE::Stop(int duration_ms) {
    mStopped = true;
    mStopDurationMs = duration_ms;
    mStopClock.restart();
}

// ----------------------------------------------------------------
// Resume — Cho xe chay lai ngay lap tuc
// ----------------------------------------------------------------
void CVEHICLE::Resume() {
    mStopped = false;
    mStopDurationMs = 0;
}

// ----------------------------------------------------------------
// UpdateStopStatus — Tu Resume() neu da dung du thoi gian
// ----------------------------------------------------------------
void CVEHICLE::UpdateStopStatus() {
    if (!mStopped) return;
    int elapsed = (int)mStopClock.getElapsedTime().asMilliseconds();
    if (elapsed >= mStopDurationMs) Resume();
}

// ================================================================
// CCAR
// ================================================================
CCAR::CCAR() : CVEHICLE(0, 5, 2, 1, COLOR_CAR, 4) {
    loadAssets({
        "Assets/images/vehicle/car.png",
        "Assets/images/vehicle/car2.png",
        "Assets/images/vehicle/car3.png"
    });
}

CCAR::CCAR(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_CAR, 4) {
    loadAssets({
        "Assets/images/vehicle/car.png",
        "Assets/images/vehicle/car2.png",
        "Assets/images/vehicle/car3.png"
    });
}

// ================================================================
// CTRUCK
// ================================================================
CTRUCK::CTRUCK() : CVEHICLE(0, 7, 4, -1, COLOR_TRUCK, 4) {
    loadAssets({
        "Assets/images/vehicle/truck.png",
        "Assets/images/vehicle/truck2.png",
        "Assets/images/vehicle/truck3.png"
    });
}

CTRUCK::CTRUCK(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_TRUCK, 4) {
    loadAssets({
        "Assets/images/vehicle/truck.png",
        "Assets/images/vehicle/truck2.png",
        "Assets/images/vehicle/truck3.png"
    });
}

// ================================================================
// CTRAFFICLIGHT
// ================================================================
CTRAFFICLIGHT::CTRAFFICLIGHT(int topLaneY, int bottomLaneY, int greenMs, int redMs, int initialPhaseMs)
    : mState(GREEN),
      mLaneY(topLaneY),
      mLaneBottomY(bottomLaneY > 0 ? bottomLaneY : topLaneY),
      mGreenDurationMs(greenMs > 0 ? greenMs : 3000),
      mRedDurationMs(redMs > 0 ? redMs : 2000),
      mElapsedMs(initialPhaseMs),
      mGreenAnim(0.12f),
      mStopAssetLoaded(false) {
    if (mElapsedMs >= mGreenDurationMs) {
        mState = RED;
        mElapsedMs %= mRedDurationMs;
    }
}

int CTRAFFICLIGHT::updateTimer(int dtMs) {
    mElapsedMs += dtMs;
    const int duration = (mState == GREEN) ? mGreenDurationMs : mRedDurationMs;
    if (mElapsedMs >= duration) {
        mElapsedMs = 0;
        State newState = (mState == GREEN ? RED : GREEN);
        setState(newState);
        return (newState == RED) ? 1 : 2;
    }
    return 0;
}

// ----------------------------------------------------------------
// loadAssets — Load anh den dung va animation den chay
// ----------------------------------------------------------------
bool CTRAFFICLIGHT::loadAssets(const std::string& stopImg,
                               const std::vector<std::string>& goFrames) {
    const bool stopLoaded = mTexStop.loadFromFile(stopImg);
    if (stopLoaded) {
        mTexStop.setSmooth(false);
    }
    const bool greenLoaded = mGreenAnim.loadAssets(goFrames);

    mStopAssetLoaded = stopLoaded;
    if (stopLoaded) {
        mStopSprite.emplace(mTexStop);
    }
    return stopLoaded && greenLoaded;
}

void CTRAFFICLIGHT::setState(State state) {
    if (mState == state) {
        return;
    }

    mState = state;
    if (mState == GREEN) {
        mGreenAnim.reset();
    }
}

void CTRAFFICLIGHT::updateAnimation(float dt) {
    if (mState == GREEN) {
        mGreenAnim.update(dt);
    }
}

// ----------------------------------------------------------------
// Draw — Ve den giao thong lon, ro rang
// ----------------------------------------------------------------
void CTRAFFICLIGHT::Draw(RenderWindow& window, int x, int y) {
    const float px = CellToPixel(x) - 4.f;
    const float py = CellToPixel(y) - 8.f;
    const float drawWidth = 36.f;
    const float drawHeight = 72.f;

    if (mState == GREEN && mGreenAnim.isLoaded()) {
        mGreenAnim.drawExact(window, px, py, drawWidth, drawHeight);
        return;
    }

    if (mState == RED && mStopAssetLoaded && mStopSprite.has_value()) {
        const FloatRect bounds = mStopSprite->getLocalBounds();
        if (bounds.size.x > 0.f && bounds.size.y > 0.f) {
            mStopSprite->setScale(Vector2f(
                drawWidth / bounds.size.x,
                drawHeight / bounds.size.y));
            mStopSprite->setPosition(Vector2f(px, py));
            window.draw(*mStopSprite);
            return;
        }
    }

    RectangleShape fallback(Vector2f(drawWidth - 2.f, drawHeight - 2.f));
    fallback.setPosition(Vector2f(px + 1.f, py + 1.f));
    fallback.setFillColor(mState == RED ? COLOR_LIGHT_RED : COLOR_LIGHT_GREEN);
    window.draw(fallback);
}
