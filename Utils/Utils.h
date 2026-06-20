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
const int SCREEN_WIDTH = 40; // 40 o luoi ngang
const int SCREEN_HEIGHT = 24; // 24 o luoi doc

//==============================================================
// KICH THUOC PIXEL
//==============================================================
const int CELL_SIZE = 24; // 1 o luoi = 24 pixel
const int WINDOW_WIDTH = SCREEN_WIDTH * CELL_SIZE;  // chiều rộng cửa sổ = số ô ngang * kích thước ô 
const int WINDOW_HEIGHT = SCREEN_HEIGHT * CELL_SIZE;  // chiều cao cửa sổ = số ô dọc * kích thước ô

//==============================================================
// VI TRI DUONG
//==============================================================
const int ROAD_TOP = 2; // 2 ô lưới từ trên xuống là đường, 2 ô cuối cùng (22, 23) là vạch START
const int ROAD_BOTTOM = SCREEN_HEIGHT - 3; // 2 ô lưới từ dưới lên là đường, 2 ô cuối cùng (0, 1) là vạch FINISH
const int FINISH_Y = ROAD_TOP; // Y của vạch FINISH
const int START_Y = ROAD_BOTTOM; // Y của vạch START

//==============================================================
// CAU HINH GAME
//==============================================================
const int MAX_LEVEL = 5; // Mức độ khó tối đa của trò chơi 
const int MAX_TRUCKS = 3; // Số xe tải tối đa trên đường cùng lúc, theo yêu cầu do an
const int MAX_CARS = 4; // Số xe hơi tối đa trên đường cùng lúc, theo yêu cầu do an
const int MAX_DINOS = 2; // Số da lan tối đa trên đường cùng lúc, theo yêu cầu do an
const int MAX_BIRDS = 3; // Số chim tối đa trên đường cùng lúc, theo yêu cầu do an
const int LANE_COUNT = 8; // Số làn đường có xe (không tính làn đường trên cùng và dưới cùng chỉ có thú)

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
}

inline RectangleShape MakeShape(int x, int y, int widthCells, Color color) {
	// Tao RectangleShape voi vi tri (x, y) tinh theo o luoi, chieu rong widthCells o, mau color
	// Tra ve RectangleShape da duoc cau hinh
	// Note: RectangleShape se duoc ve voi position la (x, y) tinh theo pixel, size la (widthCells * CELL_SIZE, CELL_SIZE)
}

#endif
