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
#define MAPRATE 50
#define CENTER MAPSIZE/2



#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define PI M_PI

double transformation_angle(double theta1);

#pragma once
