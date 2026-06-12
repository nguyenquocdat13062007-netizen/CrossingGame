#pragma once
#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <iostream>
#include <string>
#include <conio.h>
using namespace std;
//------------------------------KÍCH THƯỚC MÀN HÌNH TRÊN CONSOLE------------------------------ 		
const int SCREEN_WIDTH = 80;   // Chiều rộng 80 ký tự — chuẩn console Windows
const int SCREEN_HEIGHT = 30;  // Chiều cao 30 dòng

//------------------------------VỊ TRÍ CON ĐƯỜNG TRÊN CONSOLE------------------------------  
const int ROAD_TOP = 3;  // Dòng bắt đầu của đường
const int ROAD_BOTTOM = 26;  // Dòng kết thúc của đường
const int FINISH_Y = ROAD_TOP;  // Đích Đến của người chơi
const int START_Y = ROAD_BOTTOM;  // Vị trí bắt đầu của người chơi

//------------------------------CẤU HÌNH GAME------------------------------
const int MAX_LEVEL = 5;  // Tổng số cấp độ trong game
const int MAX_TRUCKS = 3;  // Tối đa 3 xe tải trên console cùng lúc 
const int MAX_CARS = 4;  // Tối đa 4 xe con trên console cùng lúc
const int MAX_DINOS = 2;  // Tối đa 2 khủng long trên console cùng lúc
const int MAX_BIRDS = 3;  // Tối đa 3 con chim trên console cùng lúc
const int LANE_COUNT = 8;  // Số làn đường (bao gồm cả làn trên cùng và dưới cùng)

//------------------------------MÀU SẮC CHO CÁC ĐỐI TƯỢNG TRÊN CONSOLE------------------------------
const int COLOR_DEFAULT = 7;  // Xám trắng màu chữ mặc định 
const int COLOR_PLAYER = 14;  // Vàng — nhân vật người chơi
const int COLOR_TRUCK = 12;  // Đỏ tươi — xe tải
const int COLOR_CAR = 10;  // Xanh Đỏ tươi — xe tải
const int COLOR_DINO = 13;  // Tím hồng — khủng long
const int COLOR_BIRD = 11;  // Xanh cyan — chim
const int COLOR_ROAD = 8;   // Xám tối — viền đường, nền
const int COLOR_INFO = 15;  // Trắng sáng — thông tin HUD (điểm, cấp độ)
const int COLOR_TITLE = 14;  // Vàng — tiêu đề menu (dùng lại màu vàng như player)

//------------------------------BIẾN TOÀN CỤC------------------------------
extern volatile bool IS_RUNNING; // Biến toàn cục để kiểm soát vòng lặp chính của game true = game đang chạy, false = thoát SubThread
extern volatile char MOVING;  // Biến toàn cục để lưu hướng di chuyển hiện tại của người chơi 'W', 'A', 'S', 'D' hoặc ' ' (không di chuyển)

//------------------------------HÀM TIỆN ÍCH------------------------------
void GotoXY(int x, int y);
// Hàm này di chuyển con trỏ console đến vị trí (x, y) được chỉ định. Đây là một hàm tiện ích để in các đối tượng game tại vị
// x = cột (ngang), y = dòng (dọc), góc trên trái là (0,0)
// Dùng trước mỗi lần in ký tự để in đúng vị trí
//  trên console. Ví dụ, GotoXY(10, 5) sẽ di chuyển con trỏ đến cột 10 và dòng 5 của console.

void FixConsoleWindow();
// Hàm này thực hiện khóa cửa sổ console — không cho người dùng kéo to hoặc maximize
// Bỏ WS_MAXIMIZEBOX (nút phóng to) và WS_THICKFRAME (viền kéo thay đổi kích thước)
// Gọi 1 lần trong main() ngay khi khởi động

void SetConsoleSize(int width, int height);
// Hàm này thiết lập kích thước của cửa sổ console và buffer console
// Đặt kích thước cửa sổ console theo số ký tự
// Buffer size cao hơn window 5 dòng để tránh scroll bar xuất hiện
// Gọi với SetConsoleSize(SCREEN_WIDTH, SCREEN_HEIGHT)

void HideCursor();
// Hàm này ẩn con trỏ console để không hiển thị khi chơi game
// Ẩn con trỏ nhấp nháy (dấu gạch dưới)
// Gọi ngay đầu main() — nếu không ẩn, con trỏ nhảy lung tung khi vẽ game trông rất xấu

void ShowCursor();
// Hàm này hiện lại con trỏ — gọi khi cần người dùng nhập liệu (ví dụ nhập đường dẫn file save)

void SetColor(int color);
// Hàm này thiết lập màu sắc cho văn bản console
// Đặt màu chữ cho tất cả text in ra sau lệnh này
// Dùng các hằng COLOR_* ở trên, ví dụ: SetColor(COLOR_PLAYER)

void ResetColor();
// Hàm này đặt lại màu sắc về mặc định (COLOR_DEFAULT)
// Gọi sau mỗi lần SetColor() để không làm ảnh hưởng text in tiếp theo

void ClearPos(int x, int y, int width = 1);
// Xóa một đoạn width ký tự tại vị trí (x,y) bằng cách in đè bằng dấu cách
// QUAN TRỌNG: T2 và T3 dùng hàm này để xóa vết xe/thú cũ trước khi vẽ ở vị trí mới
// Ví dụ: ClearPos(oldX, y, 4) xóa xe hơi rộng 4 ký tự tại vị trí cũ

void PrintAt(int x, int y, const std::string& text, int color = COLOR_DEFAULT);
// Hàm này in một chuỗi text tại vị trí (x,y) với màu sắc color
// Gộp GotoXY + SetColor + cout + ResetColor vào 1 lệnh cho tiện
// Ví dụ: PrintAt(10, 5, "[=>]", COLOR_CAR) — vẽ xe hơi màu xanh tại (10,5)
// T2, T3, T5 đều dùng hàm này trong Draw() của từng lớp

DWORD GetCurrentTimeMs();
// Hàm này trả về thời gian hiện tại tính bằng milliseconds kể từ khi hệ thống khởi động
// Trả về thời gian hiện tại tính bằng milliseconds (từ lúc khởi động Windows)
// T2 có thể dùng để làm timer cho CTRAFFICLIGHT thay vì đếm frame
// Ví dụ: if(GetCurrentTimeMs() - lastSwitch > 3000) { đổi đèn }

#endif
