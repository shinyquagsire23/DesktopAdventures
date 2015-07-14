#define GAME_OBJECT 					BIT(0)
#define UNDER_PLAYER_NONCOLLIDING 		BIT(1)
#define MIDDLE_LAYER_COLLIDING 			BIT(2)
#define PUSH_PULL_BLOCK 				BIT(3)
#define ABOVE_PLAYER_NONCOLLIDING		BIT(4)

#define MINI_MAP_TILE					BIT(5)
#define WEAPON							BIT(6)
#define ITEM							BIT(7)
#define CHARACTER						BIT(8)

//WEAPON
#define LIGHT_BLASTER					BIT(16)
#define HEAVY_BLASTER					BIT(17)
#define LIGHTSABER						BIT(18)
#define THE_FORCE						BIT(19)

//ITEMS
#define KEYCARD							BIT(16)
#define PUZZLE_ITEM_1					BIT(17)
#define PUZZLE_ITEM_2					BIT(18)
#define PUZZLE_ITEM_3					BIT(19)
#define LOCATOR							BIT(20)
#define HEALTH_PACK						BIT(22)

//CHARACTER
#define PLAYER							BIT(16)
#define ENEMY							BIT(17)
#define FRIENDLY						BIT(18)

//OTHER
#define IS_DOOR							BIT(16)

//MINI-MAP
#define HOME							BIT(17)
#define PUZZLE_UNSOLVED					BIT(18)
#define PUZZLE_SOLVED					BIT(19)
#define GATEWAY_UNSOLVED				BIT(20)
#define GATEWAY_SOLVED					BIT(21)
#define WALL_UP_LOCKED					BIT(22)
#define WALL_DOWN_LOCKED				BIT(23)
#define WALL_LEFT_LOCKED				BIT(24)
#define WALL_RIGHT_LOCKED				BIT(25)
#define WALL_UP_UNLOCKED				BIT(26)
#define WALL_DOWN_UNLOCKED				BIT(27)
#define WALL_LEFT_UNLOCKED				BIT(28)
#define WALL_RIGHT_UNLOCKED				BIT(29)
#define OBJECTIVE						BIT(30)

//TNAME **tile_names;
