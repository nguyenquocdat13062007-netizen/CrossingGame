#include "ProfileManager.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <filesystem>
#include <algorithm>

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <commdlg.h>
#else
#include <cstdio>
#include <array>
#endif

namespace fs = std::filesystem;

ProfileManager::ProfileManager() : mActiveIndex(0), mPresetsLoaded(false) {
	ensureDirectories();
	initPresetAvatars();
	loadFromFile();
}

ProfileManager::~ProfileManager() {
	saveToFile();
}

void ProfileManager::ensureDirectories() {
	try {
		fs::create_directories("Profiles/avatars");
	}
	catch (const std::exception& e) {
		std::cerr << "Warning: Could not create directory Profiles/avatars: " << e.what() << std::endl;
	}
}

std::string ProfileManager::getCurrentTimeString() {
	auto now = std::chrono::system_clock::now();
	std::time_t now_time = std::chrono::system_clock::to_time_t(now);
	std::tm tm_buf{};
#ifdef _WIN32
	localtime_s(&tm_buf, &now_time);
#else
	localtime_r(&now_time, &tm_buf);
#endif
	std::stringstream ss;
	ss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M");
	return ss.str();
}

std::string ProfileManager::generateUniqueId() {
	auto now = std::chrono::system_clock::now().time_since_epoch();
	auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now).count();
	return "p_" + std::to_string(ms);
}

