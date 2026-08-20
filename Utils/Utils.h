#pragma once
#ifndef UTILS_H
#define UTILS_H

#pragma warning(push) // 
#pragma warning(disable: 4275)
#include <SFML/Graphics.hpp>
#pragma warning(pop)
#include <string>
using namespace std;
using namespace sf;

//==============================================================
// KICH THUOC LUOI (GRID)
//==============================================================
const int SCREEN_WIDTH = 40;  // 40 o luoi ngang
const int SCREEN_HEIGHT = 24; // 24 o luoi doc (chieu cao khung nhin - Viewport)
const int MAP_HEIGHT = 72;    // 72 o luoi doc (chieu cao toan ban do - gap 3 lan man hinh goc)

//==============================================================
// KICH THUOC PIXEL
//==============================================================
const int CELL_SIZE = 24; // 1 o luoi = 24 pixel
const int WINDOW_WIDTH = SCREEN_WIDTH * CELL_SIZE;    // 960 pixel
const int WINDOW_HEIGHT = SCREEN_HEIGHT * CELL_SIZE;  // 576 pixel
const int MAP_PIXEL_HEIGHT = MAP_HEIGHT * CELL_SIZE;  // 1728 pixel

//==============================================================
// VI TRI DUONG
//==============================================================
const int ROAD_TOP = 2;              // 2 o dau tren cung la vach FINISH (y = 0, 1, 2)
const int ROAD_BOTTOM = MAP_HEIGHT - 3; // Start o duoi cung (y = 68, 69)
const int FINISH_Y = ROAD_TOP;       // Y cua vach FINISH
const int START_Y = ROAD_BOTTOM;     // Y cua vach START

//==============================================================
// CAU HINH GAME
//==============================================================
const int MAX_LEVEL = 5;  // Muc do kho toi da cua tro choi
const int MAX_TRUCKS = 36; // So xe tai toi da tren toan ban do dai
const int MAX_CARS = 56;   // So xe hoi toi da tren toan ban do dai
const int MAX_DINOS = 8;   // So da lan toi da tren toan ban do dai
const int MAX_BIRDS = 36;  // So zombie toi da tren toan ban do dai
const int MAX_LIGHTS = 8;  // So den giao thong toi da tren toan ban do
const int LANE_COUNT = 48; // So lan duong toan ban do

//==============================================================
// BIEN TOAN CUC - dung chung giua main va SubThread
// (tuong duong IS_RUNNING va MOVING trong ban console goc)
//==============================================================
extern volatile bool IS_RUNNING;   // true = SubThread dang chay
extern volatile char MOVING;       // 'W'/'A'/'S'/'D' hoac ' '

//==============================================================
// MAU SAC
//==============================================================
const Color COLOR_BG(20, 20, 20); // Màu nền chung của trò chơi
const Color COLOR_ROAD(60, 60, 60); // Màu đường, khác với COLOR_BG để tạo độ tương phản
const Color COLOR_LANE(110, 110, 110); // Màu làn đường
const Color COLOR_PLAYER(255, 215, 0); // Màu người chơi
const Color COLOR_TRUCK(220, 40, 40); // Màu xe tải
const Color COLOR_CAR(40, 200, 80); // Màu xe hơi
const Color COLOR_DINO(200, 40, 200); // Màu khủng long
const Color COLOR_BIRD(40, 200, 220); // Màu chim
const Color COLOR_TEXT(240, 240, 240); // Màu văn bản
const Color COLOR_TITLE(255, 215, 0); // Màu tiêu đề
const Color COLOR_FINISH(255, 215, 0); // Màu vạch kết thúc
const Color COLOR_START(40, 200, 80); // Màu vạch bắt đầu
const Color COLOR_LIGHT_RED(255, 40, 40); // Màu đỏ nhạt
const Color COLOR_LIGHT_GREEN(40, 255, 40); // Màu xanh nhạt
const Color COLOR_BUILDING_BG(35, 32, 30); // Màu nền tòa nhà
const Color COLOR_STREET_BG(45, 45, 48); // Màu nền đường phố

//==============================================================
// HAM TIEN ICH
//==============================================================
inline float CellToPixel(int cell) {
	// Chuyen doi tu cell (o luoi) sang pixel
	return (float)(cell * CELL_SIZE);
}

inline RectangleShape MakeShape(int x, int y, int widthCells, Color color) {
	// Tao RectangleShape voi vi tri (x, y) tinh theo o luoi, chieu rong widthCells o, mau color
	// Tra ve RectangleShape da duoc cau hinh
	// Note: RectangleShape se duoc ve voi position la (x, y) tinh theo pixel, size la (widthCells * CELL_SIZE, CELL_SIZE)
	RectangleShape shape;
	shape.setSize(Vector2f((float)(widthCells * CELL_SIZE - 2), (float)(widthCells * CELL_SIZE - 2)));
	shape.setPosition(Vector2f(CellToPixel(x) + 1.f, CellToPixel(y) + 1.f));
	shape.setFillColor(color);
	return shape;
}

#endif
