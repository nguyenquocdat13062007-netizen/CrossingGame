// ================================================================
// CGame.cpp - Tich hop: logic game + Menu + SaveLoad
//
// Ly do tich hop:
//   Do an yeu cau CGAME la trung tam dieu phoi duy nhat.
//   Menu.cpp va SaveLoad.cpp duoc ghep vao day de tuan theo
//   yeu cau "khong tach file tuy chon ra ngoai class chinh".
// ================================================================
#include "CGame.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
using namespace std;
using namespace sf;

namespace {
constexpr int TILE_SIZE = 48;
constexpr int TILE_COUNT = 2;
constexpr int TILE_BACKGROUND = 0;
constexpr int TILE_ROAD = 1;
constexpr const char* DEFAULT_MAP_PATH = "Assets/map.txt";

bool readTileMap(const string& filename,
                 int (&tiles)[SCREEN_HEIGHT][SCREEN_WIDTH]) {
	ifstream input(filename);
	if (!input.is_open()) {
		return false;
	}

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
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
      mState(GameState::MENU),
      mTrafficState(CTRAFFICLIGHT::GREEN),
      mGreenDurationMs(3000), mRedDurationMs(2000),
      mTrafficStartLoaded(false), mTrafficStoppingLoaded(false)
// khoi tao constructor co dinh voi level 1 0 diem 3 mang 0 xe 0 animal va xuat hien o giao dien menu dau tien
{
	for (int i = 0; i < MAX_TRUCKS; i++)
		axt[i] = nullptr;
	for (int i = 0; i < MAX_CARS; i++)
		axh[i] = nullptr;
	for (int i = 0; i < MAX_DINOS; i++)
		akl[i] = nullptr;
	for (int i = 0; i < MAX_BIRDS; i++)
		ac[i] = nullptr;
	for (int i = 0; i < MAX_TRUCKS + MAX_CARS; i++)
		mLights[i] = nullptr;

	loadMapFromFile("Assets/map.txt");

	mTrafficStartLoaded = mTrafficStart.openFromFile(
		"Assets/music/Traffic-Start.wav");
	mTrafficStoppingLoaded = mTrafficStopping.openFromFile(
		"Assets/music/Traffic-Stopping.wav");
	if (mTrafficStartLoaded) {
		mTrafficStart.setLooping(true);
	} else {
		cerr << "Could not load Assets/music/Traffic-Start.wav.\n";
	}
	if (mTrafficStoppingLoaded) {
		mTrafficStopping.setLooping(true);
	} else {
		cerr << "Could not load Assets/music/Traffic-Stopping.wav.\n";
	}
}

CGAME::~CGAME() {
	for (int i = 0; i < MAX_TRUCKS; i++)
		delete axt[i];
	for (int i = 0; i < MAX_CARS; i++)
		delete axh[i];
	for (int i = 0; i < MAX_DINOS; i++)
		delete akl[i];
	for (int i = 0; i < MAX_BIRDS; i++)
		delete ac[i];
	for (int i = 0; i < MAX_TRUCKS + MAX_CARS; i++)
		delete mLights[i];
}

// ================================================================
// GETTER DANH SACH XE / THU (theo yeu cau do an)
// getVehicle() -> CVEHICLE**
// getAnimal()  -> CANIMAL**
// ================================================================
CVEHICLE** CGAME::getVehicle() {
	// Tra ve mang con tro axt axh bang mang khac 
	static CVEHICLE* a[MAX_TRUCKS + MAX_CARS];

	int idx = 0;
	for (int i = 0; i < mNumTrucks; i++)
		a[idx++] = axt[i];
	for (int i = 0; i < mNumCars; i++)
		a[idx++] = axh[i];
	return a;
}

CANIMAL** CGAME::getAnimal() {
	static CANIMAL* a[MAX_DINOS + MAX_BIRDS];

	int idx = 0;
	for (int i = 0; i < mNumDinos; i++)
		a[idx++] = akl[i];
	for (int i = 0; i < mNumBirds; i++)
		a[idx++] = ac[i];
	return a;
}

// ================================================================
// GET LANE Y
// ================================================================
int CGAME::GetLaneY(int laneIndex) {
	int laneHeight = (ROAD_BOTTOM - ROAD_TOP) / (LANE_COUNT + 1);
	return ROAD_TOP + (laneIndex + 1) * laneHeight;
}

// ================================================================
// INIT LANES - Tao xe/thu/den theo mLevel
// ================================================================
void CGAME::InitLanes() {
	loadMapFromFile("Assets/map_level" + std::to_string(mLevel) + ".txt");

	for (int i = 0; i < MAX_TRUCKS; i++)
	{
		delete axt[i];
		axt[i] = nullptr;
	}
	for (int i = 0; i < MAX_CARS; i++)
	{
		delete axh[i];
		axh[i] = nullptr;
	}
	for (int i = 0; i < MAX_BIRDS; i++)
	{
		delete ac[i];
		ac[i] = nullptr;
	}
	for (int i = 0; i < MAX_DINOS; i++)
	{
		delete akl[i];
		akl[i] = nullptr;
	}
	for (int i = 0; i < MAX_CARS + MAX_TRUCKS; i++)
	{
		delete mLights[i];
		mLights[i] = nullptr;
	}
	struct LaneSetup { int trucks, cars, dinos, birds; };
	static const LaneSetup setups[] = {
		{1, 2, 1, 1}, {2, 2, 1, 1}, {1, 3, 2, 2}, {3, 2, 2, 1}, {3, 3, 2, 0},
	};
	const int NumSetups = (int)(sizeof(setups) / sizeof(setups[0]));
	int idx = max(0, min(mLevel - 1, NumSetups - 1));
	mNumTrucks = min(setups[idx].trucks, MAX_TRUCKS);
	mNumCars = min(setups[idx].cars, MAX_CARS);
	mNumBirds = min(setups[idx].birds, MAX_BIRDS);
	mNumDinos = min(setups[idx].dinos, MAX_DINOS);
	// Dam bao tong lane <= LANE_COUNT
	int used = 0;
	mNumTrucks = min(mNumTrucks, LANE_COUNT - used);
	used += mNumTrucks;
	mNumCars = min(mNumCars, LANE_COUNT - used);
	used += mNumCars;
	mNumBirds = min(mNumBirds, LANE_COUNT - used);
	used += mNumBirds;
	mNumDinos = min(mNumDinos, LANE_COUNT - used);
	used += mNumDinos;
	int BaseSpeed = 1 + (mLevel - 1) / 2;

	for (int i = 0; i < mNumTrucks; i++)
	{
		int y = GetLaneY(i);
		int dir = (i % 2 == 0) ? 1 : -1;
		int x = (dir == 1) ? 0 : SCREEN_WIDTH - 3;
		axt[i] = new CTRUCK(x, y, BaseSpeed, dir);
		mLights[i] = new CTRAFFICLIGHT(y);
	}

	for (int i = 0; i < mNumCars; i++)
	{
		int y = GetLaneY(mNumTrucks + i);
		int dir = (i % 2 == 0) ? 1 : -1;
		int x = (dir == 1) ? 0 : SCREEN_WIDTH - 2;
		axh[i] = new CCAR(x, y, BaseSpeed + 1, dir);
		mLights[mNumTrucks + i] = new CTRAFFICLIGHT(y);
	}

	for (int i = 0; i < mNumDinos; i++)
	{
		int y = GetLaneY(mNumTrucks + mNumCars + i);
		int dir = (i % 2 == 0) ? 1 : -1;
		int x = (dir == 1) ? 0 : SCREEN_WIDTH - 3;
		akl[i] = new CROCK(x, y, BaseSpeed, dir);
	}

	for (int i = 0; i < mNumBirds; i++)
	{
		int y = GetLaneY(mNumDinos + mNumTrucks + mNumCars + i);
		int dir = (i % 2 == 0) ? -1 : 1;
		int x = (dir == 1) ? 0 : SCREEN_WIDTH - 2;
		ac[i] = new CBIRD(x, y, BaseSpeed, dir);
	}

	mTrafficState = CTRAFFICLIGHT::GREEN;
	mGreenDurationMs = max(1000, 3000 - mLevel * 200);
	mRedDurationMs = 2000;
	mTrafficClock.restart();
	loadAllAssets();
	updateTrafficAudio();
}

// ================================================================
// LOAD MAP FROM FILE - Nap ma tran ban do tu file text
// ================================================================
void CGAME::loadMapFromFile(const string& filename) {
	int loadedMap[SCREEN_HEIGHT][SCREEN_WIDTH]{};
	bool loaded = readTileMap(filename, loadedMap);

	if (!loaded && filename != DEFAULT_MAP_PATH) {
		cerr << "Invalid map '" << filename
		     << "'; falling back to " << DEFAULT_MAP_PATH << ".\n";
		loaded = readTileMap(DEFAULT_MAP_PATH, loadedMap);
	}

	if (!loaded) {
		cerr << "Could not load a valid tile map; using the built-in map.\n";
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			for (int x = 0; x < SCREEN_WIDTH; ++x) {
				loadedMap[y][x] =
					(y >= ROAD_TOP && y <= ROAD_BOTTOM)
						? TILE_ROAD
						: TILE_BACKGROUND;
			}
		}
	}

	for (int y = 0; y < SCREEN_HEIGHT; ++y) {
		for (int x = 0; x < SCREEN_WIDTH; ++x) {
			mTileMap[y][x] = loadedMap[y][x];
		}
	}
}

// ================================================================
// LOAD ALL ASSETS - Gan PNG cho tung doi tuong
// ================================================================
void CGAME::loadAllAssets() {
	cn.loadAssets("Assets/images/player/run-up-1.png", "Assets/images/player/run-up-2.png");
	for (int i = 0; i < mNumTrucks; i++)
	{
		if (axt[i] != nullptr)
		{
			axt[i]->loadAssets("Assets/images/vehicle/truck.png", "Assets/images/vehicle/truck2.png");
		}
	}
	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr)
		{
			axh[i]->loadAssets("Assets/images/vehicle/car.png", "Assets/images/vehicle/car2.png");
		}
	}
	for (int i = 0; i < mNumDinos; i++)
	{
		if (akl[i] != nullptr)
		{
			akl[i]->loadAssets("Assets/images/other/rock1.png", "Assets/images/other/rock2.png");
		}
	}
	for (int i = 0; i < mNumBirds; i++)
	{
		if (ac[i] != nullptr)
		{
			ac[i]->loadAssets("Assets/images/entities/bird1.png", "Assets/images/entities/bird2.png");
		}
	}
	const vector<string> trafficGreenFrames = {
		"Assets/images/environment(for-map)/light1.png",
		"Assets/images/environment(for-map)/light2.png",
		"Assets/images/environment(for-map)/light3.png",
		"Assets/images/environment(for-map)/light4.png"
	};
	for (int i = 0; i < mNumTrucks + mNumCars; i++)
	{
		if (mLights[i] != nullptr)
		{
			if (!mLights[i]->loadAssets(
				"Assets/images/environment(for-map)/lightstop.png",
				trafficGreenFrames)) {
				cerr << "Could not load traffic-light assets for lane "
				     << mLights[i]->getLaneY() << ".\n";
			}
			mLights[i]->setState(mTrafficState);
		}
	}
}

