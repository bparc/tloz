typedef struct
{
	int32_t x;
	int32_t y;
} v2_t;

static v2_t V2I(int32_t x, int32_t y)
{
	v2_t result = { x, y };
	return result;
}

static v2_t SubI(v2_t A, v2_t B)
{
	v2_t result = { A.x - B.x,A.y - B.y };
	return result;
}

typedef struct v2f_t
{
	float x;
	float y;
} v2f_t;

static v2f_t V2(float x, float y)
{
	v2f_t result = { x, y };
	return result;
}

static int32_t RoundToInt(float value)
{
	int32_t result = (int32_t)roundf(value);
	return result;
}

static float Lerp1(float A, float B, float T)
{
	float result = A;
	if (T < 0.0f)
	{
		result = A;
	}
	if (T > 1.0f)
	{
		result = B;
	}
	else
	{
		result = ((A * (1.0f - T)) + B * T);
	}
	return result;
}

static int32_t LerpI(int32_t A, int32_t B, float T)
{
	int32_t result = (int32_t)Lerp1((float)A, (float)B, T);
	return result;
}

static float Remainder(int32_t X, float value)
{
	float result = value;
	if (value > 0.0f)
	{
		result = (float)X - value;
	}
	if (value < 0.0f)
	{
		result = value - (float)X;
	}
	return result;
}

static int32_t Signumf(float value)
{
	int32_t result = value >= 0.0f ?
		1 : -1;
	return result;
}

typedef union
{
	struct
	{
		int32_t X1;
		int32_t Y1;
		int32_t X2;
		int32_t Y2;
	};
	struct
	{
		v2_t Min;
		v2_t Max;
	};
} bb_t;

static bb_t BoundingBoxFromRectangleI(int32_t x, int32_t y, int32_t width, int32_t height)
{
	bb_t result = { x,y,x + width,y + height };
	return result;
}

static v2_t PickRandomCardinalDir(void)
{
	static const v2_t CardinalDirs[4] =
	{
		{+0,-1},
		{+0,+1},
		{-1,+0},
		{+1,+0},
	};
	v2_t result = CardinalDirs[rand() % 4];
	return result;
}