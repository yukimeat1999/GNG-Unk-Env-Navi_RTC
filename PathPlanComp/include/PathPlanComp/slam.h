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

#define MAPSIZE 500
#define ERR -999999999
#define MAPRATE 50.0
#define CENTER MAPSIZE/2
#define GALM 3
#define GANM 600

#ifndef M_PI
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

	double gen_m[GANM + 1][GALM + 1];
	double fit_m[GANM + 1];
	int best_gene;
	int slam_count;
};

struct robot_position* initialize_RobotPosition(struct robot_position* robot, int dflag);
