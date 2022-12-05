#include <stdio.h>
#include"Serial.h"

#if defined(__linux__) || defined(__APPLE__)
#include <fcntl.h>
#include <termios.h>
#define STDIN_FILENO 0
#elif defined(_WIN32) || defined(_WIN64)
#include <conio.h>
#endif

#include <stdlib.h>
#include <stdio.h>


#pragma comment(lib, "winmm.lib")

#include <windows.h>

HANDLE hTimer = NULL;
using namespace std;

//Variable declarations
HANDLE hCom;
DCB dcb;
OVERLAPPED o = { 0 };

int iRobotInitialize(const char* port_num) {
	hCom = CreateFile(TEXT((const char*)port_num),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL
	);

	// Error to catch wrong port
	if (hCom == INVALID_HANDLE_VALUE)
	{
		// Handle the error. 
		printf("Probable wrong port number\n. CreateFile failed with error %d.\n", GetLastError());
		return 0;
	}
	else {
	}

	// Get dcb from com port
	GetCommState(hCom, &dcb);
	if (GetCommState(hCom, &dcb) == 0)
	{
		// Handle the error. 
		printf("GetCommState failed with error %d.\n", GetLastError());
		return 0;
	}
	else {
	}

	// Set com port params
	dcb.BaudRate = 115200;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;


	if (SetCommState(hCom, &dcb) == 0)
	{
		// Handle the error. 
		printf("SetCommState failed with error %d.\n", GetLastError());
		return 0;
	}
	else {
	}

	Sleep(50);

	return 1;
}

void controlMotor(short r, short l) {
	BYTE driveBytes[6] = { 145,0,0,0,0, NULL };
	unsigned char *tmp;

	driveBytes[0] = 0x91;

	if (r > 500)
		r = 500;
	else if (r < -500)
		r = -500;

	if (l > 500)
		l = 500;
	else if (l < -500)
		l = -500;

	tmp = (unsigned char*)(&r);
	driveBytes[2] = *tmp;
	driveBytes[1] = *(tmp+1);

	tmp = (unsigned char*)(&l);
	driveBytes[4] = *tmp;
	driveBytes[3] = *(tmp + 1);

	WriteFile(hCom, &driveBytes, 5, NULL, &o);
}

void safe(void) {
	BYTE safeBytes[6] = { 131, 0, 0, 0, 0, NULL };
	WriteFile(hCom, safeBytes, 5, NULL, &o);
}

void start(void) {
	BYTE startBytes[6] = { 128, 0, 0, 0, 0, NULL };
	WriteFile(hCom, startBytes, 5, NULL, &o);
}
