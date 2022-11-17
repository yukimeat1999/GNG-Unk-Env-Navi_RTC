/*
 * slam.cpp
 * Created by Yuichiro Toda on 10/04/12. 
 * Modified by Wei Hong CHin on 12/07/17.
 * Copyright 2012 TMU. All rights reserved.
 *
 */

#include "slam.h"

double transformation_angle(double theta1)
{
	double err_a;

	if (fabs(theta1) > 180) {
		err_a = fabs(theta1) - 180.0;
		if (theta1 > 0) {
			theta1 = -180.0;
			theta1 += err_a;
		}
		else {
			theta1 = 180.0;
			theta1 -= err_a;
		}
	}

	return theta1;
}

