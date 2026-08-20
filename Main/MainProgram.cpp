#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

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

            if (cg.getPeople().isAlive() && !cg.isGodMode()) {
                if (cg.checkCollision()) {
                    cg.setState(cg.getLives() <= 0
                        ? GameState::GAMEOVER : GameState::DEAD);
                }
            }

            if (cg.getPeople().isAlive()) {
                if (cg.checkFinish()) {
                    if (cg.getLevel() >= MAX_LEVEL) {
                        cg.setState(GameState::WIN);
                    }
                    else {
                        cg.nextLevel();
                    }
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
    bool fontLoaded = font.openFromFile("Assets/images/Press_Start_2P/PressStart2P-Regular.ttf") ||
                       font.openFromFile("/System/Library/Fonts/Supplemental/Arial.ttf") ||
                       font.openFromFile("/Library/Fonts/Arial.ttf") ||
                       font.openFromFile("C:/Windows/Fonts/arial.ttf") ||
                       font.openFromFile("C:/Windows/Fonts/Arial.ttf");
    if (!fontLoaded) {
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
                window.close();
                break;
            }

            const auto* key = event->getIf<Event::KeyPressed>();
            if (!key) continue;

            // ========================================================
            // GLOBAL DEBUG CONSOLE TOGGLE (` key)
            // ========================================================
            if (key->code == Keyboard::Key::Grave) {
                cg.toggleDebugMenu();
                cg.playMenuSound();
                continue;
            }

            // Neu Debug Menu dang mo, dieu huong menu debug
            if (cg.isDebugMenuOpen()) {
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W) {
                    cg.debugMenuUp();
                }
                else if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S) {
                    cg.debugMenuDown();
                }
                else if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.debugMenuSelect();
                }
                else if (key->code == Keyboard::Key::Num1 || key->code == Keyboard::Key::Numpad1) {
                    cg.toggleGodMode();
                    cg.playMenuSound();
                }
                else if (key->code == Keyboard::Key::Num2 || key->code == Keyboard::Key::Numpad2) {
                    cg.jumpLevel(1);
                }
                else if (key->code == Keyboard::Key::Num3 || key->code == Keyboard::Key::Numpad3) {
                    cg.jumpLevel(-1);
                }
                else if (key->code == Keyboard::Key::Num4 || key->code == Keyboard::Key::Numpad4) {
                    cg.addLife();
                    cg.playMenuSound();
                }
                else if (key->code == Keyboard::Key::Num5 || key->code == Keyboard::Key::Numpad5) {
                    cg.teleportFinish();
                }
                else if (key->code == Keyboard::Key::Num6 || key->code == Keyboard::Key::Numpad6 || key->code == Keyboard::Key::Escape) {
                    cg.setDebugMenuOpen(false);
                    cg.playMenuSound();
                }
                continue;
            }

            // MENU
            if (cg.getState() == GameState::MENU) {
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W) {
                    cg.menuUp();
                }
                else if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S) {
                    cg.menuDown();
                }
                else if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.playMenuSound();
                    int opt = cg.getMenuOption();
                    if (opt == 0) {
                        cg.startGame();
                        holdDelayClock.restart();
                        holdRepeatClock.restart();
                    }
                    else if (opt == 1) {
                        cg.setState(GameState::LOAD_GAME);
                    }
                    else if (opt == 2) {
                        cg.setState(GameState::SETTINGS);
                    }
                    else if (opt == 3) {
                        cg.exitGame((HANDLE)t1.native_handle());
                        window.close();
                    }
                }
                else if (key->code == Keyboard::Key::Num1 || key->code == Keyboard::Key::Numpad1) {
                    cg.setMenuOption(0);
                    cg.playMenuSound();
                    cg.startGame();
                    holdDelayClock.restart();
                    holdRepeatClock.restart();
                }
                else if (key->code == Keyboard::Key::Num2 || key->code == Keyboard::Key::Numpad2) {
                    cg.setMenuOption(1);
                    cg.playMenuSound();
                    cg.setState(GameState::LOAD_GAME);
                }
                else if (key->code == Keyboard::Key::Num3 || key->code == Keyboard::Key::Numpad3) {
                    cg.setMenuOption(2);
                    cg.playMenuSound();
                    cg.setState(GameState::SETTINGS);
                }
                else if (key->code == Keyboard::Key::Num4 || key->code == Keyboard::Key::Numpad4 || key->code == Keyboard::Key::Escape) {
                    cg.exitGame((HANDLE)t1.native_handle());
                    window.close();
                }
            }

            // LOAD_GAME
            else if (cg.getState() == GameState::LOAD_GAME) {
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W) {
                    cg.slotUp();
                }
                else if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S) {
                    cg.slotDown();
                }
                else if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    if (cg.loadGameSlot(cg.getSelectedSlot())) {
                        cg.setState(GameState::PLAYING);
                        holdDelayClock.restart();
                        holdRepeatClock.restart();
                    }
                }
                else if (key->code == Keyboard::Key::Num1 || key->code == Keyboard::Key::Numpad1) {
                    cg.setSelectedSlot(0);
                    if (cg.loadGameSlot(0)) { cg.setState(GameState::PLAYING); holdDelayClock.restart(); holdRepeatClock.restart(); }
                }
                else if (key->code == Keyboard::Key::Num2 || key->code == Keyboard::Key::Numpad2) {
                    cg.setSelectedSlot(1);
                    if (cg.loadGameSlot(1)) { cg.setState(GameState::PLAYING); holdDelayClock.restart(); holdRepeatClock.restart(); }
                }
                else if (key->code == Keyboard::Key::Num3 || key->code == Keyboard::Key::Numpad3) {
                    cg.setSelectedSlot(2);
                    if (cg.loadGameSlot(2)) { cg.setState(GameState::PLAYING); holdDelayClock.restart(); holdRepeatClock.restart(); }
                }
                else if (key->code == Keyboard::Key::Num4 || key->code == Keyboard::Key::Numpad4) {
                    cg.setSelectedSlot(3);
                    if (cg.loadGameSlot(3)) { cg.setState(GameState::PLAYING); holdDelayClock.restart(); holdRepeatClock.restart(); }
                }
                else if (key->code == Keyboard::Key::Escape) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // SAVE_GAME
            else if (cg.getState() == GameState::SAVE_GAME) {
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W) {
                    cg.slotUp();
                }
                else if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S) {
                    cg.slotDown();
                }
                else if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.saveGameSlot(cg.getSelectedSlot());
                    cg.setState(GameState::PLAYING);
                }
                else if (key->code == Keyboard::Key::Num1 || key->code == Keyboard::Key::Numpad1) {
                    cg.setSelectedSlot(0);
                    cg.saveGameSlot(0);
                    cg.setState(GameState::PLAYING);
                }
                else if (key->code == Keyboard::Key::Num2 || key->code == Keyboard::Key::Numpad2) {
                    cg.setSelectedSlot(1);
                    cg.saveGameSlot(1);
                    cg.setState(GameState::PLAYING);
                }
                else if (key->code == Keyboard::Key::Num3 || key->code == Keyboard::Key::Numpad3) {
                    cg.setSelectedSlot(2);
                    cg.saveGameSlot(2);
                    cg.setState(GameState::PLAYING);
                }
                else if (key->code == Keyboard::Key::Num4 || key->code == Keyboard::Key::Numpad4) {
                    cg.setSelectedSlot(3);
                    cg.saveGameSlot(3);
                    cg.setState(GameState::PLAYING);
                }
                else if (key->code == Keyboard::Key::Escape) {
                    cg.playMenuSound();
                    cg.setState(GameState::PLAYING);
                }
            }

            // SETTINGS
            else if (cg.getState() == GameState::SETTINGS) {
                if (key->code == Keyboard::Key::Up || key->code == Keyboard::Key::W) {
                    cg.settingsUp();
                }
                else if (key->code == Keyboard::Key::Down || key->code == Keyboard::Key::S) {
                    cg.settingsDown();
                }
                else if (key->code == Keyboard::Key::Left || key->code == Keyboard::Key::A) {
                    cg.settingsLeft();
                }
                else if (key->code == Keyboard::Key::Right || key->code == Keyboard::Key::D) {
                    cg.settingsRight();
                }
                else if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.settingsSelect();
                }
                else if (key->code == Keyboard::Key::M) {
                    cg.toggleMusic();
                }
                else if (key->code == Keyboard::Key::Escape || key->code == Keyboard::Key::Num3 || key->code == Keyboard::Key::Numpad3 || key->code == Keyboard::Key::Num4) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // PLAYING
            else if (cg.getState() == GameState::PLAYING) {
                if (key->code == Keyboard::Key::P) {
                    cg.playMenuSound();
                    cg.pauseGame((HANDLE)t1.native_handle());
                }
                else if (key->code == Keyboard::Key::L) {
                    cg.playMenuSound();
                    cg.setState(GameState::SAVE_GAME);
                }
                else if (key->code == Keyboard::Key::T) {
                    cg.playMenuSound();
                    cg.setState(GameState::LOAD_GAME);
                }
                else if (key->code == Keyboard::Key::M) {
                    cg.toggleMusic();
                }
                else if (key->code == Keyboard::Key::Escape) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
                // WASD: phan hoi ngay lan nhan dau
                else if (key->code == Keyboard::Key::W || key->code == Keyboard::Key::Up) handleMovement('W');
                else if (key->code == Keyboard::Key::S || key->code == Keyboard::Key::Down) handleMovement('S');
                else if (key->code == Keyboard::Key::A || key->code == Keyboard::Key::Left) handleMovement('A');
                else if (key->code == Keyboard::Key::D || key->code == Keyboard::Key::Right) handleMovement('D');
            }

            // PAUSED
            else if (cg.getState() == GameState::PAUSED) {
                if (key->code == Keyboard::Key::P) {
                    cg.playMenuSound();
                    cg.resumeGame((HANDLE)t1.native_handle());
                    holdDelayClock.restart();
                    holdRepeatClock.restart();
                }
                else if (key->code == Keyboard::Key::M) {
                    cg.toggleSound();
                }
                else if (key->code == Keyboard::Key::Escape) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // DEAD
            else if (cg.getState() == GameState::DEAD) {
                if (key->code == Keyboard::Key::Y || key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.playMenuSound();
                    cg.getPeople().Reset();
                    cg.setState(GameState::PLAYING);
                    MOVING = ' '; lastKey = ' '; holdStarted = false;
                    holdDelayClock.restart(); holdRepeatClock.restart();
                }
                else if (key->code == Keyboard::Key::Escape) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // WIN
            else if (cg.getState() == GameState::WIN) {
                if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space || key->code == Keyboard::Key::Escape || key->code == Keyboard::Key::Y) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // GAMEOVER
            else if (cg.getState() == GameState::GAMEOVER) {
                if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space || key->code == Keyboard::Key::Escape || key->code == Keyboard::Key::Y) {
                    cg.playMenuSound();
                    cg.setState(GameState::MENU);
                }
            }

            // LEVEL_UP
            else if (cg.getState() == GameState::LEVEL_UP) {
                if (key->code == Keyboard::Key::Enter || key->code == Keyboard::Key::Space) {
                    cg.playMenuSound();
                    cg.setState(GameState::PLAYING);
                }
            }
        }

        // ============================================================
        // GIU PHIM (isKeyPressed) - dung handleMovement() chung
        // 2 giai doan: delay 350ms -> lap moi 150ms
        // ============================================================
        if (cg.getState() == GameState::PLAYING && !keyHandledThisFrame && !cg.isDebugMenuOpen()) {
            char curKey = ' ';
            if (lastKey == 'W' && (Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up))) curKey = 'W';
            else if (lastKey == 'S' && (Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down))) curKey = 'S';
            else if (lastKey == 'A' && (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left))) curKey = 'A';
            else if (lastKey == 'D' && (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right))) curKey = 'D';
            else if (Keyboard::isKeyPressed(Keyboard::Key::W) || Keyboard::isKeyPressed(Keyboard::Key::Up)) curKey = 'W';
            else if (Keyboard::isKeyPressed(Keyboard::Key::S) || Keyboard::isKeyPressed(Keyboard::Key::Down)) curKey = 'S';
            else if (Keyboard::isKeyPressed(Keyboard::Key::A) || Keyboard::isKeyPressed(Keyboard::Key::Left)) curKey = 'A';
            else if (Keyboard::isKeyPressed(Keyboard::Key::D) || Keyboard::isKeyPressed(Keyboard::Key::Right)) curKey = 'D';

            if (curKey != ' ') {
                if (curKey != lastKey) handleMovement(curKey);

                if (!holdStarted &&
                    holdDelayClock.getElapsedTime().asSeconds() >= MOVE_HOLD_DELAY) {
                    holdStarted = true;
                    holdRepeatClock.restart();
                }

                if (holdStarted &&
                    holdRepeatClock.getElapsedTime().asSeconds() >= MOVE_HOLD_INTERVAL) {
                    MOVING = curKey;
                    holdRepeatClock.restart();
                }
            }
            else {
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
        window.clear(COLOR_BG);

        switch (cg.getState()) {
        case GameState::MENU:
            cg.drawGame(window, font);
            cg.renderMenu(window, font);
            break;
        case GameState::SETTINGS:
            cg.drawGame(window, font);
            cg.renderSettings(window, font);
            break;
        case GameState::LOAD_GAME:
            cg.renderLoadMenu(window, font);
            break;
        case GameState::SAVE_GAME:
            cg.drawGame(window, font);
            cg.renderSaveMenu(window, font);
            break;
        case GameState::PLAYING:
            cg.drawGame(window, font);
            break;
        case GameState::PAUSED:
            cg.drawGame(window, font);
            cg.renderPauseMsg(window, font);
            break;
        case GameState::DEAD:
            cg.drawGame(window, font);
            cg.renderDeadMsg(window, font);
            break;
        case GameState::GAMEOVER:
            cg.drawGame(window, font);
            cg.renderGameOver(window, font);
            break;
        case GameState::WIN:
            cg.drawGame(window, font);
            cg.renderWin(window, font);
            break;
        case GameState::LEVEL_UP:
            cg.drawGame(window, font);
            cg.renderLevelUp(window, font);
            break;
        default:
            break;
        }

        // ============================================================
        // DEVELOPER DEBUG CONSOLE OVERLAY (Toggle with ` or ~)
        // ============================================================
        if (cg.isDebugMenuOpen()) {
            cg.renderDebugMenu(window, font);
        }

        window.display();
    }

    IS_RUNNING = false;
    if (t1.joinable()) {
        t1.join();
    }
    return 0;
}
