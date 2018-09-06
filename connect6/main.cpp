#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
using namespace std;
Connect6 c = Connect6(true, true);
StoneCOORD random_ai() {
	int y, x;
	StoneCOORD rtr;
	rtr.stones = c.get_numStones();

	for (int i = 0; i < rtr.stones; i++) {
		do {
			y = rand() % BOARD_SIZE;
			x = rand() % BOARD_SIZE;
		} while (!c.isOccupied(y, x));
		if (i == 0) {
			rtr.x1 = x; rtr.y1 = y;
		}
		else {
			rtr.x2 = x; rtr.y2 = y;
		}
	}
	return rtr;
}
int main() {
	srand(time(NULL));
	c.init();
	c.set_blackAI(&random_ai);
	c.set_whiteAI(&random_ai);
	c.play_connect6();
//	c.simulate(&random_ai, &random_ai, 10000);
	return 0;
}
