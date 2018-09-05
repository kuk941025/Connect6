#include "consoleAPI.h"

void showCursor(int visibility) {
	CONSOLE_CURSOR_INFO curinfo;
	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curinfo);
	curinfo.bVisible = visibility;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &curinfo);

}
void MoveCursor(int x, int y) {
	COORD Cur;
	Cur.X = x;
	Cur.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}
COORD getCursor() {
	COORD curPoint;
	CONSOLE_SCREEN_BUFFER_INFO pos;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &pos);
	curPoint.X = pos.dwCursorPosition.X;
	curPoint.Y = pos.dwCursorPosition.Y;
	return curPoint;
}
