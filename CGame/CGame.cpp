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
using namespace std;
using namespace sf;
// ================================================================
// DINH NGHIA BIEN TOAN CUC (khai bao extern trong Utils.h)
// Dung chung giua main() va SubThread()
// ================================================================
volatile bool IS_RUNNING = true;
volatile char MOVING = ' ';

// ================================================================
// CONSTRUCTOR & DESTRUCTOR
// ================================================================
CGAME::CGAME() : mLevel(1), mScore(0), mLives(3), mNumTrucks(0), mNumCars(0), mNumDinos(0), mNumBirds(0), mState(GameState::MENU)
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
		mLights[i] = new CTRAFFICLIGHT(y, 3000 - mLevel * 200, 2000);
	}

	for (int i = 0; i < mNumCars; i++)
	{
		int y = GetLaneY(mNumTrucks + i);
		int dir = (i % 2 == 0) ? 1 : -1;
		int x = (dir == 1) ? 0 : SCREEN_WIDTH - 2;
		axh[i] = new CCAR(x, y, BaseSpeed + 1, dir);
		mLights[mNumTrucks + i] = new CTRAFFICLIGHT(y, 2500 - mLevel * 150, 1500);
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
	loadAllAssets();
}

// ================================================================
// LOAD MAP FROM FILE - Nap ma tran ban do tu file text
// ================================================================
void CGAME::loadMapFromFile(const string& filename) {
	ifstream ifs(filename);
	bool fileOk = false;
	if (ifs.is_open()) {
		fileOk = true;
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				if (!(ifs >> mTileMap[y][x])) {
					fileOk = false;
					break;
				}
			}
			if (!fileOk) break;
		}
		ifs.close();
	}

	if (!fileOk && filename != "Assets/map.txt") {
		ifs.open("Assets/map.txt");
		if (ifs.is_open()) {
			fileOk = true;
			for (int y = 0; y < SCREEN_HEIGHT; y++) {
				for (int x = 0; x < SCREEN_WIDTH; x++) {
					if (!(ifs >> mTileMap[y][x])) {
						fileOk = false;
						break;
					}
				}
				if (!fileOk) break;
			}
			ifs.close();
		}
	}

	if (!fileOk) {
		// Fallback neu khong tim thay bat ky file map nao
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				if (y >= ROAD_TOP && y <= ROAD_BOTTOM) {
					mTileMap[y][x] = 1; // Duong
				} else {
					mTileMap[y][x] = 0; // Vi he / Co
				}
			}
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
	for (int i = 0; i < mNumTrucks + mNumCars; i++)
	{
		if (mLights[i] != nullptr)
		{
			mLights[i]->loadAssets("Assets/images/environment(for-map)/lightstop.png", "Assets/images/environment(for-map)/light1.png");
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
	mState = GameState::PLAYING;
}

void CGAME::resetGame() {
	mLevel = 1; mScore = 0; mLives = 3;
	cn.Reset();
	InitLanes();
}

void CGAME::nextLevel() {
	mLevel++;
	cn.Reset();
	InitLanes();
	mState = GameState::PLAYING;
}

// ================================================================
// PAUSE / RESUME / EXIT THREAD
// Theo yeu cau do an: pauseGame(HANDLE), resumeGame(HANDLE), exitGame(HANDLE)
// ================================================================
void CGAME::pauseGame(HANDLE hThread) {
	SuspendThread(hThread);
	mState = GameState::PAUSED;

}

void CGAME::resumeGame(HANDLE hThread) {
	if (mState == GameState::PAUSED)
	{
		mState = GameState::PLAYING;
		ResumeThread(hThread);
	}
}

void CGAME::exitGame(HANDLE hThread) {
	IS_RUNNING = false;
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
			axt[i]->Tell();
		}
	}
	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr)
		{
			axh[i]->Move(SCREEN_WIDTH);
			axh[i]->Tell();
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
	for (int i = 0; i < mNumTrucks + mNumCars; i++)
	{
		if (mLights[i] == nullptr)
		{
			continue;
		}
		bool wasRed = mLights[i]->isRed();
		mLights[i]->Update();
		bool isNowRed = mLights[i]->isRed();
		int laneY = (i < mNumTrucks) ? (axt[i] != nullptr ? axt[i]->getY() : -1) : (axh[i - mNumTrucks] ? axh[i - mNumTrucks]->getY() : -1);

		if (!wasRed && isNowRed)
		{
			for (int j = 0; j < mNumTrucks; j++)
			{
				if (axt[j] != nullptr && axt[j]->getY() == laneY)
				{
					axt[j]->Stop(99999);
				}
			}
			for (int j = 0; j < mNumCars; j++)
			{
				if (axh[j] != nullptr && axh[j]->getY() == laneY)
				{
					axh[j]->Stop(99999);
				}
			}
		}
		else if (wasRed && !isNowRed)
		{
			for (int j = 0; j < mNumTrucks; j++)
			{
				if (axt[j] != nullptr && axt[j]->getY() == laneY)
				{
					axt[j]->Resume();
				}
			}
			for (int j = 0; j < mNumCars; j++)
			{
				if (axh[j] != nullptr && axh[j]->getY() == laneY)
				{
					axh[j]->Resume();
				}
			}
		}
	}
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

	// Nạp file 96x48
	if (!bgTried) {
		bgTried = true;
		bgLoaded = tilesetTex.loadFromFile("Assets/images/environment(for-map)/background_base.png");
		if (bgLoaded) {
			tilesetTex.setSmooth(true);
		}
	}

	if (bgLoaded) {
		Sprite tileSprite(tilesetTex);
		float scaleFactor = (float)CELL_SIZE / 48.0f;
		tileSprite.setScale(scaleFactor, scaleFactor);
		for (int y = 0; y < SCREEN_HEIGHT; y++) {
			for (int x = 0; x < SCREEN_WIDTH; x++) {
				int tileID = mTileMap[y][x];
				tileSprite.setTextureRect(IntRect(tileID * 48, 0, 48, 48));
				float px = (float)(x * CELL_SIZE);
				float py = (float)(y * CELL_SIZE);
				tileSprite.setPosition(px, py);
				window.draw(tileSprite);
			}
		}
	}

	// ---------- 2) Nen duong ----------
	/*RectangleShape road;
	road.setSize(Vector2f((float)WINDOW_WIDTH, (float)((ROAD_BOTTOM - ROAD_TOP + 1) * CELL_SIZE)));
	road.setPosition(Vector2f(0.f, CellToPixel(ROAD_TOP)));*/
	//road.setFillColor(bgLoaded ? Color )

	// ---------- 3) ve lane đại diện cho FINISH va START ----------


	// ---------- 4) Ve xe, da lan, chim ----------


	// ---------- 5) Den giao thong ----------

	// ---------- 6) Nguoi choi ----------


	// ---------- 7) HUD ----------

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
	box.setOrigin(w / 2.0f, h / 2.0f);

	// Đặt hình chữ nhật vào chính giữa màn hình
	box.setPosition(winSize.x / 2.0f, winSize.y / 2.0f);

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
	text.setOrigin(bounds.left + bounds.width / 2.0f, bounds.top + bounds.height / 2.0f);

	// Đặt text vào tọa độ mong muốn
	text.setPosition(targetX, targetY);
}

