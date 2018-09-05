#pragma once
#include "consoleAPI.h"
#include <string.h>
#define BOARD_SIZE 19
#define STONE_BLACK 1
#define STONE_WHITE 2
#define PLAY_TYPE_USER 1
#define PLAY_TYPE_AI 2

extern int gBoard[BOARD_SIZE][BOARD_SIZE];
extern int stone_type, play_type_black, play_type_white;
class Connect6 {
private:
	int stone_type, play_type[2];
	bool isBlackAI, isWhiteAI;
	int gBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };
	int connect_num = 6;
	COORD(*black_ai)(), (*white_ai)();
	COORD message_crd;
	void set_messageCrd();
	void clear_message();
	int check_connect6(int last_y, int last_x);
	int board_size = BOARD_SIZE;
	void show_gBoard(); //initialize
	void switch_stone();
	int place_stone(int y, int x);
public:
	int isOccupied(int y, int x);
	void play_connect6();
	void set_blackAI(COORD(*ai)());
	void set_whiteAI(COORD(*ai)());

	Connect6(bool _isBlackAI, bool _isWhiteAI) {
		isBlackAI = _isBlackAI;
		isWhiteAI = _isWhiteAI;
		stone_type = STONE_BLACK;
		memset(gBoard, 0, sizeof(gBoard));
	}
};
