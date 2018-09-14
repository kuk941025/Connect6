#include "gameboard.h"
#include "ai_sourcefile.h"
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <ctime>
using namespace std;


int Connect6::check_connect6(int last_y, int last_x) {
	
	int y, x, cnt;
	y = last_y; x = last_x;
	
	//horizontal check
	cnt = 0;
	while ((gBoard[y][x - 1] == stone_type || gBoard[y][x - 1] == STONE_BLOCK) && x - 1 >= 0)
		x--;
	while ((gBoard[y][x] == stone_type || gBoard[y][x] == STONE_BLOCK) && x < board_size) {
		cnt++;
		x++;
	}
	
	if (cnt == connect_num) 
		return 1;

	//vertical check
	cnt = 0;
	y = last_y; x = last_x;
	while ((gBoard[y - 1][x] == stone_type || gBoard[y - 1][x] == STONE_BLOCK) && y - 1>= 0)
		y--;
	while ((gBoard[y][x] == stone_type || gBoard[y][x] == STONE_BLOCK) && y < board_size) {
		cnt++;
		y++;
	}

	if (cnt == connect_num) 
		return 1;

	//diagonal bottom right
	cnt = 0;
	y = last_y; x = last_x;
	while ((gBoard[y - 1][x - 1] == stone_type || gBoard[y - 1][x - 1] == STONE_BLOCK) && x - 1>= 0 && y - 1 >= 0) {
		y--;
		x--;
	}
	while ((gBoard[y][x] == stone_type || gBoard[y][x] == STONE_BLOCK) && x < board_size && y < board_size) {
		cnt++;
		y++; x++;
	}

	if (cnt == connect_num)
		return 1;

	//diagonal bottom left
	cnt = 0;
	y = last_y; x = last_x;
	while ((gBoard[y - 1][x + 1] == stone_type || gBoard[y - 1][x + 1] == STONE_BLOCK )&& x + 1 >= 0 && y - 1 >= 0) {
		y--;
		x++;
	}
	while ((gBoard[y][x] == stone_type || gBoard[y][x] == STONE_BLOCK) && x < board_size && y < board_size) {
		cnt++;
		y++; x--;
	}

	if (cnt == connect_num)
		return 1;

	return 0;
}
int Connect6::get_connectnum()
{
	return connect_num;
}

int Connect6::get_numStones() {
	return numStones;
}

int Connect6::showBoard(int x, int y) {
	//opponent stone is represented as 2
	if (stone_type == STONE_BLACK)	return gBoard[y][x];
	else {
		if (gBoard[y][x] == STONE_BLACK) return STONE_WHITE;
		else if (gBoard[y][x] == STONE_WHITE) return STONE_BLACK;
		else return gBoard[y][x];
	}
}
void Connect6::notify_stone() {
	string message = "Enter coordinate for stone ";
	if (stone_type == STONE_BLACK) message = message + "BLACK: ";
	else message = message + "WHITE: ";
	show_message(message);

}
void Connect6::show_message(string message) {
	clear_message();
	MoveCursor(message_crd.X, message_crd.Y);
	cout << message;
}


void Connect6::init() {
	this->connect_num = CONNECT_NUMBER;
	this->remaining_spaces = BOARD_SIZE * BOARD_SIZE;
	this->last_x = this->last_y = BOARD_SIZE / 2;
	memset(gBoard, 0, sizeof(gBoard));
	if (generate_blocking) {
		srand(time(NULL));
		int blocks = rand() % 8;
		//int blocks = 7;
		for (int i = 0; i < blocks; i++) {
			int rand_y, rand_x;
			do {
				rand_y = rand() % BOARD_SIZE;
				rand_x = rand() % BOARD_SIZE;
			} while (!isFree(rand_y, rand_x));
			gBoard[rand_y][rand_x] = STONE_BLOCK;
		}
		
	}
}

