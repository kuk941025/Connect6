#pragma once
#define INF 10000000
#define DEPTH_LVL 4

#include "gameboard.h"
#include <queue>
#include <algorithm>


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
//	priority_queue<coordInfo, vector<coordInfo>, cmp> pq, oppP, myP;
	vector<coordInfo> combZone, myZone, oppZone;
	int board[BOARD_SIZE][BOARD_SIZE][4];
};

StoneCOORD random_ai();
int minimax (int gBoard[BOARD_SIZE][BOARD_SIZE] , relevanceZone zone, int depth, int alpha, int beta, bool max);
relevanceZone getRelevanceZone(int gBoard[BOARD_SIZE][BOARD_SIZE]);
StoneCOORD mansoon();
bool cmpPriority(const coordInfo &a, const coordInfo &b);
StoneCOORD locate_center(int gameboard[BOARD_SIZE][BOARD_SIZE]);