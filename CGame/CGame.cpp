// ================================================================
// CGame.cpp - Tich hop: logic game + Menu + 4-Slot Save/Load + Audio + HUD + Camera + Danger Wave
// ================================================================
#include "CGame.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <chrono>
#include <ctime>

using namespace std;
using namespace sf;

namespace {
constexpr int TILE_SIZE = 48;
constexpr int TILE_COUNT = 2;
constexpr int TILE_BACKGROUND = 0;
constexpr int TILE_ROAD = 1;
constexpr const char* DEFAULT_MAP_PATH = "Assets/map.txt";

bool readTileMap(const string& filename,
                 int (&tiles)[MAP_HEIGHT][SCREEN_WIDTH]) {
	ifstream input(filename);
	if (!input.is_open()) {
		return false;
	}

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			int tileId = 0;
			if (!(input >> tileId) || tileId < 0 || tileId >= TILE_COUNT) {
				return false;
			}
			tiles[y][x] = tileId;
		}
	}

	string extraToken;
	return !(input >> extraToken);
}

string GetSlotPath(int slotIndex) {
	return "save_slot_" + to_string(slotIndex + 1) + ".crossgame";
}

string GetCurrentDateTimeStr() {
	auto now = chrono::system_clock::now();
	time_t t = chrono::system_clock::to_time_t(now);
	tm localTm;
#ifdef _WIN32
	localtime_s(&localTm, &t);
#else
	localtime_r(&t, &localTm);
#endif
	char buf[64];
	strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M", &localTm);
	return string(buf);
}
} // namespace

// ================================================================
// DINH NGHIA BIEN TOAN CUC (khai bao extern trong Utils.h)
// Dung chung giua main() va SubThread()
// ================================================================
volatile bool IS_RUNNING = true;
volatile char MOVING = ' ';

// ================================================================
// CONSTRUCTOR & DESTRUCTOR
// ================================================================
CGAME::CGAME()
    : mLevel(1), mScore(0), mLives(3),
      mNumTrucks(0), mNumCars(0), mNumDinos(0), mNumBirds(0),
      mStartY(68), mFinishY(2),
      mState(GameState::MENU),
      mMenuOption(0), mSettingsOption(0), mSelectedSlot(0),
      mTrafficState(CTRAFFICLIGHT::GREEN),
      mTrafficElapsedMs(0),
      mGreenDurationMs(3000), mRedDurationMs(2000),
      mCameraY(CellToPixel(MAP_HEIGHT - SCREEN_HEIGHT)),
      mGodMode(false), mDebugMenuOpen(false), mDebugMenuOption(0),
      mMusicEnabled(true), mSfxEnabled(true),
      mMusicVolume(70.f), mSfxVolume(80.f),
      mMusicMenuLoaded(false), mMusicGameLoaded(false), mMusicCarPassingLoaded(false),
      mMusicWinLoaded(false), mMusicGameOverLoaded(false),
      mTrafficStoppingLoaded(false),
      mSfxDieLoaded(false),
      mSfxLevelWinLoaded(false),
      mSfxUIMenuLoaded(false), mSfxUIErrorLoaded(false),
      mMenuAssetsLoaded(false)
{
	for (int i = 0; i < MAX_TRUCKS; i++)
		axt[i] = nullptr;
	for (int i = 0; i < MAX_CARS; i++)
		axh[i] = nullptr;
	for (int i = 0; i < MAX_DINOS; i++)
		akl[i] = nullptr;
	for (int i = 0; i < MAX_BIRDS; i++)
		ac[i] = nullptr;
	for (int i = 0; i < MAX_LIGHTS; i++)
		mLights[i] = nullptr;

	loadMapFromFile("Assets/map.txt");
	initAudio();

	// Load Menu & Popup UI Assets
	bool b1 = mTexMenuBg.loadFromFile("Assets/images/menu/menu_background.png");
	bool b2 = mTexTitleLogo.loadFromFile("Assets/images/menu/game_title_logo.png");
	bool b3 = mTexPanelPopup.loadFromFile("Assets/images/menu/panel_popup.png");
	bool b4 = mTexBtnPlay.loadFromFile("Assets/images/menu/btn_play.png");
	bool b5 = mTexBtnLoad.loadFromFile("Assets/images/menu/btn_load.png");
	bool b6 = mTexBtnSetting.loadFromFile("Assets/images/menu/btn_setting.png");
	bool b7 = mTexBtnResume.loadFromFile("Assets/images/menu/btn_resume.png");
	bool b8 = mTexBtnExit.loadFromFile("Assets/images/menu/btn_exit.png");

	if (b1) mTexMenuBg.setSmooth(false);
	if (b2) mTexTitleLogo.setSmooth(false);
	if (b3) mTexPanelPopup.setSmooth(false);
	if (b4) mTexBtnPlay.setSmooth(false);
	if (b5) mTexBtnLoad.setSmooth(false);
	if (b6) mTexBtnSetting.setSmooth(false);
	if (b7) mTexBtnResume.setSmooth(false);
	if (b8) mTexBtnExit.setSmooth(false);

	mMenuAssetsLoaded = (b1 && b2 && b3);
}

CGAME::~CGAME() {
	stopTrafficAudio();
	if (mMusicMenuLoaded) mMusicMenu.stop();
	if (mMusicGameLoaded) mMusicGame.stop();
	if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
	if (mMusicWinLoaded) mMusicWin.stop();
	if (mMusicGameOverLoaded) mMusicGameOver.stop();
	CBIRD::stopSound();

	for (int i = 0; i < MAX_TRUCKS; i++)
		delete axt[i];
	for (int i = 0; i < MAX_CARS; i++)
		delete axh[i];
	for (int i = 0; i < MAX_DINOS; i++)
		delete akl[i];
	for (int i = 0; i < MAX_BIRDS; i++)
		delete ac[i];
	for (int i = 0; i < MAX_LIGHTS; i++)
		delete mLights[i];
}

// ================================================================
// INIT AUDIO - Nap nhac nen va hieu ung am thanh
// ================================================================
void CGAME::initAudio() {
	mMusicMenuLoaded = mMusicMenu.openFromFile("Assets/music/Menu-BGM.wav") ||
	                   mMusicMenu.openFromFile("Assets/music/Menu-BGM.mp3");
	if (mMusicMenuLoaded) mMusicMenu.setLooping(true);

	mMusicGameLoaded = mMusicGame.openFromFile("Assets/music/game-background-sound.wav");
	if (mMusicGameLoaded) mMusicGame.setLooping(true);

	mMusicCarPassingLoaded = mMusicCarPassing.openFromFile("Assets/music/car-passing-background.wav");
	if (mMusicCarPassingLoaded) mMusicCarPassing.setLooping(true);

	mTrafficStoppingLoaded = mTrafficStopping.openFromFile("Assets/music/Traffic-Stopping.wav");
	if (mTrafficStoppingLoaded) mTrafficStopping.setLooping(false);

	mMusicWinLoaded = mMusicWin.openFromFile("Assets/music/Win-Game.wav");
	if (mMusicWinLoaded) mMusicWin.setLooping(false);

	mMusicGameOverLoaded = mMusicGameOver.openFromFile("Assets/music/game-over.wav");
	if (mMusicGameOverLoaded) mMusicGameOver.setLooping(false);

	mSfxDieLoaded = mBufDie.loadFromFile("Assets/music/player-die.wav");
	if (mSfxDieLoaded) mSfxDie.emplace(mBufDie);

	mSfxLevelWinLoaded = mBufLevelWin.loadFromFile("Assets/music/Level-Win.wav");
	if (mSfxLevelWinLoaded) mSfxLevelWin.emplace(mBufLevelWin);

	mSfxUIMenuLoaded = mBufUIMenu.loadFromFile("Assets/music/UI-menu.wav");
	if (mSfxUIMenuLoaded) mSfxUIMenu.emplace(mBufUIMenu);

	mSfxUIErrorLoaded = mBufUIError.loadFromFile("Assets/music/UI-error.wav");
	if (mSfxUIErrorLoaded) mSfxUIError.emplace(mBufUIError);

	applyVolumes();
}

void CGAME::applyVolumes() {
	if (mMusicMenuLoaded) mMusicMenu.setVolume(mMusicEnabled ? mMusicVolume : 0.f);
	if (mMusicGameLoaded) mMusicGame.setVolume(mMusicEnabled ? (mMusicVolume * 0.8f) : 0.f);
	if (mMusicCarPassingLoaded) mMusicCarPassing.setVolume(mMusicEnabled ? (mMusicVolume * 0.55f) : 0.f);
	if (mMusicWinLoaded) mMusicWin.setVolume(mMusicEnabled ? mMusicVolume : 0.f);
	if (mMusicGameOverLoaded) mMusicGameOver.setVolume(mMusicEnabled ? mMusicVolume : 0.f);

	float sfxVol = mSfxEnabled ? mSfxVolume : 0.f;
	if (mTrafficStoppingLoaded) mTrafficStopping.setVolume(sfxVol * 0.85f);
	if (mSfxDieLoaded && mSfxDie.has_value()) mSfxDie->setVolume(sfxVol);
	if (mSfxLevelWinLoaded && mSfxLevelWin.has_value()) mSfxLevelWin->setVolume(sfxVol);
	if (mSfxUIMenuLoaded && mSfxUIMenu.has_value()) mSfxUIMenu->setVolume(sfxVol);
	if (mSfxUIErrorLoaded && mSfxUIError.has_value()) mSfxUIError->setVolume(sfxVol);
	CBIRD::setVolume(sfxVol);
}

void CGAME::setMusicEnabled(bool enabled) {
	mMusicEnabled = enabled;
	applyVolumes();
	if (!mMusicEnabled) {
		if (mMusicMenuLoaded) mMusicMenu.stop();
		if (mMusicGameLoaded) mMusicGame.stop();
		if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
	}
	else {
		updateStateAudio(mState, mState);
	}
}

void CGAME::setSfxEnabled(bool enabled) {
	mSfxEnabled = enabled;
	applyVolumes();
	if (!mSfxEnabled) {
		stopTrafficAudio();
		CBIRD::stopSound();
	}
}

void CGAME::setMusicVolume(float vol) {
	mMusicVolume = max(0.f, min(100.f, vol));
	applyVolumes();
}

void CGAME::setSfxVolume(float vol) {
	mSfxVolume = max(0.f, min(100.f, vol));
	applyVolumes();
}

void CGAME::toggleMusic() {
	setMusicEnabled(!mMusicEnabled);
}

void CGAME::toggleSfx() {
	setSfxEnabled(!mSfxEnabled);
}

void CGAME::changeMusicVolume(float delta) {
	setMusicVolume(mMusicVolume + delta);
}

void CGAME::changeSfxVolume(float delta) {
	setSfxVolume(mSfxVolume + delta);
}

void CGAME::setSoundEnabled(bool enabled) {
	setMusicEnabled(enabled);
	setSfxEnabled(enabled);
}

void CGAME::toggleSound() {
	bool any = isSoundEnabled();
	setSoundEnabled(!any);
}

void CGAME::playDieSound() {
	if (mSfxEnabled && mSfxDieLoaded && mSfxDie.has_value()) {
		mSfxDie->play();
	}
}

void CGAME::playGameOverSound() {
	if (mMusicEnabled && mMusicGameOverLoaded) {
		mMusicGameOver.stop();
		mMusicGameOver.play();
	}
}

void CGAME::playLevelWinSound() {
	if (mSfxEnabled && mSfxLevelWinLoaded && mSfxLevelWin.has_value()) {
		mSfxLevelWin->play();
	}
}

void CGAME::playWinSound() {
	if (mMusicEnabled && mMusicWinLoaded) {
		mMusicWin.stop();
		mMusicWin.play();
	}
}

