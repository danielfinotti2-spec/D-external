#pragma once

// Brand.hpp - single source of truth for identity strings.
// Edit here; every other file reads these.

namespace Brand
{
	// Identity
	inline constexpr const char* Name = "D";
	inline constexpr const char* Tag = "D";
	inline constexpr const char* Version = "1.0.0";
	inline constexpr const char* Build = "3258";

	// Menu watermark line:  "D  |  v1.0.0"
	inline constexpr const char* WatermarkName = Name;
	inline constexpr const char* WatermarkTag = Version;

	// Window / config namespace
	inline constexpr const char* WindowClass = "D";
	inline constexpr const char* ConfigFile = "C:\\Windows\\SysWOW64\\slmgr\\0416\\slmgrs.ini";
	inline constexpr const char* BinaryName = "D";

	// Colors (0-255 RGBA)
	inline constexpr int AccentR = 214;
	inline constexpr int AccentG = 40;
	inline constexpr int AccentB = 40;
	inline constexpr int AccentA = 255;

	// Watermark layout
	inline constexpr float WatermarkHeight = 34.f;
	inline constexpr float WatermarkPadding = 12.f;
	inline constexpr float WatermarkLogoBox = 22.f;
	inline constexpr float WatermarkGap = 8.f;
	inline constexpr float WatermarkMarginX = 12.f;
	inline constexpr float WatermarkMarginY = 12.f;
}
