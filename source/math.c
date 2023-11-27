typedef struct
{
	int32_t x;
	int32_t y;
} v2_t;

static v2_t V2(int32_t x, int32_t y)
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

static v2f_t V2F(float x, float y)
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

static int32_t Lerp(int32_t A, int32_t B, float T)
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

static int32_t TestAABB(bb_t A, bb_t B)
{
	int32_t result = 0;
	return result;
}

static bb_t BoundingBoxFromRectangleI(int32_t x, int32_t y, int32_t width, int32_t height)
{
	bb_t result = { x,y,x + width,y + height };
	return result;
}

static const v2_t CardinalDirs[8] =
{
	{+0,-1},
	{+0,+1},
	{-1,+0},
	{+1,+0},
	{+1,+1},
	{-1,+1},
	{+1,-1},
	{+1,+1},
};

static v2_t PickRandomCardinalDir(void)
{
	v2_t result = CardinalDirs[rand() % 4];
	return result;
}

static v2_t PickRandomDir(void)
{
	v2_t result = CardinalDirs[rand() % 8];
	return result;
}

static int32_t ClampI(int32_t X, int32_t min, int32_t max)
{
	int32_t result = X;
	if (X < min)
	{
		result = min;
	}
	if (X > max)
	{
		result = max;
	}
	return result;
}

static int32_t JumpArc(float height, float t)
{
	int32_t result = (int32_t)floorf(-fabsf(sinf((-t * (float)M_PI))) * height);
	return result;
}

static float Length(v2f_t X)
{
	float result = sqrtf(X.x * X.x + X.y * X.y);
	return result;
}

static v2f_t NormalizeI(v2_t X)
{
	v2f_t result = V2F((float)X.x,(float)X.y);
	float length = Length(result);
	if (length > 0.0f)
	{
		result.x /= length;
		result.y /= length;
	}
	return result;
}

static v2_t Opposite(v2_t X)
{
	v2_t result = { -X.x,-X.y };
	return result;
}

static bb_t BB(int32_t X1, int32_t Y1, int32_t X2, int32_t Y2)
{
	bb_t result = { X1,Y1,X2,Y2 };
	return result;
}
static bb_t BoundsFromRectangle(int32_t X, int32_t Y, int32_t width, int32_t height)
{
	bb_t result = { X, Y };
	result.X2 = X + width;
	result.Y2 = Y + height;
	return result;
}

static bb_t Transform(bb_t A, v2_t B)
{
	bb_t result = A;
	result.X1 += B.x;
	result.X2 += B.x;
	result.Y1 += B.y;
	result.Y2 += B.y;
	return result;
}

static int32_t TestBounds(bb_t A, bb_t B)
{
	int32_t T1 = A.Max.x < B.Min.x;
	int32_t T2 = B.Max.x < A.Min.x;
	int32_t T3 = B.Max.y < A.Min.y;
	int32_t T4 = A.Max.y < B.Min.y;
	int32_t result = !(T1 || T2 || T3 || T4);
	return result;
}