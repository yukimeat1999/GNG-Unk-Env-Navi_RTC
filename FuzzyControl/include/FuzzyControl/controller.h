/*
*  controller.h
*  slam
*
*  Created by Yuichiro Toda on 16/07/12 and modified by Wei Hong on 12/7/17.
*  Copyright 2012 首都大学東京. All rights reserved.
*
*/
#pragma once

#define MAXRANGE 1000
#define MEMNUM 5
#define RULENUM 16
#define CTRLNUM 2
#define MEMNUM1 2
#define RULENUM1 10
#define TRANGE 800
#define BEHVNUM 2
//テスト用
#define RULE	6		// Fuzzy Rules
#define MEMF	5		// Membership Function
#define CNTL	2		// Control Parameters
#define MAX_LRF_DISTANCE 1000.0	//Maximum distance using collision avoidance
//#define MAX_OUTPUT 550.0	// Maximum output value of omni-directional mobile robot
#define MAX_OUTPUT 150.0	// Maximum output value of omni-directional mobile robot
#define M_PI 3.14159265358979323846


void fuzzyinf(double *sdt, double *outp);   	// Fuzzy inference　テスト用
void fuzzy_inf(int min[], double outp[]);
void fuzzyinf3(double det, double t_theta, double outp[]);
void cal_outp(double outp[CTRLNUM], double toutp[BEHVNUM][CTRLNUM]);
void init_mobc();
void Update_wgt(double wgt2[],double si[BEHVNUM]);