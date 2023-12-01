
static chr_ram_t LoadROMFromFile(const char* path)
{
	chr_ram_t result;
	memset(&result, 0, sizeof(result));
	file_contents_t contents = LoadFileContents(path);
	if (contents.Size)
	{
		result.Memory = contents.Data;
		const uint8_t* at = result.Memory;
		if (at[0] == 0x4e && at[1] == 0x45 && at[2] == 0x53 && at[3] == 0x1a)
		{
			result.Base = &at[16];
		}
		else
		{
			result.Memory = NULL;
		}
	}
	return result;
}

static void BlitCHR(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t X, int32_t Y, int32_t MirrorX, int32_t MirrorY, int32_t address)
{
	const uint8_t* base = RAM->Base + address;
	const uint8_t* pattern1 = &base[0];
	const uint8_t* pattern2 = &base[8];
	for (int32_t y = 0; y < 8; y++)
	{
		for (int32_t x = 0; x < 8; x++)
		{
			int32_t mask = MirrorX ? x : (7 - x);
			int32_t yOffset = MirrorY ? 7 - y : y;
			uint8_t bit1 = (((pattern1[yOffset] & (1 << mask))));
			uint8_t bit2 = (((pattern2[yOffset] & (1 << mask))));
			int32_t index = 0;
			if (!bit1 && bit2)
			{
				index = 2;
			}
			if (bit1 && !bit2)
			{
				index = 1;
			}
			if (bit1 && bit2)
			{
				index = 3;
			}
			if (!bit1 && !bit2)
			{
				index = 0;
			}
			Pixel(buffer,X + x,Y + y,*(palette + index));
		}
	}
}

static void BlitCHR16x32(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t x, int32_t y,
	int32_t rotateX, int32_t rotateY, int32_t address)
{
	int32_t offsetY = (8 * rotateY);
	BlitCHR(buffer, RAM, palette, x, y + offsetY, rotateX, rotateY, address);
	BlitCHR(buffer, RAM, palette, x, y + (8-offsetY), rotateX, rotateY, address + 16);
}

static void BlitCHR32x16(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t x, int32_t y,
	int32_t rotateX, int32_t rotateY, int32_t address)
{
	int32_t offsetY = (8 * rotateY);
	BlitCHR(buffer, RAM, palette, x + offsetY, y, rotateX, rotateY, address);
	BlitCHR(buffer, RAM, palette, x + (8 - offsetY), y, rotateX, rotateY, address + 16);
}

static void BlitCHR32x32(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette, int32_t x, int32_t y, int32_t rotate, int32_t address)
{
	int32_t offset = (8 * rotate);
	BlitCHR(buffer,RAM, palette, x + offset, y, rotate, 0, address);
	BlitCHR(buffer, RAM, palette, x + offset, y + 8, rotate,0, address+16);
	BlitCHR(buffer, RAM, palette, (x + 8) - offset, y, rotate,0, address+32);
	BlitCHR(buffer, RAM, palette, (x + 8) - offset, y + 8, rotate,0, address+48);
}