// ================================================================
// UPDATE ANIMATIONS
// ================================================================
void CGAME::updateAnimations(float dt) {
	cn.updateAnim(dt);
	for (int i = 0; i < mNumTrucks; i++)
	{
		if (axt[i] != nullptr)
		{
			axt[i]->updateAnim(dt);
		}
	}

	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr)
		{
			axh[i]->updateAnim(dt);
		}
	}

	for (int i = 0; i < mNumDinos; i++)
	{
		if (akl[i] != nullptr)
		{
			akl[i]->updateAnim(dt);
		}
	}

	for (int i = 0; i < mNumBirds; i++)
	{
		if (ac[i] != nullptr)
		{
			ac[i]->updateAnim(dt);
		}
	}

	for (int i = 0; i < mNumTrucks + mNumCars; i++)
	{
		if (mLights[i] != nullptr)
		{
			mLights[i]->updateAnimation(dt);
		}
	}
}

// ================================================================
// INIT / STARTGAME / RESETGAME / NEXTLEVEL
// startGame() = yeu cau do an: "Bat dau vao tro choi"
// ================================================================
void CGAME::Init() {
	mLevel = 1; mScore = 0; mLives = 3;
	cn.Reset();
	InitLanes();
}

void CGAME::startGame() {
	mLevel = 1; mScore = 0; mLives = 3;
	cn.Reset();
	InitLanes();
	setState(GameState::PLAYING);
}

