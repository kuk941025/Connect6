#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
using namespace std;
Connect6 c = Connect6(true, false);
COORD test_ai() {
	int y, x;
	do {
		y = rand() % BOARD_SIZE;
		x = rand() % BOARD_SIZE;
	} while (!c.isOccupied(y, x));
	COORD rtr;
	rtr.X = x; rtr.Y = y;
	return rtr;
}
int main() {
	srand(time(NULL));
	c.set_blackAI(&test_ai);
	c.set_whiteAI(&test_ai);
	c.play_connect6();
	return 0;
}