void CGAME::playMenuSound() {
	if (mSfxEnabled && mSfxUIMenuLoaded && mSfxUIMenu.has_value()) {
		mSfxUIMenu->play();
	}
}

void CGAME::playErrorSound() {
	if (mSfxEnabled && mSfxUIErrorLoaded && mSfxUIError.has_value()) {
		mSfxUIError->play();
	}
}

// ================================================================
// MENU & SETTINGS NAVIGATION
// ================================================================
void CGAME::menuUp() {
	mMenuOption = (mMenuOption + 3) % 4;
	playMenuSound();
}

void CGAME::menuDown() {
	mMenuOption = (mMenuOption + 1) % 4;
	playMenuSound();
}

void CGAME::settingsUp() {
	mSettingsOption = (mSettingsOption + 2) % 3;
	playMenuSound();
}

void CGAME::settingsDown() {
	mSettingsOption = (mSettingsOption + 1) % 3;
	playMenuSound();
}

void CGAME::settingsLeft() {
	if (mSettingsOption == 0) {
		changeMusicVolume(-10.f);
		playMenuSound();
	}
	else if (mSettingsOption == 1) {
		changeSfxVolume(-10.f);
		playMenuSound();
	}
}

void CGAME::settingsRight() {
	if (mSettingsOption == 0) {
		changeMusicVolume(10.f);
		playMenuSound();
	}
	else if (mSettingsOption == 1) {
		changeSfxVolume(10.f);
		playMenuSound();
	}
}

void CGAME::settingsSelect() {
	if (mSettingsOption == 0) {
		toggleMusic();
		playMenuSound();
	}
	else if (mSettingsOption == 1) {
		toggleSfx();
		playMenuSound();
	}
	else if (mSettingsOption == 2) {
		playMenuSound();
		setState(GameState::MENU);
	}
}

void CGAME::slotUp() {
	mSelectedSlot = (mSelectedSlot + 3) % 4;
	playMenuSound();
}

void CGAME::slotDown() {
	mSelectedSlot = (mSelectedSlot + 1) % 4;
	playMenuSound();
}

// ================================================================
// DEBUG CONSOLE & GOD MODE CONTROLS
// ================================================================
void CGAME::debugMenuUp() {
	mDebugMenuOption = (mDebugMenuOption + 5) % 6;
	playMenuSound();
}

void CGAME::debugMenuDown() {
	mDebugMenuOption = (mDebugMenuOption + 1) % 6;
	playMenuSound();
}

void CGAME::jumpLevel(int delta) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	int nextLvl = mLevel + delta;
	if (nextLvl < 1) nextLvl = 1;
	if (nextLvl > MAX_LEVEL) nextLvl = MAX_LEVEL;
	mLevel = nextLvl;
	InitLanes();
	playLevelWinSound();
}

void CGAME::teleportFinish() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	cn.Reset(cn.getX(), mFinishY);
	playMenuSound();
}

void CGAME::debugMenuSelect() {
	switch (mDebugMenuOption) {
	case 0: // Toggle God Mode
		toggleGodMode();
		playMenuSound();
		break;
	case 1: // Next Level
		jumpLevel(1);
		break;
	case 2: // Previous Level
		jumpLevel(-1);
		break;
	case 3: // Add +1 Life
		addLife();
		playMenuSound();
		break;
	case 4: // Teleport Finish
		teleportFinish();
		break;
	case 5: // Close Menu
		mDebugMenuOpen = false;
		playMenuSound();
		break;
	default:
		break;
	}
}

// ================================================================
// 4 SAVE SLOTS INFORMATION & MANAGEMENT
// ================================================================
SaveSlotInfo CGAME::getSlotInfo(int slotIndex) const {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	SaveSlotInfo info;
	info.exists = false;
	info.level = 1;
	info.score = 0;
	info.lives = 3;
	info.timeStr = "";

	string path = GetSlotPath(slotIndex);
	ifstream ifs(path);
	if (!ifs.is_open()) return info;

	string tag;
	if (ifs >> tag) {
		if (tag == "SLOT_V2") {
			string dummy;
			getline(ifs, dummy); // Consume newline
			getline(ifs, info.timeStr);
			ifs >> info.level >> info.score >> info.lives;
			info.exists = true;
		}
		else {
			try {
				info.level = stoi(tag);
				ifs >> info.score >> info.lives;
				info.timeStr = "SAVED GAME";
				info.exists = true;
			} catch (...) {
				info.exists = false;
			}
		}
	}
	ifs.close();
	return info;
}

bool CGAME::saveGameSlot(int slotIndex) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	string path = GetSlotPath(slotIndex);
	ofstream ofs(path);
	if (!ofs.is_open()) {
		playErrorSound();
		return false;
	}

	string dtStr = GetCurrentDateTimeStr();
	ofs << "SLOT_V2\n";
	ofs << dtStr << '\n';
	ofs << mLevel << '\n';
	ofs << mScore << '\n';
	ofs << mLives << '\n';
	ofs << cn.getX() << '\n';
	ofs << cn.getY() << '\n';
	ofs << mStartY << '\n';
	ofs << mFinishY << '\n';

	ofs << mNumTrucks << '\n';
	for (int i = 0; i < mNumTrucks; i++) {
		ofs << (axt[i] ? axt[i]->getX() : 0) << '\n';
	}

	ofs << mNumCars << '\n';
	for (int i = 0; i < mNumCars; i++) {
		ofs << (axh[i] ? axh[i]->getX() : 0) << '\n';
	}

	ofs << mNumBirds << '\n';
	for (int i = 0; i < mNumBirds; i++) {
		ofs << (ac[i] ? ac[i]->getX() : 0) << '\n';
	}

	ofs << (mTrafficState == CTRAFFICLIGHT::RED ? 0 : 1) << '\n';
	ofs << mTrafficElapsedMs << '\n';
	ofs << mCameraY << '\n';

	ofs.close();

	// Also backup to save.crossgame
	saveGame("save.crossgame");
	playMenuSound();
	return true;
}

bool CGAME::loadGameSlot(int slotIndex) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	string path = GetSlotPath(slotIndex);
	ifstream ifs(path);
	if (!ifs.is_open()) {
		playErrorSound();
		return false;
	}

	string tag;
	if (!(ifs >> tag)) {
		ifs.close();
		playErrorSound();
		return false;
	}

	int level = 1, score = 0, live = 3, px = 20, py = 20;
	if (tag == "SLOT_V2") {
		string dtStr;
		getline(ifs, dtStr); // Consume newline
		getline(ifs, dtStr);
		ifs >> level >> score >> live >> px >> py;
	}
	else {
		try {
			level = stoi(tag);
			ifs >> score >> live >> px >> py;
		} catch (...) {
			ifs.close();
			playErrorSound();
			return false;
		}
	}

	if (!ifs.good()) {
		ifs.close();
		playErrorSound();
		return false;
	}

	mLevel = level;
	mScore = score;
	mLives = live;

	int savedStartY = 68, savedFinishY = 2;
	if (ifs >> savedStartY >> savedFinishY) {
		mStartY = savedStartY;
		mFinishY = savedFinishY;
	}

	InitLanes();
	cn.Reset(px, py);

	int n, x;
	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumTrucks && axt[i]) axt[i]->setX(x);
		}
	}

	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumCars && axh[i]) axh[i]->setX(x);
		}
	}

	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumBirds && ac[i]) ac[i]->setX(x);
		}
	}

	int savedTrafficState = 1;
	int savedElapsedMs = 0;
	if (ifs >> savedTrafficState) {
		mTrafficState = (savedTrafficState == 0) ? CTRAFFICLIGHT::RED : CTRAFFICLIGHT::GREEN;
		if (ifs >> savedElapsedMs) {
			mTrafficElapsedMs = max(0, savedElapsedMs);
		}
		if (mLights[0] != nullptr) {
			mLights[0]->setState(mTrafficState);
		}
		const bool stopVehicles = (mTrafficState == CTRAFFICLIGHT::RED);
		for (int i = 0; i < mNumTrucks; i++) {
			if (axt[i]) {
				if (stopVehicles) axt[i]->Stop(99999);
				else axt[i]->Resume();
			}
		}
		for (int i = 0; i < mNumCars; i++) {
			if (axh[i]) {
				if (stopVehicles) axh[i]->Stop(99999);
				else axh[i]->Resume();
			}
		}
	}

	float savedCameraY = 0.f;
	if (ifs >> savedCameraY) {
		mCameraY = savedCameraY;
	}

	mTrafficClock.restart();
	updateTrafficAudio();

	ifs.close();
	playMenuSound();
	return true;
}

// ================================================================
// GETTER DANH SACH XE / THU (theo yeu cau do an)
// ================================================================
CVEHICLE** CGAME::getVehicle() {
	static CVEHICLE* a[MAX_TRUCKS + MAX_CARS];
	int idx = 0;
	for (int i = 0; i < mNumTrucks; i++)
		a[idx++] = axt[i];
	for (int i = 0; i < mNumCars; i++)
		a[idx++] = axh[i];
	while (idx < MAX_TRUCKS + MAX_CARS)
		a[idx++] = nullptr;
	return a;
}

CANIMAL** CGAME::getAnimal() {
	static CANIMAL* a[MAX_DINOS + MAX_BIRDS];
	int idx = 0;
	for (int i = 0; i < mNumDinos; i++)
		a[idx++] = akl[i];
	for (int i = 0; i < mNumBirds; i++)
		a[idx++] = ac[i];
	while (idx < MAX_DINOS + MAX_BIRDS)
		a[idx++] = nullptr;
	return a;
}

// ================================================================
// GET LANE Y
// ================================================================
int CGAME::GetLaneY(int laneIndex) {
	static const int laneRows[LANE_COUNT] = { 4, 6, 8, 10, 14, 16, 18, 20 };
	if (laneIndex >= 0 && laneIndex < LANE_COUNT) {
		return laneRows[laneIndex];
	}
	int laneHeight = (ROAD_BOTTOM - ROAD_TOP) / (LANE_COUNT + 1);
	return ROAD_TOP + (laneIndex + 1) * laneHeight;
}

