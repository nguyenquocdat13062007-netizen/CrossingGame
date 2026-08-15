#pragma once
#ifndef CVEHICLE_H
#define CVEHICLE_H

#include "Utils.h"
#include "AnimatedSprite.h"
#include <SFML/Graphics.hpp>
#include <optional>
#include <string>
#include <vector>
using namespace std;
using namespace sf;

// ================================================================
// LOP CO SO: CVEHICLE
// ================================================================
class CVEHICLE {
protected:
	int    mX, mY; // Tọa độ hiện tại của xe trên đường (mX: cột, mY: hàng)
	int    mSpeed; // Số ký tự xe di chuyển mỗi lần gọi Move()
	int    mDirection;   // 1 = di sang phải, -1 = di sang trái
	int    mWidth; // Chiều rộng xe (tính theo số ký tự)
	Color  mColor; // Màu sắc xe (Color)

	bool   mStopped; // true = xe đang dừng, false = xe đang di chuyển
	Clock  mStopClock; // Đồng hồ đếm thời gian dừng của xe
	int    mStopDurationMs; // Thời gian dừng còn lại (ms), cập nhật mỗi frame trong UpdateStopStatus()

	AnimatedSprite mAnim; // Animation của xe, quản lý bởi AnimatedSprite

public:
    CVEHICLE();
    CVEHICLE(int x, int y, int speed, int dir, Color color, int width);
    virtual ~CVEHICLE() {}

	int  getX() const { return mX; } // Trả về cột hiện tại của xe
	int  getY() const { return mY; } // Trả về hàng hiện tại của xe
	int  getWidth()  const { return mWidth; } // Trả về chiều rộng của xe (số ô chiếm chỗ)
	bool isStopped() const { return mStopped; } // Trả về true nếu xe đang dừng, false nếu đang di chuyển
	void setX(int x) { mX = x; } // Setter cho mX, CGAME có thể dùng để đặt lại vị trí xe

    // Load anh sprite - goi sau khi new
	void loadAssets(const std::string& frame1, const std::string& frame2 = ""); // Gọi sau khi new, nếu load thất bại thì animation sẽ không hiển thị nhưng xe vẫn hoạt động bình thường
	// Clear anh sprite - goi khi delete

    // Cap nhat animation
    void updateAnim(float dt) { mAnim.update(dt); }
   
	virtual void Move(int screenWidthCells); // Di chuyển xe theo mSpeed và mDirection, cập nhật mX, và xử lý khi xe
	virtual void Draw(RenderWindow& window); // Vẽ xe tại vị trí (mX, mY) trên window, sử dụng mAnim để vẽ animation
	bool isImpact(int px, int py) const;

	void Stop(int duration_ms); // Dừng xe trong duration_ms milliseconds, cập nhật mStopped, mStopDurationMs, và start mStopClock
	void Resume(); // Cho xe tiếp tục di chuyển nếu đang dừng, cập nhật mStopped và reset mStopDurationMs
	void UpdateStopStatus(); // Cập nhật trạng thái dừng của xe, gọi mỗi frame từ CGAME.updateVehicles()
};

// ================================================================
// CCAR - Xe hoi (nho, nhanh)
// Assets: Assets/images/vehicle/car.png, car2.png
// ================================================================
class CCAR : public CVEHICLE {
public:
    CCAR();
    CCAR(int x, int y, int speed, int dir);
};

// ================================================================
// CTRUCK - Xe tai (to, cham)
// Assets: Assets/images/vehicle/truck.png, truck2.png
// ================================================================
class CTRUCK : public CVEHICLE {
public:
    CTRUCK();
    CTRUCK(int x, int y, int speed, int dir);
};

// ================================================================
// CTRAFFICLIGHT - Nguoi cam gay chi huy giao thong
// Fix SFML 3.0: Sprite khong co default constructor
//   -> dung optional<Sprite> thay the
// ================================================================
class CTRAFFICLIGHT {
public:
	enum State { RED, GREEN }; // Trạng thái đèn giao thông: RED = dừng, GREEN = đi

private:
	State mState; // Trạng thái hiện tại của đèn giao thông
	int mLaneY; // Hàng của làn đường mà đèn điều khiển
	AnimatedSprite mGreenAnim; // light1 -> light4 khi giao thông đang chạy
	Texture mTexStop; // Ảnh dừng giao thông
	optional<Sprite> mStopSprite;
	bool mStopAssetLoaded;

public:
	explicit CTRAFFICLIGHT(int laneY);

	// Load ảnh dừng và các frame animation cho trạng thái chạy
	bool loadAssets(const std::string& stopImg,
		const std::vector<std::string>& goFrames);

	void setState(State state);
	void updateAnimation(float dt);

	State getState() const { return mState; }
	bool isRed() const { return mState == RED; }
	int getLaneY() const { return mLaneY; }

	// Vẽ đèn tại vị trí x, y tính theo ô lưới
	void Draw(RenderWindow& window, int x, int y);
};

#endif
