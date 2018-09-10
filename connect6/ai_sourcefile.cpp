#include "ai_sourcefile.h"
#include <vector>
#include <math.h>

StoneCOORD stone_pos;
bool cmpPriority(const coordInfo &a, const coordInfo &b) {
	return a.priority > b.priority;
}
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
				int x, y; x = j; y = i;
				
				//draw relevancezone for each stone
				//horizontal
				int priority = zone_length;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2;
					cur_x1 = x + cnt; cur_x2 = x - cnt;
					if (cur_x1 >= 0 && cur_x1 < BOARD_SIZE ) {
						if (gBoard[y][cur_x1] == 0) z.board[y][cur_x1][cur_stone] += priority;
						else {
							int copy_x = cur_x1, cnt = 1;
							int temp_stone; //to allow blocking to be in relevant zone calculations.
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x + 1 < BOARD_SIZE && (gBoard[y][copy_x] == temp_stone || gBoard[y][copy_x] == 3) ; copy_x++, cnt++);
							if (gBoard[y][copy_x] == 0) z.board[y][copy_x][temp_stone] += priority;

							//threat zone search
							if (cnt >= 4 && gBoard[y][copy_x] == 0) {
									z.board[y][copy_x][temp_stone] += THREAT;
								
							}
						}
					}
					

					if (cur_x2 >= 0 && cur_x2 < BOARD_SIZE) {
						if (gBoard[y][cur_x2] == 0) z.board[y][cur_x2][cur_stone] += priority;
						else {
							int copy_x = cur_x2, cnt = 1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1 >= 0 && (gBoard[y][copy_x] == temp_stone|| gBoard[y][copy_x] == STONE_BLOCK); copy_x--, cnt++);
							if (gBoard[y][copy_x] == 0) z.board[y][copy_x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[y][copy_x] == 0) {

									z.board[y][copy_x][temp_stone] += THREAT;

							}
						}
						
					}
					
					priority--;
				}

				//vertical
				priority = zone_length;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_y1, cur_y2;
					cur_y1 = y + cnt; cur_y2 = y - cnt;
					if (cur_y1 >= 0 && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][x] == 0) z.board[cur_y1][x][cur_stone] += priority;
						else {
							int copy_y = cur_y1, cnt = 1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][x] == temp_stone || gBoard[copy_y][x] == STONE_BLOCK); copy_y++, cnt++);
							if (gBoard[copy_y][x] == 0) z.board[copy_y][x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][x] == 0) z.board[copy_y][x][temp_stone] += THREAT;
						}

					}
					if (cur_y2 >= 0 && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y2][x] == 0) z.board[cur_y2][x][cur_stone] += priority;
						else {
							int copy_y = cur_y2, cnt = 1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y - 1 >= 0 && (gBoard[copy_y][x] == temp_stone || gBoard[copy_y][x] == STONE_BLOCK); copy_y--, cnt++);
							if (gBoard[copy_y][x] == 0) z.board[copy_y][x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][x] == 0) z.board[copy_y][x][temp_stone] += THREAT;
						}
		
					}

					priority--;
				}
				//diagonal right down
				priority = zone_length;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2, cur_y1, cur_y2;
					cur_x1 = x + cnt; cur_x2 = x - cnt;
					cur_y1 = y + cnt; cur_y2 = y - cnt;

					if (cur_x1 >= 0 && cur_y1 >= 0 && cur_x1 < BOARD_SIZE && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][cur_x1] == 0) z.board[cur_y1][cur_x1][cur_stone] += priority;
						else {
							int copy_x, copy_y, cnt = 1;
							copy_x = cur_x1; copy_y = cur_y1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x + 1 < BOARD_SIZE && copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK);
								copy_x++, copy_y++, cnt++);
							if (gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += THREAT;
						}
						
					}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE) {
						if (gBoard[cur_y2][cur_x2] == 0) z.board[cur_y2][cur_x2][cur_stone] += priority;
						else {
							int copy_x, copy_y, cnt = 1;
							copy_x = cur_x2; copy_y = cur_y2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1>= 0 && copy_y - 1 >= 0 && ((gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK));
								copy_y--, copy_x--, cnt++);
							if (gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += THREAT;
						}
						
					}

					priority--;
				}

				//diagonal left down
				priority = zone_length;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2, cur_y1, cur_y2;
					cur_x1 = x - cnt; cur_x2 = x + cnt;
					cur_y1 = y + cnt; cur_y2 = y - cnt;
					if (cur_x1 >= 0 && cur_y1 >= 0 && cur_x1 < BOARD_SIZE && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y1][cur_x1] == 0) {
							z.board[cur_y1][cur_x1][cur_stone] += priority;
						}
						else {
							int copy_x, copy_y, cnt = 1;
							copy_x = cur_x1; copy_y = cur_y1;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][copy_x];
							else temp_stone = cur_stone;

							for (; copy_x - 1 >= 0 && copy_y + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK); 
								copy_x--, copy_y++, cnt++);
							if (gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += THREAT;
						}
											}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE) {
						if (gBoard[cur_y2][cur_x2] == 0)
							z.board[cur_y2][cur_x2][cur_stone] += priority;
						else {
							int copy_y, copy_x, cnt = 1;
							copy_y = cur_y2; copy_x = cur_x2;
							int temp_stone;
							if (cur_stone == STONE_BLOCK) temp_stone = gBoard[copy_y][x];
							else temp_stone = cur_stone;

							for (; copy_y - 1 >= 0 && copy_x + 1 < BOARD_SIZE && (gBoard[copy_y][copy_x] == temp_stone || gBoard[copy_y][copy_x] == STONE_BLOCK); 
								copy_y--, copy_x++, cnt++);
							if (gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += priority;
							if (cnt >= 4 && gBoard[copy_y][copy_x] == 0) z.board[copy_y][copy_x][temp_stone] += THREAT;
						}
					}

					priority--;
				}
			}
		}
	}
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

int evaluate_board(int gBoard[BOARD_SIZE][BOARD_SIZE]) {
	return 0;
}
int minimax(int gameboard[BOARD_SIZE][BOARD_SIZE], relevanceZone zone, int depth, int alpha, int beta, bool max) {
	//1 represents mine, 2 represennts opponents
	//first find relevance-zone connect 6 range
	//start search + alpha beta		
	return 0;
	if (depth == 0) {
		return evaluate_board(gameboard);
	}

	if (max) {
		int maxEval = -INF;
		int zone_size = zone.myZone.size();
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