// ================================================================
// INIT LANES - Bo cuc rieng biet doc dao cho tung Man choi (Level 1..5)
// Ho tro toan bo 72 hang ban do, nhieu xe va zombie tren tung lan
// ================================================================
void CGAME::InitLanes() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	loadMapFromFile("Assets/map_level" + std::to_string(mLevel) + ".txt");

	for (int i = 0; i < MAX_TRUCKS; i++) {
		delete axt[i];
		axt[i] = nullptr;
	}
	for (int i = 0; i < MAX_CARS; i++) {
		delete axh[i];
		axh[i] = nullptr;
	}
	for (int i = 0; i < MAX_BIRDS; i++) {
		delete ac[i];
		ac[i] = nullptr;
	}
	for (int i = 0; i < MAX_DINOS; i++) {
		delete akl[i];
		akl[i] = nullptr;
	}
	for (int i = 0; i < MAX_LIGHTS; i++) {
		delete mLights[i];
		mLights[i] = nullptr;
	}

	struct ObjectSpec {
		int lane;
		int dir;
		int startX;
		int speed;
	};

	struct LevelLayout {
		int startY;
		int finishY;
		int numTrucks;
		int numCars;
		int numBirds;
		ObjectSpec trucks[MAX_TRUCKS];
		ObjectSpec cars[MAX_CARS];
		ObjectSpec birds[MAX_BIRDS];
	};

	static const LevelLayout layouts[MAX_LEVEL] = {
		// Level 1: TOC DO CHUAN DE NGUOI CHOI LAM QUEN
		// Zombie: 5 tick (0.5s/o = 2.0 o/s), Xe tai: 4 tick (0.4s/o = 2.5 o/s), Xe hoi: 2..3 tick (0.2..0.3s/o = 3.3..5.0 o/s)
		{ 68, 2, 20, 22, 30,
		  // Trucks (20)
		  { { 52, 1, 0, 4 }, { 52, 1, 20, 4 }, { 49, -1, 36, 4 }, { 49, -1, 16, 4 }, { 47, -1, 28, 4 }, { 47, -1, 8, 4 }, { 30, -1, 36, 4 }, { 30, -1, 16, 4 }, { 28, -1, 30, 4 }, { 28, -1, 10, 4 }, { 26, -1, 36, 4 }, { 26, -1, 16, 4 }, { 24, -1, 24, 4 }, { 24, -1, 4, 4 }, { 17, -1, 36, 4 }, { 17, -1, 16, 4 }, { 14, 1, 5, 4 }, { 14, 1, 25, 4 }, { 12, 1, 12, 4 }, { 12, 1, 32, 4 } },
		  // Cars (22)
		  { { 53, -1, 36, 3 }, { 53, -1, 16, 3 }, { 51, -1, 30, 3 }, { 51, -1, 10, 3 }, { 50, 1, 5, 2 }, { 50, 1, 25, 2 }, { 48, 1, 12, 2 }, { 48, 1, 32, 2 }, { 29, 1, 0, 3 }, { 29, 1, 20, 3 }, { 27, 1, 15, 2 }, { 27, 1, 35, 2 }, { 25, 1, 8, 2 }, { 25, 1, 28, 2 }, { 18, 1, 0, 2 }, { 18, 1, 20, 2 }, { 16, 1, 10, 2 }, { 16, 1, 30, 2 }, { 15, -1, 30, 3 }, { 15, -1, 10, 3 }, { 13, -1, 36, 3 }, { 13, -1, 16, 3 } },
		  // Zombies (30)
		  { { 64, -1, 38, 5 }, { 64, -1, 18, 5 }, { 63, 1, 0, 5 }, { 63, 1, 20, 5 }, { 62, -1, 30, 5 }, { 62, -1, 10, 5 }, { 61, 1, 8, 5 }, { 61, 1, 28, 5 }, { 60, -1, 36, 5 }, { 60, -1, 16, 5 }, { 59, 1, 4, 5 }, { 59, 1, 24, 5 }, { 41, -1, 38, 5 }, { 41, -1, 18, 5 }, { 40, 1, 0, 5 }, { 40, 1, 20, 5 }, { 39, -1, 30, 5 }, { 39, -1, 10, 5 }, { 38, 1, 8, 5 }, { 38, 1, 28, 5 }, { 37, -1, 36, 5 }, { 37, -1, 16, 5 }, { 36, 1, 4, 5 }, { 36, 1, 24, 5 }, { 6, -1, 38, 5 }, { 6, -1, 18, 5 }, { 5, 1, 0, 5 }, { 5, 1, 20, 5 }, { 4, -1, 30, 5 }, { 4, -1, 10, 5 } }
		},

		// Level 2: Zombie (4 tick = 2.5 o/s), Xe tai (4 tick = 2.5 o/s), Xe hoi (2..3 tick = 3.3..5.0 o/s)
		{ 68, 2, 20, 22, 30,
		  // Trucks (20)
		  { { 52, 1, 0, 4 }, { 52, 1, 20, 4 }, { 49, -1, 36, 4 }, { 49, -1, 16, 4 }, { 47, -1, 28, 4 }, { 47, -1, 8, 4 }, { 30, -1, 36, 4 }, { 30, -1, 16, 4 }, { 28, -1, 30, 4 }, { 28, -1, 10, 4 }, { 26, -1, 36, 4 }, { 26, -1, 16, 4 }, { 24, -1, 24, 4 }, { 24, -1, 4, 4 }, { 17, -1, 36, 4 }, { 17, -1, 16, 4 }, { 14, 1, 5, 4 }, { 14, 1, 25, 4 }, { 12, 1, 12, 4 }, { 12, 1, 32, 4 } },
		  // Cars (22)
		  { { 53, -1, 36, 3 }, { 53, -1, 16, 3 }, { 51, -1, 30, 3 }, { 51, -1, 10, 3 }, { 50, 1, 5, 2 }, { 50, 1, 25, 2 }, { 48, 1, 12, 2 }, { 48, 1, 32, 2 }, { 29, 1, 0, 3 }, { 29, 1, 20, 3 }, { 27, 1, 15, 2 }, { 27, 1, 35, 2 }, { 25, 1, 8, 2 }, { 25, 1, 28, 2 }, { 18, 1, 0, 2 }, { 18, 1, 20, 2 }, { 16, 1, 10, 2 }, { 16, 1, 30, 2 }, { 15, -1, 30, 2 }, { 15, -1, 10, 2 }, { 13, -1, 36, 2 }, { 13, -1, 16, 2 } },
		  // Zombies (30)
		  { { 64, -1, 38, 4 }, { 64, -1, 18, 4 }, { 63, 1, 0, 4 }, { 63, 1, 20, 4 }, { 62, -1, 30, 4 }, { 62, -1, 10, 4 }, { 61, 1, 8, 4 }, { 61, 1, 28, 4 }, { 60, -1, 36, 4 }, { 60, -1, 16, 4 }, { 59, 1, 4, 4 }, { 59, 1, 24, 4 }, { 41, -1, 38, 4 }, { 41, -1, 18, 4 }, { 40, 1, 0, 4 }, { 40, 1, 20, 4 }, { 39, -1, 30, 4 }, { 39, -1, 10, 4 }, { 38, 1, 8, 4 }, { 38, 1, 28, 4 }, { 37, -1, 36, 4 }, { 37, -1, 16, 4 }, { 36, 1, 4, 4 }, { 36, 1, 24, 4 }, { 6, -1, 38, 4 }, { 6, -1, 18, 4 }, { 5, 1, 0, 4 }, { 5, 1, 20, 4 }, { 4, -1, 30, 4 }, { 4, -1, 10, 4 } }
		},

		// Level 3: Xe tai (3 tick = 3.3 o/s), Xe hoi (2 tick = 5.0 o/s), Zombie (4 tick = 2.5 o/s)
		{ 68, 2, 20, 22, 30,
		  // Trucks (20)
		  { { 52, 1, 0, 3 }, { 52, 1, 20, 3 }, { 49, -1, 36, 3 }, { 49, -1, 16, 3 }, { 47, -1, 28, 3 }, { 47, -1, 8, 3 }, { 30, -1, 36, 3 }, { 30, -1, 16, 3 }, { 28, -1, 30, 3 }, { 28, -1, 10, 3 }, { 26, -1, 36, 3 }, { 26, -1, 16, 3 }, { 24, -1, 24, 3 }, { 24, -1, 4, 3 }, { 17, -1, 36, 3 }, { 17, -1, 16, 3 }, { 14, 1, 5, 3 }, { 14, 1, 25, 3 }, { 12, 1, 12, 3 }, { 12, 1, 32, 3 } },
		  // Cars (22)
		  { { 53, -1, 36, 2 }, { 53, -1, 16, 2 }, { 51, -1, 30, 2 }, { 51, -1, 10, 2 }, { 50, 1, 5, 2 }, { 50, 1, 25, 2 }, { 48, 1, 12, 2 }, { 48, 1, 32, 2 }, { 29, 1, 0, 2 }, { 29, 1, 20, 2 }, { 27, 1, 15, 2 }, { 27, 1, 35, 2 }, { 25, 1, 8, 2 }, { 25, 1, 28, 2 }, { 18, 1, 0, 2 }, { 18, 1, 20, 2 }, { 16, 1, 10, 2 }, { 16, 1, 30, 2 }, { 15, -1, 30, 2 }, { 15, -1, 10, 2 }, { 13, -1, 36, 2 }, { 13, -1, 16, 2 } },
		  // Zombies (30)
		  { { 64, -1, 38, 4 }, { 64, -1, 18, 4 }, { 63, 1, 0, 4 }, { 63, 1, 20, 4 }, { 62, -1, 30, 4 }, { 62, -1, 10, 4 }, { 61, 1, 8, 4 }, { 61, 1, 28, 4 }, { 60, -1, 36, 4 }, { 60, -1, 16, 4 }, { 59, 1, 4, 4 }, { 59, 1, 24, 4 }, { 41, -1, 38, 4 }, { 41, -1, 18, 4 }, { 40, 1, 0, 4 }, { 40, 1, 20, 4 }, { 39, -1, 30, 4 }, { 39, -1, 10, 4 }, { 38, 1, 8, 4 }, { 38, 1, 28, 4 }, { 37, -1, 36, 4 }, { 37, -1, 16, 4 }, { 36, 1, 4, 4 }, { 36, 1, 24, 4 }, { 6, -1, 38, 4 }, { 6, -1, 18, 4 }, { 5, 1, 0, 4 }, { 5, 1, 20, 4 }, { 4, -1, 30, 4 }, { 4, -1, 10, 4 } }
		},

		// Level 4: Thu thach - xe hoi tren cung tang toc (1..2 tick), Zombie (3 tick = 3.3 o/s)
		{ 68, 2, 20, 22, 30,
		  // Trucks (20)
		  { { 52, 1, 0, 3 }, { 52, 1, 20, 3 }, { 49, -1, 36, 3 }, { 49, -1, 16, 3 }, { 47, -1, 28, 3 }, { 47, -1, 8, 3 }, { 30, -1, 36, 3 }, { 30, -1, 16, 3 }, { 28, -1, 30, 3 }, { 28, -1, 10, 3 }, { 26, -1, 36, 3 }, { 26, -1, 16, 3 }, { 24, -1, 24, 3 }, { 24, -1, 4, 3 }, { 17, -1, 36, 3 }, { 17, -1, 16, 3 }, { 14, 1, 5, 3 }, { 14, 1, 25, 3 }, { 12, 1, 12, 3 }, { 12, 1, 32, 3 } },
		  // Cars (22)
		  { { 53, -1, 36, 2 }, { 53, -1, 16, 2 }, { 51, -1, 30, 2 }, { 51, -1, 10, 2 }, { 50, 1, 5, 2 }, { 50, 1, 25, 2 }, { 48, 1, 12, 2 }, { 48, 1, 32, 2 }, { 29, 1, 0, 2 }, { 29, 1, 20, 2 }, { 27, 1, 15, 2 }, { 27, 1, 35, 2 }, { 25, 1, 8, 2 }, { 25, 1, 28, 2 }, { 18, 1, 0, 1 }, { 18, 1, 20, 1 }, { 16, 1, 10, 1 }, { 16, 1, 30, 1 }, { 15, -1, 30, 1 }, { 15, -1, 10, 1 }, { 13, -1, 36, 1 }, { 13, -1, 16, 1 } },
		  // Zombies (30)
		  { { 64, -1, 38, 3 }, { 64, -1, 18, 3 }, { 63, 1, 0, 3 }, { 63, 1, 20, 3 }, { 62, -1, 30, 3 }, { 62, -1, 10, 3 }, { 61, 1, 8, 3 }, { 61, 1, 28, 3 }, { 60, -1, 36, 3 }, { 60, -1, 16, 3 }, { 59, 1, 4, 3 }, { 59, 1, 24, 3 }, { 41, -1, 38, 3 }, { 41, -1, 18, 3 }, { 40, 1, 0, 3 }, { 40, 1, 20, 3 }, { 39, -1, 30, 3 }, { 39, -1, 10, 3 }, { 38, 1, 8, 3 }, { 38, 1, 28, 3 }, { 37, -1, 36, 3 }, { 37, -1, 16, 3 }, { 36, 1, 4, 3 }, { 36, 1, 24, 3 }, { 6, -1, 38, 3 }, { 6, -1, 18, 3 }, { 5, 1, 0, 3 }, { 5, 1, 20, 3 }, { 4, -1, 30, 3 }, { 4, -1, 10, 3 } }
		},

		// Level 5: Man cuoi - Toc do cao nhat (Xe tai 2 tick, Xe hoi 1 tick, Zombie 3 tick)
		{ 68, 2, 20, 22, 30,
		  // Trucks (20)
		  { { 52, 1, 0, 2 }, { 52, 1, 20, 2 }, { 49, -1, 36, 2 }, { 49, -1, 16, 2 }, { 47, -1, 28, 2 }, { 47, -1, 8, 2 }, { 30, -1, 36, 2 }, { 30, -1, 16, 2 }, { 28, -1, 30, 2 }, { 28, -1, 10, 2 }, { 26, -1, 36, 2 }, { 26, -1, 16, 2 }, { 24, -1, 24, 2 }, { 24, -1, 4, 2 }, { 17, -1, 36, 2 }, { 17, -1, 16, 2 }, { 14, 1, 5, 2 }, { 14, 1, 25, 2 }, { 12, 1, 12, 2 }, { 12, 1, 32, 2 } },
		  // Cars (22)
		  { { 53, -1, 36, 2 }, { 53, -1, 16, 2 }, { 51, -1, 30, 2 }, { 51, -1, 10, 2 }, { 50, 1, 5, 2 }, { 50, 1, 25, 2 }, { 48, 1, 12, 2 }, { 48, 1, 32, 2 }, { 29, 1, 0, 1 }, { 29, 1, 20, 1 }, { 27, 1, 15, 1 }, { 27, 1, 35, 1 }, { 25, 1, 8, 1 }, { 25, 1, 28, 1 }, { 18, 1, 0, 1 }, { 18, 1, 20, 1 }, { 16, 1, 10, 1 }, { 16, 1, 30, 1 }, { 15, -1, 30, 1 }, { 15, -1, 10, 1 }, { 13, -1, 36, 1 }, { 13, -1, 16, 1 } },
		  // Zombies (30)
		  { { 64, -1, 38, 3 }, { 64, -1, 18, 3 }, { 63, 1, 0, 3 }, { 63, 1, 20, 3 }, { 62, -1, 30, 3 }, { 62, -1, 10, 3 }, { 61, 1, 8, 3 }, { 61, 1, 28, 3 }, { 60, -1, 36, 3 }, { 60, -1, 16, 3 }, { 59, 1, 4, 3 }, { 59, 1, 24, 3 }, { 41, -1, 38, 3 }, { 41, -1, 18, 3 }, { 40, 1, 0, 3 }, { 40, 1, 20, 3 }, { 39, -1, 30, 3 }, { 39, -1, 10, 3 }, { 38, 1, 8, 3 }, { 38, 1, 28, 3 }, { 37, -1, 36, 3 }, { 37, -1, 16, 3 }, { 36, 1, 4, 3 }, { 36, 1, 24, 3 }, { 6, -1, 38, 3 }, { 6, -1, 18, 3 }, { 5, 1, 0, 3 }, { 5, 1, 20, 3 }, { 4, -1, 30, 3 }, { 4, -1, 10, 3 } }
		}
	};

	int lIdx = max(0, min(mLevel - 1, MAX_LEVEL - 1));
	const LevelLayout& layout = layouts[lIdx];

	mStartY = layout.startY;
	mFinishY = layout.finishY;
	mCameraY = max(0.f, CellToPixel(mStartY) - (float)WINDOW_HEIGHT * 0.65f);

	mNumTrucks = layout.numTrucks;
	mNumCars = layout.numCars;
	mNumDinos = 0;
	mNumBirds = layout.numBirds;

	// 1. Trucks
	for (int i = 0; i < mNumTrucks; i++) {
		int y = layout.trucks[i].lane;
		int dir = layout.trucks[i].dir;
		int x = layout.trucks[i].startX;
		int spd = layout.trucks[i].speed;
		axt[i] = new CTRUCK(x, y, spd, dir);
	}

	// 2. Cars
	for (int i = 0; i < mNumCars; i++) {
		int y = layout.cars[i].lane;
		int dir = layout.cars[i].dir;
		int x = layout.cars[i].startX;
		int spd = layout.cars[i].speed;
		axh[i] = new CCAR(x, y, spd, dir);
	}

	// 3. Traffic Lights cho tung khu vuc xe chay (Doc lap, khong dong bo nhau)
	// Sector 2 (lanes 47..53): Light 0 o dau khu vuc (lane 47)
	mLights[0] = new CTRAFFICLIGHT(47, 53, max(1400, 3600 - mLevel * 200), 2200, 0);
	// Sector 4 (lanes 24..30): Light 1 o dau khu vuc (lane 24)
	mLights[1] = new CTRAFFICLIGHT(24, 30, max(1400, 3000 - mLevel * 150), 2400, 1800);
	// Sector 5 (lanes 12..18): Light 2 o dau khu vuc (lane 12)
	mLights[2] = new CTRAFFICLIGHT(12, 18, max(1400, 3400 - mLevel * 200), 2000, 3600);
	mNumLights = 3;

	// 4. Zombies
	for (int i = 0; i < mNumBirds; i++) {
		int y = layout.birds[i].lane;
		int dir = layout.birds[i].dir;
		int x = layout.birds[i].startX;
		int spd = layout.birds[i].speed;
		ac[i] = new CBIRD(x, y, spd, dir);
	}

	mTrafficState = CTRAFFICLIGHT::GREEN;
	mGreenDurationMs = max(1200, 3500 - mLevel * 250);
	mRedDurationMs = 2200;
	mTrafficElapsedMs = 0;
	mTrafficClock.restart();

	loadAllAssets();
	cn.Reset(SCREEN_WIDTH / 2, mStartY);
	updateTrafficAudio();
}

