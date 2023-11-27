#pragma once
typedef struct
{
	v2_t DPad;
	int32_t A;
	int32_t B;
} controller_t;

typedef enum
{
	ENT_NONE,
	ENT_EMPTY,
	ENT_PLAYER,
	ENT_BULLET,
	ENT_OCTOROCK,
	ENT_TEKTITE,
	ENT_KEESE,
	ENT_SPAWN,
} entity_type_t;

typedef enum
{
	ENT_FLAGS_ENEMY = 1 << 0,
} game_object_flags_t;

typedef struct
{
	entity_type_t Type;

	int32_t Flags;
	int32_t Removed;
	v2f_t Offset;
	int32_t X, Y;
	v2_t Base;
	bb_t Bounds;
	bb_t HitBox;

	union
	{
		v2_t Facing;
		v2_t ExitDir;
	};

	float tMove;
	union
	{
		struct
		{
			float tAttack;
			float tRecovery;
			int32_t IsAttacking;
		}; // Player
		struct
		{
			float tChangeMovingDir;
			float tShoot;
		}; // Octorock, Keese
		struct
		{
			float tJump;
			v2_t JumpFrom;
			v2_t JumpTo;
			int32_t HasJumped;
		}; // Tektite
		struct
		{
			int32_t SpawnType;
			float tSpawn;
		}; // Spawn
	};
} game_object_t;

static void Remove(game_object_t* entity)
{
	entity->Removed = TRUE;
}

static void SetPosition(game_object_t* entity, v2_t X)
{
	entity->X = X.x;
	entity->Y = X.y;
}

typedef struct
{
	v2_t Position;
	bb_t Bounds;
} sword_t;

enum
{
	MOVE_ALIGN = 1 << 0,
	MOVE_DO_NOT_INTERSECT = 1 << 1
} move_flags_t;

typedef struct
{
	sword_t Sword;
	game_object_t Player;
	int32_t RoomIndex;

	int32_t EntityCount;
	game_object_t Entities[1024];

	tile_map_t Map;
	room_data_t Rooms[16];

	int32_t MemoryInitialized;
	double PrevTime;

	float tTransit;
	room_data_t *RequestedRoom;
} game_state_t;

typedef enum
{
	INTERSECT_TILE = 1 << 1,
	INTERSECT_OUT_OF_BOUNDS = 1 << 2,
} intersection_flags_t;

static void Intersect(game_state_t* state, game_object_t* entity, v2_t N, int32_t flags);

static game_object_t* spawn(game_state_t* state, int32_t X, int32_t Y, entity_type_t type)
{
	assert(state->EntityCount < ArrayCount(state->Entities));
	game_object_t* result = state->Entities + state->EntityCount++;
	memset(result, 0, sizeof(*result));
	result->Type = type;
	result->X = X;
	result->Y = Y;
	result->Base = V2(X, Y);
	return result;
}

static game_object_t* CreateProjectile(game_state_t* state, int32_t X, int32_t Y, v2_t direction, int32_t flags)
{
	game_object_t* result = spawn(state, X, Y, ENT_BULLET);
	result->Facing = direction;
	result->Bounds.X1 = -5;
	result->Bounds.X2 = +5;
	result->Bounds.Y1 = -5;
	result->Bounds.Y2 = +5;
	return result;
}

static game_object_t* CreateSpawn(game_state_t *state, int32_t x, int32_t y, int32_t Type)
{
	game_object_t* result = spawn(state, x, y, ENT_SPAWN);
	result->SpawnType = Type;
	return result;
}

