#include "CPeople.h"
#include <iostream>
using namespace std;
CPEOPLE::CPEOPLE()
    : mX(SCREEN_WIDTH / 2), mY(ROAD_BOTTOM),
    mAlive(true), mFinished(false) {
}

CPEOPLE::CPEOPLE(int startX, int startY)
    : mX(startX), mY(startY),
    mAlive(true), mFinished(false) {
}

void CPEOPLE::Up(int step) {
   
}

void CPEOPLE::Down(int step) {
   
}

void CPEOPLE::Left(int step) {
   
}

void CPEOPLE::Right(int step) {
    
}

void CPEOPLE::Draw() {
   
}

void CPEOPLE::Clear() {
   
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

void CPEOPLE::DrawDeathEffect() {
   
}
