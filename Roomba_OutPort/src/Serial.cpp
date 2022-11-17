//Roomba irobot control c++ code 
//Modified by Wei Hong
//Date: 12/07/2017

// #include "pch.h"
#include <stdio.h>
#include"Serial.h"

/*******************************************************************************
* Copyright 2017 ROBOTIS CO., LTD.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*******************************************************************************/

/* Author: Ryu Woon Jung (Leon) */

//
// *********     Read and Write Example      *********
//
//
// Available Dynamixel model on this example : All models using Protocol 2.0
// This example is designed for using a Dynamixel PRO 54-200, and an USB2DYNAMIXEL.
// To use another Dynamixel model, such as X series, see their details in E-Manual(emanual.robotis.com) and edit below "#define"d variables yourself.
// Be sure that Dynamixel PRO properties are already set as %% ID : 1 / Baudnum : 1 (Baudrate : 57600)
//

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
/*
extern "C" {
#include "dynamixel_sdk.h"
};
*/
// Control table address
#define ADDR_PRO_TORQUE_ENABLE          64                 // Control table address is different in Dynamixel model
#define ADDR_PRO_LED_ENABLE				65
#define ADDR_PRO_GOAL_POSITION          116
#define ADDR_PRO_PRESENT_POSITION       132
#define ADDR_PRO_PWM_LIMIT               36
#define ADDR_PRO_GOAL_PWM               100
#define ADDR_PRO_PRESENT_PWM            124          

// Protocol version
#define PROTOCOL_VERSION                2.0                 // See which protocol version is used in the Dynamixel

// Default setting
#define DXL_ID                          1                   // Dynamixel ID: 1
#define DXL2_ID                         2                   // Dynamixel ID: 2

#define BAUDRATE                        57600
// #define DEVICENAME                      "COM6"				// Check which port is being used on your controller
															// ex) Windows: "COM6"   Linux: "/dev/ttyUSB0" Mac: "/dev/tty.usbserial-*"
const char* port_num;

#define TORQUE_ENABLE                   1                   // Value for enabling the torque
#define TORQUE_DISABLE                  0                   // Value for disabling the torque
#define DXL_MINIMUM_POSITION_VALUE      0					// Dynamixel will rotate between this value
#define DXL_MAXIMUM_POSITION_VALUE      4095				// and this value (note that the Dynamixel would not move when the position value is out of movable range. Check e-manual about the range of the Dynamixel you use.)
#define DXL_MOVING_STATUS_THRESHOLD     20                  // Dynamixel moving status threshold

#define DXL_MINIMUM_PWM                 0
#define DXL_MAXIMUM_PWM                 885

#define ESC_ASCII_VALUE                 0x1b

	//int index = 0;
// int dxl_comm_result = COMM_TX_FAIL;             // Communication result

// uint8_t dxl_error = 0;                          // Dynamixel error


// example program demonstrating how functions can be used
HANDLE hTimer = NULL;
using namespace std;

//Variable declarations
HANDLE hCom;
DCB dcb;
DWORD iBytesWritten = 0;
BYTE writeByte_1[3] = { 128, 0, NULL };
BYTE writeByte_2[3] = { 130, 0, NULL };
BYTE writeByte2[6] = { 137, 127, 56, 128, 0, NULL };
BYTE writeByte3[3] = { 142, 19, NULL };
OVERLAPPED o = { 0 };
BYTE rxString[3] = { NULL };
DWORD bytesRx = 0;
int dist = 0;
int angleS = 0;

// ポート番号，ソケット
int srcSocket;  // 自分
int dstSocket;  // 相手

				// sockaddr_in 構造体
struct sockaddr_in srcAddr;
struct sockaddr_in dstAddr;
int dstAddrSize = sizeof(dstAddr);
int status;