void CGAME::resetGame() {
	mLevel = 1; mScore = 0; mLives = 3;
	cn.Reset();
	InitLanes();
	setState(GameState::MENU);
}

void CGAME::nextLevel() {
	mLevel++;
	cn.Reset();
	InitLanes();
	setState(GameState::PLAYING);
}

// ================================================================
// PAUSE / RESUME / EXIT THREAD
// Theo yeu cau do an: pauseGame(HANDLE), resumeGame(HANDLE), exitGame(HANDLE)
// ================================================================
void CGAME::pauseGame(HANDLE hThread) {
	SuspendThread(hThread);
	setState(GameState::PAUSED);
}

void CGAME::resumeGame(HANDLE hThread) {
	if (mState == GameState::PAUSED)
	{
		setState(GameState::PLAYING);
		ResumeThread(hThread);
	}
}

void CGAME::exitGame(HANDLE hThread) {
	IS_RUNNING = false;
	stopTrafficAudio();
	SuspendThread(hThread);
}

// ================================================================
// CAP NHAT VI TRI
// Theo yeu cau do an: updatePosPeople(char), updatePosVehicle(), updatePosAnimal()
// ================================================================
void CGAME::updatePosPeople(char key) {
	if (cn.isAlive()) return;
	switch (key) {
	case 'W': cn.Up(); break;
	case 'S': cn.Down(); break;
	case 'A': cn.Left(); break;
	case 'D': cn.Right(); break;
	default: break;
	}

}