void CGAME::renderMenu(RenderWindow& window, Font& font) {
	//Lấy kích thước màn hình hiện tại làm mốc
	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	// Vẽ khung nền (Rộng 400, cao 300) tự động center
	DrawOverlayBox(window, 400.f, 300.f, Color::Green);

	// Tiêu đề
	Text title("CROSSING GAME", font, 40);
	title.setFillColor(Color::Yellow);
	CenterTextAt(title, centerX, centerY - 100.f); // Đặt cao hơn tâm màn hình 100px

	// menu options
	Text opt1("1. New Game", font, 25);
	opt1.setFillColor(Color::White);
	CenterTextAt(opt1, centerX, centerY - 20.f);

	Text opt2("2. Load Game", font, 25);
	opt2.setFillColor(Color::White);
	CenterTextAt(opt2, centerX, centerY + 30.f);

	Text opt3("3. Settings", font, 25.f);
	opt3.setFillColor(Color::White);
	CenterTextAt(opt3, centerX, centerY + 60.f);

	Text opt4("4. Exit", font, 25);
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

	Text title("PAUSED", font, 45);
	title.setFillColor(Color::Blue);
	CenterTextAt(title, centerX, centerY - 30.f);

	Text prompt("Press P to resume", font, 25);
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

	Text title("YOU DIED", font, 45);
	title.setFillColor(Color::Red);
	CenterTextAt(title, centerX, centerY - 30.f);

	Text promtLive("Remaining lives " + to_string(mLives), font, 25);
	promtLive.setFillColor(Color::White);
	CenterTextAt(promtLive, centerX, centerY + 25.f);

	Text prompt("Press Y to continue", font, 25);
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

	Text title("LEVELED UP! -> Level " + to_string(mLevel), font, 45);
	title.setFillColor(Color::Magenta);
	CenterTextAt(title, centerX, centerY - 30.f);

	window.draw(title);
}

void CGAME::renderWin(RenderWindow& window, Font& font) {
	Vector2u winSize = window.getSize();
	float centerX = winSize.x / 2.0f;
	float centerY = winSize.y / 2.0f;

	DrawOverlayBox(window, 500.f, 250.f, Color::Yellow);

	Text title("VICTORY!", font, 55);
	title.setFillColor(Color::Yellow);
	CenterTextAt(title, centerX, centerY - 40.f);

	Text prompt("Fianal scores " + to_string(mScore), font, 25);
	prompt.setFillColor(Color::White);
	CenterTextAt(prompt, centerX, centerY + 20.f);

	Text exitPrompt("Press any keys to Exit", font, 20);
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

	Text title("GAME OVER", font, 55);
	title.setFillColor(Color::Red);
	title.setStyle(Text::Bold);
	CenterTextAt(title, centerX, centerY - 40.f);

	Text prompt("Finals scores " + to_string(mScore), font, 25);
	prompt.setFillColor(Color::White);
	CenterTextAt(prompt, centerX, centerY + 20.f);

	Text exitPrompt("Press anny keys to Exit", font, 20);
	exitPrompt.setFillColor(Color(200, 200, 200));
	CenterTextAt(exitPrompt, centerX, centerY + 70.f);

	window.draw(title);
	window.draw(prompt);
	window.draw(exitPrompt);
}