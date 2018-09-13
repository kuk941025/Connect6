#include "ai_sourcefile.h"
#include <vector>
#include <math.h>
#include <time.h>

StoneCOORD stone_pos;
StoneCOORD simple_stone_pos;
int eval_called;
bool cmpPriority(const coordInfo &a, const coordInfo &b) {
	return a.priority > b.priority;
}
int threatZone[][6] = {
	{1, 1, 1, 1, 0, 0},
	{0, 1, 1, 1, 1, 0},
	{0, 0, 1, 1, 1, 1},
	{1, 1, 1, 0, 0, 1},
	{1, 1, 1, 0, 1, 0},
	{0, 1, 0, 1, 1, 1},
	{0, 1, 1, 1, 0, 1},
	{1, 0, 1, 1, 1, 0},
	{0, 1, 1, 1, 0, 1},
	{1, 0, 0, 1, 1, 1},
	{1, 1, 0, 0, 1, 1},
	{1, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 1, 1},
	{1, 1, 0, 1, 1, 0},
	{0, 1, 1, 0, 1, 1},
	{1, 1, 1, 1, 1, 0},
	{0, 1, 1, 1, 1, 1},
	{1, 0, 1, 1, 1, 1},
	{1, 1, 0, 1, 1, 1},
	{1 ,1, 1, 0, 1, 1},
	{1, 1, 1, 1, 0, 1}
};

StoneCOORD mansoon() {
	StoneCOORD rtr;
	int cnt = c.get_numStones();
	int board[BOARD_SIZE][BOARD_SIZE];
	//init gametable
	int init_stones, numOpps;
	init_stones = numOpps = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = c.showBoard(j, i);
			if (board[i][j] != 0) { 
				init_stones++; 
				if (board[i][j] == STONE_WHITE) numOpps++;
			}			

		}
	}

	
	if (cnt == 1) {
		if (init_stones == 0) {
			rtr.x1 = BOARD_SIZE / 2;
			rtr.y1 = BOARD_SIZE / 2;
		}
		else { //if there exist blockings, choose a relevance zone with the highest priority and close to the center.
			rtr = locate_center(board);
		}
	}
	else {
		//rtr = minimax(board, 4, 0, 0, true);
		if (numOpps == 1) {
			StoneCOORD temp1 = locate_center(board);
			board[temp1.y1][temp1.x1] = STONE_BLACK;
			StoneCOORD temp2 = locate_center(board);
			rtr.x1 = temp1.x1; 
			rtr.y1 = temp1.y1;
			rtr.x2 = temp2.x1;
			rtr.y2 = temp2.y1;
		}
		else {
			minimax(board, getRelevanceZone(board), DEPTH_LVL, -INF, +INF, true);
			rtr.x1 = 1; rtr.x2 = 2;
			rtr.y1 = 1; rtr.y2 = 2;
		}
	}

	return rtr;
}