// ================================================================
// LOAD MAP FROM FILE
// ================================================================
void CGAME::loadMapFromFile(const string& filename) {
	int loadedMap[MAP_HEIGHT][SCREEN_WIDTH]{};
	bool loaded = readTileMap(filename, loadedMap);

	if (!loaded && filename != DEFAULT_MAP_PATH) {
		loaded = readTileMap(DEFAULT_MAP_PATH, loadedMap);
	}

	if (!loaded) {
		for (int y = 0; y < MAP_HEIGHT; ++y) {
			for (int x = 0; x < SCREEN_WIDTH; ++x) {
				loadedMap[y][x] = (y >= 4 && y < MAP_HEIGHT - 4)
					? TILE_ROAD
					: TILE_BACKGROUND;
			}
		}
	}

	for (int y = 0; y < MAP_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			mTileMap[y][x] = loadedMap[y][x];
		}
	}
}

// ================================================================
// LOAD ALL ASSETS
// ================================================================
void CGAME::loadAllAssets() {
	cn.loadAssets();

	const vector<string> truckFrames = {
		"Assets/images/vehicle/truck.png",
		"Assets/images/vehicle/truck2.png",
		"Assets/images/vehicle/truck3.png"
	};
	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] != nullptr) {
			axt[i]->loadAssets(truckFrames);
		}
	}

	const vector<string> carFrames = {
		"Assets/images/vehicle/car.png",
		"Assets/images/vehicle/car2.png",
		"Assets/images/vehicle/car3.png"
	};
	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] != nullptr) {
			axh[i]->loadAssets(carFrames);
		}
	}

	const vector<string> zombieFrames = {
		"Assets/images/entities/zombie_1.png",
		"Assets/images/entities/zombie_2.png",
		"Assets/images/entities/zombie_3.png",
		"Assets/images/entities/zombie_4.png",
		"Assets/images/entities/zombie_5.png",
		"Assets/images/entities/zombie_6.png",
		"Assets/images/entities/zombie_7.png",
		"Assets/images/entities/zombie_8.png"
	};
	for (int i = 0; i < mNumBirds; i++) {
		if (ac[i] != nullptr) {
			ac[i]->loadAssets(zombieFrames);
		}
	}

	const vector<string> trafficGreenFrames = {
		"Assets/images/environment(for-map)/light1.png",
		"Assets/images/environment(for-map)/light2.png",
		"Assets/images/environment(for-map)/light3.png",
		"Assets/images/environment(for-map)/light4.png"
	};
	for (int i = 0; i < mNumLights; i++) {
		if (mLights[i] != nullptr) {
			mLights[i]->loadAssets(
				"Assets/images/environment(for-map)/lightstop.png",
				trafficGreenFrames);
		}
	}
}

// ================================================================
// UPDATE ANIMATIONS
// ================================================================
void CGAME::updateZombieAudio() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	if (mState != GameState::PLAYING || !mSfxEnabled || mNumBirds == 0) {
		CBIRD::stopSound();
		return;
	}

	float px = (float)cn.getX();
	float py = (float)cn.getY();
	float minDistSq = 999999.f;

	for (int i = 0; i < mNumBirds; i++) {
		if (ac[i] == nullptr) continue;
		float dx = (float)ac[i]->getX() - px;
		float dy = (float)ac[i]->getY() - py;
		float d2 = dx * dx + dy * dy;
		if (d2 < minDistSq) {
			minDistSq = d2;
		}
	}

	float minDist = std::sqrt(minDistSq);
	const float MAX_AUDIBLE_DIST = 16.f;
	if (minDist <= MAX_AUDIBLE_DIST) {
		float factor = 1.0f - (minDist / MAX_AUDIBLE_DIST);
		CBIRD::playProximityGroan(factor);
	}
	else {
		CBIRD::playProximityGroan(0.f);
	}
}

void CGAME::updateAnimations(float dt) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	cn.updateAnim(dt);
	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] != nullptr) axt[i]->updateAnim(dt);
	}
	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] != nullptr) axh[i]->updateAnim(dt);
	}
	for (int i = 0; i < mNumBirds; i++) {
		if (ac[i] != nullptr) ac[i]->updateAnim(dt);
	}
	for (int i = 0; i < mNumLights; i++) {
		if (mLights[i] != nullptr) {
			mLights[i]->updateAnimation(dt);
		}
	}
}

// ================================================================
// INIT / STARTGAME / RESETGAME / NEXTLEVEL
// ================================================================
void CGAME::Init() {
	mLevel = 1; mScore = 0; mLives = 3;
	InitLanes();
}

void CGAME::startGame() {
	mLevel = 1; mScore = 0; mLives = 3;
	InitLanes();
	setState(GameState::PLAYING);
}

void CGAME::resetGame() {
	mLevel = 1; mScore = 0; mLives = 3;
	InitLanes();
	setState(GameState::MENU);
}

void CGAME::nextLevel() {
	mLevel++;
	playLevelWinSound();
	InitLanes();
	setState(GameState::PLAYING);
}

// ================================================================
// PAUSE / RESUME / EXIT THREAD
// ================================================================
void CGAME::pauseGame(HANDLE hThread) {
	SuspendThread(hThread);
	setState(GameState::PAUSED);
}

void CGAME::resumeGame(HANDLE hThread) {
	if (mState == GameState::PAUSED) {
		setState(GameState::PLAYING);
		ResumeThread(hThread);
	}
}

void CGAME::exitGame(HANDLE hThread) {
	IS_RUNNING = false;
	stopTrafficAudio();
	if (mMusicMenuLoaded) mMusicMenu.stop();
	if (mMusicGameLoaded) mMusicGame.stop();
	if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
	if (mMusicWinLoaded) mMusicWin.stop();
	if (mMusicGameOverLoaded) mMusicGameOver.stop();
	CBIRD::stopSound();
	SuspendThread(hThread);
}

// ================================================================
// CAP NHAT VI TRI
// ================================================================
void CGAME::updatePosPeople(char key) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	if (!cn.isAlive()) return;
	switch (key) {
	case 'W': cn.Up(); break;
	case 'S': cn.Down(); break;
	case 'A': cn.Left(); break;
	case 'D': cn.Right(); break;
	default: break;
	}
}

