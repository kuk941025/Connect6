#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
#include "ai_sourcefile.h"
#include "mandeuk.h"
using namespace std;
Connect6 c = Connect6(false, true);

int main() {
	srand(time(NULL));
	
	c.set_blackAI(&mandeuk);
	c.set_whiteAI(&dumb_minimax);
	c.set_blocking(true);
	c.init();
	c.play_connect6();
	//c.simulate(&deeper_dumb_minimax, &dumb_minimax, 10);
	return 0;
}
	