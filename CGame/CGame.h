#pragma once
#ifndef CGAME_H
#define CGAME_H

// ================================================================
// FIX: them windows.h TRUOC SFML de co HANDLE, SuspendThread,
//      ResumeThread, Sleep - tranh loi "HANDLE undefined"
// ================================================================
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "Utils.h"
#include "CPeople.h"
#include "CVehicle.h"
#include "CAnimal.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio/Music.hpp>
#include <fstream>
#include <string>
using namespace std;
using namespace sf;

// ================================================================
// ENUM TRANG THAI MAN HINH
// ================================================================
enum class GameState {
	MENU, // Trang thai hien menu chinh, cho nguoi choi chon Start, Load, Exit
	PLAYING, // Trang thai dang choi, SubThread cap nhat logic, main render
	PAUSED, // Trang thai tam dung, SubThread khong cap nhat logic, main render menu pause
	DEAD, // Trang thai nguoi choi da chet, SubThread khong cap nhat logic, main render menu dead
	GAMEOVER, // Trang thai ket thuc game, nguoi choi het mang, SubThread khong cap nhat logic, main render menu game over
	WIN, // Trang thai nguoi choi chien thang, dat vạch FINISH, SubThread khong cap nhat logic, main render menu win
    LEVEL_UP   // Dat boi SubThread khi qua man, main xu ly nextLevel() an toan
};

// ================================================================
// LOP CGAME - Quan ly toan bo game
//
// Tuong ung voi yeu cau do an:
//   axt  = CTRUCK*  mang xe tai      (ten goc: axt)
//   axh  = CCAR*    mang xe hoi      (ten goc: axh)
//   akl  = CROCK*   mang da lan      (ten goc: akl, thay CDINAUSOR)
//   ac   = CBIRD*   mang chim        (ten goc: ac)
//   cn   = CPEOPLE  nguoi choi       (ten goc: cn)
//
// Ham theo yeu cau do an:
//   CGAME()           - Chuan bi du lieu tat ca doi tuong
//   drawGame()        - Ve tro choi ra man hinh
//   ~CGAME()          - Huy tai nguyen
//   getPeople()       - Lay thong tin nguoi
//   getVehicle()      - Lay danh sach xe
//   getAnimal()       - Lay danh sach thu
//   resetGame()       - Thiet lap lai toan bo du lieu
//   startGame()       - Bat dau vao tro choi
//   loadGame()        - Tai lai tro choi da luu
//   saveGame()        - Luu du lieu tro choi
//   pauseGame()       - Tam dung Thread (nhan HANDLE tu main)
//   resumeGame()      - Quay lai Thread (nhan HANDLE tu main)
//   exitGame()        - Thoat Thread    (nhan HANDLE tu main)
//   updatePosPeople() - Dieu khien di chuyen CPEOPLE
//   updatePosVehicle()- CTRUCK & CCAR di chuyen
//   updatePosAnimal() - CROCK & CBIRD di chuyen
// ================================================================
class CGAME {
private:
    // ---- Mang doi tuong (ten theo yeu cau do an) ----
    CTRUCK* axt[MAX_TRUCKS];                 // xe tai
    CCAR* axh[MAX_CARS];                   // xe hoi
    CROCK* akl[MAX_DINOS];                  // da lan (thay CDINAUSOR)
    CBIRD* ac[MAX_BIRDS];                   // chim
    CTRAFFICLIGHT* mLights[MAX_TRUCKS + MAX_CARS];  // den giao thong

    CPEOPLE cn;  // nguoi choi (composition)

    // ---- Trang thai ----
	int mLevel, mScore, mLives; // level, diem, mang (theo yeu cau do an)
	int mNumTrucks, mNumCars, mNumDinos, mNumBirds; // so luong xe/thu hien tai tren duong, cap nhat moi level trong InitLanes()

	GameState mState; // Trang thai hien tai cua game (MENU, PLAYING, PAUSED, DEAD, GAMEOVER, WIN, LEVEL_UP)

	// ---- Chu ky den giao thong dung chung ----
	CTRAFFICLIGHT::State mTrafficState;
	Clock mTrafficClock;
	int mTrafficElapsedMs;
	int mGreenDurationMs;
	int mRedDurationMs;

	// ---- Am thanh den giao thong ----
	Music mTrafficStart;
	Music mTrafficStopping;
	bool mTrafficStartLoaded;
	bool mTrafficStoppingLoaded;
	void updateTrafficAudio();
	void stopTrafficAudio();

