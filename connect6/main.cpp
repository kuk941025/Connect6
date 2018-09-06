#include "gameboard.h"
#include <time.h>
#include <stdlib.h>
#include "ai_sourcefile.h"
using namespace std;
Connect6 c = Connect6(true, true);

int main() {
	srand(time(NULL));
	c.init();
	c.set_blackAI(&random_ai);
	c.set_whiteAI(&random_ai);
	c.play_connect6();
	c.simulate(&random_ai, &random_ai, 10000);
	return 0;
}
	