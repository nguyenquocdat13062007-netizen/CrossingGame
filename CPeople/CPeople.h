#pragma once
#ifndef CPEOPLE_H
#define CPEOPLE_H

#include "Utils.h"
#include "CVehicle.h"
#include "CAnimal.h"
using namespace std;
// =============================================
// LỚP CPEOPLE — Người qua đường
// =============================================
class CPEOPLE {
private:
    int  mX, mY;
    // Tọa độ hiện tại của nhân vật trên màn hình console
   // mX = cột (ngang), mY = dòng (dọc)
   // Bắt đầu từ giữa màn hình, dòng ROAD_BOTTOM

    bool mAlive;
	// Trạng thái sống chết của nhân vật
     // true  = nhân vật còn sống, đang chơi bình thường
    // false = đã chết do va chạm xe hoặc thú

    bool mFinished;
	// Trạng thái đã về đích hay chưa
     // true  = nhân vật đã qua đường thành công (chạm FINISH_Y)
    // false = chưa qua được, đang trên đường

public:
    CPEOPLE();
    // Constructor mặc định — tạo nhân vật tại vị trí mặc định
   // mX = SCREEN_WIDTH / 2 (giữa màn hình), mY = ROAD_BOTTOM
   // mAlive = true, mFinished = false

    CPEOPLE(int startX, int startY);
    // Constructor có tham số — tạo nhân vật tại vị trí tuỳ chỉnh
   // Dùng khi loadGame() cần phục hồi vị trí đã lưu của T4

    int  getX() const { return mX; }
    // Trả về tọa độ cột hiện tại của nhân vật
   // const ở cuối = hàm này không thay đổi bất kỳ field nào của object
   // T1 dùng để kiểm tra va chạm trong checkCollision()

    int  getY() const { return mY; }
    // Trả về tọa độ dòng hiện tại của nhân vật
   // T4 dùng để lưu vị trí vào GameSaveData khi saveGame()

    bool isAlive() const { return mAlive; }
	// Trả về trạng thái sống chết của nhân vật
	// T1 dùng để kiểm tra va chạm trong checkCollision()
	// T4 dùng để lưu trạng thái vào GameSaveData khi saveGame()

    bool isDead() const { return !mAlive; }
	// Trả về trạng thái đã chết của nhân vật
	// T1 dùng để kiểm tra va chạm trong checkCollision()
	// T4 dùng để lưu trạng thái vào GameSaveData khi saveGame()

    bool isFinished() const { return mFinished; }
	// Trả về trạng thái đã về đích hay chưa của nhân vật
	// T1 dùng để kiểm tra khi nhân vật chạm vào điểm kết thúc
	// T4 dùng để lưu trạng thái vào GameSaveData khi saveGame()

    void Up(int step = 1);
    // Di chuyển nhân vật lên trên step dòng (mY -= step)
   // step = 1 mặc định — có thể truyền 2 để đi nhanh hơn
   // Có kiểm tra biên: không cho vượt quá ROAD_TOP

    void Down(int step = 1);
    // Di chuyển xuống dưới (mY += step)
    // Có kiểm tra biên: không cho vượt quá ROAD_BOTTOM
   
    void Left(int step = 1);
    // Di chuyển sang trái (mX -= step)
    // Có kiểm tra biên: không cho vượt quá cột 0

    void Right(int step = 1);
    // Di chuyển sang phải (mX += step)
    // Có kiểm tra biên: không cho vượt quá SCREEN_WIDTH - 1

    void Draw();
    // Vẽ ký tự đại diện nhân vật tại (mX, mY) lên màn hình
    // Dùng PrintAt(mX, mY, "(Y)", COLOR_PLAYER) từ Utils.h
    // SubThread gọi hàm này mỗi frame trong drawGame()
   
    void Clear();
    // Xóa ký tự nhân vật tại vị trí cũ bằng ClearPos(mX, mY, 3)
    // Phải gọi Clear() TRƯỚC khi di chuyển, sau đó mới Draw() lại
    // Tránh để lại "vết ma" trên màn hình

    bool isImpact(const CVEHICLE* v) const;
    // Kiểm tra nhân vật có đang trùng tọa độ với xe v không
    // const CVEHICLE* = con trỏ tới xe, không được sửa đổi xe đó
    // Trả về true nếu mX nằm trong phạm vi xe và mY == v->getY()

    bool isImpact(const CANIMAL* a) const;
    // Overload của hàm trên — kiểm tra va chạm với thú a
   // Cùng logic nhưng dùng CANIMAL thay vì CVEHICLE
   // Đây là ví dụ về function overloading trong OOP

    bool checkFinish();
    // Kiểm tra mY có bằng FINISH_Y(ROAD_TOP = 3) không
    // Nếu đúng: đặt mFinished = true và trả về true
    // CGAME gọi hàm này trong checkCollision() sau mỗi bước di chuyển

    void Reset(int startX = SCREEN_WIDTH / 2, int startY = ROAD_BOTTOM);
    // Đặt lại nhân vật về vị trí ban đầu
   // SCREEN_WIDTH / 2 = 80 / 2 = 40 (cột giữa màn hình)
   // ROAD_BOTTOM = 26 (dòng cuối con đường, khai báo trong Utils.h)
   // Đặt lại mAlive = true, mFinished = false
   // CGAME gọi sau khi người chơi qua đường hoặc bắt đầu màn mới

    void Die();
    // Đặt mAlive = false
    // Gọi khi phát hiện va chạm trong isImpact()
    // Sau khi Die(), SubThread ngừng gọi updatePosPeople()

    void DrawDeathEffect();
    // Hiển thị hiệu ứng chết tại vị trí (mX, mY)
    // Ví dụ: in "[X]" màu đỏ + Beep(400, 300)
    // T2 cũng có thể hỗ trợ hàm này bằng âm thanh va chạm
};

#endif