StoneCOORD locate_center(int gameboard[BOARD_SIZE][BOARD_SIZE]) {
	StoneCOORD rtr;
	relevanceZone rz = getRelevanceZone(gameboard);
	vector <pair <int, int>> candidates;

	/*int priority = rz.pq.top().priority;
	candidates.push_back(make_pair(rz.pq.top().x, rz.pq.top().y)); rz.pq.pop();

	while (priority == rz.pq.top().priority) {
	candidates.push_back(make_pair(rz.pq.top().x, rz.pq.top().y));
	rz.pq.pop();
	}*/

	int idx = 1, zone_size = rz.combZone.size();
	int priority = rz.combZone[0].priority;

	candidates.push_back(make_pair(rz.combZone[0].x, rz.combZone[0].y));
	while (priority == rz.combZone[idx].priority) {
		candidates.push_back(make_pair(rz.combZone[idx].x, rz.combZone[idx].y));
		idx++;
		if (idx >= zone_size) break;
	}

	int size = candidates.size();
	int center, center_x, center_y;
	int centerCrd = BOARD_SIZE / 2;
	center = INF; center_x = center_y = 0;
	for (int i = 0; i < size; i++) {
		int value = 0;
		value += abs(candidates[i].first - centerCrd);
		value += abs(candidates[i].second - centerCrd);
		if (value < center) {
			center_x = candidates[i].first;
			center_y = candidates[i].second;
			center = value;
		}
	}

	rtr.x1 = center_x; rtr.y1 = center_y;
	return rtr;
}
int find_nearestStone(int board[BOARD_SIZE][BOARD_SIZE], int x, int y, int dir_x, int dir_y, int range) {

	for (int cnt = 0; cnt < range && x < BOARD_SIZE && y < BOARD_SIZE && x >= 0 && y >= 0; x += dir_x, y += dir_y) {
		if (board[y][x] != 0 && board[y][x] != STONE_BLOCK) return board[y][x];
	}
	return 0;
}
int setThreatZone(int board[BOARD_SIZE][BOARD_SIZE], relevanceZone *zone) {
	int threat_rows = sizeof(threatZone) / sizeof(threatZone[0]);
	int threat_cols = sizeof(threatZone[0]) / sizeof(threatZone[0][0]);
	bool flag = false;
	for (int y = 0; y < BOARD_SIZE; y++) {
		for (int x = 0; x < BOARD_SIZE; x++) {

			int stone_type;
			int cnt;

			//horizontal
			if (x <= BOARD_SIZE - threat_cols) {
				stone_type = board[y][x];
				if (!stone_type) {
					//if empty, find nearest stone within the range
					stone_type = find_nearestStone(board, x, y, 1, 0, threat_cols);
				}
				if (stone_type != 0) { //nearest stone does not exist, then skip.
					for (int k = 0; k < threat_rows; k++) {
						cnt = 0;

						vector <COORD> threat_pos;

						for (int i = 0; i < threat_cols; i++) {

							if (threatZone[k][i] == 1 && (board[y][x + i] == stone_type || board[y][x + i] == STONE_BLOCK)) {
								if (stone_type == STONE_BLOCK && board[y][x + i] != 0) stone_type = board[y][x + i];
								cnt++;
							}
							else if (threatZone[k][i] == 0 && board[y][x + i] == 0) {
								cnt++;
								COORD crd;
								crd.X = x + i; crd.Y = y;
								threat_pos.push_back(crd);
							}
							
							if (cnt < i) break;
						}
						//makes 4 counts a six
						bool deadzone_flag = false;
						if (x + 6 < BOARD_SIZE && (board[y][x + 6] == stone_type || board[y][x + 6] == STONE_BLOCK)) deadzone_flag = true;
						if (x - 1 >= 0 && (board[y][x - 1] == stone_type || board[y][x - 1] == STONE_BLOCK)) deadzone_flag = true;

						if (cnt == 6 && !deadzone_flag) {

							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < 0) zone->board[rtr.Y][rtr.X][stone_type] = THREAT; //restore
								else zone->board[rtr.Y][rtr.X][stone_type] += THREAT;
							}
							flag = true;
							break;
						}
						else if (cnt == 6 && deadzone_flag) {
							//deadzone
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < THREAT)
									zone->board[rtr.Y][rtr.X][stone_type] = -THREAT;
							}
						}
						else
							threat_pos.clear();
					}
				}
			}

			if (y <= BOARD_SIZE - threat_cols){
				//vertical
				stone_type = board[y][x];
				if (!stone_type || stone_type == STONE_BLOCK) {
					//if empty, find nearest stone within the range

					stone_type = find_nearestStone(board, x, y, 0, 1, threat_cols);
				}
				if (stone_type != 0){
					for (int k = 0; k < threat_rows; k++) {
						cnt = 0;

						vector <COORD> threat_pos;
						for (int i = 0; i < threat_cols; i++) {

							if (threatZone[k][i] == 1 && (board[y + i][x] == stone_type || board[y + i][x] == STONE_BLOCK)) {
								if (stone_type == STONE_BLOCK && board[y + i][x] != 0) stone_type = board[y + i][x];
								cnt++;
							}
							else if (threatZone[k][i] == 0 && board[y + i][x] == 0) {
								cnt++;
								COORD crd;
								crd.X = x; crd.Y = y + i;
								threat_pos.push_back(crd);
							}
							if (cnt < i) break;
						}
						bool deadzone_flag = false;
						if (y + 6 < BOARD_SIZE && (board[y + 6][x] == stone_type || board[y + 6][x] == STONE_BLOCK)) deadzone_flag = true;
						if (y - 1 >= 0 && (board[y - 1][x] == stone_type || board[y - 1][x] == STONE_BLOCK)) deadzone_flag = true;

						if (cnt == 6 && !deadzone_flag) {
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < 0) zone->board[rtr.Y][rtr.X][stone_type] = THREAT;
								else zone->board[rtr.Y][rtr.X][stone_type] += THREAT;
							}
							flag = true;
							break;
						}
						else if (cnt == 6 && deadzone_flag) {
							//deadzone
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < THREAT)
									zone->board[rtr.Y][rtr.X][stone_type] = -THREAT;
							}
						}
						else threat_pos.clear();
					}
				}
			}

			if (x <= BOARD_SIZE - 6 && y <= BOARD_SIZE - 6){
				//diagonal right 
				stone_type = board[y][x];
				if (!stone_type || stone_type == STONE_BLOCK) {
					//if empty, find nearest stone within the range

					stone_type = find_nearestStone(board, x, y, 1, 1, threat_cols);
				}
				if (stone_type != 0){
					for (int k = 0; k < threat_rows; k++) {
						cnt = 0;

						vector <COORD> threat_pos;
						for (int i = 0; i < threat_cols; i++) {

							if (threatZone[k][i] == 1 && (board[y + i][x + i] == stone_type || board[y + i][x + i] == STONE_BLOCK)) {
								if (stone_type == STONE_BLOCK && board[y + i][x + i] != 0) stone_type = board[y + i][x + i];
								cnt++;
							}
							else if (threatZone[k][i] == 0 && board[y + i][x + i] == 0) {
								cnt++;
								COORD crd;
								crd.X = x + i; crd.Y = y + i;
								threat_pos.push_back(crd);
							}
							if (cnt < i) break;
						}

						bool deadzone_flag = false;
						if (y + 6 < BOARD_SIZE && x + 6 < BOARD_SIZE && (board[y + 6][x + 6] == stone_type || board[y + 6][x + 6] == STONE_BLOCK)) deadzone_flag = true;
						if (y - 1 >= 0 && x - 1 >= 0 && (board[y - 1][x - 1] == stone_type || board[y - 1][x - 1] == STONE_BLOCK)) deadzone_flag = true;

						if (cnt == 6 && !deadzone_flag) {
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < 0) zone->board[rtr.Y][rtr.X][stone_type] = THREAT;
								else zone->board[rtr.Y][rtr.X][stone_type] += THREAT;
							}
							flag = true;
							break;
						}
						else if (cnt == 6 && deadzone_flag) {
							//deadzone
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < THREAT)
									zone->board[rtr.Y][rtr.X][stone_type] = -THREAT;
							}
						}
						else threat_pos.clear();
					}
				}

			}

			if (y >= 5 && x <= BOARD_SIZE - 6) {
				//diagonal left
				stone_type = board[y][x];
				if (!stone_type || stone_type == STONE_BLOCK) {
					//if empty, find nearest stone within the range

					stone_type = find_nearestStone(board, x, y, 1, -1, threat_cols);
				}
				if (stone_type != 0) {
					for (int k = 0; k < threat_rows; k++) {
						cnt = 0;

						vector <COORD> threat_pos;
						for (int i = 0; i < threat_cols; i++) {

							if (threatZone[k][i] == 1 && (board[y - i][x + i] == stone_type || board[y - i][x + i] == STONE_BLOCK)) {
								if (stone_type == STONE_BLOCK && board[y - i][x + i] != 0) stone_type = board[y - i][x + i];
								cnt++;
							}
							else if (threatZone[k][i] == 0 && board[y - i][x + i] == 0) {
								cnt++;
								COORD crd;
								crd.X = x + i; crd.Y = y - i;
								threat_pos.push_back(crd);
							}
							if (cnt < i) break;
						}
						bool deadzone_flag = false;
						if (y - 6 >= 0 && x + 6 < BOARD_SIZE && (board[y - 6][x + 6] == stone_type || board[y - 6][x + 6] == STONE_BLOCK)) deadzone_flag = true;
						if (y + 1 < BOARD_SIZE && x - 1 >= 0 && (board[y + 1][x - 1] == stone_type || board[y + 1][x - 1] == STONE_BLOCK)) deadzone_flag = true;

						if (cnt == 6 && !deadzone_flag) {
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < 0) zone->board[rtr.Y][rtr.X][stone_type] = THREAT;
								else zone->board[rtr.Y][rtr.X][stone_type] += THREAT;
							}
							flag = true;
							break;
						}
						else if (cnt == 6 && deadzone_flag) {
							//deadzone
							while (!threat_pos.empty()) {
								COORD rtr = threat_pos.back(); threat_pos.pop_back();
								if (zone->board[rtr.Y][rtr.X][stone_type] < THREAT)
									zone->board[rtr.Y][rtr.X][stone_type] = -THREAT;
							}
						}
						else threat_pos.clear();
					}

				}
			}

		}
	}

	if (flag) {
		for (int i = 0; i < BOARD_SIZE; i++) {
			for (int j = 0; j < BOARD_SIZE; j++) {
				coordInfo crd;
				if (zone->board[i][j][STONE_BLACK] >= THREAT) {
					crd.x = j; crd.y = i; crd.priority = zone->board[i][j][STONE_BLACK];
					zone->myZone.push_back(crd);
				}
				else if (zone->board[i][j][STONE_BLACK] < 0) {
					crd.x = j; crd.y = i; crd.priority = zone->board[i][j][STONE_BLACK];
					zone->myDeadZone.push_back(crd);
				}
				else if (zone->board[i][j][STONE_WHITE] >= THREAT) {
					crd.x = j; crd.y = i; crd.priority = zone->board[i][j][STONE_WHITE];
					zone->oppZone.push_back(crd);
				}
				else if (zone->board[i][j][STONE_WHITE] < 0) {
					crd.x = j; crd.y = i; crd.priority = zone->board[i][j][STONE_WHITE];
					zone->oppDeadZone.push_back(crd);
				}
			}
		}
		return 1;
	}
	else return 0;
}
relevanceZone getRelevanceZone(int gBoard[BOARD_SIZE][BOARD_SIZE]) {
	//the function searches relevance area and threat zones where four connections are made
	relevanceZone z;
	memset(z.board, 0, sizeof(z.board));
	int zone_length = 3;
	int temp_board[BOARD_SIZE][BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			temp_board[i][j] = gBoard[i][j];


	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (gBoard[i][j] != 0) { 
				//this is to include blockings into the zone calculation
				//for the case where an optimal pos is asked to be searched in the beginning with existence of blockings
 				int cur_stone = gBoard[i][j]; //black stone = my stone 
				int x, y; x = j; y = i; int priority;
				bool diff_flag1, diff_flag2;
				vector<coordInfo> crd_1, crd_2;
				//draw relevancezone for each stone
				//horizontal

				priority = zone_length;
				diff_flag1 = diff_flag2 = false;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2;
					cur_x1 = x + cnt; cur_x2 = x - cnt;
					if (cur_x1 >= 0 && cur_x1 < BOARD_SIZE ) {
						if (gBoard[y][cur_x1] == 0 && !diff_flag1) //z.board[y][cur_x1][cur_stone] += priority;
							crd_1.push_back({ cur_x1, y, cur_stone, priority });
						else {
							int copy_x = cur_x1;
							int temp_stone; //to allow blocking to be in relevant zone calculations.
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x + 1 < BOARD_SIZE && (gBoard[y][copy_x] == temp_stone || gBoard[y][copy_x] == STONE_BLOCK) ; copy_x++);
							if (gBoard[y][copy_x] == 0 && !diff_flag1) {
								/*z.board[y][copy_x][temp_stone] += priority;
								coordInfo info;*/
								crd_1.push_back({ copy_x, y, temp_stone, priority });
							}
							else diff_flag1 = true;
							
						}
					}
					

					if (cur_x2 >= 0 && cur_x2 < BOARD_SIZE) {
						if (gBoard[y][cur_x2] == 0 && !diff_flag2) //z.board[y][cur_x2][cur_stone] += priority;
							crd_2.push_back({ cur_x2, y, cur_stone, priority });
						else {
							int copy_x = cur_x2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1 >= 0 && (gBoard[y][copy_x] == temp_stone|| gBoard[y][copy_x] == STONE_BLOCK); copy_x--);
							if (gBoard[y][copy_x] == 0 && !diff_flag2) crd_2.push_back({ copy_x, y, temp_stone, priority });
							else diff_flag2 = true;
						}
						
					}
					if (diff_flag1 && diff_flag2) break;
					priority--;
				}
				//if there exist no opp stones in the way, update relevant zone
				if (!diff_flag1) {
					while (!crd_1.empty()) {
						coordInfo info = crd_1.back(); crd_1.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_1.clear();

				if (!diff_flag2) {
					while (!crd_2.empty()) {
						coordInfo info = crd_2.back(); crd_2.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_2.clear();


				//vertical
				priority = zone_length;
				diff_flag1 = diff_flag2 = false;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_y1, cur_y2;
					cur_y1 = y + cnt; cur_y2 = y - cnt;
					if (cur_y1 >= 0 && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][x] == 0 && !diff_flag1) //z.board[cur_y1][x][cur_stone] += priority; 
							crd_1.push_back({ x, cur_y1, cur_stone, priority });
						else {
							int copy_y = cur_y1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][x] == temp_stone || gBoard[copy_y][x] == STONE_BLOCK); copy_y++);
							if (gBoard[copy_y][x] == 0 && !diff_flag1) //z.board[copy_y][x][temp_stone] += priority;
								crd_1.push_back({ x, copy_y, temp_stone, priority });
							else diff_flag1 = true;
						}

					}
					if (cur_y2 >= 0 && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y2][x] == 0 && !diff_flag2) //z.board[cur_y2][x][cur_stone] += priority;
							crd_2.push_back({ x, cur_y2, cur_stone, priority });
						else {
							int copy_y = cur_y2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y - 1 >= 0 && (gBoard[copy_y][x] == temp_stone || gBoard[copy_y][x] == STONE_BLOCK); copy_y--);
							if (gBoard[copy_y][x] == 0 && !diff_flag2) //z.board[copy_y][x][temp_stone] += priority;
								crd_2.push_back({ x, copy_y, temp_stone, priority });
							else diff_flag2 = true;
						}
		
					}
					if (diff_flag1 && diff_flag2) break;
					priority--;
				}
				//if there exist no opp stones in the way, update relevant zone
				if (!diff_flag1) {
					while (!crd_1.empty()) {
						coordInfo info = crd_1.back(); crd_1.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_1.clear();

				if (!diff_flag2) {
					while (!crd_2.empty()) {
						coordInfo info = crd_2.back(); crd_2.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_2.clear();


				//diagonal right down
				priority = zone_length;
				diff_flag1 = diff_flag2 = false;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2, cur_y1, cur_y2;
					cur_x1 = x + cnt; cur_x2 = x - cnt;
					cur_y1 = y + cnt; cur_y2 = y - cnt;

					if (cur_x1 >= 0 && cur_y1 >= 0 && cur_x1 < BOARD_SIZE && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][cur_x1] == 0 && !diff_flag1) //z.board[cur_y1][cur_x1][cur_stone] += priority;
							crd_1.push_back({ cur_x1, cur_y1, cur_stone, priority });
						else {
							int copy_x, copy_y;
							copy_x = cur_x1; copy_y = cur_y1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x + 1 < BOARD_SIZE && copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK);
								copy_x++, copy_y++);
							if (gBoard[copy_y][copy_x] == 0 && !diff_flag1) //z.board[copy_y][copy_x][temp_stone] += priority;
								crd_1.push_back({ copy_x, copy_y, temp_stone, priority });
							else diff_flag1 = true;
						}
						
					}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE) {
						if (gBoard[cur_y2][cur_x2] == 0 && !diff_flag2) //z.board[cur_y2][cur_x2][cur_stone] += priority;
							crd_2.push_back({ cur_x2, cur_y2, cur_stone, priority });
						else {
							int copy_x, copy_y;
							copy_x = cur_x2; copy_y = cur_y2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1>= 0 && copy_y - 1 >= 0 && ((gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK));
								copy_y--, copy_x--);
							if (gBoard[copy_y][copy_x] == 0 && !diff_flag2) //z.board[copy_y][copy_x][temp_stone] += priority;
								crd_2.push_back({ copy_x, copy_y, temp_stone, priority });
							else diff_flag2 = true;
						}
						
					}
					if (diff_flag1 && diff_flag2) break;
					priority--;
				}
				//if there exist no opp stones in the way, update relevant zone
				if (!diff_flag1) {
					while (!crd_1.empty()) {
						coordInfo info = crd_1.back(); crd_1.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_1.clear();

				if (!diff_flag2) {
					while (!crd_2.empty()) {
						coordInfo info = crd_2.back(); crd_2.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_2.clear();


				//diagonal left down
				priority = zone_length;
				diff_flag1 = diff_flag2 = false;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2, cur_y1, cur_y2;
					cur_x1 = x - cnt; cur_x2 = x + cnt;
					cur_y1 = y + cnt; cur_y2 = y - cnt;
					if (cur_x1 >= 0 && cur_y1 >= 0 && cur_x1 < BOARD_SIZE && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][cur_x1] == 0 && !diff_flag1) {
							//z.board[cur_y1][cur_x1][cur_stone] += priority;
							crd_1.push_back({ cur_x1, cur_y1, cur_stone, priority });
						}
						else {
							int copy_x, copy_y;
							copy_x = cur_x1; copy_y = cur_y1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1 >= 0 && copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK); 
								copy_x--, copy_y++, cnt++);
							if (gBoard[copy_y][copy_x] == 0 && !diff_flag1) //z.board[copy_y][copy_x][temp_stone] += priority;
								crd_1.push_back({ copy_x, copy_y, temp_stone, priority });
							else diff_flag1 = true;
						}
											}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE) {
						if (gBoard[cur_y2][cur_x2] == 0 && !diff_flag2)
							//z.board[cur_y2][cur_x2][cur_stone] += priority;
							crd_2.push_back({ cur_x2, cur_y2, cur_stone, priority });
						else {
							int copy_y, copy_x;
							copy_y = cur_y2; copy_x = cur_x2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y - 1 >= 0 && copy_x + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK); 
								copy_y--, copy_x++);
							if (gBoard[copy_y][copy_x] == 0 && !diff_flag2) //z.board[copy_y][copy_x][temp_stone] += priority;
								crd_2.push_back({ copy_x, copy_y, temp_stone, priority });
							else diff_flag2 = true;
						}
					}
					if (diff_flag1 && diff_flag2) break;
					priority--;
				}
				//if there exist no opp stones in the way, update relevant zone
				if (!diff_flag1) {
					while (!crd_1.empty()) {
						coordInfo info = crd_1.back(); crd_1.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_1.clear();

				if (!diff_flag2) {
					while (!crd_2.empty()) {
						coordInfo info = crd_2.back(); crd_2.pop_back();
						z.board[info.y][info.x][info.stone_type] += info.priority;
					}
				}
				else crd_2.clear();
			}
		}
	}
	setThreatZone(gBoard, &z);
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (z.board[i][j][STONE_BLACK] + z.board[i][j][STONE_WHITE] + z.board[i][j][STONE_BLOCK] > 0) {
				coordInfo info;
				info.y = i; info.x = j; 
				info.priority = z.board[i][j][STONE_BLACK] + z.board[i][j][STONE_WHITE] + z.board[i][j][STONE_BLOCK];
				z.combZone.push_back(info);
//				z.pq.push(info);
				info.priority = z.board[i][j][STONE_BLACK] + z.board[i][j][STONE_BLOCK];
				z.myZone.push_back(info);
//				z.myP.push(info);
				info.priority = z.board[i][j][STONE_WHITE] + z.board[i][j][STONE_BLOCK];
				z.oppZone.push_back(info);
//				z.oppP.push(info);
			}
		}
	}

	std::sort(z.combZone.begin(), z.combZone.end(), cmpPriority);
	std::sort(z.myZone.begin(), z.myZone.end(), cmpPriority);
	std::sort(z.oppZone.begin(), z.oppZone.end(), cmpPriority);
	return z;
}

