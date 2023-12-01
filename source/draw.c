//
static void DrawBitmap(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette,
	int32_t x, int32_t y, int32_t rotate, int32_t frame, int32_t address)
{
	BlitCHR32x32(buffer, RAM, palette, x, y, rotate, (address + (frame * 64)));
}

static void DrawBitmapMirrorX(framebuffer_t* buffer, const chr_ram_t* RAM, const uint32_t* palette,
	int32_t x, int32_t y, int32_t rotate, int32_t frame, int32_t address)
{
	BlitCHR16x32(buffer, RAM, palette, x + 0, y, 0, rotate, (address + (frame * 32)));
	BlitCHR16x32(buffer, RAM, palette, x + 8, y, 1, rotate, (address + (frame * 32)));
}

static void DrawLink(framebuffer_t* buffer, const chr_ram_t* RAM,const ent_player_t*entity,int32_t X, int32_t Y,sword_t Sword)
{
	int32_t offset = entity->Facing.y > 0 ? 0 : 1;
	int32_t E = (abs(entity->Facing.x) == 1);
	if (entity->IsAttacking)
	{
		if (E)
		{
			BlitCHR32x32(buffer, RAM, PAL_GREEN, X, Y, entity->Facing.x < 0, CHR_LINK_ATTACK_E);
			BlitCHR32x32(buffer, RAM, PAL_RED, Sword.Bounds.X1, Sword.Bounds.Y1-7, entity->Facing.x<0, CHR_SWORD_E);
		}
		else
		{
			BlitCHR32x32(buffer, RAM, PAL_GREEN, X, Y, 0, CHR(CHR_LINK_ATTACK_N,offset));
			BlitCHR16x32(buffer, RAM, PAL_RED, Sword.Bounds.X1-2, Sword.Bounds.Y1, 0, entity->Facing.y>0, CHR_SWORD_N);
		}
#if 0
		BlitBoundingBox(buffer, 0, 0, Sword.Bounds, RGB(255, 0, 0));
#endif
	}
	else
	{
		int32_t framecount = 2;
		int32_t frame = (int32_t)fmodf(entity->tMove * 6.0f, (float)framecount);
		if (E)
		{
			BlitCHR32x32(buffer, RAM, PAL_GREEN, X, Y, entity->Facing.x < 0, CHR(CHR_LINK_E, frame % 2));
		}
		else
		{
			BlitCHR32x32(buffer, RAM, PAL_GREEN, X, Y, frame % 2, CHR(CHR_LINK_N, offset));
		}
	}
}

static void DrawWall(framebuffer_t* buffer, const chr_ram_t* RAM,
	int32_t W, int32_t H, int32_t X, int32_t Y)
{
	for (int32_t y = 0; y < (H * 2) + 8; y++)
	{
		int32_t MinX = X - 32;
		int32_t MaxX = X + ((W * 16)) + (24);
		for (int32_t x = 0; x < 2; x++)
		{
			BlitCHR(buffer, RAM, PAL_BLUE, MinX + x * 8, (Y + y * 8) - 32, 0, 0, CHR_DNG_WALL);
			BlitCHR(buffer, RAM, PAL_BLUE, MaxX - x * 8, (Y + y * 8) - 32, 0, 0, CHR_DNG_WALL);
		}
	}
	for (int32_t x = 0; x < (W * 2) + 6; x++)
	{
		int32_t MinY = Y - 32;
		int32_t MaxY = Y + ((H * 16)) + (24);
		for (int32_t y = 0; y < 2; y++)
		{
			BlitCHR(buffer, RAM, PAL_BLUE, (X - 24) + x * 8, MinY + y * 8, 0, 0, CHR_DNG_WALL);
			BlitCHR(buffer, RAM, PAL_BLUE, (X - 24) + x * 8, MaxY - y * 8, 0, 0, CHR_DNG_WALL);
		}
	}
}

