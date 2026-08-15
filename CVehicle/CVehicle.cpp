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

// ----------------------------------------------------------------
// loadAssets — Load 1 hoặc 2 frame ảnh vào AnimatedSprite
// Dùng mAnim.loadAssets() (hàm mới) để gộp gọn
// ----------------------------------------------------------------
void CVEHICLE::loadAssets(const std::string& frame1, const std::string& frame2) {
    vector<string> files;
    files.push_back(frame1);
    if (!frame2.empty()) files.push_back(frame2);
    mAnim.loadAssets(files);
    // Nếu load thất bại → mAnim.isLoaded() = false
    // → Draw() tự dùng fallback hình chữ nhật màu, game vẫn chạy bình thường
}

// ----------------------------------------------------------------
// Move — Di chuyển xe, wrap khi ra khỏi màn hình
// ----------------------------------------------------------------
void CVEHICLE::Move(int screenWidthCells) {
    if (mStopped) return;

    mX += mDirection * mSpeed;

    if (mX >= screenWidthCells)    mX = -mWidth;          // Ra phải → vào trái
    else if (mX + mWidth <= 0)     mX = screenWidthCells; // Ra trái → vào phải
}

// ----------------------------------------------------------------
// Draw — Vẽ xe lên cửa sổ SFML
// Nếu có ảnh PNG → dùng AnimatedSprite (với flipX khi đi trái)
// Nếu chưa có ảnh → vẽ hình chữ nhật màu fallback
// ----------------------------------------------------------------
void CVEHICLE::Draw(RenderWindow& window) {
    float px = CellToPixel(mX);
    float py = CellToPixel(mY);

    if (mAnim.isLoaded()) {
        bool flipX = (mDirection == -1); // Lật ảnh khi đi sang trái
        mAnim.draw(window, px, py, mWidth, 1, flipX);
    }
    else {
        // Fallback: hình chữ nhật màu — hữu ích khi test chưa có asset PNG
        RectangleShape rect = MakeShape(mX, mY, mWidth, mColor);
        window.draw(rect);
    }
}

// ----------------------------------------------------------------
// isImpact — Kiểm tra xe có đụng người tại ô lưới (px, py) không
// Điều kiện va chạm:
//   1. Cùng hàng:  mY == py
//   2. Cột người nằm trong vùng xe: mX <= px < mX + mWidth
// ----------------------------------------------------------------
bool CVEHICLE::isImpact(int px, int py) const {
    if (mY != py) return false;                        // Khác hàng → không đụng
    return (px >= mX && px < mX + mWidth);             // Người trong vùng xe?
}

// ----------------------------------------------------------------
// Stop — Dừng xe trong duration_ms milliseconds
// ----------------------------------------------------------------
void CVEHICLE::Stop(int duration_ms) {
    mStopped = true;
    mStopDurationMs = duration_ms;
    mStopClock.restart();
}

// ----------------------------------------------------------------
// Resume — Cho xe chạy lại ngay lập tức
// ----------------------------------------------------------------
void CVEHICLE::Resume() {
    mStopped = false;
    mStopDurationMs = 0;
}

// ----------------------------------------------------------------
// UpdateStopStatus — Tự Resume() nếu đã dừng đủ thời gian
// CGAME gọi hàm này mỗi frame cho từng xe
// ----------------------------------------------------------------
void CVEHICLE::UpdateStopStatus() {
    if (!mStopped) return;
    int elapsed = (int)mStopClock.getElapsedTime().asMilliseconds();
    if (elapsed >= mStopDurationMs) Resume();
}

// ================================================================
// CCAR
// ================================================================
CCAR::CCAR() : CVEHICLE(0, 5, 2, 1, COLOR_CAR, 2) {}

CCAR::CCAR(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_CAR, 2) {
}

// ================================================================
// CTRUCK
// ================================================================
CTRUCK::CTRUCK() : CVEHICLE(0, 7, 1, -1, COLOR_TRUCK, 3) {}

CTRUCK::CTRUCK(int x, int y, int speed, int dir)
    : CVEHICLE(x, y, speed, dir, COLOR_TRUCK, 3) {
}

// ================================================================
// CTRAFFICLIGHT
// ================================================================
CTRAFFICLIGHT::CTRAFFICLIGHT(int laneY)
    : mState(GREEN),
      mLaneY(laneY),
      mGreenAnim(0.12f),
      mStopAssetLoaded(false) {
}

// ----------------------------------------------------------------
// loadAssets — Load ảnh đèn dừng và animation đèn chạy
// ----------------------------------------------------------------
bool CTRAFFICLIGHT::loadAssets(const std::string& stopImg,
                               const std::vector<std::string>& goFrames) {
    const bool stopLoaded = mTexStop.loadFromFile(stopImg);
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
// Draw — Vẽ đèn tại ô lưới (x, y)
// Có ảnh → vẽ sprite | Không có ảnh → vẽ hình tròn màu fallback
// ----------------------------------------------------------------
void CTRAFFICLIGHT::Draw(RenderWindow& window, int x, int y) {
    const float px = CellToPixel(x);
    const float py = CellToPixel(y);
    const int heightCells = 2;

    if (mState == GREEN && mGreenAnim.isLoaded()) {
        mGreenAnim.draw(window, px, py, 1, heightCells);
        return;
    }

    if (mState == RED && mStopAssetLoaded && mStopSprite.has_value()) {
        const FloatRect bounds = mStopSprite->getLocalBounds();
        if (bounds.size.x > 0.f && bounds.size.y > 0.f) {
            mStopSprite->setScale(Vector2f(
                static_cast<float>(CELL_SIZE) / bounds.size.x,
                static_cast<float>(heightCells * CELL_SIZE) / bounds.size.y));
            mStopSprite->setPosition(Vector2f(px, py));
            window.draw(*mStopSprite);
            return;
        }
    }

    RectangleShape fallback(Vector2f(
        static_cast<float>(CELL_SIZE - 2),
        static_cast<float>(heightCells * CELL_SIZE - 2)));
    fallback.setPosition(Vector2f(px + 1.f, py + 1.f));
    fallback.setFillColor(mState == RED ? COLOR_LIGHT_RED : COLOR_LIGHT_GREEN);
    window.draw(fallback);
}
