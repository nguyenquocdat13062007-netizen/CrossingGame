#pragma once
#ifndef CANIMAL_H
#define CANIMAL_H
#include "Utils.h"
#include <string>
using namespace std;
// =============================================
// LỚP CƠ SỞ: CANIMAL
// =============================================
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
    // Hướng di chuyển: +1 = sang phải, -1 = sang trái
    // Move(): mX += mDirection * mSpeed

    int mWidth;
    // Độ rộng hình vẽ tính bằng số ký tự
    // CBIRD = 3 ký tự "~v~", CDINOSAUR = 4 ký tự ">D=="
    // Dùng trong Clear() để xóa đúng số ký tự
    // Dùng trong isImpact() của CPEOPLE để tính vùng va chạm

    int mColor;
    // Màu khi vẽ lên màn hình — dùng hằng COLOR_* từ Utils.h
    // CBIRD = COLOR_BIRD (11, cyan), CDINOSAUR = COLOR_DINO (13, tím)

    DWORD mLastSoundTime;
    // Lưu thời điểm lần cuối thú phát tiếng kêu (milliseconds)
    // Lấy từ GetCurrentTimeMs() trong Utils.h
    // Tell() kiểm tra: if (GetCurrentTimeMs() - mLastSoundTime > 2000)
    // → chỉ kêu mỗi 2 giây, không kêu liên tục mỗi frame

public:
    CANIMAL();
    // Constructor mặc định — khởi tạo giá trị mặc định
    // mLastSoundTime = 0 để lần đầu chạy game kêu được ngay

    CANIMAL(int x, int y, int speed, int dir, int color, int width);
    // Constructor có tham số — CGAME gọi khi tạo thú:
    // new CBIRD(x, y, 3, +1) hoặc new CDINOSAUR(x, y, 1, -1)

    virtual ~CANIMAL() {}
    // Destructor ảo — bắt buộc khi dùng con trỏ lớp cơ sở CANIMAL*
    // Đảm bảo khi CGAME gọi delete animals[i] thì destructor
    // đúng của CBIRD hoặc CDINOSAUR được gọi, tránh memory leak
    // {} rỗng vì không có tài nguyên động cần giải phóng thêm

    int getX()     const { return mX; }
    // Trả về cột hiện tại
    // CPEOPLE::isImpact() dùng để so sánh với vị trí người chơi

    int getY()     const { return mY; }
    // Trả về dòng hiện tại
    // CGAME dùng để xác định thú đang ở làn nào

    int getWidth() const { return mWidth; }
    // Trả về độ rộng hình vẽ
    // CPEOPLE::isImpact() dùng để tính vùng va chạm:
    // va chạm khi mX người >= mX thú VÀ <= mX thú + mWidth
    void setX(int x) { mX = x; }

    virtual void Move(int screenWidth, int screenHeight);
    // Di chuyển thú: mX += mDirection * mSpeed
    // Xử lý wrap màn hình: ra khỏi bên phải → xuất hiện bên trái
    // virtual = CBIRD/CDINOSAUR có thể override nếu cần di chuyển đặc biệt
    // (ví dụ CBIRD có thể bay lên xuống thêm theo trục Y)

    virtual void Draw();
    // Vẽ hình thú tại (mX, mY) bằng PrintAt(mX, mY, GetShape(), mColor)
    // virtual = lớp con override để vẽ hình riêng với màu riêng

    virtual void Clear();
    // Xóa hình thú tại vị trí hiện tại bằng ClearPos(mX, mY, mWidth)
    // Phải gọi Clear() TRƯỚC khi Move(), sau đó mới Draw() lại
    // virtual = lớp con có thể override nếu cần xóa theo cách riêng

    virtual void Tell() = 0;
    // Pure virtual — CANIMAL không có tiếng kêu mặc định
    // = 0 nghĩa là: bắt buộc mọi lớp con PHẢI override hàm này
    // Nếu CBIRD hoặc CDINOSAUR không override Tell() → lỗi compile
    // Hệ quả: CANIMAL trở thành Abstract Class — không thể new CANIMAL() trực tiếp
    // Chỉ có thể new CBIRD() hoặc new CDINOSAUR()

    virtual std::string GetShape() const;
    // Trả về chuỗi ASCII đại diện
    // CANIMAL trả về chuỗi mặc định (fallback)
    // CBIRD override: "~v~" (đi phải) hoặc "~^~" (đi trái)
    // CDINOSAUR override: ">D==" (đi phải) hoặc "==D<" (đi trái)
    // Không phải pure virtual vì có thể có giá trị mặc định hợp lệ
};

// =============================================
// LỚP CON: CBIRD — Chim
// =============================================
class CBIRD : public CANIMAL {
    // public inheritance = toàn bộ public/protected của CANIMAL
    // giữ nguyên quyền truy cập trong CBIRD

public:
    CBIRD();
    // Constructor mặc định — tạo chim với:
    // color = COLOR_BIRD (11, cyan), width = 3, speed = 3
    // Chim nhỏ và nhanh hơn khủng long

    CBIRD(int x, int y, int speed, int dir);
    // Constructor có tham số — CGAME dùng khi khởi tạo theo level

    void Draw()  override;
    // Vẽ chim tại (mX, mY) bằng PrintAt với COLOR_BIRD
    // Hình: "~v~" khi bay sang phải, "~^~" khi bay sang trái

    void Clear() override;
    // Xóa đúng 3 ký tự (width của CBIRD) tại vị trí cũ

    void Tell()  override;
    // Phát tiếng chim: Beep(1200, 80) — tần số cao (1200Hz), ngắn (80ms)
    // Kiểm tra mLastSoundTime trước khi Beep() để không kêu mỗi frame
    // Cập nhật mLastSoundTime = GetCurrentTimeMs() sau khi kêu

    std::string GetShape() const override;
    // Trả về "~v~" nếu mDirection == +1 (bay phải)
    // Trả về "~^~" nếu mDirection == -1 (bay trái)
};

// =============================================
// LỚP CON: CDINOSAUR — Khủng long
// =============================================
class CDINOSAUR : public CANIMAL {
public:
    CDINOSAUR();
    // Constructor mặc định — tạo khủng long với:
    // color = COLOR_DINO (13, tím), width = 4, speed = 1
    // Khủng long to và chậm hơn chim

    CDINOSAUR(int x, int y, int speed, int dir);
    // Constructor có tham số — CGAME dùng khi khởi tạo theo level

    void Draw()  override;
    // Vẽ khủng long tại (mX, mY) bằng PrintAt với COLOR_DINO
    // Hình: ">D==" khi chạy phải, "==D<" khi chạy trái

    void Clear() override;
    // Xóa đúng 4 ký tự (width của CDINOSAUR) tại vị trí cũ

    void Tell()  override;
    // Phát tiếng khủng long: Beep(300, 200) — tần số thấp (300Hz), dài (200ms)
    // Nghe trầm và đáng sợ hơn tiếng chim
    // Cũng kiểm tra mLastSoundTime — kêu mỗi 3 giây thay vì 2 giây như chim

    std::string GetShape() const override;
    // Trả về ">D==" nếu mDirection == +1 (chạy phải)
    // Trả về "==D<" nếu mDirection == -1 (chạy trái)
};

#endif
