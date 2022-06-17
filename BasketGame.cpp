#include <iostream>
#include <mutex>
#include <thread>
#include <windows.h>
#include <conio.h>
#include <cstdlib>
#include <atomic>
#include <condition_variable>
#include <time.h>
#include <random>

#include <Mmsystem.h>
#include <mciapi.h>
//these two headers are already included in the <Windows.h> header
#pragma comment(lib, "winmm.lib")



#define max_semaf 5

void gotoxy(int x, int y)
{
	COORD coordinates;     
	coordinates.X = x;     
	coordinates.Y = y;     
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

using namespace std;

mutex mut, mut2, mut3;
condition_variable_any cva;
atomic <int> sem = max_semaf, score = 0;

//variabile x del cestino
int x = 76;

//variabile y del cuore e vite
atomic <int> yy = 17, vite = 3;

//variabile while generatore
bool gen = true;

void StampaCampo();
void StampaCuori();
void StampaCuore(int y);
void CancellaCuore();
void StampaCestino(int x, int xx);
void Engine();
void Generatore();
void OggInCaduta();
void Score(int pun);
void Vita(int mode);
void Restart();
void CancRes();

int main() {
	
	StampaCampo();
	Engine();
}

void StampaCampo() {

	Sleep(1000);
	gotoxy(35, 1);
	cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *";
	for (int y = 2; y < 42; y++) {
		gotoxy(35, y);
		cout << "*                                                                                         *";
	}
	gotoxy(35, 42);
	cout << "* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *";

}

void StampaCuori() {

	for (int y = 3; y < 24; y += 7) {
		StampaCuore(y);
	}
	
}

void StampaCuore(int y) {
	
	mut2.lock();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (12 + (0 * 16)));
	gotoxy(10, y);
	cout << " * *   * * ";
	gotoxy(10, y + 1);
	cout << "* * * * * *";
	gotoxy(10, y + 2);
	cout << " * * * * *";
	gotoxy(10, y + 3);
	cout << "  * * * *";
	gotoxy(10, y + 4);
	cout << "    * *";
	mut2.unlock();

}

void CancellaCuore() {
	
	mut2.lock();
	gotoxy(10, yy);
	cout << "           ";
	gotoxy(10, yy + 1);
	cout << "           ";
	gotoxy(10, yy + 2);
	cout << "          ";
	gotoxy(10, yy + 3);
	cout << "         ";
	gotoxy(10, yy + 4);
	cout << "       ";
	mut2.unlock();
}

void StampaCestino(int x, int xx) {
	
	mut2.lock();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (11 + (0 * 16)));
	gotoxy(xx, 40);
	cout << "       ";
	gotoxy(xx, 41);
	cout << "       ";

	gotoxy(x, 40);
	cout << "*     *";
	gotoxy(x, 41);
	cout << "* * * *";
	mut2.unlock();

}

void Engine() {

	bool eng = true;
	int num = 0;

	StampaCestino(x, x);
	
	while (true) {
		
		eng = true; num = 0;

		Score(0);
		StampaCuori();
		thread tt(Generatore);
		tt.detach();

		while (eng) {
		
			if (GetKeyState(VK_RIGHT) < 0) {
				if (x < 118 && num == 5000) {
					x += 2; StampaCestino(x, x - 2);
					num = 0;
				}
				else
					if (num < 50000 && x < 118)
						num++;
			}

			if (GetKeyState(VK_LEFT) < 0) {
				if (x > 36 && num == 5000)  { 
					x -= 2; StampaCestino(x, x + 2); 
					num = 0;
				}      
				else
					if (num < 50000 && x > 36)				
						num++;
			}
			         
			if (GetKeyState(VK_RETURN) < 0) {
				if (gen == false) { 
					gen = true; eng = false; CancRes(); 
				} 
			}
		}
	}
}

void Generatore() {

	while (gen) {
		if (vite > 0) {
			mut.lock();
			if (sem > 0) {
				thread tt2(OggInCaduta);
				tt2.detach();
				sem -= 1;
			}
			else {
				cva.wait(mut);
			}
			mut.unlock();
			Sleep(100);
		}
		else {
			gen = false;
		}	
	}
}

void OggInCaduta() {

	random_device e;
	uniform_int_distribution <int> d(37, 117), u(10, 70), p(0, 2);
	
	int xx = 0, y = 3, sl = 0, pun = 0, col = 0;

	xx = d(e); sl = u(e); pun = p(e);

	switch (pun) {
		case 0: pun = 100;  col = 10; break;
		case 1: pun = 500;  col = 14; break;
		case 2: pun = -1000; col = 12; break;
	}
	
	for (; y < 42; y++) {
		
		if (y <= 39 || (xx != x && xx != x + 6)) {

			if (!(y == 40 && (xx > x && xx < x + 6))) {
				mut2.lock();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (col + (0 * 16)));
				gotoxy(xx, y - 1);
				cout << " ";
				gotoxy(xx, y);
				cout << "$";
				mut2.unlock();
				Sleep(sl);
			}
			else {
				
				break;
			}
								
	
			if (y == 41) {
				if (pun > 0)
					pun = -(pun / 10);
				else
					pun = 0;
			}
		}
		else {
			pun = 0;
			break;
		}
	}
	
	mut2.lock();
	gotoxy(xx, y - 1); cout << " ";
	mut2.unlock();
	
	sem += 1;
	cva.notify_one();

	Score(pun);
}

void Score(int pun) {

	score += pun;

	mut2.lock();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (13 + (0 * 16)));
	gotoxy(135, 3);
	cout << "Score: " << score << " $    ";
	mut2.unlock();

	mut3.lock();
	if (vite > 0) {
		if (pun < 0 && score < 0) {		
			Vita(0);
		}
		else {
			if (vite < 3 && score >= 0) {
				Vita(1);
			}
		}
	}
	else {
		gen = false;
		if (sem == max_semaf)
			Restart();
	}
	mut3.unlock();
}

void Vita(int mode) {

	if (mode == 0) {
		for (int i = 0; i < 3; i++) {
			StampaCuore(yy);
			Sleep(50);
			CancellaCuore();
			Sleep(50);
		}
		vite -= 1; yy -= 7;
	}
	else {
		while (vite < 3) {
			vite += 1; yy += 7;
			for (int i = 0; i < 3; i++) {
				CancellaCuore();
				Sleep(50);
				StampaCuore(yy);
				Sleep(50);
			}
		}
	}
}

void Restart() {

	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), (12 + (0 * 16)));
	gotoxy(70, 22);
	cout << "Press Enter to restart";

	score = 0;
	vite = 3;
	yy = 17;
	sem = max_semaf;
}

void CancRes() {
	gotoxy(70, 22);
	cout << "                      ";
}

