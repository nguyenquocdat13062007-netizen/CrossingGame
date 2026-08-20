#include "CAnimal.h"
#include <iostream>

using namespace std;
using namespace sf;

// ================================================================
// CANIMAL Implementation
// ================================================================

CANIMAL::CANIMAL()
    : mX(0), mY(10), mSpeed(4), mMoveCounter(0), mDirection(1),
      mWidth(2), mColor(COLOR_BIRD), mAnim(0.12f) {
}

CANIMAL::CANIMAL(int x, int y, int speed, int dir, Color color, int width)
    : mX(x), mY(y), mSpeed(speed > 0 ? speed : 4), mMoveCounter(0), mDirection(dir),
      mWidth(width), mColor(color), mAnim(0.12f) {
}

void CANIMAL::loadAssets(const vector<string>& files) {
    mAnim.loadAssets(files);
}

void CANIMAL::loadAssets(const std::string& frame1, const std::string& frame2) {
    vector<string> files;
    if (!frame1.empty()) files.push_back(frame1);
    if (!frame2.empty()) files.push_back(frame2);
    mAnim.loadAssets(files);
}

void CANIMAL::Move(int screenWidthCells) {
    mMoveCounter++;
    if (mMoveCounter < mSpeed) {
        return;
    }
    mMoveCounter = 0;

    mX += mDirection;

    // Screen wrap-around boundary logic
    if (mX >= screenWidthCells)        mX = -mWidth;          // Off right edge -> enter left edge
    else if (mX + mWidth <= 0)         mX = screenWidthCells; // Off left edge -> enter right edge
}

void CANIMAL::Draw(RenderWindow& window) {
    float px = CellToPixel(mX);
    float py = CellToPixel(mY);

    if (mAnim.isLoaded()) {
        // Zombie / Animal quay mat sang PHAI (+1) -> can lat flipX khi di sang TRAI (-1)
        bool flipX = (mDirection == -1);
        mAnim.drawExact(window, px - 3.f, py - 10.f, (float)(mWidth * CELL_SIZE + 4), (float)(mWidth * CELL_SIZE * 1.5f + 4), flipX);
    }
    else {
        // Fallback shape if textures fail to load
        RectangleShape rect = MakeShape(mX, mY, mWidth, mColor);
        window.draw(rect);
    }
}

// ================================================================
// CBIRD Subclass (Zombie - 8 frame animation)
// ================================================================

SoundBuffer CBIRD::s_soundBuffer;
bool CBIRD::s_soundLoaded = false;
optional<Sound> CBIRD::s_sound;
Clock CBIRD::s_tellClock;
float CBIRD::s_masterVolume = 80.f;

CBIRD::CBIRD()
    : CANIMAL(0, 12, 5, 1, COLOR_BIRD, 2) {
    loadAssets({
        "Assets/images/entities/zombie_1.png",
        "Assets/images/entities/zombie_2.png",
        "Assets/images/entities/zombie_3.png",
        "Assets/images/entities/zombie_4.png",
        "Assets/images/entities/zombie_5.png",
        "Assets/images/entities/zombie_6.png",
        "Assets/images/entities/zombie_7.png",
        "Assets/images/entities/zombie_8.png"
    });

    if (!s_soundLoaded) {
        s_soundLoaded = s_soundBuffer.loadFromFile("Assets/music/zombie-sound.wav");
        if (s_soundLoaded) {
            s_sound.emplace(s_soundBuffer);
        }
    }
}

CBIRD::CBIRD(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_BIRD, 2) {
    loadAssets({
        "Assets/images/entities/zombie_1.png",
        "Assets/images/entities/zombie_2.png",
        "Assets/images/entities/zombie_3.png",
        "Assets/images/entities/zombie_4.png",
        "Assets/images/entities/zombie_5.png",
        "Assets/images/entities/zombie_6.png",
        "Assets/images/entities/zombie_7.png",
        "Assets/images/entities/zombie_8.png"
    });

    if (!s_soundLoaded) {
        s_soundLoaded = s_soundBuffer.loadFromFile("Assets/music/zombie-sound.wav");
        if (s_soundLoaded) {
            s_sound.emplace(s_soundBuffer);
        }
    }
}

void CBIRD::Tell() {
    Tell(1.0f);
}

void CBIRD::Tell(float distanceFactor) {
    playProximityGroan(distanceFactor);
}

void CBIRD::playProximityGroan(float distanceFactor) {
    if (!s_soundLoaded || !s_sound.has_value()) return;

    if (distanceFactor <= 0.01f) {
        if (s_sound->getStatus() == SoundSource::Status::Playing) {
            s_sound->setVolume(0.f);
        }
        return;
    }

    float clampedFactor = max(0.f, min(1.f, distanceFactor));
    float currentVol = s_masterVolume * clampedFactor;

    if (s_sound->getStatus() == SoundSource::Status::Playing) {
        s_sound->setVolume(currentVol);
    }
    else if (s_tellClock.getElapsedTime().asSeconds() > 3.5f) {
        s_sound->setVolume(currentVol);
        s_sound->play();
        s_tellClock.restart();
    }
}

void CBIRD::stopSound() {
    if (s_soundLoaded && s_sound.has_value()) {
        s_sound->stop();
    }
}

void CBIRD::setVolume(float vol) {
    s_masterVolume = max(0.f, min(100.f, vol));
    if (s_soundLoaded && s_sound.has_value() && s_sound->getStatus() == SoundSource::Status::Playing) {
        s_sound->setVolume(s_masterVolume);
    }
}

// ================================================================
// CROCK Subclass (Rock roll - 3 frame animation) - Hoan toan im lang
// ================================================================

CROCK::CROCK()
    : CANIMAL(0, 15, 4, -1, COLOR_DINO, 2) {
    loadAssets({
        "Assets/images/other/rock1.png",
        "Assets/images/other/rock2.png",
        "Assets/images/other/rock3.png"
    });
}

CROCK::CROCK(int x, int y, int speed, int dir)
    : CANIMAL(x, y, speed, dir, COLOR_DINO, 2) {
    loadAssets({
        "Assets/images/other/rock1.png",
        "Assets/images/other/rock2.png",
        "Assets/images/other/rock3.png"
    });
}
