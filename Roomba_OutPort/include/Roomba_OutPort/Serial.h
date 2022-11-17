#pragma once
/*
//Function definitions
int RobotInitialize(int* port_num);
void action(int* port_num);
void wait(int* port_num);
void PWMcontrol(int* port_num, int r, int l);
void Frash(int* port_num);
void Torque_OFF(int* port_num);
*/

//controller.h
#define MAX_OUTPUT 150.0	// Maximum output value of omni-directional mobile robot


//Function definitions
int iRobotInitialize(const char* port_num);
//void drive(int wheel1, int wheel2);
//void stop(void);
void start(void);
void safe(void);
//void turn(int wheel1, int wheel2);
//void turnLeft90(void);
//void turnRight90(void);
void controlMotor(short r, short l);


