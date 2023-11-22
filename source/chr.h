#pragma once
typedef struct
{
	const void* Memory;
	const uint8_t* Base;
} chr_ram_t;

#define CHR_OFFSET16(Base,Offset) (Base + Offset*64)

enum
{
	CHR_BANK1 = 0x4db4, // TITLE SCREEN
	CHR_BANK2 = 0xC11B, // TILES, ENEMIES
	CHR_BANK3 = 0x807f, // LINK, GUI, FONT
	CHR_LINK_E = CHR_OFFSET16(CHR_BANK3, 0),
	CHR_LINK_N = CHR_OFFSET16(CHR_BANK3, 2),
	CHR_LINK_ATTACK_E = CHR_OFFSET16(CHR_BANK3, 4),
	CHR_LINK_ATTACK_N = CHR_OFFSET16(CHR_BANK3, 5),
	CHR_SWORD_E = CHR_OFFSET16(CHR_BANK1, 4) + 32,
	CHR_SWORD_N = CHR_OFFSET16(CHR_BANK3, 7) + 32 + (8 * 3),
	CHR_STAIR = CHR_OFFSET16(CHR_BANK2, 0),
	CHR_FLOOR = CHR_OFFSET16(CHR_BANK2, 1),
	CHR_WALL = CHR_OFFSET16(CHR_BANK2, 16),
	CHR_DNG_WALL = CHR_OFFSET16(CHR_BANK3, 57),
	CHR_ICON_LIFE = (CHR_OFFSET16(CHR_BANK3, 53) + 8),
	CHR_OCTOROCK = (CHR_OFFSET16(CHR_BANK2, 73) + 32),
	CHR_OCTOCOK_BULLET = CHR_OFFSET16(CHR_BANK2, 87)+32,
};

static const int32_t PAL_GREEN[4] = { 0, RGB(122,231,0),RGB(250,158,0),RGB(157,84,0) }; // Link
static const int32_t PAL_BLUE[4] = { 0xff000000, RGB(0,60,60),RGB(0,116, 132),RGB(75,203,218) }; // Dungeons
static const int32_t PAL_RED[4] = { 0, RGB(182,49,32),RGB(234,159,32),RGB(255,255,255) };

static chr_ram_t LoadROMFromFile(const char* path);
static void BlitCHR(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t X, int32_t Y, int32_t MirrorX, int32_t MirrorY, int32_t address);
static void BlitCHR16x32(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t x, int32_t y, int32_t rotateX, int32_t rotateY, int32_t address);
static void BlitCHR32x32(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t x, int32_t y, int32_t rotate, int32_t address);