// ================================================================
// PROCEDURAL RETRO PRESET AVATARS GENERATION
// ================================================================
void ProfileManager::generatePresetImage(int presetId, sf::Image& outImg) {
	const unsigned int sz = 64;
	outImg = sf::Image(sf::Vector2u(sz, sz), sf::Color(20, 24, 38));

	// Predefined high-contrast color themes
	sf::Color primaryCol, secondaryCol, accentCol;
	switch (presetId) {
	case 0: // Cyber Gamer
		primaryCol = sf::Color(0, 230, 255);    // Cyan
		secondaryCol = sf::Color(255, 0, 128);  // Magenta
		accentCol = sf::Color(255, 230, 0);     // Yellow
		break;
	case 1: // Golden Legend
		primaryCol = sf::Color(255, 215, 0);    // Gold
		secondaryCol = sf::Color(255, 120, 0);  // Orange
		accentCol = sf::Color(255, 255, 255);   // White
		break;
	case 2: // Neon Matrix
		primaryCol = sf::Color(0, 255, 128);    // Emerald
		secondaryCol = sf::Color(0, 180, 80);   // Dark green
		accentCol = sf::Color(200, 255, 220);   // Light mint
		break;
	case 3: // Crimson Ninja
		primaryCol = sf::Color(255, 50, 70);    // Red
		secondaryCol = sf::Color(140, 20, 40);   // Dark Crimson
		accentCol = sf::Color(255, 200, 200);   // Pinkish White
		break;
	case 4: // Cosmic Star
		primaryCol = sf::Color(160, 80, 255);   // Purple
		secondaryCol = sf::Color(60, 120, 255);  // Blue
		accentCol = sf::Color(255, 240, 150);   // Light Gold
		break;
	default: // Retro Robot
		primaryCol = sf::Color(120, 200, 255);  // Sky Blue
		secondaryCol = sf::Color(80, 100, 140);  // Steel
		accentCol = sf::Color(255, 180, 0);     // Amber
		break;
	}

	// 1. Background gradient pattern
	for (unsigned int y = 0; y < sz; ++y) {
		for (unsigned int x = 0; x < sz; ++x) {
			float distFromCenter = std::sqrt((float)((x - 32)*(x - 32) + (y - 32)*(y - 32)));
			if (distFromCenter < 28.0f) {
				int blend = (int)(25.0f + (32.0f - distFromCenter) * 1.5f);
				blend = std::clamp(blend, 0, 255);
				outImg.setPixel(sf::Vector2u(x, y), sf::Color((uint8_t)blend, (uint8_t)(blend * 1.1f), (uint8_t)(blend * 1.4f)));
			}
			else {
				outImg.setPixel(sf::Vector2u(x, y), sf::Color(14, 16, 24));
			}
		}
	}

	// 2. Character Icon / Shape
	auto drawPixelBlock = [&](int bx, int by, int bw, int bh, const sf::Color& c) {
		for (int dy = 0; dy < bh; ++dy) {
			for (int dx = 0; dx < bw; ++dx) {
				int px = bx + dx;
				int py = by + dy;
				if (px >= 0 && px < (int)sz && py >= 0 && py < (int)sz) {
					outImg.setPixel(sf::Vector2u(px, py), c);
				}
			}
		}
	};

	// Pixel Avatar Archetypes
	if (presetId == 0) {
		// Visor & Headset Gamer
		drawPixelBlock(18, 14, 28, 34, primaryCol);      // Head
		drawPixelBlock(22, 22, 20, 8, secondaryCol);     // Cyber Visor
		drawPixelBlock(24, 24, 16, 4, accentCol);        // Visor Glow
		drawPixelBlock(14, 20, 4, 14, accentCol);        // Headset Left
		drawPixelBlock(46, 20, 4, 14, accentCol);        // Headset Right
		drawPixelBlock(18, 36, 28, 4, secondaryCol);     // Chin guard
		drawPixelBlock(16, 48, 32, 12, secondaryCol);    // Shoulders
	}
	else if (presetId == 1) {
		// Crown & Hero
		drawPixelBlock(20, 18, 24, 26, primaryCol);      // Face
		drawPixelBlock(16, 10, 32, 8, primaryCol);       // Crown Base
		drawPixelBlock(16, 6, 6, 6, accentCol);          // Crown spike 1
		drawPixelBlock(29, 4, 6, 8, accentCol);          // Crown spike 2 (center)
		drawPixelBlock(42, 6, 6, 6, accentCol);          // Crown spike 3
		drawPixelBlock(24, 24, 4, 6, sf::Color::Black);  // Eye L
		drawPixelBlock(36, 24, 4, 6, sf::Color::Black);  // Eye R
		drawPixelBlock(26, 36, 12, 4, secondaryCol);     // Smile
		drawPixelBlock(14, 46, 36, 14, secondaryCol);    // Cape / Armor
	}
	else if (presetId == 2) {
		// Pixel Cat
		drawPixelBlock(18, 18, 28, 26, primaryCol);      // Face
		drawPixelBlock(16, 8, 8, 12, primaryCol);        // Ear Left
		drawPixelBlock(40, 8, 8, 12, primaryCol);        // Ear Right
		drawPixelBlock(18, 10, 4, 8, accentCol);         // Inner Ear L
		drawPixelBlock(42, 10, 4, 8, accentCol);         // Inner Ear R
		drawPixelBlock(22, 26, 6, 6, accentCol);         // Cat Eye L
		drawPixelBlock(36, 26, 6, 6, accentCol);         // Cat Eye R
		drawPixelBlock(29, 34, 6, 4, secondaryCol);      // Nose
		drawPixelBlock(16, 46, 32, 14, secondaryCol);    // Body
	}
	else if (presetId == 3) {
		// Ninja Mask
		drawPixelBlock(18, 14, 28, 30, secondaryCol);    // Ninja Hood
		drawPixelBlock(22, 22, 20, 10, sf::Color(240, 200, 170)); // Eye Skin Slot
		drawPixelBlock(24, 25, 4, 4, sf::Color::Black);  // Eye L
		drawPixelBlock(36, 25, 4, 4, sf::Color::Black);  // Eye R
		drawPixelBlock(16, 32, 32, 14, primaryCol);      // Mask scarf
		drawPixelBlock(14, 46, 36, 14, sf::Color(30, 30, 45)); // Ninja Outfit
	}
	else if (presetId == 4) {
		// Star Galaxy Traveler
		drawPixelBlock(20, 16, 24, 26, primaryCol);      // Helmet
		drawPixelBlock(24, 22, 16, 12, accentCol);       // Golden Visor
		drawPixelBlock(28, 6, 8, 10, accentCol);         // Star Antenna
		drawPixelBlock(30, 2, 4, 4, primaryCol);         // Beacon Top
		drawPixelBlock(16, 44, 32, 16, secondaryCol);    // Suit
	}
	else {
		// Retro Robot
		drawPixelBlock(18, 14, 28, 28, primaryCol);      // Metal Head
		drawPixelBlock(28, 4, 8, 10, secondaryCol);      // Antenna
		drawPixelBlock(30, 2, 4, 4, accentCol);          // Light
		drawPixelBlock(22, 22, 6, 6, accentCol);         // Square Eye L
		drawPixelBlock(36, 22, 6, 6, accentCol);         // Square Eye R
		drawPixelBlock(24, 34, 16, 4, secondaryCol);     // Grill Mouth
		drawPixelBlock(16, 44, 32, 16, secondaryCol);    // Metal Body
	}

	// 3. Pixel Border
	for (unsigned int i = 0; i < sz; ++i) {
		outImg.setPixel(sf::Vector2u(i, 0), primaryCol);
		outImg.setPixel(sf::Vector2u(i, sz - 1), primaryCol);
		outImg.setPixel(sf::Vector2u(0, i), primaryCol);
		outImg.setPixel(sf::Vector2u(sz - 1, i), primaryCol);
	}
}

