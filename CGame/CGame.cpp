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
CGAME::CGAME()
    : mLevel(1), mScore(0), mLives(3),
    mNumTrucks(0), mNumCars(0), mNumDinos(0), mNumBirds(0),
    mState(GameState::MENU)
{
}

CGAME::~CGAME() {
	
}

// ================================================================
// GETTER DANH SACH XE / THU (theo yeu cau do an)
// getVehicle() -> CVEHICLE**
// getAnimal()  -> CANIMAL**
// ================================================================
CVEHICLE** CGAME::getVehicle() {
    
}

CANIMAL** CGAME::getAnimal() {
	
}

// ================================================================
// GET LANE Y
// ================================================================
int CGAME::GetLaneY(int laneIndex) {
	
}

// ================================================================
// INIT LANES - Tao xe/thu/den theo mLevel
// ================================================================
void CGAME::InitLanes() {
   
} 

// ================================================================
// LOAD ALL ASSETS - Gan PNG cho tung doi tuong
// ================================================================
void CGAME::loadAllAssets() {
	
}

// ================================================================
// UPDATE ANIMATIONS
// ================================================================
void CGAME::updateAnimations(float dt) {
	
	
}

// ================================================================
// INIT / STARTGAME / RESETGAME / NEXTLEVEL
// startGame() = yeu cau do an: "Bat dau vao tro choi"
// ================================================================
void CGAME::Init() {
   
}

void CGAME::startGame() {
	
}

void CGAME::resetGame() {
	
}

void CGAME::nextLevel() {
	
}

// ================================================================
// PAUSE / RESUME / EXIT THREAD
// Theo yeu cau do an: pauseGame(HANDLE), resumeGame(HANDLE), exitGame(HANDLE)
// ================================================================
void CGAME::pauseGame(HANDLE hThread) {
	
}

void CGAME::resumeGame(HANDLE hThread) {
	
}

void CGAME::exitGame(HANDLE hThread) {
	
}

// ================================================================
// CAP NHAT VI TRI
// Theo yeu cau do an: updatePosPeople(char), updatePosVehicle(), updatePosAnimal()
// ================================================================
void CGAME::updatePosPeople(char key) {
	
}

void CGAME::updatePosVehicle() {
   
}

void CGAME::updatePosAnimal() {
}

void CGAME::updateTrafficLights() {
	
}

// ================================================================
// KIEM TRA TRANG THAI
// isImpact theo yeu cau do an: isImpact(getVehicle()), isImpact(getAnimal())
// ================================================================
bool CGAME::checkCollision() {
	
}

bool CGAME::checkFinish() {
	
}

// ================================================================
// DRAWGAME - Theo yeu cau do an: "Ve tro choi ra man hinh"
// Dat ten drawGame() de tuong duong voi ban console goc
// Nhan them RenderWindow& vi SFML can window de ve
// ================================================================
void CGAME::drawGame(RenderWindow& window, Font& font) {
    // ---------- 1) Background ----------
    static Texture bgTex;
	// Dùng static để chỉ load ảnh một lần duy nhất, tránh tốn tài nguyên khi gọi drawGame nhiều lần
    static bool bgLoaded = false, bgTried = false;
	// Cố gắng load ảnh nền một lần duy nhất, nếu thất bại thì sẽ không cố gắng lại nữa
    

    // ---------- 2) Nen duong ----------
   

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