static void DrawTileMap(framebuffer_t* buffer, const chr_ram_t* RAM, const uint8_t* tiles,
	int32_t W, int32_t H, int32_t X, int32_t Y, const int32_t DoorAlign[2])
{
	// TILES
	const uint8_t* at = tiles;
	for (int32_t y = 0; y < H; y++)
	{
		for (int32_t x = 0; x < W; x++)
		{
			uint8_t value = *at++;
			if (value == 8)
			{
				int32_t minX = X + (x * 16);
				int32_t minY = Y + (y * 16);
				BlitRectangleMinMax(buffer, minX, minY, minX + 16, minY + 16, RGB(255, 0, 0));
			}
			else
			{
				BlitCHR32x32(buffer, RAM, PAL_BLUE, X + (x * 16), Y + (y * 16),
					0, CHR(CHR_BANK2, value));
			}
		}
	}
	// DOORS
	// LEFT,RIGHT 
	{
		int32_t y = Y + DoorAlign[1];
		BlitCHR16x32(buffer, RAM, PAL_BLUE, X - 8, y, 0, 0, CHR_FLOOR + 32);
		BlitRectangle(buffer, X - 16, y, 8, 16, RGB(0, 0, 0));
		int32_t x = (X + W * 16);
		BlitCHR16x32(buffer, RAM, PAL_BLUE, x, y, 0, 0, CHR_FLOOR);
		BlitRectangle(buffer, x + 8, y, 8, 16, RGB(0, 0, 0));
	}
	// TOP,BOTTOM
#if 0
	{
		int32_t x = X + DoorAlign[0];
		BlitCHR(buffer, RAM, PAL_BLUE, x + 0, Y - 8, 0,0, CHR_FLOOR);
		BlitCHR(buffer, RAM, PAL_BLUE, x + 8, Y - 8, 0,0, CHR_FLOOR);
		BlitRectangle(buffer, x, Y - 16, 16, 8, RGB(0, 0, 0));
		int32_t y = Y + H * 16;
		BlitCHR(buffer, RAM, PAL_BLUE, x + 0, y, 0,0, CHR_FLOOR);
		BlitCHR(buffer, RAM, PAL_BLUE, x + 8, y, 0,0, CHR_FLOOR);
		BlitRectangle(buffer, x, y + 8, 16, 8, RGB(0, 0, 0));
	}
#endif
}