static room_data_t RoomData(char* tiles, int32_t X, int32_t Y)
{
	static uint8_t values[256] = "";
	values[' '] = 1;
	values['#'] = 16;
	values['!'] = 8;
	room_data_t result;
	memset(&result, 0, sizeof(result));
	assert(ArrayCount(result.Tiles) >= X * Y);
	for (int32_t y = 0; y < Y; y++)
	{
		for (int32_t x = 0; x < X; x++)
		{
			char C = ' ';
			switch (tiles[y * X + x])
			{
			case 'O':
			{
				PushSpawn(&result, ENT_OCTOROCK, x, y);
			} break;
			case 'T':
			{
				PushSpawn(&result, ENT_TEKTITE, x, y);
			} break;
			case 'K':
			{
				PushSpawn(&result, ENT_KEESE, x, y);
			} break;
			default:
				C = tiles[y * X + x];
			}
			result.Tiles[y * X + x] =
				values[C];
		}
	}
	return result;
}

static void LoadRoomFromData(game_state_t*state, room_data_t* Room)
{
	tile_map_t* map = &state->Map;
	map->Tiles = &Room->Tiles[0];
	state->EntityCount = 0;
	for (int32_t index = 0; index < Room->SpawnerCount; index++)
	{
		enemy_spawn_t* Spawn = Room->Spawners + index;
		CreateSpawn(state, Spawn->x, Spawn->y, Spawn->Type);
	}
}

static int32_t _Init(game_state_t* state)
{
	int32_t result = TRUE;
	if (!state->MemoryInitialized)
	{
		{
#define X 12
#define Y 7
			static char Room_0[Y][X] =
			{
				' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '#', 'T', ' ',
				' ', ' ', ' ', 'O', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', 'K', ' ', ' ', 'K', '#', ' ', 'O',
				' ', 'T', ' ', 'O', ' ', ' ', ' ', ' ', ' ', '#', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', 'O', ' ', ' ', '#', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', '!', '!', '!', '#', ' ', ' ',
			};
			static char Room_1[Y][X] =
			{
				'O', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
				' ', ' ', '!', ' ', ' ', '#', '#', '#', '#', '#', ' ', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
				' ', ' ', '!', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
				' ', ' ', '!', '!', ' ', ' ', ' ', ' ', ' ', ' ', 'O', ' ',
				' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
			};
			static char Room_2[Y][X] =
			{
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
				' ', ' ', ' ', ' ', ' ', ' ', ' ', 'O', ' ', 'O', ' ', ' ',
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
				'#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#', '#',
			};
			state->Rooms[0] = RoomData(Room_0[0], X, Y);
			state->Rooms[1] = RoomData(Room_1[0], X, Y);
			state->Rooms[2] = RoomData(Room_2[0], X, Y);
			tile_map_t* map = &state->Map;
			map->x = X;
			map->y = Y;
			map->TileHeight = 16;
			map->Extends = V2(map->x * map->TileHeight, map->y * map->TileHeight);
			map->DoorAlign[1] = (map->y / 2) * map->TileHeight;
			map->DoorAlign[0] = (map->Extends.x / 2) - (map->TileHeight / 2);

#undef X
#undef Y
			// LOAD MAP FROM ROOM DATA
			LoadRoomFromData(state, &state->Rooms[0]);
			// INIT PLAYER
			game_object_t* entity = &state->Player;
			entity->Type = ENT_PLAYER;
			entity->X = 18;
			entity->Y = 18;
			entity->Bounds.X1 = 0 - 7;
			entity->Bounds.X2 = 0 + 6;
			entity->Bounds.Y1 = 0;
			entity->Bounds.Y2 = 0 + 7;
			entity->Facing = V2(0, 1);
		}
		state->MemoryInitialized = TRUE;
	}
	return result;
}

static void RoomTransition(game_state_t* state)
{
	if ((state->RequestedRoom == 0))
	{
		state->RoomIndex += state->Player.Facing.x;
		if (state->RoomIndex < 0)
		{
			state->RoomIndex = 2;
		}
		state->RequestedRoom = &state->Rooms[state->RoomIndex%3];
		state->tTransit = 0.0f;
	}
}

