#include "CGame.h"
#include <fstream>

// ============================================================
// SAVE GAME
// ============================================================
// Cau truc file luu (theo thu tu ghi):
//   int  mLevel
//   int  mScore
//   int  mLives
//   int  peopleX, peopleY
//   int  mNumTrucks  -> [x cua tung truck]
//   int  mNumCars    -> [x cua tung car]
//   int  mNumDinos   -> [x cua tung dino]
//   int  mNumBirds   -> [x cua tung bird]
// ============================================================
bool CGAME::saveGame(const std::string& filename) {
    std::ofstream ofs(filename, std::ios::binary);
    if (!ofs.is_open()) return false;

    // --- Thong tin chung ---
    ofs.write((char*)&mLevel, sizeof(mLevel));
    ofs.write((char*)&mScore, sizeof(mScore));
    ofs.write((char*)&mLives, sizeof(mLives));

    // --- Vi tri nguoi choi ---
    int px = mPeople.getX();
    int py = mPeople.getY();
    ofs.write((char*)&px, sizeof(px));
    ofs.write((char*)&py, sizeof(py));

    // --- Vi tri xe tai ---
    ofs.write((char*)&mNumTrucks, sizeof(mNumTrucks));
    for (int i = 0; i < mNumTrucks; i++) {
        int x = mTrucks[i] ? mTrucks[i]->getX() : 0;
        ofs.write((char*)&x, sizeof(x));
    }

    // --- Vi tri xe hoi ---
    ofs.write((char*)&mNumCars, sizeof(mNumCars));
    for (int i = 0; i < mNumCars; i++) {
        int x = mCars[i] ? mCars[i]->getX() : 0;
        ofs.write((char*)&x, sizeof(x));
    }

    // --- Vi tri khung long ---
    ofs.write((char*)&mNumDinos, sizeof(mNumDinos));
    for (int i = 0; i < mNumDinos; i++) {
        int x = mDinos[i] ? mDinos[i]->getX() : 0;
        ofs.write((char*)&x, sizeof(x));
    }

    // --- Vi tri chim ---
    ofs.write((char*)&mNumBirds, sizeof(mNumBirds));
    for (int i = 0; i < mNumBirds; i++) {
        int x = mBirds[i] ? mBirds[i]->getX() : 0;
        ofs.write((char*)&x, sizeof(x));
    }

    ofs.close();
    return true;
}

// ============================================================
// LOAD GAME
// ============================================================
bool CGAME::loadGame(const std::string& filename) {
    std::ifstream ifs(filename, std::ios::binary);
    if (!ifs.is_open()) return false;

    int level, score, lives, px, py;

    // --- Doc thong tin chung ---
    ifs.read((char*)&level, sizeof(level));
    ifs.read((char*)&score, sizeof(score));
    ifs.read((char*)&lives, sizeof(lives));
    ifs.read((char*)&px, sizeof(px));
    ifs.read((char*)&py, sizeof(py));

    if (!ifs.good()) { ifs.close(); return false; }

    // Ap dung thong tin chung va tao lai cac doi tuong theo level
    mLevel = level;
    mScore = score;
    mLives = lives;
    InitLanes();          // Tao xe/thu/den theo mLevel moi
    mPeople.Reset(px, py); // Dat lai vi tri nguoi choi

    // --- Doc & ap dung vi tri xe tai ---
    int n;
    ifs.read((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        int x;
        ifs.read((char*)&x, sizeof(x));
        if (i < mNumTrucks && mTrucks[i]) mTrucks[i]->setX(x);
    }

    // --- Doc & ap dung vi tri xe hoi ---
    ifs.read((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        int x;
        ifs.read((char*)&x, sizeof(x));
        if (i < mNumCars && mCars[i]) mCars[i]->setX(x);
    }

    // --- Doc & ap dung vi tri khung long ---
    ifs.read((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        int x;
        ifs.read((char*)&x, sizeof(x));
        if (i < mNumDinos && mDinos[i]) mDinos[i]->setX(x);
    }

    // --- Doc & ap dung vi tri chim ---
    ifs.read((char*)&n, sizeof(n));
    for (int i = 0; i < n; i++) {
        int x;
        ifs.read((char*)&x, sizeof(x));
        if (i < mNumBirds && mBirds[i]) mBirds[i]->setX(x);
    }

    ifs.close();
    return true;
}