void CGAME::updatePosVehicle() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] != nullptr) axt[i]->Move(SCREEN_WIDTH);
	}
	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] != nullptr) axh[i]->Move(SCREEN_WIDTH);
	}
}

void CGAME::updatePosAnimal() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	for (int i = 0; i < mNumBirds; i++) {
		if (ac[i] != nullptr) {
			ac[i]->Move(SCREEN_WIDTH);
		}
	}
	updateZombieAudio();
}

void CGAME::updateTrafficLights() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	int dtMs = (int)mTrafficClock.restart().asMilliseconds();
	if (dtMs <= 0 || dtMs > 1000) dtMs = 16;

	int playerY = cn.getY();
	CTRAFFICLIGHT* relevantLight = nullptr;
	int bestDist = 99999;
	int switchEvent = 0; // 1 = turned RED, 2 = turned GREEN

	for (int i = 0; i < mNumLights; i++) {
		if (mLights[i] != nullptr) {
			int ev = mLights[i]->updateTimer(dtMs);
			int lightMidY = (mLights[i]->getLaneY() + mLights[i]->getLaneBottomY()) / 2;
			int dist = std::abs(playerY - lightMidY);
			if (dist < bestDist) {
				bestDist = dist;
				relevantLight = mLights[i];
				switchEvent = ev;
			}
		}
	}

	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] == nullptr) continue;
		int y = axt[i]->getY();
		bool stop = false;
		for (int j = 0; j < mNumLights; j++) {
			if (mLights[j] != nullptr && mLights[j]->controlsLane(y)) {
				if (mLights[j]->isRed()) {
					stop = true;
				}
				break;
			}
		}
		if (stop) axt[i]->Stop(99999);
		else axt[i]->Resume();
	}

	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] == nullptr) continue;
		int y = axh[i]->getY();
		bool stop = false;
		for (int j = 0; j < mNumLights; j++) {
			if (mLights[j] != nullptr && mLights[j]->controlsLane(y)) {
				if (mLights[j]->isRed()) {
					stop = true;
				}
				break;
			}
		}
		if (stop) axh[i]->Stop(99999);
		else axh[i]->Resume();
	}

	// Chi xet trang thai den trong pham vi tam nhin nguoi choi (~20 o luoi)
	if (relevantLight != nullptr && bestDist <= 20) {
		mTrafficState = relevantLight->isRed() ? CTRAFFICLIGHT::RED : CTRAFFICLIGHT::GREEN;
	}
	else {
		mTrafficState = CTRAFFICLIGHT::GREEN;
	}

	// Xu ly am thanh den giao thong dong bo chinh xac voi trang thai den hien tai
	if (mTrafficState == CTRAFFICLIGHT::GREEN) {
		// Den xanh / dang sang: Dung ngay am thanh dung lai
		if (mTrafficStoppingLoaded && mTrafficStopping.getStatus() == SoundSource::Status::Playing) {
			mTrafficStopping.stop();
		}
	}
	else if (mTrafficState == CTRAFFICLIGHT::RED) {
		if (mSfxEnabled && mState == GameState::PLAYING && mTrafficStoppingLoaded) {
			if (mTrafficStopping.getStatus() != SoundSource::Status::Playing) {
				mTrafficStopping.play();
			}
		}
	}

	updateTrafficAudio();
}

void CGAME::stopTrafficAudio() {
	if (mTrafficStoppingLoaded && mTrafficStopping.getStatus() == SoundSource::Status::Playing) {
		mTrafficStopping.stop();
	}
}

void CGAME::updateTrafficAudio() {
	if (!mSfxEnabled || mState != GameState::PLAYING) {
		stopTrafficAudio();
		return;
	}

	if (mTrafficState == CTRAFFICLIGHT::GREEN) {
		if (mTrafficStoppingLoaded && mTrafficStopping.getStatus() == SoundSource::Status::Playing) {
			mTrafficStopping.stop();
		}
	}
	else if (mTrafficState == CTRAFFICLIGHT::RED) {
		if (mTrafficStoppingLoaded && mTrafficStopping.getStatus() != SoundSource::Status::Playing) {
			mTrafficStopping.play();
		}
	}
}

void CGAME::updateStateAudio(GameState oldState, GameState newState) {
	if (oldState == GameState::WIN || newState == GameState::MENU || newState == GameState::SETTINGS || newState == GameState::PLAYING || newState == GameState::LOAD_GAME || newState == GameState::SAVE_GAME) {
		if (mMusicWinLoaded) mMusicWin.stop();
	}
	if (oldState == GameState::GAMEOVER || newState == GameState::MENU || newState == GameState::SETTINGS || newState == GameState::PLAYING || newState == GameState::LOAD_GAME || newState == GameState::SAVE_GAME) {
		if (mMusicGameOverLoaded) mMusicGameOver.stop();
	}
	if (oldState == GameState::DEAD || newState == GameState::PLAYING || newState == GameState::MENU) {
		if (mSfxDieLoaded && mSfxDie.has_value()) mSfxDie->stop();
	}
	if (oldState == GameState::LEVEL_UP || newState == GameState::PLAYING || newState == GameState::MENU) {
		if (mSfxLevelWinLoaded && mSfxLevelWin.has_value()) mSfxLevelWin->stop();
	}
	if (newState != GameState::PLAYING) {
		CBIRD::stopSound();
	}

	applyVolumes();

	switch (newState) {
	case GameState::MENU:
	case GameState::SETTINGS:
	case GameState::LOAD_GAME:
		if (mMusicGameLoaded) mMusicGame.stop();
		if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
		stopTrafficAudio();
		CBIRD::stopSound();
		if (mMusicEnabled && mMusicMenuLoaded && mMusicMenu.getStatus() != SoundSource::Status::Playing) {
			mMusicMenu.play();
		}
		break;

	case GameState::PLAYING:
		if (mMusicMenuLoaded) mMusicMenu.stop();
		if (mMusicEnabled && mMusicGameLoaded && mMusicGame.getStatus() != SoundSource::Status::Playing) {
			mMusicGame.play();
		}
		if (mMusicEnabled && mMusicCarPassingLoaded && mMusicCarPassing.getStatus() != SoundSource::Status::Playing) {
			mMusicCarPassing.play();
		}
		updateTrafficAudio();
		break;

	case GameState::PAUSED:
	case GameState::SAVE_GAME:
		if (mMusicGameLoaded) mMusicGame.pause();
		if (mMusicCarPassingLoaded) mMusicCarPassing.pause();
		stopTrafficAudio();
		CBIRD::stopSound();
		break;

	case GameState::DEAD:
		stopTrafficAudio();
		CBIRD::stopSound();
		playDieSound();
		break;

	case GameState::GAMEOVER:
		if (mMusicGameLoaded) mMusicGame.stop();
		if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
		stopTrafficAudio();
		CBIRD::stopSound();
		playGameOverSound();
		break;

	case GameState::WIN:
		if (mMusicGameLoaded) mMusicGame.stop();
		if (mMusicCarPassingLoaded) mMusicCarPassing.stop();
		stopTrafficAudio();
		CBIRD::stopSound();
		playWinSound();
		break;

	default:
		break;
	}
}

void CGAME::setState(GameState state) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	if (mState == state) {
		return;
	}
	GameState oldState = mState;
	mState = state;

	if (mState == GameState::PLAYING && oldState != GameState::PLAYING) {
		mTrafficClock.restart();
	}
	updateStateAudio(oldState, mState);
}

// ================================================================
// KIEM TRA VA CHAM & VE DICH
// ================================================================
bool CGAME::checkCollision() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	if (!cn.isAlive() || mGodMode) return false;

	// 1. Vehicle collisions
	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] != nullptr && cn.isImpact(axt[i])) {
			mLives--;
			cn.Die();
			playDieSound();
			return true;
		}
	}
	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] != nullptr && cn.isImpact(axh[i])) {
			mLives--;
			cn.Die();
			playDieSound();
			return true;
		}
	}

	// 2. Zombie collisions
	for (int i = 0; i < mNumBirds; i++) {
		if (ac[i] != nullptr && cn.isImpact(ac[i])) {
			mLives--;
			cn.Die();
			playDieSound();
			return true;
		}
	}
	return false;
}

bool CGAME::checkFinish() {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);
	if (cn.checkFinish(mFinishY)) {
		mScore += mLevel * 100;
		return true;
	}
	return false;
}

// ================================================================
// DRAW HUD
// ================================================================
static void DrawCrispText(RenderWindow& window, Text& text, float targetX, float targetY) {
	FloatRect bounds = text.getLocalBounds();
	text.setOrigin(Vector2f(
		std::round(bounds.position.x + bounds.size.x / 2.0f),
		std::round(bounds.position.y + bounds.size.y / 2.0f)));
	text.setPosition(Vector2f(std::round(targetX), std::round(targetY)));
	window.draw(text);
}

static void DrawTextWithShadow(RenderWindow& window, const Font& font, const string& str, unsigned int size, Color color, float targetX, float targetY) {
	Text shadow(font, str, size);
	shadow.setFillColor(Color(0, 0, 0, 230));
	DrawCrispText(window, shadow, targetX + 2.f, targetY + 2.f);

	Text mainText(font, str, size);
	mainText.setFillColor(color);
	DrawCrispText(window, mainText, targetX, targetY);
}

void CGAME::drawHUD(RenderWindow& window, Font& font) {
	RectangleShape banner(Vector2f((float)WINDOW_WIDTH, 44.f));
	banner.setPosition(Vector2f(0.f, 0.f));
	banner.setFillColor(Color(10, 14, 24, 245));
	banner.setOutlineThickness(2.f);
	banner.setOutlineColor(Color(65, 75, 100));
	window.draw(banner);

	DrawTextWithShadow(window, font, "LVL:" + to_string(mLevel), 14, Color::Yellow, 60.f, 22.f);
	DrawTextWithShadow(window, font, "SCORE:" + to_string(mScore), 14, Color::Cyan, 200.f, 22.f);

	string hearts = "";
	for (int i = 0; i < mLives; ++i) hearts += "+ ";
	DrawTextWithShadow(window, font, "HP:" + to_string(mLives) + " " + hearts, 14, Color(255, 90, 90), 380.f, 22.f);

	if (mGodMode) {
		DrawTextWithShadow(window, font, "[GOD MODE: ON]", 13, Color(255, 215, 0), 550.f, 22.f);
	}
	else {
		int totalDist = max(1, mStartY - mFinishY);
		int currentProgress = max(0, min(100, (100 * (mStartY - cn.getY())) / totalDist));
		DrawTextWithShadow(window, font, "STAGE:" + to_string(currentProgress) + "%", 13, Color(140, 220, 255), 550.f, 22.f);
	}

	string sndStr = mMusicEnabled ? "[M]MUS:ON" : "[M]MUS:OFF";
	DrawTextWithShadow(window, font, sndStr, 11, mMusicEnabled ? Color::Green : Color(160, 160, 160), (float)WINDOW_WIDTH - 250.f, 22.f);
	DrawTextWithShadow(window, font, "[P]PAUSE [`]DEBUG", 11, Color(210, 210, 220), (float)WINDOW_WIDTH - 110.f, 22.f);
}

