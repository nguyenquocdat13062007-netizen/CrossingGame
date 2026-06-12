#pragma once
#ifndef CGAME_H
#define CGAME_H
#include "Utils.h"
#include "CPeople.h"
#include "CVehicle.h"
#include "CAnimal.h"
#include <fstream>
#include <string>
#include <thread>
class CGAME {
private:
    // ---- Mảng đối tượng (cấp phát động) ----

    CTRUCK* mTrucks[MAX_TRUCKS];
    // Mảng con trỏ tới MAX_TRUCKS (=3) xe tải
    // Dùng con trỏ thay vì object trực tiếp để linh hoạt: new/delete khi đổi level
    // CGAME tạo: mTrucks[i] = new CTRUCK(x, y, speed, dir)
    // CGAME xóa: delete mTrucks[i] trong destructor hoặc resetGame()

    CCAR* mCars[MAX_CARS];
    // Mảng con trỏ tới MAX_CARS (=4) xe hơi — tương tự CTRUCK

    CDINOSAUR* mDinos[MAX_DINOS];
    // Mảng con trỏ tới MAX_DINOS (=2) khủng long
    // CDINOSAUR kế thừa CANIMAL — khai báo trong CAnimal.h

    CBIRD* mBirds[MAX_BIRDS];
    // Mảng con trỏ tới MAX_BIRDS (=3) chim — tương tự CDINOSAUR

    CTRAFFICLIGHT* mLights[MAX_TRUCKS + MAX_CARS];
    // Mảng đèn giao thông — 1 đèn cho mỗi làn xe
    // MAX_TRUCKS + MAX_CARS = 3 + 4 = 7 đèn tối đa
    // mLights[i] quản lý làn xe thứ i, khi đỏ thì dừng tất cả xe ở làn đó

    CPEOPLE mPeople;
    // Object nhân vật người chơi — dùng composition (chứa trực tiếp, không qua con trỏ)
    // Composition vì CGAME và CPEOPLE có vòng đời gắn liền: CGAME chết thì CPEOPLE cũng mất

    // ---- Trạng thái game ----

    int mLevel;
    // Cấp độ hiện tại, từ 1 đến MAX_LEVEL (=5)
    // Tăng trong nextLevel(), reset về 1 trong resetGame()

    int mScore;
    // Điểm tích lũy của người chơi
    // Tăng mỗi khi qua đường thành công: mScore += mLevel * 100

    int mLives;
    // Số mạng còn lại — thường bắt đầu là 3
    // Giảm 1 khi chết, hết mạng thì Game Over

    bool mPaused;
    // true  = game đang tạm dừng (người dùng nhấn P)
    // false = game đang chạy bình thường
    // SubThread kiểm tra mPaused trước mỗi frame update

    // ---- Số lượng xe/thú theo cấp ----

    int mNumTrucks;
    // Số xe tải thực sự đang dùng ở level hiện tại (≤ MAX_TRUCKS)
    // Level 1: 1 truck, Level 2: 2 trucks... tăng dần theo nextLevel()

    int mNumCars;
    // Số xe hơi thực sự đang dùng (≤ MAX_CARS)

    int mNumDinos;
    // Số khủng long thực sự đang dùng (≤ MAX_DINOS)

    int mNumBirds;
    // Số chim thực sự đang dùng (≤ MAX_BIRDS)

    std::thread* mSubThread;
    // Con trỏ tới thread phụ chạy game loop (vẽ + update + kiểm tra)
    // Dùng con trỏ để CGAME kiểm soát vòng đời thread
    // pauseGame() gọi SuspendThread(), resumeGame() gọi ResumeThread()

    // ---- Hàm nội bộ (private — chỉ CGAME tự gọi) ----

    void InitLanes();
    // Tạo và đặt vị trí ban đầu cho xe/thú theo từng làn đường
    // Phân bổ: làn 3-6 cho xe (ROAD_TOP → ROAD_BOTTOM), làn 1-2 cho thú
    // Init() gọi hàm này sau khi khởi tạo số lượng xe/thú

    void DrawStatus();
    // Vẽ thanh trạng thái phía dưới màn hình: Level, Score, Lives, phím tắt
    // Gọi PrintAt() với COLOR_INFO
    // drawGame() gọi hàm này ở cuối mỗi frame