void ProfileManager::initPresetAvatars() {
	if (mPresetsLoaded) return;
	mPresetTextures.resize(6);
	for (int i = 0; i < 6; ++i) {
		sf::Image img;
		generatePresetImage(i, img);
		(void)mPresetTextures[i].loadFromImage(img);
		mPresetTextures[i].setSmooth(false);
	}
	mPresetsLoaded = true;
}

// ================================================================
// IMAGE PROCESSING & CROPPING
// ================================================================
bool ProfileManager::copyAndProcessAvatar(const std::string& sourcePath, const std::string& destPath) {
	sf::Image srcImg;
	if (!srcImg.loadFromFile(sourcePath)) {
		std::cerr << "Failed to load image from: " << sourcePath << std::endl;
		return false;
	}

	sf::Vector2u srcSize = srcImg.getSize();
	if (srcSize.x == 0 || srcSize.y == 0) return false;

	// Calculate center square crop
	unsigned int cropSize = std::min(srcSize.x, srcSize.y);
	unsigned int cropX = (srcSize.x - cropSize) / 2;
	unsigned int cropY = (srcSize.y - cropSize) / 2;

	// Target 128x128 standard avatar size
	const unsigned int targetSize = 128;
	sf::Image targetImg(sf::Vector2u(targetSize, targetSize), sf::Color::Transparent);

	for (unsigned int ty = 0; ty < targetSize; ++ty) {
		for (unsigned int tx = 0; tx < targetSize; ++tx) {
			unsigned int sx = cropX + (tx * cropSize) / targetSize;
			unsigned int sy = cropY + (ty * cropSize) / targetSize;
			sx = std::min(sx, srcSize.x - 1);
			sy = std::min(sy, srcSize.y - 1);

			sf::Color c = srcImg.getPixel(sf::Vector2u(sx, sy));
			targetImg.setPixel(sf::Vector2u(tx, ty), c);
		}
	}

	ensureDirectories();
	if (!targetImg.saveToFile(destPath)) {
		std::cerr << "Failed to save avatar image to: " << destPath << std::endl;
		return false;
	}

	return true;
}

// ================================================================
// NATIVE FILE DIALOG HELPER
// ================================================================
std::string ProfileManager::openNativeImageFileDialog() {
#ifdef _WIN32
	OPENFILENAMEA ofn;
	char szFile[512] = { 0 };
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Image Files (*.png;*.jpg;*.jpeg;*.bmp)\0*.png;*.jpg;*.jpeg;*.bmp\0All Files (*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle = "Select Avatar Image";
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

	if (GetOpenFileNameA(&ofn) == TRUE) {
		return std::string(ofn.lpstrFile);
	}
	return "";
#else
	// macOS native AppleScript File Chooser
	std::string cmd = "osascript -e 'try' "
	                  "-e 'set filePath to POSIX path of (choose file of type {\"png\", \"jpg\", \"jpeg\", \"bmp\", \"public.image\"} with prompt \"Select Avatar Image\")' "
	                  "-e 'return filePath' "
	                  "-e 'on error' "
	                  "-e 'return \"\"' "
	                  "-e 'end try'";

	FILE* pipe = popen(cmd.c_str(), "r");
	if (!pipe) return "";

	char buffer[512];
	std::string result = "";
	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}
	pclose(pipe);

	// Trim trailing newline / carriage return
	while (!result.empty() && (result.back() == '\n' || result.back() == '\r' || result.back() == ' ')) {
		result.pop_back();
	}
	return result;
#endif
}

// ================================================================
// LOAD / SAVE PROFILES
// ================================================================
void ProfileManager::loadFromFile(const std::string& filepath) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	mProfiles.clear();
	mActiveIndex = 0;

	std::ifstream ifs(filepath);
	if (ifs.is_open()) {
		int activeIdx = 0;
		int count = 0;
		if (ifs >> activeIdx >> count) {
			std::string dummy;
			std::getline(ifs, dummy); // consume newline

			for (int i = 0; i < count; ++i) {
				UserProfile p;
				if (!std::getline(ifs, p.id)) break;
				if (!std::getline(ifs, p.name)) break;
				if (!std::getline(ifs, p.avatarPath)) break;
				std::string line;
				if (!std::getline(ifs, line)) break;
				std::stringstream ss(line);
				ss >> p.highScore >> p.highestLevel;
				if (!std::getline(ifs, p.createdAt)) break;

				if (!p.id.empty() && !p.name.empty()) {
					mProfiles.push_back(p);
				}
			}
			mActiveIndex = std::clamp(activeIdx, 0, std::max(0, (int)mProfiles.size() - 1));
		}
		ifs.close();
	}

	// If no profiles existed or file was empty, initialize default profile
	if (mProfiles.empty()) {
		UserProfile defaultProf;
		defaultProf.id = "p_default";
		defaultProf.name = "Player 1";
		defaultProf.avatarPath = "PRESET_1";
		defaultProf.highScore = 0;
		defaultProf.highestLevel = 1;
		defaultProf.createdAt = getCurrentTimeString();
		mProfiles.push_back(defaultProf);
		mActiveIndex = 0;
		saveToFile(filepath);
	}

	reloadTextures();
}

