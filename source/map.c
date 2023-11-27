typedef struct
{
	int32_t x;
	int32_t y;
	enum enemy_type_t Type;
} enemy_spawn_t;

typedef struct
{
	uint8_t Tiles[8 * 12];
	int32_t SpawnerCount;
	enemy_spawn_t Spawners[10];
} room_data_t;

static enemy_spawn_t *PushSpawn(room_data_t* Room, int32_t Type,int32_t tilex, int32_t tiley)
{
	assert(Room->SpawnerCount < ArrayCount(Room->Spawners));
	enemy_spawn_t* result = Room->Spawners + Room->SpawnerCount++;
	result->Type = Type;
	result->x = tilex * 16;
	result->y = tiley * 16;
	return result;
}

typedef struct
{
	int32_t TileHeight;
	int32_t x;
	int32_t y;
	uint8_t* Tiles;
	v2_t Extends;
	int32_t DoorAlign[2];
} tile_map_t;

static uint8_t* GetTile(tile_map_t* map, int32_t x, int32_t y)
{
	uint8_t* result = 0;
	if (x >= 0 && y >= 0 && x < map->x && y < map->y)
	{
		result = map->Tiles + (y * map->x + x);
	}
	return result;
}

static uint8_t GetTileValue(tile_map_t* map, int32_t x, int32_t y)
{
	uint8_t result = 0;
	uint8_t* value = GetTile(map, x, y);
	if (value)
	{
		result = *value;
	}
	return result;
}

static void SetTile(tile_map_t* map, int32_t x, int32_t y, uint8_t type)
{
	if (x >= 0 && y >= 0 && x < map->x && y < map->y)
	{
		*(map->Tiles + ((y * map->x) + x)) = type;
	}
}