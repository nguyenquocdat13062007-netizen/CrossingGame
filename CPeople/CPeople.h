#pragma once
#ifndef CPEOPLE_H
#define CPEOPLE_H

#include "Utils.h"
#include "CVehicle.h"
#include "CAnimal.h"
#include "AnimatedSprite.h"
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

// ================================================================
// LOP CPEOPLE - Nguoi choi voi sprite PNG + animation di chuyen
// ================================================================
class CPEOPLE {
private:
	int  mX, mY; // Tọa độ hiện tại của người chơi trên màn hình (mX: cột, mY: hàng)
	bool mAlive; // true = người chơi còn sống, false = người chơi đã chết
	bool mFinished; // true = người chơi đã đến vạch FINISH, false = chưa đến vạch FINISH
	int  mDirection; // 1 = di sang phải, -1 = di sang trái, 0 = đứng yên (không di chuyển)

	AnimatedSprite mAnim;  // Animation của người chơi, quản lý bởi AnimatedSprite

public:
    CPEOPLE();
    CPEOPLE(int startX, int startY);

	int  getX()       const { return mX; } // Trả về cột hiện tại của người chơi
	int  getY()       const { return mY; } // Trả về hàng hiện tại của người chơi
	bool isAlive()    const { return mAlive; } // Trả về true nếu người chơi còn sống, false nếu đã chết
	bool isDead()     const { return !mAlive; } // Trả về true nếu người chơi đã chết, false nếu còn sống
	bool isFinished() const { return mFinished; } // Trả về true nếu người chơi đã đến vạch FINISH, false nếu chưa đến vạch FINISH

    // Load anh sprite nguoi choi
    // frame1, frame2: 2 frame di chuyen (walk animation)
	void loadAssets(const std::string& frame1, const std::string& frame2 = ""); // Gọi sau khi new, nếu load thất bại thì animation sẽ không hiển thị nhưng người chơi vẫn hoạt động bình thường

    // Cap nhat animation (goi moi frame)
	void updateAnim(float dt) { mAnim.update(dt); } // Cập nhật animation theo thời gian, gọi mỗi frame từ CGAME.updatePlayer()

    // Di chuyen - co kiem tra bien man hinh
	void Up(int step = 1); // Di chuyển người chơi lên trên (giảm mY), step = số ô di chuyển (mặc định 1)
	void Down(int step = 1); // Di chuyển người chơi xuống dưới (tăng mY), step = số ô di chuyển (mặc định 1)
	void Left(int step = 1); // Di chuyển người chơi sang trái (giảm mX), step = số ô di chuyển (mặc định 1)
	void Right(int step = 1); // Di chuyển người chơi sang phải (tăng mX), step = số ô di chuyển (mặc định 1)

    // Ve nguoi choi len cua so SFML
    // font chi dung khi chua co sprite (fallback chu "Y")
	void Draw(RenderWindow& window, Font& font); // Vẽ người chơi lên cửa sổ, gọi mỗi frame từ CGAME.drawGame()

    // Kiem tra va cham
	bool isImpact(const CVEHICLE* v) const; // Va cham khi mX nguoi >= mX xe VÀ <= mX xe + mWidth, va cham chi tinh khi xe dang di chuyen (khong tinh khi xe dang dung)
	bool isImpact(const CANIMAL* a)  const; // Va cham khi mX nguoi >= mX thu VÀ <= mX thu + mWidth, va cham chi tinh khi thu dang di chuyen (khong tinh khi thu dang dung)

    // Kiem tra da den FINISH_Y chua
	bool checkFinish(); // Dung khi mY nguoi <= FINISH_Y, dat mFinished = true, va tra ve true; nguoc lai tra ve false

    // Reset ve vi tri ban dau
	void Reset(int startX = SCREEN_WIDTH / 2, int startY = START_Y); // Dat mX = startX, mY = startY, mAlive = true, mFinished = false, mDirection = 1

    // Dat trang thai chet
	void Die(); // Dat mAlive = false, mDirection = 0, va reset animation ve frame dau tien (mAnim.clear() + mAnim.addFrame(frame1))
};

#endif