// 各種パラメータ
int numrcv;
char buffer[1024];
/*
unsigned long _stdcall Timer(void*)
{
	int nCount = 0;
	while (nCount < 200)
	{
		WaitForSingleObject(hTimer, 50);
		//printf("Hello");
		//drive(10, 10);
		nCount++;
	}
	//stop();
	//cout << "50 secs\n";
	return 0;
}
*/
int iRobotInitialize(const char* port_num) {
	// Creates port settings (for COM6, change number if different port is used)
	hCom = CreateFile(TEXT((const char*)port_num),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL
	);

	/*hCom = CreateFile(TEXT("\\\\.\\COM6"),
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		NULL,
		NULL
	);*/

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

// Safe input range: speed 0 - 400, angle 5 - 45
/*
void drive(int wheel1, int wheel2) {
	BYTE driveBytes[6] = { 137,0,0,128,0,NULL };
	BYTE speed1High = 0;
	BYTE speed1Low = 0;
	BYTE speed2High = 0;
	BYTE speed2Low = 0;
	BYTE angleHigh = 0;
	BYTE angleLow = 0;
	int radius = 0;
	int circumference = 2 * M_PI * 150;
	int arc;
	int speed1, speed2, wheel1new, wheel2new;

	if (wheel1 > 0 && wheel2 > 0)
	{
		speed1 = wheel1 - 10;
		speed2 = wheel2 - 10;
	}
	else
	{
		speed1 = wheel1 + 10;
		speed2 = wheel2 + 10;

	}

	for (int i = 0; i < 10; i++)
	{
		if (wheel1 > 0 && wheel2 > 0)
		{
			wheel1new = wheel1 - speed1;
			wheel2new = wheel2 - speed2;
			speed1 -= 10;
			speed2 -= 10;
		}
		else
		{
			wheel1new = wheel1 - speed1;
			wheel2new = wheel2 - speed2;
			speed1 += 10;
			speed2 += 10;
		}

		speed1High = (wheel1new >> 8);
		speed1Low = wheel1new - speed1High;
		speed2High = (wheel2new >> 8);
		speed2Low = wheel2new - speed2High;

		driveBytes[0] = 0x91;
		driveBytes[1] = speed1High;
		driveBytes[2] = speed1Low;
		driveBytes[3] = speed2High;
		driveBytes[4] = speed2Low;
        WriteFile(hCom, &driveBytes, 5, NULL, &o);

		Sleep(100);
	}
}
*/

void controlMotor(short r, short l) {
	BYTE driveBytes[6] = { 145,0,0,0,0, NULL };
	//printf("%d,%d\n",r,l);
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
/*
void turn(int wheel1, int wheel2) {
	BYTE driveBytes[6] = { 137,0,0,128,0,NULL };
	BYTE speed1High = 0;
	BYTE speed1Low = 0;
	BYTE speed2High = 0;
	BYTE speed2Low = 0;
	BYTE angleHigh = 0;
	BYTE angleLow = 0;
	int radius = 0;
	int circumference = 2 * M_PI * 150;
	int arc;

	speed1High = (wheel1 >> 8);
	speed1Low = wheel1 - speed1High;
	speed2High = (wheel2 >> 8);
	speed2Low = wheel2 - speed2High;

	driveBytes[0] = 0x91;
	driveBytes[1] = speed1High;
	driveBytes[2] = speed1Low;
	driveBytes[3] = speed2High;
	driveBytes[4] = speed2Low;

	WriteFile(hCom, &driveBytes, 5, NULL, &o);

}

void turnLeft90(void) {
	BYTE turnBytes[9] = { 137,0,200,0,1,157, 0, 82, NULL };

	WriteFile(hCom, &turnBytes, 8, NULL, &o);
}


void turnRight90(void) {
	BYTE turnBytes[9] = { 137,0,200,255,255,157, 255, 174, NULL };

	WriteFile(hCom, &turnBytes, 8, NULL, &o);
}

void stop(void) {
	BYTE stopBytes[6] = { 137, 0, 0, 0, 0, NULL };
	WriteFile(hCom, stopBytes, 5, NULL, &o);
}
*/
void safe(void) {
	BYTE safeBytes[6] = { 131, 0, 0, 0, 0, NULL };
	WriteFile(hCom, safeBytes, 5, NULL, &o);
}

void start(void) {
	BYTE startBytes[6] = { 128, 0, 0, 0, 0, NULL };
	WriteFile(hCom, startBytes, 5, NULL, &o);
}
/*
//Returns distance travelled in mm (can be positive or negative)
int getDistance(void) {
	BYTE distBytes[3] = { 142, 19, NULL };
	int distance = 0;
	DWORD highB = 0;

	WriteFile(hCom, &distBytes, 2, NULL, &o);
	Sleep(15);
	ReadFile(hCom, &rxString, 2, &bytesRx, &o);

	//account for sign of distance
	if (rxString[0] > 128) {
		rxString[0] = ~(rxString[0]);
		rxString[1] = ~rxString[1];
		highB = (rxString[0] << 8);
		//distance = -(highB + rxString[1]);
		distance = (highB + rxString[1]);
	}
	else {
		highB = (rxString[0] << 8);
		distance = highB + rxString[1];
	}
	return distance;
}
//Returns angle turned in DEGREES since radians requires decimals (clockwise negative, anticlockwise positive)
int getAngle(void) {
	BYTE angleBytes[3] = { 142, 20, NULL };
	DWORD angle;
	DWORD highB = 0;

	WriteFile(hCom, &angleBytes, 2, NULL, &o);
	Sleep(15);
	ReadFile(hCom, &rxString, 2, &bytesRx, &o);
	//account for sign of angle
	if (rxString[0] > 128) {
		rxString[0] = ~(rxString[0]);
		rxString[1] = ~rxString[1];
		highB = (rxString[0] << 8);
		//angle = -(highB + rxString[1]);
		angle = (highB + rxString[1]);
	}
	else {
		highB = (rxString[0] << 8);
		angle = highB + rxString[1];
	}
	return angle;
}
*/

/*
int getch()
{
#if defined(__linux__) || defined(__APPLE__)
	struct termios oldt, newt;
	int ch;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	ch = getchar();
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	return ch;
#elif defined(_WIN32) || defined(_WIN64)
	return _getch();
#endif
}

int kbhit(void)
{
#if defined(__linux__) || defined(__APPLE__)
	struct termios oldt, newt;
	int ch;
	int oldf;

	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

	ch = getchar();

	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);

	if (ch != EOF)
	{
		ungetc(ch, stdin);
		return 1;
	}

	return 0;
#elif defined(_WIN32) || defined(_WIN64)
	return _kbhit();
#endif
}

int RobotInitialize(int* port_num) {

	int i = 1;
	
	*port_num = portHandler(DEVICENAME);
	packetHandler();

	// Open port																																																																																																																																																																																																																																																																																																																																																																																																																																			
	if (openPort(*port_num)) {
		printf("Succeeded to open the port!\n");
	}
	else {
		printf("Failed to open the port!\n");
		printf("Press any key to terminate...\n");
		_getch();
		return 0;
	}

	// Set port baudrate
	if (setBaudRate(*port_num, BAUDRATE)) {
		printf("Succeeded to change the baudrate!\n");
	}
	else {
		printf("Failed to change the baudrate!\n");
		printf("Press any key to terminate...\n");
		_getch();
		return 0;
	}

	// Enable Dynamixel #1 Torque
	//TRACE("%d\n", *port_num);
	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
	//TRACE("hogehoge\n");
	if ((dxl_comm_result = getLastTxRxResult(*port_num, PROTOCOL_VERSION)) != COMM_SUCCESS) {
		printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
	}
	else if ((dxl_error = getLastRxPacketError(*port_num, PROTOCOL_VERSION)) != 0) {
		printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
	}
	else {
		printf("Dynamixel #1 has been successfully connected \n");
		i = 1;
	}

	// Enable Dynamixel #2 Torque
	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE);
	if ((dxl_comm_result = getLastTxRxResult(*port_num, PROTOCOL_VERSION)) != COMM_SUCCESS) {
		printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
	}
	else if ((dxl_error = getLastRxPacketError(*port_num, PROTOCOL_VERSION)) != 0) {
		printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
	}
	else {
		printf("Dynamixel #2 has been successfully connected \n");
	}

	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_GOAL_PWM, 0);
	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_GOAL_PWM, 0);

	return i;

}

void action(int* port_num) {

	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_GOAL_PWM, 300);
	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_GOAL_PWM, -300);

}

void wait(int* port_num) {

	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_GOAL_PWM, 0);
	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_GOAL_PWM, 0);

}

void Torque_OFF(int* port_num) {

	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_GOAL_PWM, 0);
	write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_GOAL_PWM, 0);

	// Disable Dynamixel Torque #1
	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
	if ((dxl_comm_result = getLastTxRxResult(*port_num, PROTOCOL_VERSION)) != COMM_SUCCESS) {
		printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
	}
	else if ((dxl_error = getLastRxPacketError(*port_num, PROTOCOL_VERSION)) != 0) {
		printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
	}

	// Disable Dynamixel Torque #2
	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_DISABLE);
	if ((dxl_comm_result = getLastTxRxResult(*port_num, PROTOCOL_VERSION)) != COMM_SUCCESS) {
		printf("%s\n", getTxRxResult(PROTOCOL_VERSION, dxl_comm_result));
	}
	else if ((dxl_error = getLastRxPacketError(*port_num, PROTOCOL_VERSION)) != 0) {
		printf("%s\n", getRxPacketError(PROTOCOL_VERSION, dxl_error));
	}


	// Close port
	closePort(*port_num);

}

void Frash(int* port_num){

	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_LED_ENABLE, 1);
	write1ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_LED_ENABLE, 1);

}
void PWMcontrol(int* port_num, int l, int r){

	if (r > 880)
		r = 880;
	if (r < -880) 
		r = -880;
	
	if (l > 880)
		l = 880;
	if (l < -880)
		l = -880;

			write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL_ID, ADDR_PRO_GOAL_PWM, l);
			write4ByteTxRx(*port_num, PROTOCOL_VERSION, DXL2_ID, ADDR_PRO_GOAL_PWM, -r);

	
}
*/


/*
int main(int argc, char* argv[]) {

	// Initialize PortHandler Structs
  // Set the port path
  // Get methods and members of PortHandlerLinux or PortHandlerWindows
	int port_num = portHandler(DEVICENAME);

	// Initialize PacketHandler Structs

	packetHandler();
	RobotInitialize(port_num);

	while (1) {
		printf("---Press any key to Start---\n");
		if (getch() == ESC_ASCII_VALUE)
			break;

		do {
			PWMcontrol(port_num);
		} while (getch() == ESC_ASCII_VALUE);
		break;
	}

	Torque_OFF(port_num);

	return 0;
}
*/
/*
int RobotInitialize() {
	// Initialize PortHandler instance
 // Set the port path
 // Get methods and members of PortHandlerLinux or PortHandlerWindows
	dynamixel::PortHandler* portHandler = dynamixel::PortHandler::getPortHandler(DEVICENAME);

	// Initialize PacketHandler instance
	// Set the protocol version
	// Get methods and members of Protocol1PacketHandler or Protocol2PacketHandler
	dynamixel::PacketHandler* packetHandler = dynamixel::PacketHandler::getPacketHandler(PROTOCOL_VERSION);

	int index = 0;
	int dxl_comm_result = COMM_TX_FAIL;             // Communication result
	int dxl_comm_result2 = COMM_TX_FAIL;             // Communication result
	int dxl_goal_position[2] = { DXL_MINIMUM_POSITION_VALUE, DXL_MAXIMUM_POSITION_VALUE };         // Goal position

	uint8_t dxl_error = 0;                          // Dynamixel error
	int32_t dxl_present_position = 0;               // Present position

	// Open port
	if (portHandler->openPort())
	{
		printf("Succeeded to open the port!\n");
	}
	else
	{
		printf("Failed to open the port!\n");
		printf("Press any key to terminate...\n");
		getch();
		return 0;
	}

	// Set port baudrate
	if (portHandler->setBaudRate(BAUDRATE))
	{
		printf("Succeeded to change the baudrate!\n");
	}
	else
	{
		printf("Failed to change the baudrate!\n");
		printf("Press any key to terminate...\n");
		getch();
		return 0;
	}

	// Enable Dynamixel Torque #1
	dxl_comm_result = packetHandler->write1ByteTxRx(portHandler, DXL_ID, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
	dxl_comm_result2 = packetHandler->write1ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
	if (dxl_comm_result != COMM_SUCCESS)
	{
		// printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
		printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
	}
	else if (dxl_error != 0)
	{
		// printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		printf("%s\n", packetHandler->getRxPacketError(dxl_error));
	}
	else
	{
		printf("Dynamixel has been successfully connected \n");
	}

	// Enable Dynamixel Torque #2
	dxl_comm_result2 = packetHandler->write1ByteTxRx(portHandler, DXL_ID2, ADDR_PRO_TORQUE_ENABLE, TORQUE_ENABLE, &dxl_error);
	if (dxl_comm_result2 != COMM_SUCCESS)
	{
		// printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result));
		printf("%s\n", packetHandler->getTxRxResult(dxl_comm_result2));
	}
	else if (dxl_error != 0)
	{
		// printf("%s\n", packetHandler->getRxPacketError(dxl_error));
		printf("%s\n", packetHandler->getRxPacketError(dxl_error));
	}
	else
	{
		printf("Dynamixel has been successfully connected \n");
	}

}*/