// ================================================================
// DRAWGAME - Ve tro choi voi Camera View cuon doc 72 hang
// ================================================================
void CGAME::drawGame(RenderWindow& window, Font& font) {
	std::lock_guard<std::recursive_mutex> lock(mGameMutex);

	// 1. Calculate smooth camera view targeting the player
	float playerPixelY = CellToPixel(cn.getY());
	float targetCameraY = playerPixelY - (float)WINDOW_HEIGHT * 0.55f;
	float maxCameraY = (float)(MAP_PIXEL_HEIGHT - WINDOW_HEIGHT);
	targetCameraY = max(0.f, min(maxCameraY, targetCameraY));

	// Smooth dampening towards target
	mCameraY += (targetCameraY - mCameraY) * 0.22f;
	if (std::abs(targetCameraY - mCameraY) < 0.5f) {
		mCameraY = targetCameraY;
	}

	View gameView(FloatRect(Vector2f(0.f, std::round(mCameraY)), Vector2f((float)WINDOW_WIDTH, (float)WINDOW_HEIGHT)));
	window.setView(gameView);

	// 2. Render Tilemap Background (Only visible rows for efficiency)
	static Texture tilesetTex;
	static bool bgLoaded = false, bgTried = false;

	if (!bgTried) {
		bgTried = true;
		bgLoaded = tilesetTex.loadFromFile("Assets/images/environment(for-map)/background_base.png");
		if (bgLoaded) {
			tilesetTex.setSmooth(false);
		}
	}

	int startRenderRow = max(0, (int)(mCameraY / (float)CELL_SIZE) - 1);
	int endRenderRow = min(MAP_HEIGHT, (int)((mCameraY + (float)WINDOW_HEIGHT) / (float)CELL_SIZE) + 2);

	if (bgLoaded) {
		Sprite tileSprite(tilesetTex);
		const float scaleFactor = static_cast<float>(CELL_SIZE) / TILE_SIZE;
		tileSprite.setScale(Vector2f(scaleFactor, scaleFactor));
		for (int y = startRenderRow; y < endRenderRow; ++y) {
			for (int x = 0; x < SCREEN_WIDTH; ++x) {
				const int tileId = mTileMap[y][x];
				if (tileId < 0 || tileId >= TILE_COUNT) continue;

				tileSprite.setTextureRect(
					IntRect(Vector2i(tileId * TILE_SIZE, 0),
					        Vector2i(TILE_SIZE, TILE_SIZE)));
				tileSprite.setPosition(Vector2f(
					static_cast<float>(x * CELL_SIZE),
					static_cast<float>(y * CELL_SIZE)));
				window.draw(tileSprite);
			}
		}
	}

	// 3. Render Curbs (Ranh gioi giua via he va duong)
	for (int y = startRenderRow; y < endRenderRow; ++y) {
		if (y > 0 && mTileMap[y - 1][0] == TILE_BACKGROUND && mTileMap[y][0] == TILE_ROAD) {
			RectangleShape curb(Vector2f((float)WINDOW_WIDTH, 4.f));
			curb.setPosition(Vector2f(0.f, CellToPixel(y) - 2.f));
			curb.setFillColor(Color(220, 220, 230));
			window.draw(curb);
		}
		if (y < MAP_HEIGHT - 1 && mTileMap[y][0] == TILE_ROAD && mTileMap[y + 1][0] == TILE_BACKGROUND) {
			RectangleShape curb(Vector2f((float)WINDOW_WIDTH, 4.f));
			curb.setPosition(Vector2f(0.f, CellToPixel(y) + (float)CELL_SIZE - 2.f));
			curb.setFillColor(Color(220, 220, 230));
			window.draw(curb);
		}
	}

	// 4. Zebra Crossing at Start and Finish
	for (int x = 1; x < SCREEN_WIDTH - 1; x += 2) {
		RectangleShape zebraFinish(Vector2f((float)CELL_SIZE * 0.9f, (float)CELL_SIZE * 0.7f));
		zebraFinish.setPosition(Vector2f(CellToPixel(x), CellToPixel(mFinishY) + 3.f));
		zebraFinish.setFillColor(Color(255, 255, 255, 140));
		window.draw(zebraFinish);

		RectangleShape zebraStart(Vector2f((float)CELL_SIZE * 0.9f, (float)CELL_SIZE * 0.7f));
		zebraStart.setPosition(Vector2f(CellToPixel(x), CellToPixel(mStartY) + 3.f));
		zebraStart.setFillColor(Color(255, 255, 255, 140));
		window.draw(zebraStart);
	}

	// 5. Render Entities in World Coordinates
	for (int i = 0; i < mNumTrucks; ++i) {
		if (axt[i] != nullptr) axt[i]->Draw(window);
	}
	for (int i = 0; i < mNumCars; ++i) {
		if (axh[i] != nullptr) axh[i]->Draw(window);
	}
	for (int i = 0; i < mNumBirds; ++i) {
		if (ac[i] != nullptr) ac[i]->Draw(window);
	}

	// 6. Traffic Lights (Ve tat ca cac den giao thong o le duong dau moi khu vuc xe chay)
	for (int i = 0; i < mNumLights; ++i) {
		if (mLights[i] != nullptr) {
			int laneY = mLights[i]->getLaneY();
			int lx = 1;
			int ly = max(0, laneY - 2);
			mLights[i]->Draw(window, lx, ly);
		}
	}

	// 7. Player
	cn.Draw(window, font);

	// 8. Reset view to Default for Screen-Space HUD & UI
	window.setView(window.getDefaultView());
	drawHUD(window, font);
}

void CGAME::drawBuildingsZone(RenderWindow& /*window*/) {}
void CGAME::drawStreetZone(RenderWindow& /*window*/) {}

// ================================================================
// SAVEGAME / LOADGAME (Compatibility single file)
// ================================================================
void CGAME::saveGame(const string& filename) {
	ofstream ofs(filename);
	if (!ofs.is_open()) {
		playErrorSound();
		return;
	}

	ofs << mLevel << '\n';
	ofs << mScore << '\n';
	ofs << mLives << '\n';
	ofs << cn.getX() << '\n';
	ofs << cn.getY() << '\n';
	ofs << mStartY << '\n';
	ofs << mFinishY << '\n';

	ofs << mNumTrucks << '\n';
	for (int i = 0; i < mNumTrucks; i++) {
		ofs << (axt[i] ? axt[i]->getX() : 0) << '\n';
	}

	ofs << mNumCars << '\n';
	for (int i = 0; i < mNumCars; i++) {
		ofs << (axh[i] ? axh[i]->getX() : 0) << '\n';
	}

	ofs << mNumBirds << '\n';
	for (int i = 0; i < mNumBirds; i++) {
		ofs << (ac[i] ? ac[i]->getX() : 0) << '\n';
	}

	ofs << (mTrafficState == CTRAFFICLIGHT::RED ? 0 : 1) << '\n';
	ofs << mTrafficElapsedMs << '\n';
	ofs << mCameraY << '\n';

	ofs.close();
}

bool CGAME::loadGame(const string& filename) {
	ifstream ifs(filename);
	if (!ifs.is_open()) {
		playErrorSound();
		return false;
	}

	int level, score, live, px, py;
	ifs >> level;
	ifs >> score;
	ifs >> live;
	ifs >> px;
	ifs >> py;

	if (!ifs.good()) {
		ifs.close();
		playErrorSound();
		return false;
	}

	mLevel = level;
	mScore = score;
	mLives = live;

	int savedStartY = 68, savedFinishY = 2;
	if (ifs >> savedStartY >> savedFinishY) {
		mStartY = savedStartY;
		mFinishY = savedFinishY;
	}

	InitLanes();
	cn.Reset(px, py);

	int n, x;
	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumTrucks && axt[i]) axt[i]->setX(x);
		}
	}

	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumCars && axh[i]) axh[i]->setX(x);
		}
	}

	if (ifs >> n) {
		for (int i = 0; i < n; i++) {
			ifs >> x;
			if (i < mNumBirds && ac[i]) ac[i]->setX(x);
		}
	}

	int savedTrafficState = 1;
	int savedElapsedMs = 0;
	if (ifs >> savedTrafficState) {
		mTrafficState = (savedTrafficState == 0) ? CTRAFFICLIGHT::RED : CTRAFFICLIGHT::GREEN;
		if (ifs >> savedElapsedMs) {
			mTrafficElapsedMs = max(0, savedElapsedMs);
		}
		if (mLights[0] != nullptr) {
			mLights[0]->setState(mTrafficState);
		}
		const bool stopVehicles = (mTrafficState == CTRAFFICLIGHT::RED);
		for (int i = 0; i < mNumTrucks; i++) {
			if (axt[i]) {
				if (stopVehicles) axt[i]->Stop(99999);
				else axt[i]->Resume();
			}
		}
		for (int i = 0; i < mNumCars; i++) {
			if (axh[i]) {
				if (stopVehicles) axh[i]->Stop(99999);
				else axh[i]->Resume();
			}
		}
	}

	float savedCameraY = 0.f;
	if (ifs >> savedCameraY) {
		mCameraY = savedCameraY;
	}

	mTrafficClock.restart();
	updateTrafficAudio();

	ifs.close();
	return true;
}

// ================================================================
// MENU & POPUP RENDERING
// ================================================================
void CGAME::renderMenu(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;

	// 1. Menu Background
	if (mMenuAssetsLoaded) {
		Sprite bg(mTexMenuBg);
		FloatRect bgB = bg.getLocalBounds();
		if (bgB.size.x > 0.f && bgB.size.y > 0.f) {
			bg.setScale(Vector2f(winW / bgB.size.x, winH / bgB.size.y));
			window.draw(bg);
		}
	}
	else {
		RectangleShape darkBg(Vector2f(winW, winH));
		darkBg.setFillColor(Color(15, 15, 25));
		window.draw(darkBg);
	}

	// 2. Title Logo
	if (mMenuAssetsLoaded) {
		Sprite logo(mTexTitleLogo);
		FloatRect b = logo.getLocalBounds();
		if (b.size.x > 0.f && b.size.y > 0.f) {
			float targetW = 460.f;
			float scale = targetW / b.size.x;
			logo.setScale(Vector2f(scale, scale));
			logo.setOrigin(Vector2f(b.size.x / 2.0f, b.size.y / 2.0f));
			logo.setPosition(Vector2f(centerX, 110.f));
			window.draw(logo);
		}
	}
	else {
		DrawTextWithShadow(window, font, "CROSSING GAME", 28, Color::Yellow, centerX, 100.f);
	}

	// 3. Crisp Retro Arcade Menu Buttons
	static const string btnLabels[4] = {
		"1. PLAY GAME",
		"2. LOAD GAME",
		"3. SETTINGS",
		"4. EXIT GAME"
	};

	float bw = 330.f;
	float bh = 50.f;

	for (int i = 0; i < 4; ++i) {
		bool isSelected = (i == mMenuOption);
		float by = 235.f + (float)i * 68.f;

		RectangleShape btnBox(Vector2f(bw, bh));
		btnBox.setOrigin(Vector2f(bw / 2.0f, bh / 2.0f));
		btnBox.setPosition(Vector2f(centerX, by));

		if (isSelected) {
			btnBox.setFillColor(Color(25, 45, 80, 240));
			btnBox.setOutlineThickness(3.f);
			btnBox.setOutlineColor(Color(255, 215, 0)); // Bright Gold
		}
		else {
			btnBox.setFillColor(Color(14, 18, 30, 200)); // Darker non-selected
			btnBox.setOutlineThickness(2.f);
			btnBox.setOutlineColor(Color(65, 75, 100, 180));
		}
		window.draw(btnBox);

		string textStr = isSelected ? ("> " + btnLabels[i] + " <") : btnLabels[i];
		Color textColor = isSelected ? Color(255, 230, 80) : Color(200, 205, 220);
		DrawTextWithShadow(window, font, textStr, 15, textColor, centerX, by);
	}

	DrawTextWithShadow(window, font, "[W/S] or [UP/DOWN] Select  [ENTER] Confirm", 12, Color(200, 200, 220), centerX, winH - 24.f);
}

