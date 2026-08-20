#pragma once
#ifndef CANIMAL_H
#define CANIMAL_H

#include "Utils.h"
#include "AnimatedSprite.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <vector>
#include <string>
#include <optional>

using namespace std;
using namespace sf;

// ================================================================
// LOP CO SO: CANIMAL (Abstract)
// Quan ly di chuyen + ve sprite cho zombie va da lan
// ================================================================
class CANIMAL {
protected:
	int mX, mY;       // Toa do hien tai tren man hinh
	int mSpeed;       // So tick giua cac buoc di chuyen (1 = moi tick, 2 = moi 2 tick...)
	int mMoveCounter; // Bo dem tick dieu toc di chuyen
	int mDirection;   // 1 = sang phai, -1 = sang trai
	int mWidth;       // Chieu rong thu
	Color mColor;     // Mau sac fallback
	AnimatedSprite mAnim; // Animation quan ly boi AnimatedSprite

public:
	CANIMAL();
	CANIMAL(int x, int y, int speed, int dir, Color color, int width);
	virtual ~CANIMAL() {}

	int getX()     const { return mX; }
	int getY()     const { return mY; }
	int getWidth() const { return mWidth; }
	void setX(int x) { mX = x; }

	void loadAssets(const std::vector<std::string>& files);
	void loadAssets(const std::string& frame1, const std::string& frame2 = "");

	void updateAnim(float dt) { mAnim.update(dt); }

	virtual void Move(int screenWidthCells);
	virtual void Draw(RenderWindow& window);
	virtual void Tell() = 0;
	virtual void Tell(float distanceFactor) { (void)distanceFactor; Tell(); }
};

// ================================================================
// LOP CON: CBIRD (Zombie - 8 frame animation)
// ================================================================
class CBIRD : public CANIMAL {
private:
	static SoundBuffer s_soundBuffer;
	static bool s_soundLoaded;
	static optional<Sound> s_sound;
	static Clock s_tellClock;
	static float s_masterVolume;

public:
	CBIRD();
	CBIRD(int x, int y, int speed, int dir);
	void Tell() override;
	void Tell(float distanceFactor) override;
	static void stopSound();
	static void setVolume(float vol);
	static void playProximityGroan(float distanceFactor);
};

// ================================================================
// LOP CON: CROCK - Da lan (3 frame animation) - Hoan toan im lang
// ================================================================
class CROCK : public CANIMAL {
public:
	CROCK();
	CROCK(int x, int y, int speed, int dir);
	void Tell() override {}
};

#endif
