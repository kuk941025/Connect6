#pragma once
#define INF 123456789;

#include "gameboard.h"
#include <queue>

using namespace std;
struct coordInfo {
	int x, y, priority;
};

struct cmp {
	bool operator()(coordInfo a, coordInfo b) {
		return a.priority < b.priority;
	}
};

struct relevanceZone {
	priority_queue<coordInfo, vector<coordInfo>, cmp> pq;
	int board[BOARD_SIZE][BOARD_SIZE];
};

StoneCOORD random_ai();
void minimax(int gBoard[BOARD_SIZE][BOARD_SIZE], int depth, int alpha, int beta, bool max);
relevanceZone getRelevanceZone(int gBoard[BOARD_SIZE][BOARD_SIZE]);
StoneCOORD mansoon();
