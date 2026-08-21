#pragma once
#ifndef PROFILE_MANAGER_H
#define PROFILE_MANAGER_H

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <memory>
#include <mutex>

// ================================================================
// STRUCT USER PROFILE
// ================================================================
struct UserProfile {
	std::string id;          // Unique ID (e.g. "p_1", "p_1724212000")
	std::string name;        // Display Name (e.g. "GiaBao", "Player 1")
	std::string avatarPath;  // Relative or absolute path to avatar PNG, or "PRESET_1"..."PRESET_6"
	int highScore;           // Highest score achieved
	int highestLevel;        // Highest level reached
	std::string createdAt;   // Formatted creation time string

	UserProfile() : id(""), name("Player 1"), avatarPath("PRESET_1"), highScore(0), highestLevel(1), createdAt("") {}
};

// ================================================================
// LOP PROFILE MANAGER - Quan ly danh sach tai khoan va avatar
// ================================================================
class ProfileManager {
private:
	std::vector<UserProfile> mProfiles;
	int mActiveIndex;
	std::vector<sf::Texture> mAvatarTextures;
	std::vector<sf::Texture> mPresetTextures;
	bool mPresetsLoaded;
	mutable std::recursive_mutex mMutex;

	void ensureDirectories();
	void initPresetAvatars();
	void reloadTextures();
	void generatePresetImage(int presetId, sf::Image& outImg);
	std::string generateUniqueId();
	std::string getCurrentTimeString();

public:
	ProfileManager();
	~ProfileManager();

	// Load & Save
	void loadFromFile(const std::string& filepath = "Profiles/profiles.txt");
	void saveToFile(const std::string& filepath = "Profiles/profiles.txt");

	// Profile Accessors
	int getProfileCount() const;
	int getActiveIndex() const;
	void setActiveIndex(int index);
	UserProfile getActiveProfile() const;
	UserProfile getProfile(int index) const;

	// Profile Operations
	bool createProfile(const std::string& name, const std::string& avatarPath = "");
	bool renameProfile(int index, const std::string& newName);
	bool deleteProfile(int index);
	bool updateStats(int score, int level);

	// Avatar Management
	bool setAvatarFromFile(int index, const std::string& sourceImagePath);
	bool setAvatarPreset(int index, int presetId);
	bool copyAndProcessAvatar(const std::string& sourcePath, const std::string& destPath);

	// Rendering Helpers
	const sf::Texture& getAvatarTexture(int index);
	const sf::Texture& getActiveAvatarTexture();
	void drawAvatarBadge(sf::RenderWindow& window, int index, float x, float y, float size, bool isSelected = false, const sf::Color& borderColor = sf::Color(0, 220, 255));

	// Native File Dialog Helper (macOS osascript / Windows GetOpenFileName)
	static std::string openNativeImageFileDialog();
};

#endif // PROFILE_MANAGER_H
