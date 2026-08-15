#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

#include "CGame.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <thread>

CGAME cg;

const float UPDATE_INTERVAL = 0.10f;
const float MOVE_HOLD_DELAY = 0.35f;
const float MOVE_HOLD_INTERVAL = 0.15f;

RenderWindow* g_window = nullptr;
Font* g_font = nullptr;

void SubThread() {
    sf::Clock updateClock;
    while (IS_RUNNING) {
        if (cg.getState() != GameState::PLAYING) {
            Sleep(16);
            continue;
        }
        float elapsed = updateClock.getElapsedTime().asSeconds();
        if (elapsed >= UPDATE_INTERVAL) {
            updateClock.restart();

            if (cg.getPeople().isAlive()) {
                cg.updatePosPeople(MOVING);
            }
            MOVING = ' ';

            cg.updateTrafficLights();
            cg.updatePosVehicle();
            cg.updatePosAnimal();

            if (cg.getPeople().isAlive()) {
                CVEHICLE** vehicles = cg.getVehicle();
                CANIMAL** animals = cg.getAnimal();
                bool hit = false;

                for (int i = 0; i < MAX_TRUCKS + MAX_CARS && !hit; i++)
                    if (vehicles[i]) hit = cg.getPeople().isImpact(vehicles[i]);
                for (int i = 0; i < MAX_DINOS + MAX_BIRDS && !hit; i++)
                    if (animals[i])  hit = cg.getPeople().isImpact(animals[i]);

                if (hit) {
                    cg.checkCollision();
                    cg.setState(cg.getLives() <= 0
                        ? GameState::GAMEOVER : GameState::DEAD);
                }
            }

            if (cg.getPeople().isAlive() && cg.getPeople().isFinished()) {
                if (cg.checkFinish()) {
                    if (cg.getLevel() >= MAX_LEVEL)
                        cg.setState(GameState::WIN);
                    else
                        cg.nextLevel();
                }
            }
        }
        Sleep(5);
    }
}

