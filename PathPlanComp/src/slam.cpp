/*
 * slam.cpp
 * Created by Yuichiro Toda on 10/04/12. 
 * Modified by Wei Hong CHin on 12/07/17.
 * Copyright 2012 TMU. All rights reserved.
 *
 */

#include "slam.h"
#include "random.h"
#include <stdlib.h>

double x[8192], y[8192];

struct robot_position* initialize_RobotPosition(struct robot_position*robot, int dflag)
{
	if (robot == NULL)
		robot = (struct robot_position*)malloc(sizeof(struct robot_position));

	if (robot != NULL) {
		if (dflag == 0) {
			robot->real_x = 0;
			robot->real_y = 0;
			robot->rangle = 0;
			robot->dangle = 0;
			robot->map_x = CENTER;
			robot->map_y = CENTER;
			robot->slam_count = 0;
		}
		else {
			robot->real_x = ERR;
			robot->real_y = ERR;
			robot->rangle = 0;
			robot->dangle = 0;
			robot->map_x = 0;
			robot->map_y = 0;
			robot->slam_count = 0;
		}
	}
	
	return robot;
}