void CGAME::renderSettings(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	// 1. Dimmed Background
	if (mMenuAssetsLoaded) {
		Sprite bg(mTexMenuBg);
		FloatRect bgB = bg.getLocalBounds();
		if (bgB.size.x > 0.f && bgB.size.y > 0.f) {
			bg.setScale(Vector2f(winW / bgB.size.x, winH / bgB.size.y));
			bg.setColor(Color(80, 80, 95));
			window.draw(bg);
		}
	}
	else {
		RectangleShape darkBg(Vector2f(winW, winH));
		darkBg.setFillColor(Color(10, 10, 18));
		window.draw(darkBg);
	}

	// 2. High-Contrast Popup Panel Frame
	float pw = 660.f;
	float ph = 460.f;
	RectangleShape panelBox(Vector2f(pw, ph));
	panelBox.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
	panelBox.setPosition(Vector2f(centerX, centerY));
	panelBox.setFillColor(Color(12, 16, 28, 252));
	panelBox.setOutlineThickness(4.f);
	panelBox.setOutlineColor(Color(0, 210, 230)); // Cyan border
	window.draw(panelBox);

	// 3. Settings Title
	DrawTextWithShadow(window, font, "AUDIO & CONTROLS SETTINGS", 18, Color::Yellow, centerX, centerY - 190.f);

	// 4. Option 0: Music Audio Toggle & Volume Slider
	bool sel0 = (mSettingsOption == 0);
	float opt0Y = centerY - 130.f;
	RectangleShape boxOpt0(Vector2f(580.f, 48.f));
	boxOpt0.setOrigin(Vector2f(290.f, 24.f));
	boxOpt0.setPosition(Vector2f(centerX, opt0Y));
	boxOpt0.setFillColor(sel0 ? Color(25, 45, 80, 240) : Color(18, 22, 38, 200));
	boxOpt0.setOutlineThickness(sel0 ? 3.f : 1.5f);
	boxOpt0.setOutlineColor(sel0 ? Color::Yellow : Color(70, 80, 110));
	window.draw(boxOpt0);

	string musState = mMusicEnabled ? "[ ON ]" : "[ OFF ]";
	string musStr = "1. MUSIC: " + musState + "  < " + to_string((int)mMusicVolume) + "% >";
	if (sel0) musStr = "> " + musStr + " <";
	Color musCol = mMusicEnabled ? Color(80, 255, 80) : Color(255, 80, 80);
	DrawTextWithShadow(window, font, musStr, 13, musCol, centerX, opt0Y);

	// 5. Option 1: Sound FX Toggle & Volume Slider
	bool sel1 = (mSettingsOption == 1);
	float opt1Y = centerY - 72.f;
	RectangleShape boxOpt1(Vector2f(580.f, 48.f));
	boxOpt1.setOrigin(Vector2f(290.f, 24.f));
	boxOpt1.setPosition(Vector2f(centerX, opt1Y));
	boxOpt1.setFillColor(sel1 ? Color(25, 45, 80, 240) : Color(18, 22, 38, 200));
	boxOpt1.setOutlineThickness(sel1 ? 3.f : 1.5f);
	boxOpt1.setOutlineColor(sel1 ? Color::Yellow : Color(70, 80, 110));
	window.draw(boxOpt1);

	string sfxState = mSfxEnabled ? "[ ON ]" : "[ OFF ]";
	string sfxStr = "2. SOUND FX: " + sfxState + "  < " + to_string((int)mSfxVolume) + "% >";
	if (sel1) sfxStr = "> " + sfxStr + " <";
	Color sfxCol = mSfxEnabled ? Color(80, 255, 80) : Color(255, 80, 80);
	DrawTextWithShadow(window, font, sfxStr, 13, sfxCol, centerX, opt1Y);

	// 6. Option 2: Back to Menu
	bool sel2 = (mSettingsOption == 2);
	float opt2Y = centerY - 16.f;
	RectangleShape boxOpt2(Vector2f(580.f, 44.f));
	boxOpt2.setOrigin(Vector2f(290.f, 22.f));
	boxOpt2.setPosition(Vector2f(centerX, opt2Y));
	boxOpt2.setFillColor(sel2 ? Color(25, 45, 80, 240) : Color(18, 22, 38, 200));
	boxOpt2.setOutlineThickness(sel2 ? 3.f : 1.5f);
	boxOpt2.setOutlineColor(sel2 ? Color::Yellow : Color(70, 80, 110));
	window.draw(boxOpt2);

	string backStr = sel2 ? "> 3. BACK TO MAIN MENU <" : "3. BACK TO MAIN MENU";
	DrawTextWithShadow(window, font, backStr, 13, sel2 ? Color::Yellow : Color(200, 210, 230), centerX, opt2Y);

	// 7. Horizontal Divider Line
	RectangleShape div(Vector2f(580.f, 2.f));
	div.setOrigin(Vector2f(290.f, 1.f));
	div.setPosition(Vector2f(centerX, centerY + 18.f));
	div.setFillColor(Color(80, 90, 120));
	window.draw(div);

	// 8. Gameplay Controls Cheat Sheet (High Contrast, Sharp Text)
	DrawTextWithShadow(window, font, "- GAMEPLAY CONTROLS -", 12, Color::Cyan, centerX, centerY + 38.f);

	DrawTextWithShadow(window, font, "W, A, S, D  : Move Player (Hold to run)", 11, Color::White, centerX, centerY + 65.f);
	DrawTextWithShadow(window, font, "P           : Pause / Resume Game", 11, Color::White, centerX, centerY + 90.f);
	DrawTextWithShadow(window, font, "L / T       : Quick Save / Quick Load", 11, Color::White, centerX, centerY + 115.f);
	DrawTextWithShadow(window, font, "M           : Quick Mute / Unmute", 11, Color::White, centerX, centerY + 140.f);
	DrawTextWithShadow(window, font, "Traffic     : Green = GO | Red = STOP", 11, Color(255, 215, 0), centerX, centerY + 165.f);

	// 9. Footer prompt
	DrawTextWithShadow(window, font, "[ENTER] Toggle | [LEFT/RIGHT] Volume | [ESC] Back", 10, Color(160, 230, 255), centerX, centerY + 200.f);
}

// ================================================================
// 4-SLOT LOAD GAME MENU
// ================================================================
void CGAME::renderLoadMenu(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	if (mMenuAssetsLoaded) {
		Sprite bg(mTexMenuBg);
		FloatRect bgB = bg.getLocalBounds();
		if (bgB.size.x > 0.f && bgB.size.y > 0.f) {
			bg.setScale(Vector2f(winW / bgB.size.x, winH / bgB.size.y));
			bg.setColor(Color(80, 80, 95));
			window.draw(bg);
		}
	}
	else {
		RectangleShape darkBg(Vector2f(winW, winH));
		darkBg.setFillColor(Color(10, 10, 18));
		window.draw(darkBg);
	}

	float pw = 680.f;
	float ph = 470.f;
	RectangleShape panelBox(Vector2f(pw, ph));
	panelBox.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
	panelBox.setPosition(Vector2f(centerX, centerY));
	panelBox.setFillColor(Color(12, 16, 28, 252));
	panelBox.setOutlineThickness(4.f);
	panelBox.setOutlineColor(Color(0, 210, 230)); // Cyan border
	window.draw(panelBox);

	DrawTextWithShadow(window, font, "LOAD GAME - SELECT SLOT", 18, Color::Yellow, centerX, centerY - 195.f);

	float sw = 600.f;
	float sh = 64.f;

	for (int i = 0; i < 4; ++i) {
		bool isSelected = (i == mSelectedSlot);
		float sy = centerY - 120.f + (float)i * 74.f;

		RectangleShape slotBox(Vector2f(sw, sh));
		slotBox.setOrigin(Vector2f(sw / 2.0f, sh / 2.0f));
		slotBox.setPosition(Vector2f(centerX, sy));

		if (isSelected) {
			slotBox.setFillColor(Color(25, 45, 80, 245));
			slotBox.setOutlineThickness(3.f);
			slotBox.setOutlineColor(Color(255, 215, 0)); // Gold
		}
		else {
			slotBox.setFillColor(Color(16, 20, 32, 210));
			slotBox.setOutlineThickness(1.5f);
			slotBox.setOutlineColor(Color(65, 75, 100));
		}
		window.draw(slotBox);

		SaveSlotInfo info = getSlotInfo(i);
		string slotHeader = "SLOT " + to_string(i + 1);
		if (isSelected) slotHeader = "> " + slotHeader;

		if (info.exists) {
			string hearts = "";
			for (int h = 0; h < info.lives; ++h) hearts += "+ ";
			string line1 = slotHeader + " | LVL: " + to_string(info.level) + " | HP: " + to_string(info.lives) + " " + hearts + "| SCORE: " + to_string(info.score);
			string line2 = "TIME: " + info.timeStr;

			DrawTextWithShadow(window, font, line1, 12, isSelected ? Color(255, 230, 80) : Color::White, centerX, sy - 12.f);
			DrawTextWithShadow(window, font, line2, 10, Color(160, 220, 255), centerX, sy + 14.f);
		}
		else {
			string emptyStr = slotHeader + " | [ --- EMPTY / NO DATA --- ]";
			DrawTextWithShadow(window, font, emptyStr, 12, isSelected ? Color(255, 215, 0) : Color(130, 135, 150), centerX, sy);
		}
	}

	DrawTextWithShadow(window, font, "[W/S] Select | [1..4] Direct | [ENTER] Load | [ESC] Back", 10, Color(160, 230, 255), centerX, centerY + 205.f);
}

// ================================================================
// 4-SLOT SAVE GAME MENU
// ================================================================
void CGAME::renderSaveMenu(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	if (mMenuAssetsLoaded) {
		Sprite bg(mTexMenuBg);
		FloatRect bgB = bg.getLocalBounds();
		if (bgB.size.x > 0.f && bgB.size.y > 0.f) {
			bg.setScale(Vector2f(winW / bgB.size.x, winH / bgB.size.y));
			bg.setColor(Color(80, 80, 95));
			window.draw(bg);
		}
	}
	else {
		RectangleShape darkBg(Vector2f(winW, winH));
		darkBg.setFillColor(Color(10, 10, 18));
		window.draw(darkBg);
	}

	float pw = 680.f;
	float ph = 470.f;
	RectangleShape panelBox(Vector2f(pw, ph));
	panelBox.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
	panelBox.setPosition(Vector2f(centerX, centerY));
	panelBox.setFillColor(Color(12, 16, 28, 252));
	panelBox.setOutlineThickness(4.f);
	panelBox.setOutlineColor(Color(255, 215, 0)); // Gold border
	window.draw(panelBox);

	DrawTextWithShadow(window, font, "SAVE GAME - CHOOSE SLOT TO OVERWRITE", 16, Color::Yellow, centerX, centerY - 195.f);

	float sw = 600.f;
	float sh = 64.f;

	for (int i = 0; i < 4; ++i) {
		bool isSelected = (i == mSelectedSlot);
		float sy = centerY - 120.f + (float)i * 74.f;

		RectangleShape slotBox(Vector2f(sw, sh));
		slotBox.setOrigin(Vector2f(sw / 2.0f, sh / 2.0f));
		slotBox.setPosition(Vector2f(centerX, sy));

		if (isSelected) {
			slotBox.setFillColor(Color(25, 45, 80, 245));
			slotBox.setOutlineThickness(3.f);
			slotBox.setOutlineColor(Color(255, 215, 0)); // Gold
		}
		else {
			slotBox.setFillColor(Color(16, 20, 32, 210));
			slotBox.setOutlineThickness(1.5f);
			slotBox.setOutlineColor(Color(65, 75, 100));
		}
		window.draw(slotBox);

		SaveSlotInfo info = getSlotInfo(i);
		string slotHeader = "SLOT " + to_string(i + 1);
		if (isSelected) slotHeader = "> " + slotHeader;

		if (info.exists) {
			string hearts = "";
			for (int h = 0; h < info.lives; ++h) hearts += "+ ";
			string line1 = slotHeader + " | LVL: " + to_string(info.level) + " | HP: " + to_string(info.lives) + " " + hearts + "| SCORE: " + to_string(info.score);
			string line2 = "SAVED AT: " + info.timeStr;

			DrawTextWithShadow(window, font, line1, 12, isSelected ? Color(255, 230, 80) : Color::White, centerX, sy - 12.f);
			DrawTextWithShadow(window, font, line2, 10, Color(160, 220, 255), centerX, sy + 14.f);
		}
		else {
			string emptyStr = slotHeader + " | [ --- EMPTY / SAVE NEW DATA --- ]";
			DrawTextWithShadow(window, font, emptyStr, 12, isSelected ? Color(255, 215, 0) : Color(130, 135, 150), centerX, sy);
		}
	}

	DrawTextWithShadow(window, font, "[W/S] Select | [1..4] Direct | [ENTER] Save | [ESC] Cancel", 10, Color(160, 230, 255), centerX, centerY + 205.f);
}

