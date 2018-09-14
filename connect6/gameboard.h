#pragma once
#include "consoleAPI.h"
#include <string.h>
#include <conio.h>
#include <string>
#define BOARD_SIZE 19
#define STONE_BLACK 1
#define STONE_WHITE 2
#define STONE_BLOCK 3
#define PLAY_TYPE_USER 1
#define PLAY_TYPE_AI 2
#define CONNECT_NUMBER 6
#define Key_LEFT 75
#define Key_RIGHT 77
#define Key_UP 72
#define Key_DOWN 80
#define Key_SPACE 32
#define KEY_I 108
#define KEY_K 107

extern int gBoard[BOARD_SIZE][BOARD_SIZE];
extern int stone_type, play_type_black, play_type_white;
struct StoneCOORD {
	int x1, x2, y1, y2;
	int stones;
};
class Connect6 {
private:
	int stone_type, play_type[2], numStones;
	int last_x, last_y;
	bool isBlackAI, isWhiteAI, generate_blocking;
	int gBoard[BOARD_SIZE][BOARD_SIZE] = { 0 };
	int connect_num, remaining_spaces;
	StoneCOORD(*black_ai)(), (*white_ai)();
	COORD message_crd;
	void set_messageCrd();
	void clear_message();
	void notify_stone();
	int check_connect6(int last_y, int last_x);
	int board_size = BOARD_SIZE;
	void show_gBoard(); //initialize
	void switch_stone();
	void move_pos(int move_x, int move_y);
	COORD get_userinput();
	int place_stone(int y, int x, bool show);
public:
	int isFree(int y, int x);
	int get_connectnum();
	void init();
	void set_blocking(bool blocking);
	void play_connect6();
	void set_blackAI(StoneCOORD(*ai)());
	void set_whiteAI(StoneCOORD(*ai)());
	void simulate(StoneCOORD(*aiBlack)(), StoneCOORD(*aiWhite)(), int turns);
	void show_message(std::string message);
	int get_numStones();
	int showBoard(int x, int y);
	Connect6(bool _isBlackAI, bool _isWhiteAI) {
		isBlackAI = _isBlackAI;
		isWhiteAI = _isWhiteAI;
		generate_blocking = false;
		stone_type = STONE_BLACK;
		
	}
};

extern Connect6 c;