void CGAME::updatePosVehicle() {
	for (int i = 0; i < mNumTrucks; i++)
	{
		if (axt[i] != nullptr)
		{
			axt[i]->Move(SCREEN_WIDTH);
		}
	}
	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr)
		{
			axh[i]->Move(SCREEN_WIDTH);
		}
	}
}

void CGAME::updatePosAnimal() {
	for (int i = 0; i < mNumDinos; i++)
	{
		if (akl[i] != nullptr)
		{
			akl[i]->Move(SCREEN_WIDTH);
			akl[i]->Tell();
		}
	}
	for (int i = 0; i < mNumBirds; i++)
	{
		if (ac[i] != nullptr)
		{
			ac[i]->Move(SCREEN_WIDTH);
			ac[i]->Tell();
		}
	}
}

void CGAME::updateTrafficLights() {
	const int durationMs = mTrafficState == CTRAFFICLIGHT::GREEN
		? mGreenDurationMs
		: mRedDurationMs;
	if (mTrafficClock.getElapsedTime().asMilliseconds() < durationMs) {
		return;
	}

	mTrafficState = mTrafficState == CTRAFFICLIGHT::GREEN
		? CTRAFFICLIGHT::RED
		: CTRAFFICLIGHT::GREEN;
	mTrafficClock.restart();

	for (int i = 0; i < mNumTrucks + mNumCars; i++) {
		if (mLights[i] != nullptr) {
			mLights[i]->setState(mTrafficState);
		}
	}

	const bool stopVehicles = mTrafficState == CTRAFFICLIGHT::RED;
	for (int i = 0; i < mNumTrucks; i++) {
		if (axt[i] == nullptr) continue;
		if (stopVehicles) axt[i]->Stop(99999);
		else axt[i]->Resume();
	}
	for (int i = 0; i < mNumCars; i++) {
		if (axh[i] == nullptr) continue;
		if (stopVehicles) axh[i]->Stop(99999);
		else axh[i]->Resume();
	}
	updateTrafficAudio();
}

void CGAME::stopTrafficAudio() {
	if (mTrafficStartLoaded) mTrafficStart.stop();
	if (mTrafficStoppingLoaded) mTrafficStopping.stop();
}

void CGAME::updateTrafficAudio() {
	if (mState != GameState::PLAYING) {
		stopTrafficAudio();
		return;
	}

	Music* active = mTrafficState == CTRAFFICLIGHT::GREEN
		? &mTrafficStart
		: &mTrafficStopping;
	Music* inactive = mTrafficState == CTRAFFICLIGHT::GREEN
		? &mTrafficStopping
		: &mTrafficStart;
	const bool activeLoaded = mTrafficState == CTRAFFICLIGHT::GREEN
		? mTrafficStartLoaded
		: mTrafficStoppingLoaded;

	if (mTrafficStartLoaded && inactive == &mTrafficStart) mTrafficStart.stop();
	if (mTrafficStoppingLoaded && inactive == &mTrafficStopping) mTrafficStopping.stop();
	if (activeLoaded && active->getStatus() != SoundSource::Status::Playing) {
		active->play();
	}
}

