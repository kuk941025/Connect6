#include "ai_sourcefile.h"
#include <vector>
#include <math.h>

StoneCOORD mansoon() {
	StoneCOORD rtr;
	int cnt = c.get_numStones();
	int board[BOARD_SIZE][BOARD_SIZE];
	//init gametable
	int init_stones = 0;
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			board[i][j] = c.showBoard(j, i);
			if (board[i][j] != 0) init_stones++;
		}
	}

	
	if (cnt == 1) {
		if (init_stones == 0) {
			rtr.x1 = BOARD_SIZE / 2;
			rtr.y1 = BOARD_SIZE / 2;
		}
		else { //if there exist blockings, choose a relevance zone with the highest priority and close to the center.
			relevanceZone rz = getRelevanceZone(board);
			vector <pair <int, int>> candidates;
			
			int priority = rz.pq.top().priority; 
			candidates.push_back(make_pair(rz.pq.top().x, rz.pq.top().y)); rz.pq.pop();

			while (priority == rz.pq.top().priority) {
				candidates.push_back(make_pair(rz.pq.top().x, rz.pq.top().y));
				rz.pq.pop();
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
		}
	}
	else {
		rtr.x1 = 1; rtr.x2 = 2;
		rtr.y1 = 1; rtr.y2 = 2;
	}

	return rtr;
}

relevanceZone getRelevanceZone(int gBoard[BOARD_SIZE][BOARD_SIZE]) {
	relevanceZone z;
	memset(z.board, 0, sizeof(z.board));
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (gBoard[i][j] != 0) {
				int x, y; x = j; y = i;
				int zone_length = 3;
				
				//draw relevancezone for each stone
				//horizontal
				int priority = zone_length;
				for (int cnt = 1; cnt <= zone_length; cnt++) {
					int cur_x1, cur_x2;
					cur_x1 = x + cnt; cur_x2 = x - cnt;
					if (cur_x1 >= 0 && cur_x1 < BOARD_SIZE ) {
						if (gBoard[y][cur_x1] == 0) z.board[y][cur_x1] += priority;
						else {
							int copy_x = cur_x1;
							for (; copy_x < BOARD_SIZE && gBoard[y][copy_x] != 0; copy_x++);
							z.board[y][copy_x] += priority;
						}
					}
					

					if (cur_x2 >= 0 && cur_x2 < BOARD_SIZE) {
						if (gBoard[y][cur_x2] == 0) z.board[y][cur_x2] += priority;
						else {
							int copy_x = cur_x2;
							for (; copy_x >= 0 && gBoard[y][copy_x] != 0; copy_x--);
							z.board[y][copy_x] += priority;
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
						if (gBoard[cur_y1][x] == 0) z.board[cur_y1][x] += priority;
						else {
							int copy_y = cur_y1;
							for (; copy_y < BOARD_SIZE && gBoard[copy_y][x] != 0; copy_y++);
							z.board[copy_y][x] += priority;
						}

					}
					if (cur_y2 >= 0 && cur_y1 < BOARD_SIZE) {
						if (gBoard[cur_y2][x] == 0) z.board[cur_y2][x] += priority;
						else {
							int copy_y = cur_y2;
							for (; copy_y >= 0 && gBoard[copy_y][x] != 0; copy_y--);
							z.board[copy_y][x] += priority;
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
						if (gBoard[cur_y1][cur_x1] == 0) z.board[cur_y1][cur_x1] += priority;
						else {
							int copy_x, copy_y;
							copy_x = cur_x1; copy_y = cur_y1;
							for (; copy_x < BOARD_SIZE && copy_y < BOARD_SIZE && gBoard[copy_y][copy_x] != 0; copy_x++, copy_y++);
							z.board[copy_y][copy_x] += priority;
						}
						
					}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE && gBoard[cur_y2][cur_x2] == 0) {
						if (gBoard[cur_y2][cur_x2] == 0) z.board[cur_y2][cur_x2] += priority;
						else {
							int copy_x, copy_y;
							copy_x = cur_x2; copy_y = cur_y2;
							for (; copy_x >= 0 && copy_y >= 0 && gBoard[copy_y][copy_x] != 0; copy_y--, copy_x--);
							z.board[copy_y][copy_x] += priority;
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
							z.board[cur_y1][cur_x1] += priority;
						}
						else {
							int copy_x, copy_y;
							copy_x = cur_x1; copy_y = cur_y1;
							for (; copy_x >= 0 && copy_y < BOARD_SIZE && gBoard[copy_y][copy_x] != 0; copy_x--, copy_y++);
							z.board[copy_y][copy_x] += priority;
						}
											}
					if (cur_x2 >= 0 && cur_y2 >= 0 && cur_x2 < BOARD_SIZE && cur_y2 < BOARD_SIZE) {
						if (gBoard[cur_y2][cur_x2] == 0)
							z.board[cur_y2][cur_x2] += priority;
						else {
							int copy_y, copy_x;
							copy_y = cur_y2; copy_x = cur_x2;
							for (; copy_y >= 0 && copy_x < BOARD_SIZE && gBoard[copy_y][copy_x] != 0; copy_y--, copy_x++);
							z.board[copy_y][copy_x] += priority;
						}
					}

					priority--;
				}
			}
		}
	}
	for (int i = 0; i < BOARD_SIZE; i++) {
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (z.board[i][j] > 0) {
				coordInfo info;
				info.y = i; info.x = j; info.priority = z.board[i][j];
				z.pq.push(info);
			}
		}
	}
	return z;
}


void minimax(int gBoard[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, bool max) {
	//first find relevance-zone connect 6 range
	//start search + alpha beta		

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