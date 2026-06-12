#pragma once
#ifndef CVEHICLE_H
#define CVEHICLE_H
#include "Utils.h"
#include <string>
using namespace std;
// =============================================
// LỚP CƠ SỞ: CVEHICLE
// =============================================
class CVEHICLE {
protected:
    // protected = các lớp con CCAR, CTRUCK truy cập được
    // private   = chỉ CVEHICLE truy cập, lớp con KHÔNG được

    int mX, mY;
    // Tọa độ hiện tại của xe trên màn hình
    // mX = cột (ngang), mY = dòng (dọc) — cập nhật mỗi frame trong Move()

    int mSpeed;
    // Số ký tự xe di chuyển mỗi lần gọi Move()
    // CCAR speed = 2, CTRUCK speed = 1 — xe tải chậm hơn xe hơi

    int mDirection;
    // Hướng di chuyển: +1 = sang phải (mX tăng), -1 = sang trái (mX giảm)
    // Kết hợp với mSpeed: mX += mDirection * mSpeed mỗi frame

    int mWidth;
    // Độ rộng hình vẽ của xe tính bằng số ký tự
    // CCAR = 4 ký tự "[=>]", CTRUCK = 7 ký tự "[=|=>]"
    // Dùng trong Clear() để xóa đúng số ký tự, và trong isImpact() để tính vùng va chạm

    int mColor;
    // Màu xe khi vẽ lên màn hình — dùng hằng COLOR_* từ Utils.h
    // CCAR = COLOR_CAR (10, xanh lá), CTRUCK = COLOR_TRUCK (12, đỏ)

    bool mStopped;
    // true  = xe đang dừng (do đèn đỏ hoặc gọi Stop())
    // false = xe đang chạy bình thường
    // Move() kiểm tra: if (!mStopped) thì mới di chuyển

    DWORD mStopTimer;
    // Lưu thời điểm xe bắt đầu dừng (milliseconds từ GetCurrentTimeMs())
    // Dùng để tính xem đã dừng đủ thời gian chưa trong UpdateStopStatus()

    int mStopDuration;
    // Thời gian xe cần dừng tính bằng milliseconds
    // Ví dụ: Stop(2000) → xe dừng 2 giây rồi tự chạy lại

public:
    CVEHICLE();
    // Constructor mặc định — khởi tạo xe với giá trị mặc định
    // mStopped = false, mX = 0, mY = 0, mSpeed = 1...

    CVEHICLE(int x, int y, int speed, int dir, int color, int width);
    // Constructor có tham số — tạo xe tại vị trí và cấu hình cụ thể
    // CGAME gọi khi khởi tạo mảng xe: new CCAR(x, y, speed, dir)

    virtual ~CVEHICLE() {}
    // Destructor ảo — bắt buộc phải có khi dùng con trỏ lớp cơ sở
    // Nếu không có virtual: delete vehicles[i] chỉ gọi destructor của CVEHICLE
    // mà không gọi destructor của CCAR/CTRUCK → memory leak
    // {} rỗng vì CVEHICLE không có tài nguyên cần giải phóng thêm

    int  getX()      const { return mX; }
    // Trả về cột hiện tại — CGAME dùng để kiểm tra va chạm với nhân vật

    int  getY()      const { return mY; }
    // Trả về dòng hiện tại — xác định xe đang ở làn nào

    int  getWidth()  const { return mWidth; }
    // Trả về độ rộng xe — CPEOPLE dùng trong isImpact() để tính vùng va chạm
    // Va chạm xảy ra khi: mX của người >= mX xe VÀ <= mX xe + mWidth
    void setX(int x) { mX = x; }

    bool isStopped() const { return mStopped; }
    // Trả về trạng thái dừng của xe
    // CGAME kiểm tra trước khi gọi Move(): if (!vehicles[i]->isStopped())

    virtual void Move(int screenWidth, int screenHeight);
    // Di chuyển xe: mX += mDirection * mSpeed
    // Xử lý wrap màn hình: nếu ra khỏi bên phải thì xuất hiện lại bên trái và ngược lại
    // virtual = CCAR và CTRUCK có thể override nếu cần di chuyển khác

    virtual void Draw();
    // Vẽ hình xe tại (mX, mY) với màu mColor
    // Gọi PrintAt(mX, mY, GetShape(), mColor)
    // virtual = CCAR và CTRUCK override để vẽ hình riêng

    virtual void Clear();
    // Xóa hình xe tại vị trí hiện tại bằng ClearPos(mX, mY, mWidth)
    // Phải gọi Clear() TRƯỚC khi Move(), sau đó mới Draw() lại
    // virtual = lớp con có thể override nếu cần xóa theo cách riêng

    virtual std::string GetShape() const;
    // Trả về chuỗi ASCII đại diện cho xe
    // CVEHICLE trả về chuỗi mặc định
    // CCAR override trả về "[=>]" hoặc "[<=]" tuỳ mDirection
    // CTRUCK override trả về "[=|=>]" hoặc "[<=|=]"

    void Stop(int duration_ms);
    // Dừng xe trong duration_ms milliseconds
    // Đặt mStopped = true, lưu thời điểm hiện tại vào mStopTimer
    // CTRAFFICLIGHT gọi khi đèn chuyển đỏ

    void Resume();
    // Cho xe chạy lại ngay lập tức
    // Đặt mStopped = false
    // CTRAFFICLIGHT gọi khi đèn chuyển xanh

