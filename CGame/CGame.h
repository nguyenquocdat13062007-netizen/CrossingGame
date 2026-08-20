#pragma once
#ifndef CGAME_H
#define CGAME_H

// ================================================================
// Ho tro da nen tang: Windows (Visual Studio) & macOS (CLion)
// ================================================================
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <chrono>
#include <thread>
using HANDLE = void*;
inline unsigned long SuspendThread(HANDLE) { return 0; }
inline unsigned long ResumeThread(HANDLE) { return 0; }
inline void Sleep(unsigned long ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
#endif

#include "Utils.h"
#include "CPeople.h"
#include "CVehicle.h"
#include "CAnimal.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <fstream>
#include <string>
#include <vector>
#include <optional>
#include <mutex>

using namespace std;
using namespace sf;

// ================================================================
// ENUM TRANG THAI MAN HINH
// ================================================================
enum class GameState {
	MENU,      // Trang thai hien menu chinh, cho nguoi choi chon Start, Load, Settings, Exit
	SETTINGS,  // Trang thai cai dat (bat/tat am thanh, chinh volume, huong dan phim)
	LOAD_GAME, // Trang thai menu chon 4 o save slot de Load
	SAVE_GAME, // Trang thai menu chon 4 o save slot de Save
	PLAYING,   // Trang thai dang choi, SubThread cap nhat logic, main render
	PAUSED,    // Trang thai tam dung, SubThread khong cap nhat logic, main render menu pause
	DEAD,      // Trang thai nguoi choi da chet, SubThread khong cap nhat logic, main render menu dead
	GAMEOVER,  // Trang thai ket thuc game, nguoi choi het mang, SubThread khong cap nhat logic, main render menu game over
	WIN,       // Trang thai nguoi choi chien thang, dat vach FINISH, SubThread khong cap nhat logic, main render menu win
	LEVEL_UP   // Dat boi SubThread khi qua man, main xu ly nextLevel() an toan
};

// ================================================================
// STRUCT THONG TIN SAVE SLOT
// ================================================================
struct SaveSlotInfo {
	bool exists;
	int level;
	int score;
	int lives;
	string timeStr;
};

// ================================================================
// LOP CGAME - Quan ly toan bo game
// ================================================================
class CGAME {
private:
	// ---- Mang doi tuong (ten theo yeu cau do an) ----
	CTRUCK* axt[MAX_TRUCKS];                         // xe tai
	CCAR* axh[MAX_CARS];                             // xe hoi
	CROCK* akl[MAX_DINOS];                           // da lan (thay CDINAUSOR)
	CBIRD* ac[MAX_BIRDS];                            // zombie (thay CBIRD)
	CTRAFFICLIGHT* mLights[MAX_LIGHTS];              // den giao thong tung khu vuc

	CPEOPLE cn;  // nguoi choi (composition)

	// ---- Trang thai ----
	int mLevel, mScore, mLives; // level, diem, mang
	int mNumTrucks, mNumCars, mNumDinos, mNumBirds, mNumLights; // so luong xe/thu/den hien tai
	int mStartY, mFinishY; // Vi tri xuat phat va ve dich theo tung man choi

	GameState mState;    // Trang thai hien tai cua game
	int mMenuOption;     // 0: Play, 1: Load, 2: Settings, 3: Exit
	int mSettingsOption; // 0: Music, 1: SFX, 2: Back to Menu
	int mSelectedSlot;   // 0..3 (Save / Load slots 1 to 4)

	// ---- Chu ky den giao thong dung chung ----
	CTRAFFICLIGHT::State mTrafficState;
	Clock mTrafficClock;
	int mTrafficElapsedMs;
	int mGreenDurationMs;
	int mRedDurationMs;

	// ---- Camera theo doi nguoi choi ----
	float mCameraY;      // Toa do Y muot ma cua camera (pixel)

	// ---- Che do Debug / Bat tu (God Mode & Developer Console) ----
	bool mGodMode;
	bool mDebugMenuOpen;
	int  mDebugMenuOption;

	// ---- Am thanh & Nhac nen chi tiet ----
	bool mMusicEnabled;
	bool mSfxEnabled;
	float mMusicVolume;
	float mSfxVolume;

	Music mMusicMenu;
	Music mMusicGame;
	Music mMusicCarPassing;
	Music mMusicWin;
	Music mMusicGameOver;
	Music mTrafficStopping;

	bool mMusicMenuLoaded;
	bool mMusicGameLoaded;
	bool mMusicCarPassingLoaded;
	bool mMusicWinLoaded;
	bool mMusicGameOverLoaded;
	bool mTrafficStoppingLoaded;

	SoundBuffer mBufDie;
	SoundBuffer mBufLevelWin;
	SoundBuffer mBufUIMenu;
	SoundBuffer mBufUIError;

	optional<Sound> mSfxDie;
	optional<Sound> mSfxLevelWin;
	optional<Sound> mSfxUIMenu;
	optional<Sound> mSfxUIError;

	bool mSfxDieLoaded;
	bool mSfxLevelWinLoaded;
	bool mSfxUIMenuLoaded;
	bool mSfxUIErrorLoaded;

	void initAudio();
	void applyVolumes();
	void updateTrafficAudio();
	void stopTrafficAudio();
	void playTrafficStoppingSound();
	void playTrafficStartSound();
	void updateZombieAudio();
	void updateStateAudio(GameState oldState, GameState newState);

	// ---- Ban do (Tilemap) ----
	int mTileMap[MAP_HEIGHT][SCREEN_WIDTH];

	// ---- UI Textures & Sprites ----
	Texture mTexMenuBg;
	Texture mTexTitleLogo;
	Texture mTexPanelPopup;
	Texture mTexBtnPlay;
	Texture mTexBtnLoad;
	Texture mTexBtnSetting;
	Texture mTexBtnResume;
	Texture mTexBtnExit;
	bool mMenuAssetsLoaded;

	// ---- Dong bo da tieu trinh (Thread Safety) ----
	mutable std::recursive_mutex mGameMutex;

	// ---- Ham noi bo ----
	void InitLanes();
	void loadAllAssets();
	void loadMapFromFile(const std::string& filename = "Assets/map.txt");
	int  GetLaneY(int laneIndex);

	void drawBuildingsZone(RenderWindow& window);
	void drawStreetZone(RenderWindow& window);
	void drawHUD(RenderWindow& window, Font& font);

public:
	CGAME();
	~CGAME();

	// ----------------------------------------------------------------
	// CAC HAM THEO YEU CAU DO AN
	// ----------------------------------------------------------------
	void     drawGame(RenderWindow& window, Font& font);
	CPEOPLE& getPeople() { return cn; }
	CVEHICLE** getVehicle();
	CANIMAL** getAnimal();

	void resetGame();
	void startGame();
	void nextLevel();

	void saveGame(const std::string& filename);
	bool loadGame(const std::string& filename);

	void pauseGame(HANDLE hThread);
	void resumeGame(HANDLE hThread);
	void exitGame(HANDLE hThread);

	void updatePosPeople(char key);
	void updatePosVehicle();
	void updatePosAnimal();
	void updateTrafficLights();

	// ---- Cap nhat animation sprite (goi moi frame) ----
	void updateAnimations(float dt);

	// ---- Kiem tra ----
	bool checkCollision();
	bool checkFinish();

	// ---- Man hinh thong bao & Menu ----
	void renderMenu(RenderWindow& window, Font& font);
	void renderSettings(RenderWindow& window, Font& font);
	void renderLoadMenu(RenderWindow& window, Font& font);
	void renderSaveMenu(RenderWindow& window, Font& font);
	void renderPauseMsg(RenderWindow& window, Font& font);
	void renderDeadMsg(RenderWindow& window, Font& font);
	void renderLevelUp(RenderWindow& window, Font& font);
	void renderWin(RenderWindow& window, Font& font);
	void renderGameOver(RenderWindow& window, Font& font);
	void renderDebugMenu(RenderWindow& window, Font& font);

	// ---- Menu & Settings Navigation ----
	int  getMenuOption() const { return mMenuOption; }
	void setMenuOption(int opt) { mMenuOption = (opt + 4) % 4; }
	void menuUp();
	void menuDown();

	int  getSettingsOption() const { return mSettingsOption; }
	void settingsUp();
	void settingsDown();
	void settingsLeft();
	void settingsRight();
	void settingsSelect();

	// ---- Debug Menu & God Mode Controls ----
	bool isGodMode() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mGodMode; }
	void setGodMode(bool enable) { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mGodMode = enable; }
	void toggleGodMode() { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mGodMode = !mGodMode; }

	bool isDebugMenuOpen() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mDebugMenuOpen; }
	void setDebugMenuOpen(bool open) { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mDebugMenuOpen = open; }
	void toggleDebugMenu() { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mDebugMenuOpen = !mDebugMenuOpen; }

	int  getDebugMenuOption() const { return mDebugMenuOption; }
	void setDebugMenuOption(int opt) { mDebugMenuOption = (opt + 6) % 6; }
	void debugMenuUp();
	void debugMenuDown();
	void debugMenuSelect();

	void addLife() { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mLives++; }
	void addScore(int delta) { std::lock_guard<std::recursive_mutex> lock(mGameMutex); mScore += delta; }
	void jumpLevel(int delta);
	void teleportFinish();

	// ---- 4 Save Slots Management ----
	int  getSelectedSlot() const { return mSelectedSlot; }
	void setSelectedSlot(int s) { mSelectedSlot = (s % 4 + 4) % 4; }
	void slotUp();
	void slotDown();
	SaveSlotInfo getSlotInfo(int slotIndex) const;
	bool saveGameSlot(int slotIndex);
	bool loadGameSlot(int slotIndex);

	// ---- Getter / Setter trang thai (Thread-safe) ----
	int       getLevel() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mLevel; }
	int       getScore() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mScore; }
	int       getLives() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mLives; }
	int       getStartY() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mStartY; }
	int       getFinishY() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mFinishY; }
	float     getCameraY() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mCameraY; }
	GameState getState() const { std::lock_guard<std::recursive_mutex> lock(mGameMutex); return mState; }
	void      setState(GameState s);

	// ---- Sound & Music Controls ----
	bool isMusicEnabled() const { return mMusicEnabled; }
	bool isSfxEnabled() const { return mSfxEnabled; }
	float getMusicVolume() const { return mMusicVolume; }
	float getSfxVolume() const { return mSfxVolume; }

	void setMusicEnabled(bool enabled);
	void setSfxEnabled(bool enabled);
	void setMusicVolume(float vol);
	void setSfxVolume(float vol);

	void toggleMusic();
	void toggleSfx();
	void changeMusicVolume(float delta);
	void changeSfxVolume(float delta);

	// Compatibility helpers
	bool isSoundEnabled() const { return mMusicEnabled || mSfxEnabled; }
	void setSoundEnabled(bool enabled);
	void toggleSound();

	void playDieSound();
	void playGameOverSound();
	void playLevelWinSound();
	void playWinSound();
	void playMenuSound();
	void playErrorSound();

	// ---- Init CGAME ----
	void Init();
};

#endif