void CGAME::setState(GameState state) {
	if (mState == state) {
		return;
	}
	mState = state;
	updateTrafficAudio();
}

// ================================================================
// KIEM TRA TRANG THAI
// isImpact theo yeu cau do an: isImpact(getVehicle()), isImpact(getAnimal())
// ================================================================
bool CGAME::checkCollision() {
	if (!cn.isAlive())  return false;
	for (int i = 0; i < mNumTrucks; i++)
	{
		if (axt[i] != nullptr && cn.isImpact(axt[i]))
		{
			mLives--;
			cn.Die();
			return true;
		}
	}
	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr && cn.isImpact(axh[i]))
		{
			mLives--;
			cn.Die();
			return true;
		}
	}
	for (int i = 0; i < mNumDinos; i++)
	{
		if (akl[i] != nullptr && cn.isImpact(akl[i]))
		{
			mLives--;
			cn.Die();
			return true;
		}
	}
	for (int i = 0; i < mNumBirds; i++)
	{
		if (ac[i] != nullptr && cn.isImpact(ac[i]))
		{
			mLives--;
			cn.Die();
			return true;
		}
	}
	return false;
}

bool CGAME::checkFinish() {
	if (cn.checkFinish())
	{
		mScore += mLevel * 100; return true;
	}
	return false;
}

