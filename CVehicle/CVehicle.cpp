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
CTRAFFICLIGHT::CTRAFFICLIGHT(int laneY, int greenMs, int redMs)
    : mState(GREEN),
    mGreenDurationMs(greenMs),
    mRedDurationMs(redMs),
    mAssetsLoaded(false) {
    mClock.restart();
}

// ----------------------------------------------------------------
// loadAssets — Load ảnh đèn đỏ và đèn xanh
// ----------------------------------------------------------------
void CTRAFFICLIGHT::loadAssets(const std::string& stopImg, const std::string& goImg) {
    bool ok1 = mTexStop.loadFromFile(stopImg);
    bool ok2 = mTexGo.loadFromFile(goImg);

    if (ok1 && ok2) {
        mAssetsLoaded = true;
        mSprite.emplace(mTexGo); // Bắt đầu bằng đèn xanh
    }
    else {
        mAssetsLoaded = false;
    }
}

// ----------------------------------------------------------------
// Update — Đổi trạng thái đèn theo thời gian
// Khi chuyển RED  → CGAME gọi Stop()   cho xe ở làn này
// Khi chuyển GREEN → CGAME gọi Resume() cho xe ở làn này
// ----------------------------------------------------------------
void CTRAFFICLIGHT::Update() {
    int elapsed = (int)mClock.getElapsedTime().asMilliseconds();

    if (mState == GREEN && elapsed >= mGreenDurationMs) {
        mState = RED;
        mClock.restart();
        if (mAssetsLoaded) mSprite.emplace(mTexStop);
    }
    else if (mState == RED && elapsed >= mRedDurationMs) {
        mState = GREEN;
        mClock.restart();
        if (mAssetsLoaded) mSprite.emplace(mTexGo);
    }
}

// ----------------------------------------------------------------
// Draw — Vẽ đèn tại ô lưới (x, y)
// Có ảnh → vẽ sprite | Không có ảnh → vẽ hình tròn màu fallback
// ----------------------------------------------------------------
void CTRAFFICLIGHT::Draw(RenderWindow& window, int x, int y) {
    float px = CellToPixel(x);
    float py = CellToPixel(y);

    if (mAssetsLoaded && mSprite.has_value()) {
        mSprite->setPosition(Vector2f(px, py));
        window.draw(*mSprite);
    }
    else {
        // Fallback: hình tròn đỏ/xanh
        CircleShape dot(CELL_SIZE / 2.0f - 2.0f);
        dot.setPosition(Vector2f(px + 2.0f, py + 2.0f));
        dot.setFillColor(mState == RED ? COLOR_LIGHT_RED : COLOR_LIGHT_GREEN);
        window.draw(dot);
    }
}
