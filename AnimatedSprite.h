#pragma once
#ifndef ANIMATEDSPRITE_H
#define ANIMATEDSPRITE_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <cstddef>
#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <vector>

#include "Utils.h"

class AnimatedSprite {
private:
    static constexpr std::size_t MAX_FRAMES = 16;

    std::vector<std::unique_ptr<sf::Texture>> mFrames;
    std::optional<sf::Sprite> mSprite;
    std::size_t mCurrentFrame;
    float mFrameTime;
    float mElapsed;

public:
    explicit AnimatedSprite(float frameTime = 0.15f)
        : mCurrentFrame(0),
          mFrameTime(frameTime > 0.f ? frameTime : 0.15f),
          mElapsed(0.f) {
        mFrames.reserve(MAX_FRAMES);
    }

    AnimatedSprite(const AnimatedSprite&) = delete;
    AnimatedSprite& operator=(const AnimatedSprite&) = delete;
    AnimatedSprite(AnimatedSprite&&) noexcept = default;
    AnimatedSprite& operator=(AnimatedSprite&&) noexcept = default;

    void clear() {
        mSprite.reset();
        mFrames.clear();
        mCurrentFrame = 0;
        mElapsed = 0.f;
    }

    bool addFrame(const std::string& filename) {
        if (filename.empty() || mFrames.size() >= MAX_FRAMES) {
            return false;
        }

        auto texture = std::make_unique<sf::Texture>();
        if (!texture->loadFromFile(filename)) {
            return false;
        }
        texture->setSmooth(false); // Sharp pixel-perfect filtering

        mFrames.push_back(std::move(texture));
        if (!mSprite.has_value()) {
            mSprite.emplace(*mFrames.front());
        }
        return true;
    }

    bool loadAssets(const std::vector<std::string>& filenames) {
        AnimatedSprite candidate(mFrameTime);

        for (const std::string& filename : filenames) {
            if (!filename.empty() && !candidate.addFrame(filename)) {
                return false;
            }
        }

        if (!candidate.isLoaded()) {
            return false;
        }

        *this = std::move(candidate);
        return true;
    }

    bool isLoaded() const {
        return !mFrames.empty() && mSprite.has_value();
    }

    std::size_t getFrameCount() const { return mFrames.size(); }
    std::size_t getCurrentFrame() const { return mCurrentFrame; }

    void nextFrame() {
        if (mFrames.size() <= 1) return;
        mCurrentFrame = (mCurrentFrame + 1) % mFrames.size();
        mElapsed = 0.f;
        if (mSprite.has_value() && mCurrentFrame < mFrames.size()) {
            mSprite->setTexture(*mFrames[mCurrentFrame]);
        }
    }

    void update(float dt) {
        if (mFrames.size() <= 1 || !std::isfinite(dt) || dt <= 0.f) {
            return;
        }

        const double totalElapsed = static_cast<double>(mElapsed) + dt;
        const double completeFrames =
            std::floor(totalElapsed / static_cast<double>(mFrameTime));
        if (completeFrames < 1.0) {
            mElapsed = static_cast<float>(totalElapsed);
            return;
        }

        const std::size_t framesToAdvance = static_cast<std::size_t>(
            std::fmod(completeFrames, static_cast<double>(mFrames.size())));
        mCurrentFrame = (mCurrentFrame + framesToAdvance) % mFrames.size();
        mElapsed = static_cast<float>(
            std::fmod(totalElapsed, static_cast<double>(mFrameTime)));
        mSprite->setTexture(*mFrames[mCurrentFrame]);
    }

    void drawExact(sf::RenderWindow& window,
                   float px,
                   float py,
                   float widthPx,
                   float heightPx,
                   bool flipX = false) {
        if (!isLoaded() || widthPx <= 0.f || heightPx <= 0.f) {
            return;
        }

        const sf::FloatRect bounds = mSprite->getLocalBounds();
        if (bounds.size.x <= 0.f || bounds.size.y <= 0.f) {
            return;
        }

        const float scaleX = widthPx / bounds.size.x;
        const float scaleY = heightPx / bounds.size.y;

        if (flipX) {
            mSprite->setScale(sf::Vector2f(-scaleX, scaleY));
            mSprite->setPosition(sf::Vector2f(px + widthPx, py));
        } else {
            mSprite->setScale(sf::Vector2f(scaleX, scaleY));
            mSprite->setPosition(sf::Vector2f(px, py));
        }

        window.draw(*mSprite);
    }

    void draw(sf::RenderWindow& window,
              float px,
              float py,
              int widthCells = 1,
              int heightCells = 1,
              bool flipX = false) {
        drawExact(window, px, py, (float)(widthCells * CELL_SIZE), (float)(heightCells * CELL_SIZE), flipX);
    }

    void drawProportional(sf::RenderWindow& window,
                          float px,
                          float py,
                          int widthCells,
                          bool flipX = false) {
        if (!isLoaded() || widthCells <= 0) return;
        const sf::FloatRect bounds = mSprite->getLocalBounds();
        if (bounds.size.x <= 0.f || bounds.size.y <= 0.f) return;
        float w = (float)(widthCells * CELL_SIZE);
        float h = w * (bounds.size.y / bounds.size.x);
        drawExact(window, px, py - (h - (float)CELL_SIZE) * 0.5f, w, h, flipX);
    }

    void reset() {
        mCurrentFrame = 0;
        mElapsed = 0.f;
        if (isLoaded()) {
            mSprite->setTexture(*mFrames.front());
        }
    }
};

#endif
