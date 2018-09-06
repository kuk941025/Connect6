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

int Connect6::get_numStones() {
	return numStones;
}

void Connect6::notify_stone() {
	clear_message();
	MoveCursor(message_crd.X, message_crd.Y);
	cout << "Enter coordinate for stone ";
	if (stone_type == STONE_BLACK) cout << "BLACK: ";
	else cout << "WHITE: ";

}

void Connect6::init() {
	this->connect_num = CONNECT_NUMBER;
	this->remaining_spaces = BOARD_SIZE * BOARD_SIZE;
}
void Connect6::play_connect6() {
	show_gBoard();
	int x1, x2,  y1, y2;
	numStones = 1;
	while (1) {
		if ((stone_type == STONE_BLACK && !isBlackAI) || (stone_type == STONE_WHITE && !isWhiteAI)) {
			//user is playing
			for (int i = 0; i < numStones; i++) {
				notify_stone();
				while (1) {
					if (i == 0) {
						cin >> x1 >> y1;
						if (place_stone(y1, x1, true)) break;
					}
					else {
						cin >> x2 >> y2;
						if (place_stone(y2, x2, true)) break;
					}
				}
			}
		}
		else {
			//ai is playing
			StoneCOORD res;
			if (stone_type == STONE_BLACK) {
				res = black_ai();
			}
			else {
				res = white_ai();
			}
			place_stone(res.y1, res.x1, true);
			place_stone(res.y2, res.x2, true);
			x1 = res.x1; y1 = res.y1;
			x2 = res.x2; y2 = res.y2;
		}

		if (check_connect6(y1, x1) || check_connect6(y2, x2)) {
			clear_message();
			if (this->stone_type == STONE_BLACK) cout << "BLACK ";
			else cout << "WHITE ";
			cout << "Wins.\n";
			cout << "LAST STONE: (" << x1 << ", " << y1 << ")\n";
			cout << "LAST STONE: (" << x2 << ", " << y2 << ")\n";
			break;
		}

		if (this->remaining_spaces <= 1) {
			clear_message();
			cout << "TIE.\n";
			break;
		}
		//switch stone after being check
		switch_stone();
		numStones = 2;
	}
}
void Connect6::simulate(StoneCOORD(*aiBlack)(), StoneCOORD(*aiWhite)(), int turns) {
	StoneCOORD black, white, stone_crd;
	int black_wins, white_wins, ties;
	numStones = 1;
	black_wins = white_wins = ties = 0;
	while (turns--) {
		init();
		memset(gBoard, 0, sizeof(gBoard));
		cout << turns << "\r";
		while (1) {
			if (stone_type == STONE_BLACK) {
				stone_crd = aiBlack();
			}
			else {
				stone_crd = aiWhite();
			}
			place_stone(stone_crd.y1, stone_crd.x1, false);
			place_stone(stone_crd.y2, stone_crd.x2, false);

			if (check_connect6(stone_crd.y1, stone_crd.x1) || check_connect6(stone_crd.y2, stone_crd.x2)) {
				if (stone_type == STONE_BLACK) black_wins++;
				else white_wins++;

				break;
			}
			if (this->remaining_spaces <= 1) {
				ties++;
				break;
			}
			numStones = 2;
			switch_stone();
		}
	}

	//show result
	cout << "total games played: " << black_wins + white_wins + ties << "\n";
	cout << "black wins: " << black_wins << "\n";
	cout << "white wins: " << white_wins << "\n";
	cout << "ties: " << ties << "\n";
}
void Connect6::set_blackAI(StoneCOORD(*ai)()) {
	this->black_ai = ai;
}

void Connect6::set_whiteAI(StoneCOORD(*ai)()) {
	this->white_ai = ai;
}
void Connect6::switch_stone() {
	if (stone_type == STONE_BLACK)
		stone_type = STONE_WHITE;
	else stone_type = STONE_BLACK;
}
int Connect6::place_stone(int y, int x, bool show) {
	if (y < 0 || y >= BOARD_SIZE || x < 0 || x >= BOARD_SIZE) {
		if (show) {
			clear_message();
			cout << "Out of bound. Re-enter: ";
		}
		return 0;
	}
	if (this->gBoard[y][x] == 0) {
		gBoard[y][x] = this->stone_type;
		
		//Place a stone and switch the type
		if (show) {
			MoveCursor(x * 2, y);
			if (this->stone_type == STONE_BLACK) { cout << "O "; }
			else { cout << "X "; }
		}
		remaining_spaces--;
		return 1;
	}
	else {//full
		if (show) {
			clear_message();
			cout << "Space Already Occupied. Re-enter: ";
		}
		return 0;
	}

}

int Connect6::isOccupied(int y, int x) {
	if (gBoard[y][x] == 0) return 1;
	else return 0;
}
void Connect6::show_gBoard() {
	
	for (int i = 0; i < board_size; i++) {
		for (int j = 0; j < board_size; j++) {
			if (gBoard[i][j] == 0) cout << ". ";
			else if (gBoard[i][j] == 1) cout << "O ";
			else if (gBoard[i][j] == 2) cout << "X ";
		}
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