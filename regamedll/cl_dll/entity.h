/*

Created Date: 05 Mar 2020

*/

#pragma once

enum
{
	MAX_PLAYERS = 33, // including the bomb
	MAX_TEAMS = 3,
	MAX_TEAM_NAME = 16,
	MAX_HOSTAGES = 24,
};

struct extra_player_info_t
{
	short frags;
	short deaths;
	short team_id;
	bool has_c4;
	bool vip;
	Vector origin;
	float radarflash;
	float radarflashon;
	float radarflashes;
	short playerclass;
	short teamnumber;
	char teamname[16];
	bool dead;
	bool showhealth;
	int health;
	char location[32];
};