int evaluate_board(int gameboard[BOARD_SIZE][BOARD_SIZE]) {
	//higher relevanze zone weights usually represent more optimal solution
	relevanceZone threats;
	if (setThreatZone(gameboard, &threats)) { //if there exist threats
		if (threats.myZone.size() > 0) return 1000;
		else if (threats.oppZone.size() > 0) return -1000;
		else return -1; //error
	}
	
	relevanceZone relevants;
	int evals = 0;
	relevants = getRelevanceZone(gameboard);
	int size = relevants.myZone.size();
	for (int i = 0; i < size; i++) {
		evals += (relevants.myZone[i].priority - relevants.oppZone[i].priority);
	}
	eval_called++;
	return evals;
}
int minimax(int gameboard[BOARD_SIZE][BOARD_SIZE], relevanceZone zone, int depth, int alpha, int beta, bool max) {
	//1 represents mine, 2 represennts opponents
	//first find relevance-zone connect 6 range
	//start search + alpha beta		
	return 0;
	if (depth == 0) {
		//return evaluated score
		return evaluate_board(gameboard);
	}

	if (max) {
		int maxEval = -INF;
		int zone_size = zone.myZone.size();
		relevanceZone threats;
		setThreatZone(gameboard, &threats);
		int myThreatSize = threats.myZone.size();

		//finish moves
		if (myThreatSize > 0) {
			//winable situation
			if (depth == DEPTH_LVL) {
				if (myThreatSize > 2) {
					//delete unncessary threats
					for (int i = 0; i < myThreatSize; i++) {
						if (threats.myZone[i].priority < 200) threats.myZone.erase(threats.myZone.begin() + i);
					}
					
					if (threats.myZone.size() == 2) {
						stone_pos.x1 = threats.myZone[0].x;
						stone_pos.y1 = threats.myZone[0].y;
						stone_pos.x2 = threats.myZone[1].x;
						stone_pos.y2 = threats.myZone[1].y;
					}
					else {
						// if multiple answers exist
						// find a matching pair with the first
						int remain_size = threats.myZone.size();
						int matched_pair = 0;
						for (int matched_pair = 1; matched_pair < remain_size; matched_pair++) {
							if (threats.myZone[0].x == threats.myZone[matched_pair].x) break;
							else if (threats.myZone[0].y == threats.myZone[matched_pair].y) break;
							else if (abs(threats.myZone[0].y - threats.myZone[matched_pair].y) == 5 && abs(threats.myZone[0].x - threats.myZone[matched_pair].x) == 5) break;
						}

						stone_pos.x1 = threats.myZone[0].x;
						stone_pos.y1 = threats.myZone[0].y;
						stone_pos.x2 = threats.myZone[matched_pair].x;
						stone_pos.y2 = threats.myZone[matched_pair].y;
					}
				}
				else if (myThreatSize == 2) {
					stone_pos.x1 = threats.myZone[0].x;
					stone_pos.y1 = threats.myZone[0].y;
					stone_pos.x2 = threats.myZone[1].x;
					stone_pos.y2 = threats.myZone[1].y;
				}
				else if (myThreatSize == 1) {
					int rand_y, rand_x;
					stone_pos.x1 = threats.myZone[0].x;
					stone_pos.y1 = threats.myZone[0].y;
					srand(time(NULL));
					do {
						rand_y = rand() % BOARD_SIZE;
						rand_x = rand() % BOARD_SIZE;
					} while (c.isFree(rand_y, rand_x));
					stone_pos.x2 = rand_x;
					stone_pos.y2 = rand_y;
					
				}
				return 0;
			}
			else return 1000; //winable situation, no needed to be searched further
		} //yo
		//priortize defending threats and must not be visited; there must exist a condition check for below so that the blocked area wont be visited
		int oppThreatSize = threats.oppZone.size();
		std::sort(zone.oppZone.begin(), zone.oppZone.end(), cmpPriority);
		if (oppThreatSize == 4) {
			int max_eval = 0;
			//assume the first indexes are double threats and last two are single threat
			for (int i = 0; i < 2; i++) {
				for (int j = i + 1; j < 4; j++) {
					if (abs(threats.oppZone[i].x - threats.oppZone[j].x) + abs(threats.oppZone[i].y - threats.oppZone[j].y) > 2) {
						gameboard[threats.oppZone[i].y][threats.oppZone[i].x] = 1;
						gameboard[threats.oppZone[j].y][threats.oppZone[j].y] = 1;
						int eval = minimax(gameboard, zone, depth - 1, alpha, beta, false);
						if (eval > max_eval) {
							if (depth == DEPTH_LVL) {
								stone_pos.x1 = threats.oppZone[i].x;
								stone_pos.y1 = threats.oppZone[i].y;
								stone_pos.x2 = threats.oppZone[j].x;
								stone_pos.y2 = threats.oppZone[j].y;
							}
							max_eval = eval;
						}
						gameboard[threats.oppZone[i].y][threats.oppZone[i].x] = 0;
						gameboard[threats.oppZone[j].y][threats.oppZone[j].y] = 0;
					}
				}
			}
			return 0;
		}
		else if (oppThreatSize == 3) {
			int max_eval = 0;
			gameboard[threats.oppZone[0].y][threats.oppZone[1].y] = 1;
			for (int i = 1; i < 3; i++) {
				gameboard[threats.oppZone[i].y][threats.oppZone[i].x] = 1;
				int eval = minimax(gameboard, zone, depth - 1, alpha, beta, false);
				if (eval > max_eval) {
					if (depth == DEPTH_LVL) {
						stone_pos.x1 = threats.oppZone[0].x;
						stone_pos.y1 = threats.oppZone[0].y;
						stone_pos.x2 = threats.oppZone[i].x;
						stone_pos.y2 = threats.oppZone[i].y;
					}
					max_eval = eval;
				}
				gameboard[threats.oppZone[i].y][threats.oppZone[i].x] = 0;
			}
			gameboard[threats.oppZone[0].y][threats.oppZone[1].y] = 0;
			return 0;
		}
		else if (oppThreatSize == 2) {
			coordInfo first_crd = threats.oppZone[0];
			coordInfo second_crd = threats.oppZone[1];
			gameboard[first_crd.y][first_crd.x] = 1;
			gameboard[second_crd.y][second_crd.x] = 1;
			if (depth == DEPTH_LVL) {
				stone_pos.x1 = threats.oppZone[0].x;
				stone_pos.y1 = threats.oppZone[0].y;
				stone_pos.x2 = threats.oppZone[1].x;
				stone_pos.y2 = threats.oppZone[1].y;
				minimax(gameboard, zone, depth - 1, alpha, beta, false);
				return 0;
			}
			return minimax(gameboard, zone, depth - 1, alpha, beta, false);

		}
		else if (oppThreatSize == 1) {
			coordInfo first_crd = threats.oppZone[0];
			gameboard[first_crd.y][first_crd.x] = 1;
			if (depth == DEPTH_LVL) {
				stone_pos.x1 = threats.oppZone[0].x;
				stone_pos.y1 = threats.oppZone[0].y;
			}
			int eval = 0;
			for (int i = 0; i < zone_size; i++) {

			}
			
		}

		for (int i = 0; i < zone_size - 1; i++) {
			for (int j = i + 1; j < zone_size; j++) {
				//set priority of selected positions to zero so that they won't be used again in child nodes.
				if (zone.myZone[i].priority == 0 || zone.myZone[j].priority == 0) continue;

				int first_x, first_y, second_x, second_y;
				first_x = zone.myZone[i].x; first_y = zone.myZone[i].y;
				second_x = zone.myZone[j].x; second_y= zone.myZone[j].x;
				zone.myZone[i].priority = zone.myZone[j].priority = 0;

				gameboard[first_y][first_x] = STONE_BLACK;
				gameboard[second_y][second_x] = STONE_BLACK;
				int eval = minimax(gameboard, zone, depth - 1, alpha, beta, false);
				zone.myZone[i].priority = zone.myZone[j].priority = 1; 
				//the priority does not need to be restored as the list is already sorted. just need to be reminded as available.
				gameboard[first_y][first_x] = gameboard[second_y][second_x] = 0;

				if (eval > maxEval && depth == DEPTH_LVL) { //remember the position of stone
					maxEval = eval;
					stone_pos.x1 = first_x;
					stone_pos.x2 = second_x;
					stone_pos.y1 = first_y;
					stone_pos.y2 = second_y;
				}

			}
		}

		return maxEval;
	}
	else {
		int minEval = INF;
		//location
		//try to think search zone for opp or mine first
		
		return minEval;
	}

	return 1;
}