    int GetLaneY(int laneIndex);
    // Tính tọa độ Y của làn thứ laneIndex
    // Công thức: ROAD_TOP + laneIndex * ((ROAD_BOTTOM - ROAD_TOP) / LANE_COUNT)
    // InitLanes() gọi để biết đặt xe/thú ở dòng mấy

public:
    CGAME();
    // Constructor — khởi tạo tất cả con trỏ về nullptr, mLevel=1, mScore=0, mLives=3
    // KHÔNG new xe/thú ở đây — để Init() làm việc đó

    ~CGAME();
    // Destructor — delete tất cả xe, thú, đèn đã new trong Init()
    // Bắt buộc vì dùng con trỏ động: nếu không delete → memory leak

    int getmLives();
    // Getter cho mLives — main() dùng để kiểm tra còn mạng không
    // Nếu mLives == 0 → hiện Game Over

    // ---- Khởi động / Reset ----

    void Init();
    // Khởi tạo toàn bộ: new xe/thú/đèn, đặt vị trí qua InitLanes()
    // Gọi 1 lần khi bắt đầu game, và gọi lại khi loadGame()

    void startGame();
    // Reset dữ liệu + vẽ màn hình lần đầu + khởi động SubThread
    // Đây là hàm main() gọi sau khi người dùng chọn "New Game" ở menu

    void DrawBorder();
    // Vẽ khung viền xung quanh màn hình bằng ký tự '─', '│', '┌', '┐'...
    // Gọi 1 lần trong startGame(), không cần vẽ lại mỗi frame

    void DrawRoad();
    // Vẽ các làn đường: dấu gạch ngang '─' phân cách giữa các làn
    // Gọi 1 lần trong startGame() cùng với DrawBorder()

    void resetGame();
    // Reset người về vị trí xuất phát, reset mScore về 0, mLevel về 1
    // Gọi khi người chơi hết mạng và chọn chơi lại

    void nextLevel();
    // Tăng mLevel++, tăng số lượng xe/thú, tăng tốc độ
    // Delete xe/thú cũ, new lại với thông số khó hơn
    // Gọi khi checkFinish() trả về true

    // ---- Vẽ màn hình ----

    void drawGame();
    // Vẽ toàn bộ trạng thái 1 frame: xe, thú, người, đèn, trạng thái
    // SubThread gọi hàm này mỗi Sleep(100) — khoảng 10 FPS
    // KHÔNG gọi system("cls") — thay vào đó Clear() rồi Draw() từng object

    // ---- Getter cho thread chính ----

    CPEOPLE& getPeople() { return mPeople; }
    // Trả về tham chiếu tới object mPeople
    // main() dùng: if (cg.getPeople().isDead()) để kiểm tra trạng thái
    // Tham chiếu (không phải con trỏ) — đảm bảo không bao giờ null

    CVEHICLE** getVehicles();
    // Trả về mảng con trỏ CVEHICLE* (upcasting từ CCAR*/CTRUCK*)
    // T4 dùng trong saveGame() để duyệt qua tất cả xe lưu vị trí

    CANIMAL** getAnimals();
    // Trả về mảng con trỏ CANIMAL* (upcasting từ CBIRD*/CDINOSAUR*)
    // T4 dùng tương tự getVehicles()

    int getVehicleCount() const;
    // Trả về tổng số xe đang hoạt động = mNumTrucks + mNumCars

    int getAnimalCount() const;
    // Trả về tổng số thú đang hoạt động = mNumDinos + mNumBirds

    // ---- Cập nhật vị trí ----

    void updatePosPeople(char key);
    // Di chuyển nhân vật theo phím: 'W'=Up, 'S'=Down, 'A'=Left, 'D'=Right
    // Gọi Clear() trước, di chuyển, rồi Draw() sau
    // SubThread gọi với biến toàn cục MOVING mỗi frame

    void updatePosVehicle();
    // Duyệt tất cả xe: nếu không bị dừng thì gọi Clear() + Move() + Draw()
    // Kiểm tra đèn giao thông trước khi Move()