int main() {
    RenderWindow window(VideoMode(Vector2u((unsigned)WINDOW_WIDTH, (unsigned)WINDOW_HEIGHT)), "Crossing Game - SFML");
    window.setFramerateLimit(60);

    Font font;
    bool fontLoaded = font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") ||
                       font.openFromFile("/Library/Fonts/Arial.ttf") ||
                       font.openFromFile("C:/Windows/Fonts/arial.ttf");
    if (!fontLoaded) {
        // Tiep tuc ma khong return -1 neu khong tim thay font de tranh crash ngay lap tuc
        std::cout << "Warning: Could not load font file." << std::endl;
    }

    g_window = &window;
    g_font = &font;

    cg.startGame();
    cg.setState(GameState::MENU);

    thread t1(SubThread);

    // ================================================================
    // BIEN QUAN LY INPUT
    // ================================================================
    Clock dtClock;
    Clock holdDelayClock;
    Clock holdRepeatClock;
    bool  keyHandledThisFrame = false;
    bool  holdStarted = false;
    char  lastKey = ' ';

    // ================================================================
    // Ham gop logic WASD - dung chung cho KeyPressed va isKeyPressed
    // ================================================================
    auto handleMovement = [&](char key) {
        MOVING = key;
        lastKey = key;
        holdStarted = false;
        keyHandledThisFrame = true;
        holdDelayClock.restart();
        holdRepeatClock.restart();
        };

    // ================================================================
    // VONG LAP CHINH
    // ================================================================
    while (window.isOpen() && IS_RUNNING) {
        float dt = dtClock.restart().asSeconds();
        keyHandledThisFrame = false;

        // ============================================================
        // XU LY EVENT (KeyPressed)
        // ============================================================
        while (const std::optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                cg.exitGame((HANDLE)t1.native_handle());
                break;
            }

            const auto* key = event->getIf<Event::KeyPressed>();
            if (!key) continue;

            // MENU
            if (cg.getState() == GameState::MENU) {
                if (key->code == Keyboard::Key::Num1) { cg.startGame(); holdDelayClock.restart(); holdRepeatClock.restart(); }
                else if (key->code == Keyboard::Key::Num2) { if (cg.loadGame("save.crossgame")) cg.setState(GameState::PLAYING); holdDelayClock.restart(); holdRepeatClock.restart(); }
                else if (key->code == Keyboard::Key::Num4) { cg.exitGame((HANDLE)t1.native_handle()); }
            }

            // PLAYING
            else if (cg.getState() == GameState::PLAYING) {
                if (key->code == Keyboard::Key::P) { cg.pauseGame((HANDLE)t1.native_handle()); }
                else if (key->code == Keyboard::Key::L) { cg.saveGame("save.crossgame"); }
                else if (key->code == Keyboard::Key::T) { cg.loadGame("save.crossgame"); }
                else if (key->code == Keyboard::Key::Escape) { cg.setState(GameState::MENU); }
                // WASD: phan hoi ngay lan nhan dau - dung handleMovement()
                else if (key->code == Keyboard::Key::W) handleMovement('W');
                else if (key->code == Keyboard::Key::S) handleMovement('S');
                else if (key->code == Keyboard::Key::A) handleMovement('A');
                else if (key->code == Keyboard::Key::D) handleMovement('D');
            }

            // PAUSED
            else if (cg.getState() == GameState::PAUSED) {
                if (key->code == Keyboard::Key::P) { cg.resumeGame((HANDLE)t1.native_handle()); holdDelayClock.restart(); holdRepeatClock.restart(); }
                else if (key->code == Keyboard::Key::Escape) { cg.setState(GameState::MENU); }
            }

            // DEAD
            else if (cg.getState() == GameState::DEAD) {
                if (key->code == Keyboard::Key::Y) {
                    cg.getPeople().Reset();
                    cg.setState(GameState::PLAYING);
                    MOVING = ' '; lastKey = ' '; holdStarted = false;
                    holdDelayClock.restart(); holdRepeatClock.restart();
                }
                else if (key->code == Keyboard::Key::Escape) { cg.setState(GameState::MENU); }
            }

            // GAMEOVER / WIN
            else if (cg.getState() == GameState::GAMEOVER ||
                cg.getState() == GameState::WIN) {
                cg.setState(GameState::MENU);
            }
        }

        // ============================================================
        // GIU PHIM (isKeyPressed) - dung handleMovement() chung
        // 2 giai doan: delay 350ms -> lap moi 150ms
        // ============================================================
        if (cg.getState() == GameState::PLAYING && !keyHandledThisFrame) {
            // Xac dinh phim dang giu, uu tien lastKey
            char curKey = ' ';
            if (lastKey == 'W' && Keyboard::isKeyPressed(Keyboard::Key::W)) curKey = 'W';
            else if (lastKey == 'S' && Keyboard::isKeyPressed(Keyboard::Key::S)) curKey = 'S';
            else if (lastKey == 'A' && Keyboard::isKeyPressed(Keyboard::Key::A)) curKey = 'A';
            else if (lastKey == 'D' && Keyboard::isKeyPressed(Keyboard::Key::D)) curKey = 'D';
            else if (Keyboard::isKeyPressed(Keyboard::Key::W)) curKey = 'W';
            else if (Keyboard::isKeyPressed(Keyboard::Key::S)) curKey = 'S';
            else if (Keyboard::isKeyPressed(Keyboard::Key::A)) curKey = 'A';
            else if (Keyboard::isKeyPressed(Keyboard::Key::D)) curKey = 'D';

            if (curKey != ' ') {
                // Doi phim -> reset qua handleMovement()
                if (curKey != lastKey) handleMovement(curKey);

                // Giai doan 1: cho du delay moi bat dau giu
                if (!holdStarted &&
                    holdDelayClock.getElapsedTime().asSeconds() >= MOVE_HOLD_DELAY) {
                    holdStarted = true;
                    holdRepeatClock.restart();
                }

                // Giai doan 2: dang giu -> di chuyen moi MOVE_HOLD_INTERVAL
                if (holdStarted &&
                    holdRepeatClock.getElapsedTime().asSeconds() >= MOVE_HOLD_INTERVAL) {
                    MOVING = curKey;
                    holdRepeatClock.restart();
                }
            }
            else {
                // Tha phim -> reset
                if (lastKey != ' ') {
                    lastKey = ' ';
                    holdStarted = false;
                    holdDelayClock.restart();
                    holdRepeatClock.restart();
                }
            }
        }

        // ============================================================
        // CAP NHAT ANIMATION
        // ============================================================
        if (cg.getState() == GameState::PLAYING ||
            cg.getState() == GameState::PAUSED ||
            cg.getState() == GameState::DEAD) {
            cg.updateAnimations(dt);
        }

        // ============================================================
        // RENDER
        // ============================================================
        switch (cg.getState()) {
        case GameState::MENU:
            cg.renderMenu(window, font);
            break;
        case GameState::PLAYING:
            window.clear(COLOR_BG);
            cg.drawGame(window, font);
            window.display();
            break;
        case GameState::PAUSED:
            window.clear(COLOR_BG);
            cg.drawGame(window, font);
            cg.renderPauseMsg(window, font);
            window.display();
            break;
        case GameState::DEAD:
            window.clear(COLOR_BG);
            cg.drawGame(window, font);
            cg.renderDeadMsg(window, font);
            window.display();
            break;
        case GameState::GAMEOVER:
            cg.renderGameOver(window, font);
            break;
        case GameState::WIN:
            cg.renderWin(window, font);
            break;
        default:
            break;
        }
    }

    IS_RUNNING = false;
    t1.join();
    return 0;
}
