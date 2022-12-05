#pragma once

#define MAX_OUTPUT 150.0	// Maximum output value


//Function definitions
int iRobotInitialize(const char* port_num);
void start(void);
void safe(void);
void controlMotor(short r, short l);


