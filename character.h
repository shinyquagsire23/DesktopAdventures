#include "useful.h"

typedef struct char_data
{
	u32 magic;
	u32 unk_1;
	u8 name[10];
	u32 unk_2;
	u16 pad_1;
	u16 unk_3;
	u16 unk_4;
	u16 frames[26];
} ichr_data;

enum CHAR_FRAME
{
	BLNK1 = 0,
	BLNK2,

	UP,
	DOWN,
	UP_LEFT,
	LEFT,
	DOWN_LEFT,
	UP_RIGHT,
	RIGHT,

	//Sprites for hero, walking
	DOWN_RIGHT = 9,
	WALK_UP_1,
	WALK_DOWN_1,
	WALK_UP_LEFT_1,
	WALK_LEFT_1,
	WALK_DOWN_LEFT_1,
	WALK_UP_RIGHT_1,
	WALK_RIGHT_1,
	WALK_DOWN_RIGHT_1,
	WALK_UP_2,
	WALK_DOWN_2,
	WALK_UP_LEFT_2,
	WALK_LEFT_2,
	WALP_DOWN_LEFT_2,
	WALK_UP_RIGHT_2,
	WALK_RIGHT_2,
	WALK_DOWN_RIGHT_2,

	//Sprites for weapon, attacking.
	//Extend is used for whips (Indianna Jones) and Sabers (Luke)
	WEAPON_ICON = 9,
	ATTACK_UP_1,
	ATTACK_DOWN_1,
	ATTACK_EXTEND_UP_2,
	ATTACK_LEFT_1,
	ATTACK_EXTEND_DOWN_1,
	ATTACK_EXTEND_LEFT_1,
	ATTACK_RIGHT_1,
	ATTACK_EXTEND_RIGHT_1,
	ATTACK_UP_2,
	ATTACK_DOWN_2,
	ATTACK_EXTEND_UP_1,
	ATTACK_LEFT_2,
	ATTACK_EXTEND_DOWN_2,
	ATTACK_EXTEND_LEFT_2,
	ATTACK_RIGHT_2,
	ATTACK_EXTEND_RIGHT_2,
};

ichr_data **char_data;
