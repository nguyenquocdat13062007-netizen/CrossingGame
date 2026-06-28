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
CGAME::CGAME(): mLevel(1), mScore(0), mLives(3),mNumTrucks(0), mNumCars(0), mNumDinos(0), mNumBirds(0),mState(GameState::MENU)
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
		delete mLights[i] ;
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
// LOAD ALL ASSETS - Gan PNG cho tung doi tuong
// ================================================================
void CGAME::loadAllAssets() {
	cn.loadAssets("Assets/player1.png","Assets/player2.png");
	for (int i = 0; i < mNumTrucks; i++)
	{
		if (axt[i] != nullptr)
		{
			axt[i]->loadAssets("Assets/truck1..png", "Assets/truck2.png");
		}
	}
	for (int i = 0; i < mNumCars; i++)
	{
		if (axh[i] != nullptr)
		{
			axh[i]->loadAssets("Assets/car1.png", "Assets/car2.png");
		}
	}
	for (int i = 0; i < mNumDinos; i++)
	{
		if (akl[i] != nullptr)
		{
			akl[i]->loadAssets("Assets/Dinos.png", "Assets/Dinos.png");
		}
	}
	for (int i = 0; i < mNumBirds; i++)
	{
		if (ac[i] != nullptr)
		{
			ac[i]->loadAssets("Assets/bird1.png", "Assets/bird2.png");
		}
	}
	for (int i = 0; i < mNumTrucks + mNumCars; i++)
	{
		if (mLights[i] != nullptr)
		{
			mLights[i]->loadAssets("Assets/lightgo.png", "Assets/lightstop.png");
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
    // ---------- 1) Background ----------
    static Texture bgTex;
    static bool bgLoaded = false, bgTried = false;
	if (!bgTried)
	{
		bgTried = true;
		bgLoaded = bgTex.loadFromFile("Asset/background.png");
		if (bgLoaded)
		{
			bgTex.setSmooth(true);
		}
	}
	if (bgLoaded)
	{
		Sprite bgSprite(bgTex);
		auto sz = bgTex.getSize();
		bgSprite.setScale(Vector2f((float)WINDOW_WIDTH / sz.x, (float)WINDOW_HEIGHT / sz.y));
		bgSprite.setPosition(Vector2f(0.f, 0.f));
		window.draw(bgSprite);

	}

    // ---------- 2) Nen duong ----------
	RectangleShape road;
	road.setSize(Vector2f((float)WINDOW_WIDTH, (float)((ROAD_BOTTOM - ROAD_TOP + 1) * CELL_SIZE)));
	road.setPosition(Vector2f(0.f, CellToPixel(ROAD_TOP)));
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
	
}

bool CGAME::loadGame(const string& filename) {

}
// ================================================================
// MENU VA THONG BAO (tich hop tu Menu.cpp)
// ================================================================
static void DrawOverlayBox(RenderWindow& window, float x, float y, float w, float h, Color borderColor) {

}

void CGAME::renderMenu(RenderWindow& window, Font& font) {

}

void CGAME::renderPauseMsg(RenderWindow& window, Font& font) {
	
}
void CGAME::renderDeadMsg(RenderWindow& window, Font& font) {

}

void CGAME::renderLevelUp(RenderWindow& window, Font& font) {
	
}

void CGAME::renderWin(RenderWindow& window, Font& font) {
	
}

void CGAME::renderGameOver(RenderWindow& window, Font& font) {
	
}
