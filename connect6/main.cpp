#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
#include "ai_sourcefile.h"
#include "mandeuk.h"
using namespace std;
Connect6 c = Connect6(true, true);

int main() {
	srand(time(NULL));
	
	c.set_blackAI(&random_ai_highest_relevant);
	c.set_whiteAI(&random_ai_highest_relevant);
	c.set_blocking(false);
	c.init();
	c.play_connect6();
//	c.simulate(&random_ai_highest_relevant, &random_ai_highest_relevant, 100);
	return 0;
}
	