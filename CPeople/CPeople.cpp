// NOMINMAX: ngan Windows.h dinh nghia macro min/max
// gay xung dot voi std::min / std::max cua C++ standard library
#define NOMINMAX

#include "CPeople.h"
#include <algorithm>  // can cho std::min, std::max
using namespace std;
using namespace sf;

CPEOPLE::CPEOPLE()
    : mX(SCREEN_WIDTH / 2), mY(START_Y),
    mAlive(true), mFinished(false),
    mDirection(1), mAnim(0.15f) {
}

CPEOPLE::CPEOPLE(int startX, int startY)
    : mX(startX), mY(startY),
    mAlive(true), mFinished(false),
    mDirection(1), mAnim(0.15f) {
}

void CPEOPLE::loadAssets(const std::string& frame1,
    const std::string& frame2) {
  
}

void CPEOPLE::Up(int step) {
    
}

void CPEOPLE::Down(int step) {
 
}

void CPEOPLE::Left(int step) {

}

void CPEOPLE::Right(int step) {
   
}

void CPEOPLE::Draw(RenderWindow& window, Font& font) {
 
}

bool CPEOPLE::isImpact(const CVEHICLE* v) const {
  
}

bool CPEOPLE::isImpact(const CANIMAL* a) const {
  
}

bool CPEOPLE::checkFinish() {
   
}

void CPEOPLE::Reset(int startX, int startY) {
  
}

void CPEOPLE::Die() {
  
}
