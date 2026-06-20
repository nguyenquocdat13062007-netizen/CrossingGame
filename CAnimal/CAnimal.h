#pragma once
#ifndef CANIMAL_H
#define CANIMAL_H

#include "Utils.h"
#include "AnimatedSprite.h"
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

// ================================================================
// LOP CO SO: CANIMAL (Abstract)
// Quan ly di chuyen + ve sprite cho chim va da lan
// ================================================================
class CANIMAL {
protected:
    // protected = CBIRD và CDINOSAUR truy cập được trực tiếp
    // Nếu là private thì lớp con phải dùng getter — bất tiện hơn
	int mX, mY;
    // Tọa độ hiện tại của thú trên màn hình
    // Cập nhật mỗi frame trong Move()

    int mSpeed;       
    // Số ký tự thú di chuyển mỗi lần gọi Move()
   // CBIRD nhanh hơn (speed=3), CDINOSAUR chậm hơn (speed=1)

    int mDirection;  
	// 1 = di sang phải, -1 = di sang trái  
	// Cập nhật mỗi frame trong Move()
    int mWidth;       
	// Chiều rộng thú (tính theo số ký tự)
	// Cập nhật mỗi frame trong Move()

    Color mColor; 
	// Màu sắc thú (Color)
    AnimatedSprite mAnim;  
	// Animation của thú, quản lý bởi AnimatedSprite
	
public:
    CANIMAL();
	// Constructor mặc định — CGAME gọi khi khởi tạo mảng thú:
	// new CBIRD() hoặc new CDINOSAUR() trong CGame.cpp

    CANIMAL(int x, int y, int speed, int dir, Color color, int width);
    // Constructor có tham số — CGAME gọi khi tạo thú:
   // new CBIRD(x, y, 3, +1) hoặc new CDINOSAUR(x, y, 1, -1)

    virtual ~CANIMAL() {}
    // Destructor ảo — bắt buộc khi dùng con trỏ lớp cơ sở CANIMAL*
   // Đảm bảo khi CGAME gọi delete animals[i] thì destructor
   // đúng của CBIRD hoặc CDINOSAUR được gọi, tránh memory leak
   // {} rỗng vì không có tài nguyên động cần giải phóng thêm

    int getX()     const { return mX; }
    // Trả về cột hiện tại
	// CGAME sẽ dùng getX() để kiểm tra va chạm với người chơi (isImpact)
    //
    int getY()     const { return mY; }
	// Trả về hàng hiện tại
	// CGAME sẽ dùng getY() để kiểm tra va chạm với người chơi (isImpact)
    int getWidth() const { return mWidth; }
    // Trả về độ rộng hình vẽ
   // CPEOPLE::isImpact() dùng để tính vùng va chạm:
   // va chạm khi mX người >= mX thú VÀ <= mX thú + mWidth
    void setX(int x) { mX = x; }
	// CGAME sẽ dùng setX() để cập nhật vị trí thú sau mỗi lần Move()
	// Không cần setY() vì thú chỉ di chuyển ngang, hàng cố định
	// CGAME sẽ gọi Move() mỗi frame để cập nhật vị trí thú
    void loadAssets(const std::string& frame1,
        const std::string& frame2 = "");
	// Tải ảnh cho animation của thú, gọi trong constructor của CBIRD/CDINOSAUR
    void updateAnim(float dt) { mAnim.update(dt); }
	// Cập nhật animation theo thời gian, gọi mỗi frame từ CGAME.updateAnimations()
    
    virtual void Move(int screenWidthCells);
	// Cập nhật vị trí của thú dựa trên tốc độ và hướng di chuyển
   
    virtual void Draw(RenderWindow& window);
	// Vẽ thú lên cửa sổ, gọi mỗi frame từ CGAME.drawGame()
 
    virtual void Tell() = 0;
	// Phương thức ảo thuần túy (pure virtual) — bắt buộc lớp con phải override
};

// ================================================================
// LOP CON: CBIRD - Chim (nho, nhanh)
// Asset: bird_frame1_48x48.png, bird_frame2_48x48.png
// ================================================================
class CBIRD : public CANIMAL {
public:
    CBIRD();
	// Constructor mặc định — CGAME gọi khi khởi tạo mảng thú:
	// new CBIRD() trong CGame.cpp
    
    CBIRD(int x, int y, int speed, int dir);
	// Constructor có tham số — CGAME gọi khi tạo thú:
	// new CBIRD(x, y, 3, +1) hoặc new CBIRD(x, y, 3, -1)
    void Tell() override;
	// Override phương thức ảo thuần túy của CANIMAL, bắt buộc phải có
};

// ================================================================
// LOP CON: CROCK - Da lan (thay the CDINOSAUR)
// To, cham, lan ngang qua duong
// Asset: rock_frame1.png, rock_frame2.png
// ================================================================
class CROCK : public CANIMAL {
public:
    CROCK();
	// Constructor mặc định — CGAME gọi khi khởi tạo mảng thú:
	// new CROCK() trong CGame.cpp
    CROCK(int x, int y, int speed, int dir);
	// Constructor có tham số — CGAME gọi khi tạo thú:
	// new CROCK(x, y, 1, +1) hoặc new CROCK(x, y, 1, -1)
    void Tell() override;
	// Override phương thức ảo thuần túy của CANIMAL, bắt buộc phải có
};

#endif
