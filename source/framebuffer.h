#pragma once

typedef struct
{
	int32_t x;
	int32_t y;
	uint32_t* Pixels;
	v2_t Base;
} framebuffer_t;

static void ClearFramebufferBits(framebuffer_t* buffer, uint32_t color);
static void Pixel(framebuffer_t* buffer, int32_t X, int32_t Y, int32_t color);
static void BlitRectangle(framebuffer_t* buffer, int32_t X, int32_t Y, int32_t W, int32_t H, int32_t color);
static void BlitRectangleMinMax(framebuffer_t* buffer, int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, int32_t color);
static void BlitBoundingBox(framebuffer_t* buffer, int32_t X, int32_t Y, bb_t bb, int32_t color);
static void SetCoordinateSystem(framebuffer_t* buffer, int32_t X, int32_t Y);