// ================================================================
// DRAWGAME - Theo yeu cau do an: "Ve tro choi ra man hinh"
// Dat ten drawGame() de tuong duong voi ban console goc
// Nhan them RenderWindow& vi SFML can window de ve
// ================================================================
void CGAME::drawGame(RenderWindow& window, Font& font) {
	// ---------- 1 & 2) Xếp gạch Background & Nền đường ----------
	static Texture tilesetTex;
	static bool bgLoaded = false, bgTried = false;

	// The 96x48 image contains two 48x48 tiles: background then road.
	if (!bgTried) {
		bgTried = true;
		bgLoaded = tilesetTex.loadFromFile("Assets/images/environment(for-map)/background_base.png");
		if (bgLoaded) {
			const Vector2u textureSize = tilesetTex.getSize();
			bgLoaded = textureSize.x == TILE_SIZE * TILE_COUNT &&
			           textureSize.y == TILE_SIZE;
			if (bgLoaded) {
				tilesetTex.setSmooth(true);
			} else {
				cerr << "Invalid background tileset size; expected 96x48.\n";
			}
		}
	}

	if (bgLoaded) {
		Sprite tileSprite(tilesetTex);
		const float scaleFactor = static_cast<float>(CELL_SIZE) / TILE_SIZE;
		tileSprite.setScale(Vector2f(scaleFactor, scaleFactor));
		for (int y = 0; y < SCREEN_HEIGHT; ++y) {
			for (int x = 0; x < SCREEN_WIDTH; ++x) {
				const int tileId = mTileMap[y][x];
				if (tileId < 0 || tileId >= TILE_COUNT) {
					continue;
				}
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

	// ---------- 2) Nen duong ----------
	/*RectangleShape road;
	road.setSize(Vector2f((float)WINDOW_WIDTH, (float)((ROAD_BOTTOM - ROAD_TOP + 1) * CELL_SIZE)));
	road.setPosition(Vector2f(0.f, CellToPixel(ROAD_TOP)));*/
	//road.setFillColor(bgLoaded ? Color )

	// ---------- 3) Ve cac doi tuong dang chay ----------
	for (int i = 0; i < mNumTrucks; ++i) {
		if (axt[i] != nullptr) {
			axt[i]->Draw(window);
		}
	}
	for (int i = 0; i < mNumCars; ++i) {
		if (axh[i] != nullptr) {
			axh[i]->Draw(window);
		}
	}
	for (int i = 0; i < mNumDinos; ++i) {
		if (akl[i] != nullptr) {
			akl[i]->Draw(window);
		}
	}
	for (int i = 0; i < mNumBirds; ++i) {
		if (ac[i] != nullptr) {
			ac[i]->Draw(window);
		}
	}

	// ---------- 4) Den giao thong ----------
	for (int i = 0; i < mNumTrucks + mNumCars; ++i) {
		if (mLights[i] != nullptr) {
			mLights[i]->Draw(window, SCREEN_WIDTH - 1,
				mLights[i]->getLaneY() - 1);
		}
	}

	// ---------- 5) Nguoi choi ----------
	cn.Draw(window, font);

}

// ================================================================
// DRAWBUILDINGSZONE / DRAWSTREETZONE
// ================================================================
void CGAME::drawBuildingsZone(RenderWindow& window) {

}

void CGAME::drawStreetZone(RenderWindow& window) {

}

// ================================================================
// SAVEGAME / LOADGAME (tich hop tu SaveLoad.cpp)
// Theo yeu cau do an: saveGame(istream), loadGame(istream)
// -> Giu ten ham, doi kieu tham so thanh string cho thuc te hon
// ================================================================
void CGAME::saveGame(const string& filename) {
	ofstream ofs(filename);
	if (!ofs.is_open()) std::cout << "Khong the luu file" << '\n'; return;

	ofs << mLevel << '\n';
	ofs << mScore << '\n';
	ofs << mLives << '\n';
	ofs << cn.getX() << '\n';
	ofs << cn.getY() << '\n';

	ofs << mNumTrucks << '\n';
	for (int i = 0; i < mNumTrucks; i++) {
		ofs << (axt[i] ? axt[i]->getX() : 0) << '\n';
	}

	ofs << mNumCars << '\n';
	for (int i = 0; i < mNumCars; i++) {
		ofs << (axh[i] ? axh[i]->getX() : 0) << '\n';
	}

	ofs << mNumDinos << '\n';
	for (int i = 0; i < mNumDinos; i++) {
		ofs << (akl[i] ? akl[i]->getX() : 0) << '\n';
	}

	ofs << mNumBirds << '\n';
	for (int i = 0; i < mNumBirds; i++) {
		ofs << (ac[i] ? ac[i]->getX() : 0) << '\n';
	}

	ofs.close();
}

bool CGAME::loadGame(const string& filename) {
	ifstream ifs(filename);
	if (!ifs.is_open()) cout << "Khong the mo file save" << '\n'; return false;

	int level, score, live, px, py;
	ifs >> level;
	ifs >> score;
	ifs >> live;
	ifs >> px;
	ifs >> py;

	if (!ifs.good()) {
		ifs.close();
		return false;
	}

	mLevel = level;
	mScore = score;
	mLives = live;

	InitLanes();

	cn.Reset(px, py);

	// n: số lượng vật thể (car, truck,...) - x: các vật thể
	int n, x;

	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> x;
		if (i < mNumTrucks && axt[i]) axt[i]->setX(x); //update truck
	}

	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> x;
		if (i < mNumCars && axh[i]) axh[i]->setX(x);
	}

	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> x;
		if (i < mNumDinos && akl[i]) akl[i]->setX(x);
	}

	ifs >> n;
	for (int i = 0; i < n; i++) {
		ifs >> x;
		if (i < mNumBirds && ac[i]) ac[i]->setX(x);
	}

	ifs.close();
	return true;
}
// ================================================================
// MENU VA THONG BAO (tich hop tu Menu.cpp)
// ================================================================
static void DrawOverlayBox(RenderWindow& window, float w, float h, Color borderColor) {
	//Tạo hình chữ nhật
	RectangleShape box(Vector2f(w, h));

	// Lấy kích thước cửa sổ
	Vector2u winSize = window.getSize();

	// Dời điểm neo của hình chữ nhật vào chính giữa nó
	box.setOrigin(Vector2f(w / 2.0f, h / 2.0f));

	// Đặt hình chữ nhật vào chính giữa màn hình
	box.setPosition(Vector2f(winSize.x / 2.0f, winSize.y / 2.0f));

	// Đổ màu và viền
	box.setFillColor(Color(0, 0, 0, 200));
	box.setOutlineThickness(4.f);
	box.setOutlineColor(borderColor);

	window.draw(box);
}

//Căn giữa text với khung render và màn hình
static void CenterTextAt(Text& text, float targetX, float targetY) {
	// Lấy kích thước của text
	FloatRect bounds = text.getLocalBounds();

	// Dời tâm text vào chính giữa
	text.setOrigin(Vector2f(
		bounds.position.x + bounds.size.x / 2.0f,
		bounds.position.y + bounds.size.y / 2.0f));

	// Đặt text vào tọa độ mong muốn
	text.setPosition(Vector2f(targetX, targetY));
}

