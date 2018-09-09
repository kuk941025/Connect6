#pragma once
#define INF 1000000;

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
	priority_queue<coordInfo, vector<coordInfo>, cmp> pq, whiteP, blackP;
	int board[BOARD_SIZE][BOARD_SIZE][3];
};

StoneCOORD random_ai();
int minimax (int gBoard[BOARD_SIZE][BOARD_SIZE] , relevanceZone zone, int depth, int alpha, int beta, bool max);
relevanceZone getRelevanceZone(int gBoard[BOARD_SIZE][BOARD_SIZE]);
StoneCOORD mansoon();
