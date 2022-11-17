/*
 *  gng.c
 *  Claster
 *
 *  Created by Yuichiro Toda on 12/05/18.
 *  Copyright 2012 Žñ“s‘åŠw“Œ‹ž. All rights reserved.
 *
 */


#include "gng.h"

#include "random.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

struct gng* init_gng()
{
	int i, j;
	struct gng* net = NULL;
	if (net == NULL)
		net = (struct gng*)malloc(sizeof(struct gng));

	if (net != NULL) {
		for (i = 0; i < GNGN; i++) {
			for (j = 0; j < GNGN; j++) {
				net->edge[i][j] = 0;
				net->cedge[i][j] = 0;
				net->age[i][j] = 0;
			}
			net->edge_ct[i] = 0;
		}

		for (i = 0; i < 2; i++) {
			net->edge_ct[i] = 1;

			for (j = 0; j < 2; j++) {
				if (i == j) continue;
				net->edge[i][j] = 1;
			}
		}

		for (i = 0; i < GNGN; i++) {
			net->node[i][0] = 100.0 * rnd() + 750;
			net->node[i][1] = 100.0 * rnd() + 750;
			net->node[i][2] = rnd() - 0.5;
			net->gng_err[i] = 0;
			net->contour[i] = 1;	//0115
		}

		net->weight[0] = 1.0;
		net->weight[1] = 1.0;
		net->weight[2] = 0.0;

		net->node_n = 2;
	}
	
	return net;
}