    // ---- Bản đồ (Tilemap) ----
	int mTileMap[SCREEN_HEIGHT][SCREEN_WIDTH]; // Ma trận lưu vị trí tile background

    // ---- Ham noi bo ----
	void InitLanes(); // Tao xe/thu/den theo mLevel
	void loadAllAssets(); // Gan PNG cho tung doi tuong, goi sau khi new doi tuong trong InitLanes()
	void loadMapFromFile(const std::string& filename = "Assets/map.txt"); // Nạp ma trận bản đồ từ file text
	int  GetLaneY(int laneIndex); // Tra ve Y cua lan laneIndex (0-based, chi co xe/thu o lane 0-7, lane 8 la vạch FINISH, lane 9 la vạch START)

	void drawBuildingsZone(RenderWindow& window); // Ve khu vuc nha cao tang o 2 ben duong, chi ve khi mLevel >= 3
	void drawStreetZone(RenderWindow& window); // Ve khu vuc duong pho o giua, luon ve o moi level

public:
    CGAME();   // Chuan bi du lieu cho tat ca cac doi tuong
    ~CGAME();  // Huy tai nguyen da cap phat

    // ----------------------------------------------------------------
    // CAC HAM THEO YEU CAU DO AN
    // ----------------------------------------------------------------
    void     drawGame(RenderWindow& window, Font& font); // Ve tro choi ra man hinh
    CPEOPLE& getPeople() { return cn; }               // Lay thong tin nguoi
    CVEHICLE** getVehicle();                             // Lay danh sach xe
    CANIMAL** getAnimal();                              // Lay danh sach thu

    void resetGame();                        // Thiet lap lai toan bo du lieu nhu luc dau
    void startGame();                        // Bat dau vao tro choi (reset + khoi tao)
    void nextLevel();                        // Tang level, tao lai xe/thu kho hon

    void saveGame(const std::string& filename); // Luu du lieu tro choi
    bool loadGame(const std::string& filename); // Tai lai tro choi da luu

    // FIX: HANDLE duoc dinh nghia boi windows.h da include o tren
    void pauseGame(HANDLE hThread);   // Tam dung Thread
    void resumeGame(HANDLE hThread);  // Quay lai Thread
    void exitGame(HANDLE hThread);    // Thoat Thread (IS_RUNNING=false)

    void updatePosPeople(char key);   // Dieu khien di chuyen CPEOPLE
    void updatePosVehicle();          // CTRUCK & CCAR di chuyen
    void updatePosAnimal();           // CROCK & CBIRD di chuyen
    void updateTrafficLights();       // Cap nhat den giao thong

    // ---- Cap nhat animation sprite (goi moi frame) ----
	void updateAnimations(float dt); // Cap nhat animation cho nguoi choi, xe, thu, goi moi frame tu main.cpp

    // ---- Kiem tra ----
	bool checkCollision(); // Kiem tra va cham giua nguoi choi va xe/thu, tra ve true neu co va cham
	bool checkFinish(); // Kiem tra nguoi choi da den vạch FINISH chua, tra ve true neu da den

    // ---- Man hinh thong bao (tich hop tu Menu.cpp) ----
	void renderMenu(RenderWindow& window, Font& font); // Ve menu chinh khi mState = MENU
	void renderPauseMsg(RenderWindow& window, Font& font); // Ve menu tam dung khi mState = PAUSED
	void renderDeadMsg(RenderWindow& window, Font& font); // Ve menu chet khi mState = DEAD
	void renderLevelUp(RenderWindow& window, Font& font); // Ve menu tang level khi mState = LEVEL_UP
	void renderWin(RenderWindow& window, Font& font); // Ve menu chien thang khi mState = WIN
	void renderGameOver(RenderWindow& window, Font& font); // Ve menu ket thuc khi mState = GAMEOVER

    // ---- Getter trang thai ----
	int       getLevel() const { return mLevel; } // Tra ve level hien tai
	int       getScore() const { return mScore; } // Tra ve diem hien tai
	int       getLives() const { return mLives; } // Tra ve so mang hien tai
	GameState getState() const { return mState; } // Tra ve trang thai hien tai cua game
	void      setState(GameState s); // Dat trang thai va dong bo am thanh gameplay

    // ---- Them cho main.cpp goi Init() truoc startGame() ----
	void Init(); // Khoi tao CGAME, load anh, khoi tao lane, goi sau khi new CGAME trong main.cpp, truoc khi startGame() de chuan bi du lieu cho tro choi
};

#endif