void CGAME::renderMenu(RenderWindow& window, Font& font) {
	//Lấy kích thước màn hình hiện tại làm mốc
	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	// Vẽ khung nền (Rộng 400, cao 300) tự động center
	DrawOverlayBox(window, 400.f, 300.f, Color::Green);

	// Tiêu đề
	Text title(font, "CROSSING GAME", 40);
	title.setFillColor(Color::Yellow);
	CenterTextAt(title, centerX, centerY - 100.f); // Đặt cao hơn tâm màn hình 100px

	// menu options
	Text opt1(font, "1. New Game", 25);
	opt1.setFillColor(Color::White);
	CenterTextAt(opt1, centerX, centerY - 20.f);

	Text opt2(font, "2. Load Game", 25);
	opt2.setFillColor(Color::White);
	CenterTextAt(opt2, centerX, centerY + 30.f);

	Text opt3(font, "3. Settings", 25);
	opt3.setFillColor(Color::White);
	CenterTextAt(opt3, centerX, centerY + 60.f);

	Text opt4(font, "4. Exit", 25);
	opt4.setFillColor(Color::White);
	CenterTextAt(opt3, centerX, centerY + 80.f);

	// Vẽ ra màn hình
	window.draw(title);
	window.draw(opt1);
	window.draw(opt2);
	window.draw(opt3);
	window.draw(opt4);
}

void CGAME::renderPauseMsg(RenderWindow& window, Font& font) {

	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 350.f, 200.f, Color::Blue);

	Text title(font, "PAUSED", 45);
	title.setFillColor(Color::Blue);
	CenterTextAt(title, centerX, centerY - 30.f);

	Text prompt(font, "Press P to resume", 25);
	prompt.setFillColor(Color::Blue);
	CenterTextAt(prompt, centerX, centerY + 40.f);

	window.draw(title);
	window.draw(prompt);
}

void CGAME::renderDeadMsg(RenderWindow& window, Font& font) {

	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 400.f, 300.f, Color::Red);

	Text title(font, "YOU DIED", 45);
	title.setFillColor(Color::Red);
	CenterTextAt(title, centerX, centerY - 30.f);

	Text promtLive(font, "Remaining lives " + to_string(mLives), 25);
	promtLive.setFillColor(Color::White);
	CenterTextAt(promtLive, centerX, centerY + 25.f);

	Text prompt(font, "Press Y to continue", 25);
	prompt.setFillColor(Color::White);
	CenterTextAt(prompt, centerX, centerY + 40.f);

	window.draw(title);
	window.draw(promtLive);
	window.draw(prompt);

}

void CGAME::renderLevelUp(RenderWindow& window, Font& font) {

	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 400.f, 250.f, Color::Magenta);

	Text title(font, "LEVELED UP! -> Level " + to_string(mLevel), 45);
	title.setFillColor(Color::Magenta);
	CenterTextAt(title, centerX, centerY - 30.f);

	window.draw(title);
}

void CGAME::renderWin(RenderWindow& window, Font& font) {
	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 500.f, 250.f, Color::Yellow);

	Text title(font, "VICTORY!", 55);
	title.setFillColor(Color::Yellow);
	CenterTextAt(title, centerX, centerY - 40.f);

	Text prompt(font, "Fianal scores " + to_string(mScore), 25);
	prompt.setFillColor(Color::White);
	CenterTextAt(prompt, centerX, centerY + 20.f);

	Text exitPrompt(font, "Press any keys to Exit", 20);
	exitPrompt.setFillColor(Color::Cyan);
	CenterTextAt(exitPrompt, centerX, centerY + 70.f);

	window.draw(title);
	window.draw(prompt);
	window.draw(exitPrompt);
}

void CGAME::renderGameOver(RenderWindow& window, Font& font) {
	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 450.f, 250.f, Color::Red);

	Text title(font, "GAME OVER", 55);
	title.setFillColor(Color::Red);
	title.setStyle(Text::Bold);
	CenterTextAt(title, centerX, centerY - 40.f);

	Text prompt(font, "Finals scores " + to_string(mScore), 25);
	prompt.setFillColor(Color::White);
	CenterTextAt(prompt, centerX, centerY + 20.f);

	Text exitPrompt(font, "Press anny keys to Exit", 20);
	exitPrompt.setFillColor(Color(200, 200, 200));
	CenterTextAt(exitPrompt, centerX, centerY + 70.f);

	window.draw(title);
	window.draw(prompt);
	window.draw(exitPrompt);
}