static void _DrawFrame(framebuffer_t* buffer, const game_state_t* state, const chr_ram_t* RAM, double time)
{
	ClearFramebufferBits(buffer, RGB(64, 64, 64));
#if 1 // // NOTE: Game/CHR banks
	const tile_map_t* map = &state->Map;
	SetCoordinateSystem(buffer, 32, (buffer->y - map->Extends.y) - 32);
	// NOTE: Rendering
	if ((state->RequestedRoom == 0))
	{
		DrawTileMap(buffer, RAM, map->Tiles, map->x, map->y, 0, 0, map->DoorAlign);
		DrawWall(buffer, RAM, map->x, map->y, 0, 0);
		// NOTE: Entities
#if 1
		for (int32_t index = 0; index < state->EntityCount; index++)
		{
			const game_object_t* Generic = state->Entities + index;
			int32_t X = (Generic->X) - 8;
			int32_t Y = (Generic->Y) - 8;
			switch(Generic->Type)
			{
			case ENT_OCTOROCK:
			{
				const ent_octorock_t* entity = &Generic->_Octorock;
				int32_t frame = (int32_t)fmodf((float)time * 5.0f, 2.0f);
				//DrawBitmapMirrorX(buffer, RAM, PAL_RED, X, Y, entity->Facing, frame, CHR_OCTOROCK);
				v2_t facingdir = entity->Facing;
				if ((abs(facingdir.x) == 1 && facingdir.y == 0))
				{
					DrawBitmap(buffer, RAM, PAL_RED, X, Y, (facingdir.x > 0 ? 1 : 0), frame, CHR(CHR_OCTOROCK,1));
				}
				else
				{
					DrawBitmapMirrorX(buffer, RAM, PAL_RED, X, Y, facingdir.y > 0 ? 0 : 1, frame, CHR_OCTOROCK);
				}
			} break;
			case ENT_TEKTITE:
			{
				DrawBitmapMirrorX(buffer, RAM, PAL_RED, X, Y, 0, (int32_t)(time * 5.0f) % 2, CHR_TEKTITE);
			} break;
			case ENT_KEESE:
			{
				DrawBitmapMirrorX(buffer, RAM, PAL_BLUE_ALPHA, X, Y, 0, (int32_t)(time * 5.0f) % 2, CHR_KEESE);
			} break;
			case ENT_PROJECTILE_OCTOROCK:
			{
				//DrawBitmap(buffer, RAM, PAL_RED, X, Y, CHR_OCTOCOK_BULLET);
				//BlitRectangle(buffer, X, Y, 16, 16, RGB(255, 255, 255));
				BlitBoundingBox(buffer, Generic->X, Generic->Y, Generic->Bounds, RGB(255, 255, 255));
			} break;
			case ENT_SPAWN:
			{
				//int32_t R = LerpI(0, 255, sinf(entity->tSpawn));
				//BlitRectangle(buffer, X, Y, 16, 16, RGB(R, R, 255-R));
				const ent_spawn_t* entity = &Generic->_Spawn;
				DrawBitmapMirrorX(buffer, RAM, PAL_RED, X, Y, 0, (int32_t)(entity->tRemaining * 2.0f), CHR_EFFECT_SPAWN);
			} break;
			case ENT_PARTICLE_EFFECT:
			{
				const ent_particle_t* entity = &Generic->_Particle;
				DrawBitmapMirrorX(buffer, RAM, PAL_RED, X, Y, 0, (int32_t)(entity->tRemaining * 2.0f), CHR_EFFECT_SPAWN);
			} break;
			}
		}
#endif
		// LINK
		{
			const game_object_t* entity = &state->Player;
			DrawLink(buffer, RAM, &entity->_Player, entity->X - 8, entity->Y - 8, state->Sword);
		}
	}
	// NOTE: Transition
	if ((state->RequestedRoom != 0))
	{
		v2_t exitdir = state->Player.Facing;
		const game_object_t* entity = &state->Player;
		int32_t X = (buffer->x * -exitdir.x);
		int32_t Y = ((buffer->y - 64) * exitdir.y);
		int32_t cameraX = Lerp(0, X, state->tTransit);
		int32_t cameraY = Lerp(0, Y, state->tTransit);
		DrawTileMap(buffer, RAM, map->Tiles, map->x, map->y, cameraX, cameraY, map->DoorAlign);
		DrawTileMap(buffer,RAM,state->RequestedRoom->Tiles,map->x,map->y, (cameraX - X), (cameraY - Y), map->DoorAlign);
		DrawWall(buffer,RAM,map->x,map->y,cameraX,cameraY);
		DrawWall(buffer,RAM,map->x,map->y,(cameraX - X),(cameraY - Y));
	}

	// NOTE: GUI
	SetCoordinateSystem(buffer, 0, 0);
	BlitRectangleMinMax(buffer, 0, 0, buffer->x, (64 - 1), RGB(0, 0, 0));
	for (int32_t x = 0; x < 3; x++)
	{
		BlitCHR(buffer, RAM, PAL_GREEN, 8 + x * 10, 8, 0, 0, CHR_ICON_LIFE);
	}
#else // NOTE: CHR banks
	{
		///BlitCHR16(buffer, RAM, PAL_BLUE, 0, 0, 0, );
		SetCoordinateSystem(buffer, 0, 0);
#if 1
		int32_t off = 0;
		for (int32_t y = 0; y < 16; y++)
		{
			for (int32_t x = 0; x < 16; x++)
			{
				BlitCHR32x32(
					buffer,
					RAM,
					PAL_BLUE,
					x * 16,
					y * 16,
					0,
					CHR(CHR_BANK2, off++));
			}
		}
#else
#endif
	}
#endif
}