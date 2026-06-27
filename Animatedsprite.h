#pragma once
#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <string>
#include "Utils.h"
using namespace sf;
using namespace std;

// ================================================================
// CLASS AnimatedSprite - Fix SFML 3.0
//
// BUG GOC: vector<Texture> khi push_back(tex2) co the REALLOCATE
//   -> &mFrames[0] thay doi -> mSprite giu reference cu -> DANGLING -> CRASH
//
// FIX: reserve(MAX_FRAMES) TRUOC khi push_back bat ky
//   -> vector khong bao gio reallocate trong gioi han do
//   -> &mFrames[0] luon hop le
// ================================================================
class AnimatedSprite {
private:
    static const int MAX_FRAMES = 8;
	// số frame tối đa của một animation (theo yêu cầu do an)
    vector<Texture>  mFrames;
	// vector quản lý các frame của animation, mỗi frame là một Texture
    optional<Sprite> mSprite;
	// Sprite dùng để vẽ animation, giữ reference đến Texture trong mFrames
    int mCurrentFrame;
	// Chỉ số frame hiện tại đang hiển thị (0-based)
    float mFrameTime;
	// Thời gian (giây) mỗi frame được hiển thị trước khi chuyển sang frame tiếp theo
    float mElapsed;
	// Thời gian đã trôi qua kể từ khi frame hiện tại bắt đầu hiển thị, dùng để quyết định khi nào chuyển frame

public:
    AnimatedSprite(float frameTime = 0.15f)
        : mCurrentFrame(0), mFrameTime(frameTime), mElapsed(0.f)
		// Khởi tạo AnimatedSprite với thời gian mỗi frame là frameTime (mặc định 0.15s)
		// mCurrentFrame = 0: bắt đầu từ frame đầu tiên
		// mElapsed = 0.f: chưa trôi qua thời gian nào kể từ khi frame đầu tiên bắt đầu hiển thị
    {
        mFrames.reserve(MAX_FRAMES);
		// Reserve MAX_FRAMES để tránh reallocation khi push_back
    }

    void clear() {
		mFrames.clear();
        mSprite.reset();
    }

    bool addFrame(const string& filename) {
        // Them 1 frame anh vao animation tu file PNG
        // - Load Texture tu duong dan filename
        // - Neu load that bai (file khong ton tai / sai ten): return false
        Texture tex;

        if (!tex.loadFromFile(filename)) {
            return false;
		}

		mFrames.push_back(tex);


        if (!mSprite.has_value()) {
            mSprite.emplace(mFrames[0]);
		}
        
        return true;


        // - Neu thanh cong: push vao mFrames, return true
        // - Frame dau tien duoc them: dat lam frame hien tai (index 0)
        // Goi bao nhieu lan = bao nhieu frame animation
        // Vi du: addFrame("bird1.png") + addFrame("bird2.png") = 2 frame vo canh
    }

    bool isLoaded() const {

		return !mFrames.empty();

        // Kiem tra xem AnimatedSprite da co anh chua
        // Return true  = da load it nhat 1 frame -> co the Draw()
        // Return false = chua co frame nao        -> Draw() se dung fallback mau sac
        // Dung truoc khi goi draw() de tranh crash khi chua load asset
        // Vi du: if (mAnim.isLoaded()) mAnim.draw(...); else ve hinh chu nhat
    }

    void update(float dt) {

        // Có 1 ảnh hoặc không có ảnh thì không cần lật
        if (mFrames.size() <= 1) return;

        mElapsed += dt; // Cộng dồn thời gian trôi qua

        if (mElapsed >= mFrameTime) {
            // Chuyển sang frame kế tiếp, quay vòng về 0 nếu đụng ranh giới
            mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
            mElapsed = 0.f; // Reset đồng hồ bấm giờ

            // Cập nhật lại bức ảnh mới vào khung tranh
            if (mSprite.has_value()) {
                mSprite->setTexture(mFrames[mCurrentFrame]);
            }
        }

        // Cap nhat frame animation theo thoi gian thuc
        // dt = thoi gian frame hien tai (giay), lay tu Clock.restart()
        // - Cong dt vao mElapsed (bieu tham so dem thoi gian)
        // - Khi mElapsed >= mFrameTime: chuyen sang frame tiep theo
        //   mCurrentFrame = (mCurrentFrame + 1) % so_frame
        //   Reset mElapsed = 0 de bat dau dem lai
        // - Neu chi co 1 frame: khong lam gi (khong can chuyen frame)
        // Goi moi frame render (60fps) de animation chay muot
    }

    void draw(RenderWindow& window, float px, float py, int widthCells = 1, int heightCells = 1, bool flipX = false)
    {

        if (!isLoaded() || !mSprite.has_value()) return;

        // Giả sử CELL_SIZE là hằng số kích thước ô lưới (ví dụ: 50 pixel)
        // Bạn cần thay thế CELL_SIZE bằng biến hệ thống của nhóm bạn
        const float CELL_SIZE = 50.0f;

        // Lấy kích thước gốc của bức ảnh
        FloatRect bounds = mSprite->getLocalBounds();

        // Tính toán tỷ lệ phóng to/thu nhỏ (Scale) để ảnh vừa khít ô lưới
        float scaleX = (widthCells * CELL_SIZE) / bounds.width;
        float scaleY = (heightCells * CELL_SIZE) / bounds.height;

        // Lật ảnh nếu nhân vật đi sang trái
        if (flipX) {
            mSprite->setScale(-scaleX, scaleY);
            // Khi lật âm trục X, gốc tọa độ bị ngược, phải dịch chuyển qua phải một chút
            mSprite->setPosition(px + (widthCells * CELL_SIZE), py);
        }
        else {
            mSprite->setScale(scaleX, scaleY);
            mSprite->setPosition(px, py);
        }

        // Vẽ ra màn hình
        window.draw(*mSprite);


        // Ve frame hien tai len cua so SFML tai vi tri pixel (px, py)
        // px, py     : toa do pixel goc trai tren cua doi tuong
        //              Lay tu CellToPixel(mX), CellToPixel(mY)
        // widthCells : do rong doi tuong tinh theo O LUOI (xe tai=3, xe hoi=2...)
        //              Sprite se duoc scale de vua dung widthCells * CELL_SIZE pixel
        // heightCells: do cao theo O LUOI (thong thuong = 1)
        // flipX      : true = lat anh ngang (khi doi tuong di sang trai)
        //              false = giu nguyen (khi doi tuong di sang phai)
        // - Tao Sprite LOCAL tren stack voi texture frame hien tai
        // - Tinh scaleX, scaleY de anh vua khit vung o luoi
        // - Neu flipX: scale am tren truc X + dich vi tri sang phai
        // - window.draw(sprite) -> hien thi len man hinh
    }

    void reset() {

        mCurrentFrame = 0;
        mElapsed = 0.f;
        if (isLoaded() && mSprite.has_value()) {
            mSprite->setTexture(mFrames[0]);
        }

        // Dat lai animation ve frame dau tien (index 0)
        // Reset mCurrentFrame = 0
        // Reset mElapsed = 0 (xoa thoi gian dem da tich luy)
        // Goi khi: nguoi choi Reset() ve START_Y sau khi chet hoac qua level
        // Muc dich: tranh hien thi frame giua chung sau khi respawn
    }
};

#endif