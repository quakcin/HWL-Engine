#include "map.h"
#include "loader.h"
#include "main.h"

map_t * create_map()
{
	map_t * map = malloc(sizeof(struct map_s));

	if (map == NULL)
		I_Error("Map", "Failed to create object");

	map->tiles = malloc(sizeof(uint8_t) * (TILES_SIZE + 1));
	map->light = malloc(sizeof(uint8_t) * (128 * 128));

	map->clight = 0;
	unsigned int i;
	for (i = 0; i < 0xFFF; i++)
		map->smudged[i] = 0;
	I_Print("Map", "Object created!");
	return map;
}

void destroy_map(map_t * map)
{
	unsigned int i;
	for (i = 0; i != map->textures_count; i++)
		al_destroy_bitmap(map->textures[i + 1]);
	free(map->tiles);
	free(map);
	I_Print("Map", "Object destroyed!");
}

void map_load_texture(engine_t * engine, map_t * map, unsigned int tx_index)
{
	char texture_path[256];
	sprintf(texture_path, "data/%u/textures/%u.png", engine->level, tx_index);
	map->textures[tx_index] = al_load_bitmap(texture_path);
	if (map->textures[tx_index] != NULL)
		printf("[Map] Loaded %s.\n", texture_path);
	else
		printf("[Map] Failed to load %s.\n", texture_path);
}

void map_load(engine_t * engine, map_t * map, data_t * data)
{
	// -- load map meta
	char config_path[0xFF];
	sprintf(config_path, "data/%u/config", engine->level);
	parse_file(data, config_path);

	map->textures_count = GetVal(data, "tiles");

	printf("[Map] Loading %u tiles...\n",
		map->textures_count);

	// -- load textures

	unsigned int i;
	for (i = 1; i != map->textures_count + 1; i++)
		map_load_texture(engine, map, i);

	// -- load props

	map->props = 255 - GetVal(data, "props");
	map->colliders = GetVal(data, "colliders");

	for (i = map->props + 1; i < 256; i++)
		map_load_texture(engine, map, i);

	// -- load tiles
	printf("[Map] Loading Tile Data...\n");
	char tile_path[0xFF];
	sprintf(tile_path, "data/%u/tiles", engine->level);
	FILE * tile_file = fopen(tile_path, "r");
	tile_file = fopen(tile_path, "r");
	fread(map->tiles, sizeof(uint8_t), 256 * 256, tile_file);
	fclose(tile_file);

	// -- load light map
	printf("[Map] Loading Light Data...\n");
	char light_path[0xFF];
	sprintf(light_path, "data/%u/light", engine->level);
	FILE * light_file = fopen(light_path, "r");
	light_file = fopen(light_path, "r");
	fread(map->light, sizeof(uint8_t), 128 * 128, light_file);
	fclose(light_file);

	// -- load colors
	
	map->sky = al_map_rgb(
		GetVal(data, "sky_r"), GetVal(data, "sky_g"), GetVal(data, "sky_b")
	);

	map->floor = al_map_rgb(
		GetVal(data, "floor_r"), GetVal(data, "floor_g"), GetVal(data, "floor_b")
	);

	// -- load stars
	map->stars = GetVal(data, "stars");

	// -- load door file
	map->door = map->textures_count - GetVal(data, "doors");

	// -- Finish
	printf("[Map] Finished Loading!\n");
}