#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
#include "ai_sourcefile.h"
#include "mandeuk.h"
using namespace std;
Connect6 c = Connect6(false , false);

int main() {
	srand(time(NULL));
	
	c.set_blackAI(&mandeuk);
	c.set_whiteAI(&mandeuk);
	c.set_blocking(false);
	c.init();
	c.play_connect6();
//	c.simulate(&random_ai, &mandeuk, 10);
	return 0;
}
	