void ProfileManager::saveToFile(const std::string& filepath) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	ensureDirectories();

	std::ofstream ofs(filepath);
	if (!ofs.is_open()) {
		std::cerr << "Error: Could not save profiles to " << filepath << std::endl;
		return;
	}

	ofs << mActiveIndex << " " << mProfiles.size() << "\n";
	for (const auto& p : mProfiles) {
		ofs << p.id << "\n";
		ofs << p.name << "\n";
		ofs << p.avatarPath << "\n";
		ofs << p.highScore << " " << p.highestLevel << "\n";
		ofs << p.createdAt << "\n";
	}
	ofs.close();
}

void ProfileManager::reloadTextures() {
	mAvatarTextures.clear();
	mAvatarTextures.resize(mProfiles.size());

	for (size_t i = 0; i < mProfiles.size(); ++i) {
		const std::string& path = mProfiles[i].avatarPath;
		bool loaded = false;

		if (path.rfind("PRESET_", 0) == 0) {
			int presetIdx = 0;
			try {
				presetIdx = std::stoi(path.substr(7)) - 1;
			}
			catch (...) { presetIdx = 0; }
			presetIdx = std::clamp(presetIdx, 0, 5);
			if (mPresetsLoaded && presetIdx < (int)mPresetTextures.size()) {
				mAvatarTextures[i] = mPresetTextures[presetIdx];
				loaded = true;
			}
		}
		else if (!path.empty() && fs::exists(path)) {
			loaded = mAvatarTextures[i].loadFromFile(path);
			if (loaded) {
				mAvatarTextures[i].setSmooth(true);
			}
		}

		if (!loaded) {
			// Fallback to preset based on index
			int presetIdx = (int)(i % 6);
			if (mPresetsLoaded) {
				mAvatarTextures[i] = mPresetTextures[presetIdx];
			}
		}
	}
}

// ================================================================
// ACCESSORS & OPERATIONS
// ================================================================
int ProfileManager::getProfileCount() const {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	return (int)mProfiles.size();
}

int ProfileManager::getActiveIndex() const {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	return mActiveIndex;
}

void ProfileManager::setActiveIndex(int index) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index >= 0 && index < (int)mProfiles.size()) {
		mActiveIndex = index;
		saveToFile();
	}
}

UserProfile ProfileManager::getActiveProfile() const {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (mProfiles.empty()) return UserProfile();
	return mProfiles[mActiveIndex];
}

UserProfile ProfileManager::getProfile(int index) const {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index >= 0 && index < (int)mProfiles.size()) {
		return mProfiles[index];
	}
	return UserProfile();
}

bool ProfileManager::createProfile(const std::string& name, const std::string& avatarPath) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	std::string cleanName = name;
	if (cleanName.empty()) cleanName = "Player " + std::to_string(mProfiles.size() + 1);
	if (cleanName.size() > 16) cleanName = cleanName.substr(0, 16);

	UserProfile p;
	p.id = generateUniqueId();
	p.name = cleanName;
	p.highScore = 0;
	p.highestLevel = 1;
	p.createdAt = getCurrentTimeString();

	int presetNum = ((int)mProfiles.size() % 6) + 1;
	p.avatarPath = avatarPath.empty() ? ("PRESET_" + std::to_string(presetNum)) : avatarPath;

	mProfiles.push_back(p);
	mActiveIndex = (int)mProfiles.size() - 1;

	reloadTextures();
	saveToFile();
	return true;
}

bool ProfileManager::renameProfile(int index, const std::string& newName) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index < 0 || index >= (int)mProfiles.size()) return false;
	if (newName.empty()) return false;

	std::string cleanName = newName;
	if (cleanName.size() > 16) cleanName = cleanName.substr(0, 16);
	mProfiles[index].name = cleanName;
	saveToFile();
	return true;
}