void Connect6::move_pos(int move_x, int move_y) {
	COORD cur_cord = getCursor();
	int next_x = cur_cord.X + (move_x * 2); 
	int next_y = cur_cord.Y + move_y;
	if (next_x >= 0 && next_x < (BOARD_SIZE * 2)&& next_y >= 0 && next_y < BOARD_SIZE)
		MoveCursor(next_x, next_y);
}
COORD Connect6::get_userinput() {
	COORD rtr;
	
	MoveCursor(last_x * 2, last_y);
	while (1) {
		int key = _getch();
		if (key == 224) {
			key = _getch();

			switch (key) {

			case Key_DOWN:
				move_pos(0, 1);
				break;
			case Key_UP:
				move_pos(0, -1);
				break;
			case Key_LEFT:
				move_pos(-1, 0);
				break;
			case Key_RIGHT:
				move_pos(1, 0);
				break;
			default:
				break;
			}
		}
		else {
			if (key == Key_SPACE) {
				COORD cur_pos = getCursor();
				rtr.X = cur_pos.X / 2;
				rtr.Y = cur_pos.Y;
				return rtr;
			}
			else if (key == KEY_I) {
				COORD cur_pos = getCursor();
				relevanceZone re = getRelevanceZone(gBoard);
				relevanceZone threats;
				setThreatZone(gBoard, &threats);
				re = combineRelevanceThreat(threats, re);
				clear_message();
				for (int i = 0; i < BOARD_SIZE; i++) {
					for (int j = 0; j < BOARD_SIZE; j++) {
						for (int k = 1; k <= 2; k++) {
							if (k == 1) cout << "(" << re.board[i][j][k] << ", ";
							else cout << re.board[i][j][k] << ")  ";
						}
					}
					cout << "\n";
				}
				MoveCursor(cur_pos.X, cur_pos.Y);
			}
			else if (key == KEY_K)
				switch_stone();

		}
	}
}
void Connect6::play_connect6() {
	show_gBoard();
	int x1, x2,  y1, y2;
	COORD userInput;
	x2 = y2 = 0;
	numStones = 1;
	while (1) {	
		if ((stone_type == STONE_BLACK && !isBlackAI) || (stone_type == STONE_WHITE && !isWhiteAI)) {
			//user is playing
			for (int i = 0; i < numStones; i++) {
				notify_stone();
				while (1) {
					if (i == 0) {
						userInput = get_userinput();
						x1 = userInput.X;
						y1 = userInput.Y;
						if (place_stone(y1, x1, true)) { 
							this->last_x = x1;
							this->last_y = y1;
							break;
						}
					}
					else {

						userInput = get_userinput();
						x2 = userInput.X;
						y2 = userInput.Y;
						if (place_stone(y2, x2, true)) {
							last_x = x2;
							last_y = y2;
							break;
						}
					}
				}
			}
		}
		else {
			//ai is playing
			StoneCOORD res;
			clock_t begin = clock();
			if (stone_type == STONE_BLACK) {
				res = black_ai();
			}
			else {
				res = white_ai();
			}
			if (place_stone(res.y1, res.x1, true)) {
				x1 = res.x1; y1 = res.y1;
			}
			if (place_stone(res.y2, res.x2, true)) {
				x2 = res.x2; y2 = res.y2;
			}
			clock_t end = clock();
			double elasped_secs = double(end - begin) / CLOCKS_PER_SEC;
			string message;
			char buff[100];
			sprintf_s(buff, "(%d, %d) (%d, %d)\n%lf seconds elasped.", res.x1, res.y1, res.x2, res.y2, elasped_secs);
			show_message(buff);
			
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

void Connect6::set_blocking(bool block) {
	this->generate_blocking = block;
}
void Connect6::simulate(StoneCOORD(*aiBlack)(), StoneCOORD(*aiWhite)(), int turns) {
	StoneCOORD stone_crd;
	int black_wins, white_wins, ties;
	numStones = 1;
	black_wins = white_wins = ties = 0;
	while (turns--) {
		init();
		
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

int Connect6::isFree(int y, int x) {
	if (gBoard[y][x] == 0) return 1;
	else return 0;
}
void Connect6::show_gBoard() {
	
	for (int i = 0; i < board_size; i++) {
		for (int j = 0; j < board_size; j++) {
			if (gBoard[i][j] == 0) cout << ". ";
			else if (gBoard[i][j] == STONE_BLACK) cout << "O ";
			else if (gBoard[i][j] == STONE_WHITE) cout << "X ";
			else if (gBoard[i][j] == STONE_BLOCK) cout << "* ";
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