    void updatePosAnimal();
    // Duyệt tất cả thú: Clear() + Move() + Draw()
    // Thú không bị ảnh hưởng bởi đèn giao thông

    void updateTrafficLights();
    // Gọi Update() cho từng đèn trong mLights[]
    // Khi đèn đổi màu: gọi Stop()/Resume() cho xe ở làn tương ứng
    // SubThread gọi mỗi frame trước updatePosVehicle()

    // ---- Kiểm tra trạng thái ----

    bool checkCollision();
    // Kiểm tra nhân vật có trùng tọa độ với xe/thú nào không
    // Nếu có: gọi mPeople.Die(), DrawDeathEffect(), giảm mLives
    // Trả về true nếu người vừa chết — SubThread dừng update, hiện thông báo

    bool checkFinish();
    // Kiểm tra mPeople.isFinished() (đã chạm FINISH_Y)
    // Nếu true: tăng điểm, gọi nextLevel() hoặc showWin() nếu đã max level
    // Trả về true để SubThread biết cần chuyển level

    // ---- Pause / Resume ----

    void pauseGame(HANDLE hThread);
    // Dừng SubThread bằng SuspendThread(hThread)
    // Đặt mPaused = true, hiện thông báo "PAUSED"
    // main() gọi khi người dùng nhấn phím 'P'

    void resumeGame(HANDLE hThread);
    // Tiếp tục SubThread bằng ResumeThread(hThread)
    // Đặt mPaused = false, xóa thông báo "PAUSED"
    // main() gọi khi người dùng nhấn bất kỳ phím nào sau pause

    bool isPaused() const { return mPaused; }
    // Trả về trạng thái pause hiện tại
    // main() kiểm tra trước khi xử lý phím di chuyển:
    // if (!cg.isPaused()) MOVING = key

    // ---- Exit ----

    void exitGame(HANDLE hThread);
    // Đặt IS_RUNNING = false, gọi ResumeThread() nếu đang pause
    // Chờ SubThread tự kết thúc, sau đó dọn dẹp bộ nhớ
    // main() gọi khi người dùng nhấn ESC

    // ---- Save / Load ----

    bool saveGame(const std::string& filename);
    // Đóng gói toàn bộ trạng thái game vào GameSaveData struct
    // Ghi binary ra file bằng ofstream
    // Trả về true nếu lưu thành công, false nếu lỗi mở file
    // T4 implement hàm này trong SaveLoad.cpp

    bool loadGame(const std::string& filename);
    // Đọc file binary vào GameSaveData struct
    // Delete đối tượng cũ, new lại từ dữ liệu đọc được
    // Trả về true nếu load thành công
    // T4 implement hàm này trong SaveLoad.cpp

    // ---- Menu và thông báo ----

    void showMenu();
    // Hiện menu chính: New Game / Load Game / Settings / Thoát
    // Dùng SetColor(COLOR_TITLE) cho tiêu đề, GotoXY căn giữa
    // T5 implement hàm này trong Menu.cpp

    void showPauseMsg();
    // Hiện "[PAUSED] Nhan phim bat ky de tiep tuc" ở giữa màn hình
    // T5 implement

    void showDeadMsg();
    // Hiện thông báo chết: "[X] GAME OVER" hoặc "Nhan Y de choi lai"
    // T5 implement

    void showLevelUp();
    // Hiện thông báo lên cấp: "LEVEL UP! → Level X" trong 1 giây
    // T5 implement, dùng Sleep(1000) rồi tự xóa

    void showWin();
    // Hiện thông báo thắng khi qua hết MAX_LEVEL cấp
    // T5 implement

    // ---- Getters ----

    int getLevel() const { return mLevel; }
    // Trả về cấp độ hiện tại
    // DrawStatus() và T4 (saveGame) dùng

    int getScore() const { return mScore; }
    // Trả về điểm hiện tại
    // DrawStatus() và T4 (saveGame) dùng
};

void SubThread();
// Hàm chạy trong thread phụ — khai báo ngoài class vì std::thread cần hàm tự do
// Không phải method của CGAME — nhưng truy cập object CGAME qua biến toàn cục
// Vòng lặp: while(IS_RUNNING) { update + draw + check + Sleep(100) }

#endif 