    void UpdateStopStatus();
    // Kiểm tra nếu đã dừng đủ mStopDuration milliseconds thì tự Resume()
    // Công thức: if (GetCurrentTimeMs() - mStopTimer >= mStopDuration) Resume()
    // SubThread gọi hàm này mỗi frame cho từng xe
};

// =============================================
// LỚP CON: CCAR — Xe hơi
// =============================================
class CCAR : public CVEHICLE {
    // public inheritance = toàn bộ public/protected của CVEHICLE giữ nguyên quyền truy cập
    // CCAR thừa kế tất cả field và hàm của CVEHICLE, chỉ override những gì khác biệt

public:
    CCAR();
    // Constructor mặc định — tạo xe hơi tại vị trí mặc định
    // Tự động gọi CVEHICLE(...) với color = COLOR_CAR, width = 4, speed = 2

    CCAR(int x, int y, int speed, int dir);
    // Constructor có tham số — CGAME dùng khi khởi tạo xe cho từng level

    void Draw()  override;
    // Vẽ xe hơi với hình "[=>]" (đi phải) hoặc "[<=]" (đi trái) màu COLOR_CAR
    // override = báo compiler kiểm tra xem CVEHICLE có hàm Draw() virtual không
    // Nếu CVEHICLE không có Draw() virtual → compiler báo lỗi ngay

    void Clear() override;
    // Xóa đúng 4 ký tự (width của CCAR) tại vị trí cũ

    std::string GetShape() const override;
    // Trả về "[=>]" nếu mDirection == +1, "[<=]" nếu mDirection == -1
};

// =============================================
// LỚP CON: CTRUCK — Xe tải
// =============================================
class CTRUCK : public CVEHICLE {
public:
    CTRUCK();
    // Constructor mặc định — color = COLOR_TRUCK, width = 7, speed = 1

    CTRUCK(int x, int y, int speed, int dir);
    // Constructor có tham số

    void Draw()  override;
    // Vẽ xe tải với hình "[=|=>]" hoặc "[<=|=]" màu COLOR_TRUCK (đỏ)

    void Clear() override;
    // Xóa đúng 7 ký tự (width của CTRUCK)

    std::string GetShape() const override;
    // Trả về "[=|=>]" hoặc "[<=|=]" tuỳ mDirection
};

// =============================================
// ĐÈN GIAO THÔNG: CTRAFFICLIGHT
// =============================================
class CTRAFFICLIGHT {
public:
    enum State { RED, GREEN };
    // enum = kiểu liệt kê — định nghĩa tập hợp giá trị có tên
    // State chỉ có 2 giá trị: RED = 0, GREEN = 1 (tự động gán số)
    // Dùng enum thay vì int để code rõ nghĩa hơn:
    // mState == RED  dễ đọc hơn  mState == 0

private:
    int mLane;
    // Số thứ tự làn đường mà đèn này quản lý (0 đến LANE_COUNT - 1)
    // CGAME dùng để biết đèn nào ảnh hưởng xe ở làn nào

    State mState;
    // Trạng thái hiện tại của đèn: RED hoặc GREEN
    // Khởi tạo = GREEN, sau đó Update() tự chuyển đổi theo timer

    DWORD mTimer;
    // Lưu thời điểm đèn bắt đầu trạng thái hiện tại (milliseconds)
    // Update() so sánh GetCurrentTimeMs() - mTimer với duration tương ứng

    int mGreenDuration;
    // Thời gian đèn xanh tính bằng milliseconds — mặc định 3000ms (3 giây)
    // Có thể tăng lên ở level cao để khó hơn

    int mRedDuration;
    // Thời gian đèn đỏ tính bằng milliseconds — mặc định 2000ms (2 giây)

public:
    CTRAFFICLIGHT(int lane, int greenMs = 3000, int redMs = 2000);
    // Constructor — tạo đèn cho làn lane với thời gian tuỳ chỉnh
    // greenMs = 3000, redMs = 2000 là giá trị mặc định nếu không truyền vào
    // Ví dụ: CTRAFFICLIGHT(3) → đèn làn 3, xanh 3s, đỏ 2s
    // Ví dụ: CTRAFFICLIGHT(3, 2000, 3000) → xanh 2s, đỏ 3s (khó hơn)

    void Update();
    // Kiểm tra thời gian đã trôi qua, nếu đủ thì chuyển trạng thái
    // Logic: if (now - mTimer >= duration) { đổi State, lưu mTimer = now }
    // Khi chuyển RED → gọi Stop() cho tất cả xe ở làn mLane
    // Khi chuyển GREEN → gọi Resume() cho tất cả xe ở làn mLane
    // SubThread gọi hàm này mỗi frame cho từng đèn

    State getState() const { return mState; }
    // Trả về trạng thái hiện tại: RED hoặc GREEN
    // Dùng khi cần biết trạng thái mà không muốn dùng isRed()

    bool isRed() const { return mState == RED; }
    // Trả về true nếu đang đèn đỏ
    // CGAME kiểm tra: if (lights[i].isRed()) → dừng xe ở làn đó

    void Draw(int x, int y);
    // Vẽ ký tự đèn tại toạ độ (x, y) trên màn hình
    // Đèn đỏ: PrintAt(x, y, "[R]", COLOR_TRUCK)  → màu đỏ
    // Đèn xanh: PrintAt(x, y, "[G]", COLOR_CAR)  → màu xanh lá
};

#endif