void CGAME::renderPauseMsg(RenderWindow& window, Font& font) {
	float centerX = (float)WINDOW_WIDTH / 2.0f;
	float centerY = (float)WINDOW_HEIGHT / 2.0f;

	float pw = 480.f, ph = 240.f;
	RectangleShape box(Vector2f(pw, ph));
	box.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
	box.setPosition(Vector2f(centerX, centerY));
	box.setFillColor(Color(12, 18, 32, 245));
	box.setOutlineThickness(4.f);
	box.setOutlineColor(Color(80, 150, 255));
	window.draw(box);

	DrawTextWithShadow(window, font, "GAME PAUSED", 26, Color(100, 180, 255), centerX, centerY - 50.f);
	DrawTextWithShadow(window, font, "[P] Resume Game", 14, Color::Yellow, centerX, centerY + 5.f);
	DrawTextWithShadow(window, font, "[ESC] Return to Menu", 13, Color(200, 200, 210), centerX, centerY + 50.f);
}

void CGAME::renderDeadMsg(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	if (mMenuAssetsLoaded) {
		Sprite panel(mTexPanelPopup);
		FloatRect pb = panel.getLocalBounds();
		if (pb.size.x > 0.f && pb.size.y > 0.f) {
			panel.setOrigin(Vector2f(pb.size.x / 2.0f, pb.size.y / 2.0f));
			panel.setScale(Vector2f(1.1f, 0.95f));
			panel.setPosition(Vector2f(centerX, centerY));
			window.draw(panel);
		}
	}
	else {
		float pw = 520.f, ph = 320.f;
		RectangleShape box(Vector2f(pw, ph));
		box.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
		box.setPosition(Vector2f(centerX, centerY));
		box.setFillColor(Color(25, 10, 15, 245));
		box.setOutlineThickness(4.f);
		box.setOutlineColor(Color::Red);
		window.draw(box);
	}

	DrawTextWithShadow(window, font, "YOU DIED!", 28, Color::Red, centerX, centerY - 80.f);
	DrawTextWithShadow(window, font, "LIVES REMAINING: " + to_string(mLives), 14, Color::Yellow, centerX, centerY - 20.f);
	DrawTextWithShadow(window, font, "[Y / ENTER] Continue Game", 13, Color::White, centerX, centerY + 35.f);
	DrawTextWithShadow(window, font, "[ESC] Return to Menu", 11, Color(180, 180, 190), centerX, centerY + 75.f);
}

void CGAME::renderLevelUp(RenderWindow& window, Font& font) {
	float centerX = (float)WINDOW_WIDTH / 2.0f;
	float centerY = (float)WINDOW_HEIGHT / 2.0f;

	if (mMenuAssetsLoaded) {
		Sprite panel(mTexPanelPopup);
		FloatRect pb = panel.getLocalBounds();
		if (pb.size.x > 0.f && pb.size.y > 0.f) {
			panel.setOrigin(Vector2f(pb.size.x / 2.0f, pb.size.y / 2.0f));
			panel.setScale(Vector2f(1.0f, 0.8f));
			panel.setPosition(Vector2f(centerX, centerY));
			window.draw(panel);
		}
	}

	DrawTextWithShadow(window, font, "LEVEL COMPLETED!", 22, Color::Magenta, centerX, centerY - 25.f);
	DrawTextWithShadow(window, font, "NEXT: LEVEL " + to_string(mLevel), 16, Color::Yellow, centerX, centerY + 30.f);
}

void CGAME::renderWin(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	// 1. Dimmed Background
	if (mMenuAssetsLoaded) {
		Sprite bg(mTexMenuBg);
		FloatRect bgB = bg.getLocalBounds();
		if (bgB.size.x > 0.f && bgB.size.y > 0.f) {
			bg.setScale(Vector2f(winW / bgB.size.x, winH / bgB.size.y));
			bg.setColor(Color(90, 85, 110, 220));
			window.draw(bg);
		}
	}

	// 2. Popup Panel
	if (mMenuAssetsLoaded) {
		Sprite panel(mTexPanelPopup);
		FloatRect pb = panel.getLocalBounds();
		if (pb.size.x > 0.f && pb.size.y > 0.f) {
			panel.setOrigin(Vector2f(pb.size.x / 2.0f, pb.size.y / 2.0f));
			panel.setScale(Vector2f(1.22f, 1.12f));
			panel.setPosition(Vector2f(centerX, centerY));
			window.draw(panel);
		}
	}
	else {
		float pw = 600.f, ph = 400.f;
		RectangleShape box(Vector2f(pw, ph));
		box.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
		box.setPosition(Vector2f(centerX, centerY));
		box.setFillColor(Color(18, 16, 30, 250));
		box.setOutlineThickness(4.f);
		box.setOutlineColor(Color::Yellow);
		window.draw(box);
	}

	// 3. Victory Header
	DrawTextWithShadow(window, font, "VICTORY!", 28, Color::Yellow, centerX, centerY - 130.f);
	DrawTextWithShadow(window, font, "YOU BEAT ALL 5 LEVELS!", 13, Color::Cyan, centerX, centerY - 80.f);

	// 4. Stats Container
	RectangleShape statBox(Vector2f(480.f, 88.f));
	statBox.setOrigin(Vector2f(240.f, 44.f));
	statBox.setPosition(Vector2f(centerX, centerY - 5.f));
	statBox.setFillColor(Color(20, 25, 45, 230));
	statBox.setOutlineThickness(2.f);
	statBox.setOutlineColor(Color(255, 215, 0, 200));
	window.draw(statBox);

	DrawTextWithShadow(window, font, "FINAL SCORE : " + to_string(mScore), 15, Color::White, centerX, centerY - 22.f);
	DrawTextWithShadow(window, font, "LIVES SAVED : " + to_string(mLives) + " / 3", 13, Color(80, 255, 80), centerX, centerY + 14.f);

	// 5. Footer Prompt
	DrawTextWithShadow(window, font, "[ENTER] or [SPACE] Return to Menu", 12, Color(255, 230, 100), centerX, centerY + 95.f);
}

void CGAME::renderGameOver(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	if (mMenuAssetsLoaded) {
		Sprite panel(mTexPanelPopup);
		FloatRect pb = panel.getLocalBounds();
		if (pb.size.x > 0.f && pb.size.y > 0.f) {
			panel.setOrigin(Vector2f(pb.size.x / 2.0f, pb.size.y / 2.0f));
			panel.setScale(Vector2f(1.15f, 1.0f));
			panel.setPosition(Vector2f(centerX, centerY));
			window.draw(panel);
		}
	}
	else {
		float pw = 540.f, ph = 340.f;
		RectangleShape box(Vector2f(pw, ph));
		box.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
		box.setPosition(Vector2f(centerX, centerY));
		box.setFillColor(Color(25, 10, 15, 250));
		box.setOutlineThickness(4.f);
		box.setOutlineColor(Color::Red);
		window.draw(box);
	}

	DrawTextWithShadow(window, font, "GAME OVER", 30, Color::Red, centerX, centerY - 90.f);
	DrawTextWithShadow(window, font, "FINAL SCORE: " + to_string(mScore), 15, Color::White, centerX, centerY - 20.f);
	DrawTextWithShadow(window, font, "[ENTER] or [SPACE] Return to Menu", 12, Color(200, 200, 200), centerX, centerY + 65.f);
}

void CGAME::renderDebugMenu(RenderWindow& window, Font& font) {
	float winW = (float)WINDOW_WIDTH;
	float winH = (float)WINDOW_HEIGHT;
	float centerX = winW / 2.0f;
	float centerY = winH / 2.0f;

	// Dark semi-transparent overlay
	RectangleShape dimBg(Vector2f(winW, winH));
	dimBg.setFillColor(Color(0, 0, 0, 175));
	window.draw(dimBg);

	// High-contrast Cyberpunk Debug Console Box
	float pw = 680.f;
	float ph = 420.f;
	RectangleShape panel(Vector2f(pw, ph));
	panel.setOrigin(Vector2f(pw / 2.0f, ph / 2.0f));
	panel.setPosition(Vector2f(centerX, centerY));
	panel.setFillColor(Color(10, 15, 26, 250));
	panel.setOutlineThickness(3.f);
	panel.setOutlineColor(Color(0, 240, 255)); // Cyan / Neon border
	window.draw(panel);

	// Console Header
	DrawTextWithShadow(window, font, "[ DEVELOPER DEBUG CONSOLE ]", 16, Color::Yellow, centerX, centerY - 170.f);

	string opt0 = "1. GOD MODE (NO HITBOX): " + string(mGodMode ? "[ ON - INVINCIBLE ]" : "[ OFF ]");
	string opt1 = "2. JUMP TO NEXT LEVEL (+1 LVL)";
	string opt2 = "3. JUMP TO PREVIOUS LEVEL (-1 LVL)";
	string opt3 = "4. ADD +1 EXTRA LIFE (HP: " + to_string(mLives) + ")";
	string opt4 = "5. TELEPORT TO FINISH LINE (AUTO-WIN)";
	string opt5 = "6. CLOSE DEBUG MENU [` or ESC]";

	const string opts[6] = { opt0, opt1, opt2, opt3, opt4, opt5 };

	for (int i = 0; i < 6; ++i) {
		bool sel = (i == mDebugMenuOption);
		float oy = centerY - 115.f + (float)i * 44.f;

		RectangleShape btn(Vector2f(620.f, 38.f));
		btn.setOrigin(Vector2f(310.f, 19.f));
		btn.setPosition(Vector2f(centerX, oy));

		if (sel) {
			btn.setFillColor(Color(25, 55, 95, 240));
			btn.setOutlineThickness(2.5f);
			btn.setOutlineColor(Color(255, 215, 0));
		}
		else {
			btn.setFillColor(Color(16, 22, 38, 190));
			btn.setOutlineThickness(1.5f);
			btn.setOutlineColor(Color(60, 75, 105));
		}
		window.draw(btn);

		string textStr = sel ? ("> " + opts[i] + " <") : opts[i];
		Color textCol = Color(200, 210, 230);
		if (i == 0) {
			textCol = mGodMode ? Color(80, 255, 80) : Color(255, 100, 100);
			if (sel) textCol = mGodMode ? Color(130, 255, 130) : Color(255, 130, 130);
		}
		else if (sel) {
			textCol = Color(255, 235, 90);
		}

		DrawTextWithShadow(window, font, textStr, 12, textCol, centerX, oy);
	}

	// Hotkey hints
	DrawTextWithShadow(window, font, "[W/S] Select  [ENTER] Toggle/Execute  [1] God Mode  [`] Close", 11, Color(160, 180, 210), centerX, centerY + 175.f);
}