static void movex(game_state_t* state, game_object_t* entity, int32_t offset, int32_t flags)
{
	int32_t dT = offset >= 0 ? 1 : -1;
	entity->X += dT;
	if (!(flags & MOVE_DO_NOT_INTERSECT))
	{
		for (int32_t y = ((entity->Y + entity->Bounds.Y1) / state->Map.TileHeight);
			y <= ((entity->Y + entity->Bounds.Y2) / state->Map.TileHeight);
			y++)
		{
			int32_t x = dT > 0 ?
				(entity->X + entity->Bounds.X2) / state->Map.TileHeight :
				(entity->X + entity->Bounds.X1) / state->Map.TileHeight;
			if (GetTileValue(&state->Map, x, y) == 16)
			{
				entity->X -= dT;
				Intersect(state, entity, V2(-dT, 0), INTERSECT_TILE);
				return;
			}
		}
	}
}

static void movey(game_state_t* state, game_object_t* entity, int32_t offset, int32_t flags)
{
	int32_t dT = offset >= 0 ? 1 : -1;
	entity->Y += dT;
	if (!(flags & MOVE_DO_NOT_INTERSECT))
	{
		for (int32_t x = ((entity->X + entity->Bounds.X1) / state->Map.TileHeight);
			x <= ((entity->X + entity->Bounds.X2) / state->Map.TileHeight);
			x++)
		{
			int32_t y = dT > 0 ?
				(entity->Y + entity->Bounds.Y2) / state->Map.TileHeight :
				(entity->Y + entity->Bounds.Y1) / state->Map.TileHeight;
			if (GetTileValue(&state->Map, x, y) == 16)
			{
				entity->Y -= dT;
				Intersect(state, entity, V2(0, -dT), INTERSECT_TILE);
				return;
			}

		}
	}
}

static void Move(game_state_t* state, game_object_t* entity, v2_t dir, int32_t speed, float dt, int32_t flags)
{
	// SET FACING DIR
	if ((dir.x != 0) || (dir.y != 0))
	{
		entity->Facing = dir;
		entity->tMove += dt;
	}
	// INTEGRATE
	v2f_t N = NormalizeI(dir);
	entity->Offset.x += (N.x * (float)speed * dt);
	entity->Offset.y += (N.y * (float)speed * dt);
	// MOVE
	int32_t steps;
	int32_t X = (int32_t)floorf(entity->Offset.x);
	steps = abs(X);
	if (steps--)
	{
		int32_t Ymod = (entity->Y) % 8;
		if (Ymod == 0 || !(flags & MOVE_ALIGN))
		{
			movex(state, entity, X,flags);
		}
		else
		if (Ymod > 4)
		{
			movey(state, entity, +1,flags);
		}
		else
		{
			movey(state, entity, -1,flags);
		}
		entity->Offset.x = Remainder(X, entity->Offset.x);
	}
	int32_t Y = (int32_t)floorf(entity->Offset.y);
	steps = abs(Y);
	while (steps--)
	{
		int32_t Xmod = (entity->X) % 8;
		if (Xmod == 0 || !(flags & MOVE_ALIGN))
		{
			movey(state, entity, Y,flags);
		}
		else
		if (Xmod > 4)
		{
			movex(state, entity, +1,flags);
		}
		else
		{
			movex(state, entity, -1,flags);
		}
		entity->Offset.y = Remainder(Y, entity->Offset.y);
	}
	// BOUNDS
	v2_t min = SubI(V2(0, 0), entity->Bounds.Min);
	v2_t max = SubI(state->Map.Extends, entity->Bounds.Max);
	if (entity->X < min.x)
	{
		entity->X = min.x;
		Intersect(state, entity, V2(+1, 0), INTERSECT_OUT_OF_BOUNDS);
	}
	else
	if (entity->X > max.x)
	{
		entity->X = max.x;
		Intersect(state, entity, V2(-1, 0), INTERSECT_OUT_OF_BOUNDS);
	}
	if (entity->Y < min.y)
	{
		entity->Y = min.y;
		Intersect(state, entity, V2(0, +1), INTERSECT_OUT_OF_BOUNDS);
	}
	else
	if (entity->Y > max.y)
	{
		entity->Y = max.y;
		Intersect(state, entity, V2(0, -1), INTERSECT_OUT_OF_BOUNDS);
	}
}