int longest_connect(int gameboard[BOARD_SIZE][BOARD_SIZE], int last_x, int last_y) {
	return 0;
}


StoneCOORD random_ai() {
	int y, x;
	StoneCOORD rtr;
	rtr.stones = c.get_numStones();

	for (int i = 0; i < rtr.stones; i++) {
		do {
			y = rand() % BOARD_SIZE;
			x = rand() % BOARD_SIZE;
		} while (!c.isFree(y, x));
		if (i == 0) {
			rtr.x1 = x; rtr.y1 = y;
		}
		else {
			rtr.x2 = x; rtr.y2 = y;
		}
	}
	return rtr;
}

StoneCOORD random_ai_highest_relevant() {
	int gameboard[BOARD_SIZE][BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++)
		for (int j = 0; j < BOARD_SIZE; j++)
			gameboard[i][j] = c.showBoard(j, i);

	relevanceZone zone = getRelevanceZone(gameboard);
	StoneCOORD rtr;
	int zone_size = zone.myZone.size();
	int nums = c.get_numStones();
	if (nums == 1) {
		if (zone_size == 0) {
			rtr.x1 = rtr.y1 = 9;
		}
		else {
			rtr.x1 = zone.myZone[0].x;
			rtr.y1 = zone.myZone[0].y;
		}
		return rtr;
	}
	if (zone_size >= 2) {
		rtr.x1 = zone.myZone[0].x;
		rtr.y1 = zone.myZone[0].y;
		rtr.x2 = zone.myZone[1].x;
		rtr.y2 = zone.myZone[1].y;
	}
	else if (zone_size == 1) {
		rtr.x1 = zone.myZone[0].x;
		rtr.y1 = zone.myZone[0].y;
		int rnd_x, rnd_y;
		srand(time(NULL));
		do {
			rnd_x = rand() % BOARD_SIZE;
			rnd_y = rand() % BOARD_SIZE;
		} while (!c.isFree(rnd_y, rnd_x));
		rtr.x2 = rnd_x;
		rtr.y1 = rnd_y;
	}
	else {
		rtr.stones = c.get_numStones();
		int y, x;
		for (int i = 0; i < rtr.stones; i++) {
			do {
				y = rand() % BOARD_SIZE;
				x = rand() % BOARD_SIZE;
			} while (!c.isFree(y, x));
			if (i == 0) {
				rtr.x1 = x; rtr.y1 = y;
			}
			else {
				rtr.x2 = x; rtr.y2 = y;
			}
		}
	}
	return rtr;
}
StoneCOORD random_ai_relevantzone() {

	int gameboard[BOARD_SIZE][BOARD_SIZE];
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++)
			gameboard[i][j] = c.showBoard(j, i);
	}
	relevanceZone zone = getRelevanceZone(gameboard);
	StoneCOORD rtr;
	srand(time(NULL));
	int zone_size = zone.myZone.size();
	if (zone_size >= 2) {
		int rand_1 = rand() % zone_size;
		int rand_2;
		do {
			rand_2 = rand() % zone_size;
		} while (rand_1 == rand_2);
		
		rtr.x1 = zone.myZone[rand_1].x;
		rtr.y1 = zone.myZone[rand_1].y;
		rtr.x2 = zone.myZone[rand_2].x;
		rtr.y2 = zone.myZone[rand_2].y;
	}
	else if (zone_size == 1) {
		rtr.x1 = zone.myZone[0].x;
		rtr.y1 = zone.myZone[0].y;
		int rnd_x, rnd_y;
		do {
			rnd_x = rand() % BOARD_SIZE;
			rnd_y = rand() % BOARD_SIZE;
		} while (!c.isFree(rnd_y, rnd_x));
		rtr.x2 = rnd_x;
		rtr.y2 = rnd_y;
	}
	else {
	
		rtr.stones = c.get_numStones();
		int y, x;
		for (int i = 0; i < rtr.stones; i++) {
			do {
				y = rand() % BOARD_SIZE;
				x = rand() % BOARD_SIZE;
			} while (!c.isFree(y, x));
			if (i == 0) {
				rtr.x1 = x; rtr.y1 = y;
			}
			else {
				rtr.x2 = x; rtr.y2 = y;
			}
		}
	}
	return rtr;
}
int simple_minimax(int gameboard[BOARD_SIZE][BOARD_SIZE], relevanceZone zone,  int depth, int alpha, int beta, bool max) {
	if (depth == 0) {
		return evaluate_board(gameboard);
	}

	if (max) {
		int maxEval = -INF;
		int zone_size = zone.oppZone.size();
		for (int i = 0; i < zone_size - 1; i++) {
			for (int j = 1; j < zone_size; j++) {
				coordInfo first_info = zone.myZone[i];
				coordInfo second_info = zone.myZone[j];
				if (first_info.priority < 3) continue;
				if (second_info.priority < 3) continue;
				if (i == j) continue;
				
				gameboard[first_info.y][first_info.x] = 1;
				gameboard[second_info.y][second_info.x] = 1;
				zone.myZone[i].priority = zone.myZone[j].priority = 0;

				int eval = simple_minimax(gameboard, zone, depth - 1, alpha, beta, false);
				
				gameboard[first_info.y][first_info.x] = 0;
				gameboard[second_info.y][second_info.x] = 0;
				zone.myZone[i].priority = zone.myZone[j].priority = 3;
				if (eval > maxEval) {
					if (depth == 2) {
						simple_stone_pos.x1 = first_info.x;
						simple_stone_pos.y1 = first_info.y;
						simple_stone_pos.x2 = second_info.x;
						simple_stone_pos.y2 = second_info.y;
					}
				}
				if (eval > alpha) { 
					alpha = eval; 
				}
				if (beta <= alpha) break;

				
				if (j > 5) break;

			}
			if (i > 5) break;
		}
		return maxEval;
	}
	else {
		int minEval = INF;
		int zone_size = zone.oppZone.size();
		for (int i = 0; i < zone_size - 1; i++) {
			for (int j = 1; j < zone_size; j++) {
				coordInfo first_info = zone.oppZone[i];
				coordInfo second_info = zone.oppZone[j];
				if (first_info.priority < 3) continue;
				if (second_info.priority < 3) continue;

				gameboard[first_info.y][first_info.x] = 1;
				gameboard[second_info.y][second_info.x] = 1;
				zone.oppZone[i].priority = zone.oppZone[j].priority = 0;

				int eval = simple_minimax(gameboard, zone, depth - 1, alpha, beta, true);

				gameboard[first_info.y][first_info.x] = gameboard[second_info.y][second_info.x] = 0;
				zone.oppZone[i].priority = zone.oppZone[j].priority = 3;
				if (eval < minEval) minEval = eval;
				if (eval < beta) beta = eval;
				if (beta <= alpha) break;

				if (j > 5) break;
			}
			if (i > 5) break;
		}
		return minEval;
	}
}
StoneCOORD dumb_minimax() {
	int gameboard[BOARD_SIZE][BOARD_SIZE];
	int initStone, initOpps;
	eval_called = 0;
	initStone = initOpps = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			gameboard[i][j] = c.showBoard(j, i);
			if (gameboard[i][j] != 0) initStone++;
			if (gameboard[i][j] == 2) initOpps++;
		}
	}

	StoneCOORD rtr;

	if (initStone == 0) {
		rtr.x1 = BOARD_SIZE / 2;
		rtr.y1 = BOARD_SIZE / 2;
	}
	else if (initOpps == 0) {
		rtr = locate_center(gameboard);
	}
	else if (initOpps == 1) {
		StoneCOORD temp1, temp2;
		temp1 = locate_center(gameboard);
		gameboard[temp1.y1][temp1.x1] = 1;
		temp2 = locate_center(gameboard);
		rtr.x1 = temp1.x1;
		rtr.y1 = temp1.y1;
		rtr.x2 = temp2.x1;
		rtr.y2 = temp2.y1;
	}
	else {
		simple_minimax(gameboard, getRelevanceZone(gameboard),2, -INF, INF, true);
		rtr.x1 = simple_stone_pos.x1;
		rtr.y1 = simple_stone_pos.y1;
		rtr.x2 = simple_stone_pos.x2;
		rtr.y2 = simple_stone_pos.y2;
	}

	return rtr;
}