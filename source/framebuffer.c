
static void ClearFramebufferBits(framebuffer_t* buffer, uint32_t color)
{
	uint32_t* at = buffer->Pixels;
	for (int32_t y = 0; y < buffer->y; y++)
	{
		for (int32_t x = 0; x < buffer->x; x++)
		{
			*at++ = color;
		}
	}
}

static void Pixel(framebuffer_t*buffer, int32_t X, int32_t Y, int32_t color)
{
	X += buffer->Base.x;
	Y += buffer->Base.y;
	if (X >= 0 && X < buffer->x && Y >= 0 && Y < buffer->y)
	{
		if (color && 0xff000000)
		{
			buffer->Pixels[(Y * buffer->x) + X] = color;
		}
	}
}

static void BlitRectangleMinMax(framebuffer_t* buffer, int32_t X1, int32_t Y1, int32_t X2, int32_t Y2, int32_t color)
{
	for (int32_t y = Y1; y <= Y2; y++)
	{
		for (int32_t x = X1; x <= X2; x++)
		{
			Pixel(buffer, x, y, color);
		}
	}
}

static void BlitRectangle(framebuffer_t* buffer, int32_t X, int32_t Y, int32_t W, int32_t H, int32_t color)
{
	BlitRectangleMinMax(buffer, X, Y, X + (W-1), Y + (H-1),color);
}

static void BlitBoundingBox(framebuffer_t* buffer, int32_t X, int32_t Y, bb_t bb,int32_t color)
{
	BlitRectangleMinMax(buffer, X + bb.X1, Y + bb.Y1, X + bb.X2, Y + bb.Y2, color);
}

static void SetCoordinateSystem(framebuffer_t* buffer, int32_t X, int32_t Y)
{
	buffer->Base.x = X;
	buffer->Base.y = Y;
}