bool ProfileManager::deleteProfile(int index) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (mProfiles.size() <= 1) return false; // Cannot delete only profile
	if (index < 0 || index >= (int)mProfiles.size()) return false;

	// Remove avatar file if custom
	const std::string& aPath = mProfiles[index].avatarPath;
	if (!aPath.empty() && aPath.rfind("Profiles/avatars/", 0) == 0 && fs::exists(aPath)) {
		try { fs::remove(aPath); } catch (...) {}
	}

	mProfiles.erase(mProfiles.begin() + index);
	if (mActiveIndex >= (int)mProfiles.size()) {
		mActiveIndex = (int)mProfiles.size() - 1;
	}

	reloadTextures();
	saveToFile();
	return true;
}

bool ProfileManager::updateStats(int score, int level) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (mProfiles.empty() || mActiveIndex < 0 || mActiveIndex >= (int)mProfiles.size()) return false;

	bool changed = false;
	if (score > mProfiles[mActiveIndex].highScore) {
		mProfiles[mActiveIndex].highScore = score;
		changed = true;
	}
	if (level > mProfiles[mActiveIndex].highestLevel) {
		mProfiles[mActiveIndex].highestLevel = level;
		changed = true;
	}

	if (changed) {
		saveToFile();
	}
	return changed;
}

bool ProfileManager::setAvatarFromFile(int index, const std::string& sourceImagePath) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index < 0 || index >= (int)mProfiles.size()) return false;
	if (sourceImagePath.empty()) return false;

	std::string destPath = "Profiles/avatars/" + mProfiles[index].id + ".png";
	if (copyAndProcessAvatar(sourceImagePath, destPath)) {
		mProfiles[index].avatarPath = destPath;
		reloadTextures();
		saveToFile();
		return true;
	}
	return false;
}

bool ProfileManager::setAvatarPreset(int index, int presetId) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index < 0 || index >= (int)mProfiles.size()) return false;
	presetId = std::clamp(presetId, 1, 6);
	mProfiles[index].avatarPath = "PRESET_" + std::to_string(presetId);
	reloadTextures();
	saveToFile();
	return true;
}

// ================================================================
// RENDERING HELPERS
// ================================================================
const sf::Texture& ProfileManager::getAvatarTexture(int index) {
	std::lock_guard<std::recursive_mutex> lock(mMutex);
	if (index >= 0 && index < (int)mAvatarTextures.size()) {
		return mAvatarTextures[index];
	}
	initPresetAvatars();
	return mPresetTextures[0];
}

const sf::Texture& ProfileManager::getActiveAvatarTexture() {
	return getAvatarTexture(mActiveIndex);
}

void ProfileManager::drawAvatarBadge(sf::RenderWindow& window, int index, float x, float y, float size, bool isSelected, const sf::Color& borderColor) {
	const sf::Texture& tex = getAvatarTexture(index);

	// 1. Dark Badge Ground
	sf::RectangleShape bgBox(sf::Vector2f(size, size));
	bgBox.setPosition(sf::Vector2f(x, y));
	bgBox.setFillColor(sf::Color(14, 18, 30, 240));
	window.draw(bgBox);

	// 2. Avatar Sprite
	sf::Vector2u texSize = tex.getSize();
	if (texSize.x > 0 && texSize.y > 0) {
		sf::Sprite spr(tex);
		float scaleX = (size - 6.f) / (float)texSize.x;
		float scaleY = (size - 6.f) / (float)texSize.y;
		spr.setScale(sf::Vector2f(scaleX, scaleY));
		spr.setPosition(sf::Vector2f(x + 3.f, y + 3.f));
		window.draw(spr);
	}

	// 3. Crisp Cyber Border Frame
	sf::RectangleShape border(sf::Vector2f(size, size));
	border.setPosition(sf::Vector2f(x, y));
	border.setFillColor(sf::Color::Transparent);
	border.setOutlineThickness(isSelected ? 3.f : 2.f);
	border.setOutlineColor(isSelected ? sf::Color(255, 215, 0) : borderColor);
	window.draw(border);

	// Inner subtle accent
	if (size >= 40.f) {
		sf::RectangleShape innerBorder(sf::Vector2f(size - 6.f, size - 6.f));
		innerBorder.setPosition(sf::Vector2f(x + 3.f, y + 3.f));
		innerBorder.setFillColor(sf::Color::Transparent);
		innerBorder.setOutlineThickness(1.f);
		innerBorder.setOutlineColor(sf::Color(255, 255, 255, isSelected ? 120 : 50));
		window.draw(innerBorder);
	}
}
