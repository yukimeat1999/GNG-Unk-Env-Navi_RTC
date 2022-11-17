/*
*  slam.h
*  slam
*
*  Created by Yuichiro Toda on 10/04/12 and modified by Wei Hong on 12/7/17.
*  Copyright 2012 TMU. All rights reserved.
*
*/


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#ifndef Localization_v2_h
#define Localization_v2_h
#include "Localization.h"
#endif


#define MAPSIZE 500
#define ERR -999999999
//#define MAPRATE 50
#define CENTER MAPSIZE/2
#define GALM 3
//#define GANM 600
//#define T2 2400

//gng.h
#ifndef MY_GNG_H_INCLUDED
#define MY_GNG_H_INCLUDED
#define DIM 3		//ÉxÉNÉgÉãÇÃéüå≥êî
#endif

#ifndef M_PI
#define M_PI
#define M_PI 3.14159265358979323846
#endif

#define PI M_PI

struct robot_position {
	int map_x;
	int map_y;
	double real_x;
	double real_y;
	double rangle;
	double dangle;

	int mtx;
	int mty;

	double alpha;
	int move_direction;

	int path[500][2];
	int path_ct;

	double gen_m[3000 + 1][GALM + 1];
	double fit_m[3000 + 1];
	int best_gene;
	int slam_count;
};

int slam(struct robot_position* robot, long* lrf_data, double* lrf_theta, int count, int omap[MAPSIZE][MAPSIZE]);
void map_building2(struct robot_position* robot, int count, double  ve[][3], double vo[][3], int dct[2], int omap[MAPSIZE][MAPSIZE], int amap[MAPSIZE][MAPSIZE]);

void initialize_RobotPosition(struct robot_position *robot, int dflag);

#pragma once
