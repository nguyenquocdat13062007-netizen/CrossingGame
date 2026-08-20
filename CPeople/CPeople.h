#pragma once
#ifndef CPEOPLE_H
#define CPEOPLE_H

#include "Utils.h"
#include "CVehicle.h"
#include "CAnimal.h"
#include "AnimatedSprite.h"
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <optional>

using namespace std;
using namespace sf;

// ================================================================
// LOP CPEOPLE - Nguoi choi voi sprite PNG + animation di chuyen
// ================================================================
class CPEOPLE {
public:
	enum Direction { DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT };

private:
	int  mX, mY; // Toa do hien tai cua nguoi choi tren man hinh
	bool mAlive; // true = con song, false = da chet
	bool mFinished; // true = da den vach FINISH
	Direction mDir; // Huong di chuyen hien tai

	AnimatedSprite mAnimUp;    // 6 frames chay len
	AnimatedSprite mAnimDown;  // Frame chay xuong
	AnimatedSprite mAnimLeft;  // Frame chay trai
	AnimatedSprite mAnimRight; // Frame chay phai

	Texture mTexDead;
	optional<Sprite> mDeadSprite;
	bool mDeadLoaded;

public:
	CPEOPLE();
	CPEOPLE(int startX, int startY);

	int  getX()       const { return mX; }
	int  getY()       const { return mY; }
	bool isAlive()    const { return mAlive; }
	bool isDead()     const { return !mAlive; }
	bool isFinished() const { return mFinished; }
	Direction getDirection() const { return mDir; }

	// Load anh sprite nguoi choi
	void loadAssets();
	void loadAssets(const std::string& frame1, const std::string& frame2 = "");

	// Cap nhat animation theo thoi gian
	void updateAnim(float dt);

	// Di chuyen - co kiem tra bien man hinh
	void Up(int step = 1);
	void Down(int step = 1);
	void Left(int step = 1);
	void Right(int step = 1);

	// Ve nguoi choi len cua so SFML
	void Draw(RenderWindow& window, Font& font);

	// Kiem tra va cham
	bool isImpact(const CVEHICLE* v) const;
	bool isImpact(const CANIMAL* a)  const;

	// Kiem tra da den FINISH_Y chua
	bool checkFinish();
	bool checkFinish(int finishY);

	// Reset ve vi tri ban dau
	void Reset(int startX = SCREEN_WIDTH / 2, int startY = START_Y);

	// Dat trang thai chet
	void Die();
};

#endif
