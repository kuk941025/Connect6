#include "gameboard.h"
#include "consoleAPI.h"
#include <iostream>

using namespace std;


int Connect6::check_connect6(int last_y, int last_x) {
	
	int y, x, cnt;
	y = last_y; x = last_x;
	
	//horizontal check
	cnt = 0;
	while (gBoard[y][x - 1] == stone_type && x >= 0)
		x--;
	while (gBoard[y][x++] == stone_type && x < board_size)
		cnt++;
	
	if (cnt == connect_num) 
		return 1;

	//vertical check
	cnt = 0;
	y = last_y; x = last_x;
	while (gBoard[y - 1][x] == stone_type && y >= 0)
		y--;
	while (gBoard[y++][x] == stone_type && y < board_size)
		cnt++;

	if (cnt == connect_num) 
		return 1;

	//diagonal bottom right
	cnt = 0;
	y = last_y; x = last_x;
	while (gBoard[y - 1][x - 1] == stone_type && x >= 0 && y >= 0) {
		y--;
		x--;
	}
	while (gBoard[y++][x++] == stone_type && x < board_size && y < board_size)
		cnt++;

	if (cnt == connect_num)
		return 1;

	//diagonal bottom left
	cnt = 0;
	y = last_y; x = last_x;
	while (gBoard[y - 1][x + 1] == stone_type && x >= 0 && y >= 0) {
		y--;
		x++;
	}
	while (gBoard[y++][x--] == stone_type && x < board_size && y < board_size)
		cnt++;

	if (cnt == connect_num)
		return 1;

	return 0;
}
void Connect6::play_connect6() {
	show_gBoard();
	int x, y;
	while (1) {
		clear_message();
		MoveCursor(message_crd.X, message_crd.Y);
		cout << "Enter coordinate for stone ";
		if (stone_type == STONE_BLACK) cout << "BLACK: ";
		else cout << "WHITE: ";
		
		if ((stone_type == STONE_BLACK && !isBlackAI) || (stone_type == STONE_WHITE && !isWhiteAI)) {
			//user is playing
			while (1) {
				cin >> x >> y;
				if (place_stone(y, x)) break;
			}
		}
		else {
			//ai is playing
			COORD res;
			if (stone_type == STONE_BLACK) {
				res = black_ai();
			}
			else {
				res = white_ai();
			}
			place_stone(res.Y, res.X);
			y = res.Y; x = res.X;
		}

		if (check_connect6(y, x)) {
			clear_message();
			if (this->stone_type == STONE_BLACK) cout << "BLACK ";
			else cout << "WHITE ";
			cout << "Wins.\n";
			cout << "LAST STONE: (" << x << ", " << y << ")\n";
			break;
		}
		//switch stone after being check
		switch_stone();
	}
}

void Connect6::set_blackAI(COORD(*ai)()) {
	this->black_ai = ai;
}

void Connect6::set_whiteAI(COORD(*ai)()) {
	this->white_ai = ai;
}
void Connect6::switch_stone() {
	if (stone_type == STONE_BLACK)
		stone_type = STONE_WHITE;
	else stone_type = STONE_BLACK;
}
int Connect6::place_stone(int y, int x) {
	if (y < 0 || y >= BOARD_SIZE || x < 0 || x >= BOARD_SIZE) {
		clear_message();
		cout << "Out of bound. Re-enter: ";
		return 0;
	}
	if (this->gBoard[y][x] == 0) {
		gBoard[y][x] = this->stone_type;
		MoveCursor(x * 2, y);

		//Place a stone and switch the type
		if (this->stone_type == STONE_BLACK) { cout << "O"; }
		else { cout << "X"; }
		return 1;
	}
	else {//full
		clear_message();
		cout << "Space Already Occupied. Re-enter: ";
		return 0;
	}

}

int Connect6::isOccupied(int y, int x) {
	if (gBoard[y][x] == 0) return 1;
	else return 0;
}
void Connect6::show_gBoard() {
	
	for (int i = 0; i < board_size; i++) {
		for (int j = 0; j < board_size; j++)
			cout << ". ";
		cout << "\n";
	}
	cout << "\n";

	this->set_messageCrd();
}

void Connect6::set_messageCrd() {
	this->message_crd = getCursor();
}


void Connect6::clear_message() {
	MoveCursor(message_crd.X, message_crd.Y);
	for (int i = 0; i < 100; i++) cout << " ";
	MoveCursor(message_crd.X, message_crd.Y);
}