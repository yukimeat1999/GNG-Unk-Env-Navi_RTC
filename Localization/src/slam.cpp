/*
 * slam.cpp
 * Created by Yuichiro Toda on 10/04/12. 
 * Modified by Wei Hong CHin on 12/07/17.
 * Copyright 2012 TMU. All rights reserved.
 *
 */

#include "slam.h"
#include "random.h"

double x[8192], y[8192];

double MAPRATE;
int T2, GANM;

void initialize_RobotPosition(struct robot_position*robot, int dflag)
{
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

double fitcal_m(int i, struct robot_position* robot, int count, int omap[MAPSIZE][MAPSIZE])
{
	int k;
	double h;
	int num = 0;
	double x_1, y_1, x_3, y_3;
	int pt1_x, pt1_y;
	int hit2 = 0, err2 = 0, unce = 0;
	double p = 0.0;
	double x_c2, y_c2;
	double s1, c1;
	s1 = sin((robot->dangle + robot->gen_m[i][2]) * PI / 180.0);
	c1 = cos((robot->dangle + robot->gen_m[i][2]) * PI / 180.0);
	x_c2 = robot->real_x;
	y_c2 = robot->real_y;
	x_c2 += robot->gen_m[i][0] * c1 - robot->gen_m[i][1] * s1;
	y_c2 += robot->gen_m[i][1] * c1 + robot->gen_m[i][0] * s1;


	if (i >= 0) {
		for (k = 0; k < count; k++) {

			x_1 = x[k], y_1 = y[k];
			if (x_1 != ERR && y_1 != ERR) {
				x_3 = (double)x_1 * c1 - (double)y_1 * s1 + x_c2;
				y_3 = (double)x_1 * s1 + (double)y_1 * c1 + y_c2;
				pt1_x = CENTER + (int)x_3 / MAPRATE;
				pt1_y = CENTER - (int)y_3 / MAPRATE;

				if (pt1_x >= MAPSIZE || pt1_x < 0 || pt1_y >= MAPSIZE || pt1_y < 0) {
					err2++;
					continue;
				}

				h = omap[pt1_x][pt1_y];

				if (h > 0)
					hit2++;
				else if (h < 0)
					err2++;
				else
					unce++;

				num++;
			}
		}
	}

	p = ((double)hit2) / ((double)hit2 + (double)err2 + (double)unce);
	return p;
}

int g_max(struct robot_position* robot, double* best_fitness)
{
	int j;
	double max;
	for (int i = 0; i < GANM; i++) {
		if (i == 0) {
			max = robot->fit_m[i];
			j = i;
		}
		else if (max < robot->fit_m[i]) {
			max = robot->fit_m[i];
			j = i;
		}
	}
	*best_fitness = max;
	return j;
}

int g_min(struct robot_position* robot, double* worst_fitness)
{
	int j;
	double min;
	for (int i = 0; i < GANM; i++) {
		if (i == 0) {
			min = robot->fit_m[i];
			j = i;
		} 
		else if (min > robot->fit_m[i]) {
			min = robot->fit_m[i];
			j = i;
		}
	}
	*worst_fitness = min;
	return j;
}

void mga_init(struct robot_position* robot, int count, int omap[MAPSIZE][MAPSIZE])
{

	for (int i = 0; i < GANM; i++)
	{
		if (rnd() < 0.5)
			robot->gen_m[i][0] = 300.0 * rnd();
		else
			robot->gen_m[i][0] = -300.0 * rnd();

		if (rnd() < 0.5)
			robot->gen_m[i][1] = 300.0 * rnd();
		else
			robot->gen_m[i][1] = -300.0 * rnd();

		if (rnd() < 0.5)
			robot->gen_m[i][2] = 60.0 * rnd();
		else
			robot->gen_m[i][2] = -60.0 * rnd();

		robot->fit_m[i] = fitcal_m(i, robot, count, omap);
	}
}


void map_mating(struct robot_position* robot, int count, int omap[MAPSIZE][MAPSIZE]) {

	int t = 0, g, g2;
	int worst_gene;
	double worst_fitness;
	double best_fitness;

	mga_init(robot, count, omap);

	while (t < T2) {
		double r, r2;
		robot->best_gene = g_max(robot, &best_fitness);
		worst_gene = g_min(robot, &worst_fitness);

		g = (int)(GANM * rnd());
		while (g == robot->best_gene)
			g = (int)(GANM * rnd());
		r2 = (10.0 * (best_fitness - robot->fit_m[g])) / (best_fitness - worst_fitness + 0.01);
		g2 = robot->best_gene;
		for (int j = 0; j < GALM; j++) {
			r = rnd();
			//-------crossover-------------------------
			if (r < 0.7)
				robot->gen_m[worst_gene][j] = robot->gen_m[g2][j];
			else {
				double alpha = rnd();
				robot->gen_m[worst_gene][j] = (alpha * robot->gen_m[g][j] + (1.0 - alpha) * robot->gen_m[g2][j]);
			}
			//------mutation---------------------------
			if (rnd() < 0.7) {
				if (j != 2)
					robot->gen_m[worst_gene][j] += 100 * rndn_10() * (0.01 + r2 * (double)(T2 - t) / (double)T2);
				else
					robot->gen_m[worst_gene][j] += rndn() * (0.1 + r2 * (double)(T2 - t) / (double)T2);
			}
			else {
				if (j != 2)
					robot->gen_m[worst_gene][j] = 100.0 * rndn();
				else
					robot->gen_m[worst_gene][j] = rndn_10();
			}
		}

		robot->fit_m[worst_gene] = fitcal_m(worst_gene, robot, count, omap);
		t++;
	}

	robot->best_gene = g_max(robot, &best_fitness);
}


void calc_mapvalue(int k, int l, int pt_x, int pt_y, int sflag, int omap[MAPSIZE][MAPSIZE], int amap[MAPSIZE][MAPSIZE])
{
	if (sflag == 1) {
		if (k >= pt_x)
			omap[k][l] += 1;
		else
			omap[k][l]--;
	}
	else if (sflag == 2) {
		if (k <= pt_x)
			omap[k][l] += 1;
		else
			omap[k][l]--;
	}
	else if (sflag == 3) {
		if (l >= pt_y)
			omap[k][l] += 1;
		else
			omap[k][l]--;
	}
	else {
		if (l <= pt_y)
			omap[k][l] += 1;
		else
			omap[k][l]--;
	}
}


void map_building2(struct robot_position* robot, int count, double ve[][3], double vo[][3], int dct[2], int omap[MAPSIZE][MAPSIZE], int amap[MAPSIZE][MAPSIZE])
{
	int pt_x, pt_y, ptc_x, ptc_y;
	double x_3, y_3;
	//CvPoint pt, pt2;
	int dx, dy;
	int j, e, k, l;
	double s1, c1;
	static int ecount = 0;
	static int ocount = 0;
	int flag = 0;
	s1 = sin((double)robot->dangle * PI / 180.0);
	c1 = cos((double)robot->dangle * PI / 180.0);
	ptc_x = CENTER + (int)robot->real_x / MAPRATE;
	ptc_y = CENTER - (int)robot->real_y / MAPRATE;
	for (j = 0; j < count; j++) {
		if (x[j] != ERR && y[j] != ERR) {
			x_3 = (double)x[j] * c1 - (double)y[j] * s1 + (double)robot->real_x;
			y_3 = (double)x[j] * s1 + (double)y[j] * c1 + (double)robot->real_y;
			pt_x = CENTER + (int)x_3 / MAPRATE;
			pt_y = CENTER - (int)y_3 / MAPRATE;
			if (pt_x >= MAPSIZE || pt_x < 0 || pt_y >= MAPSIZE || pt_y < 0) {
				continue;
			}
			if (omap[pt_x][pt_y] <= 0)flag = 1;
			else flag = 0;
			amap[pt_x][pt_y]++;
			omap[pt_x][pt_y] += 10;
			dx = (pt_x - ptc_x);
			dy = (pt_y - ptc_y);
			if (omap[pt_x][pt_y] > 0 && flag == 1) {
				vo[ocount][0] = pt_x;
				vo[ocount][1] = pt_y;
				vo[ocount][2] = 1;
				ocount++;
			}
			e = 0;
			if (dx >= 0 && dy >= 0) {
				if (dx > dy) {
					for (l = ptc_y, k = ptc_x; k <= pt_x; k++) {
						e += dy;
						if (e > dx) {
							e -= dx;
							l++;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 1, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
				else {
					for (l = ptc_y, k = ptc_x; l <= pt_y; l++) {
						e += dx;
						if (e > dy) {
							e -= dy;
							k++;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 3, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
			}
			else if (dx >= 0 && dy <= 0) {
				dy *= -1;
				if (dx > dy) {
					for (l = ptc_y, k = ptc_x; k <= pt_x; k++) {
						e += dy;
						if (e > dx) {
							e -= dx;
							l--;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 1, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
				else {
					for (l = ptc_y, k = ptc_x; l >= pt_y; l--) {
						e += dx;
						if (e > dy) {
							e -= dy;
							k++;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 4, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
			}
			else if (dx <= 0 && dy >= 0) {
				dx *= -1;
				if (dx > dy) {
					for (l = ptc_y, k = ptc_x; k >= pt_x; k--) {
						e += dy;
						if (e > dx) {
							e -= dx;
							l++;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 2, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
				else {
					for (l = ptc_y, k = ptc_x; l <= pt_y; l++) {
						e += dx;
						if (e > dy) {
							e -= dy;
							k--;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 3, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
			}
			else {
				dx *= -1;
				dy *= -1;
				if (dx > dy) {
					for (l = ptc_y, k = ptc_x; k >= pt_x; k--) {
						e += dy;
						if (e > dx) {
							e -= dx;
							l--;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 2, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
				else {
					for (l = ptc_y, k = ptc_x; l >= pt_y; l--) {
						e += dx;
						if (e > dy) {
							e -= dy;
							k--;
						}
						if (omap[k][l] == 0) flag = 1;
						else flag = 0;
						calc_mapvalue(k, l, pt_x, pt_y, 4, omap, amap);
						if (omap[k][l] < 0 && flag == 1) {
							ve[ecount][0] = k;
							ve[ecount][1] = l;
							ve[ecount][2] = -1;
							ecount++;
						}
						if (omap[k][l] > 0 && flag == 1) {
							vo[ocount][0] = k;
							vo[ocount][1] = l;
							vo[ocount][2] = 1;
							ocount++;
						}
					}
				}
			}
		}
	}
	dct[0] = ecount;
	dct[1] = ocount;
}


int slam(struct robot_position* robot, long* lrf_data, double* lrf_theta, int count, int omap[MAPSIZE][MAPSIZE])
{
	int i, j;
	int i_l[3];
	static double m_x, m_y;
	double s_x, s_y, s_r;
	static double pre_angle;
	int tcount = 0;

	pre_angle = robot->dangle;
	if (robot->slam_count == 0) {
		for (i = 0; i < GANM + 1; i++) {
			robot->fit_m[i] = 0;
			for (j = 0; j < GALM + 1; j++)
				robot->gen_m[i][j] = 0;
		}

		robot->best_gene = 0;
	}

	//FILE* fp = fopen("Data0728_1.txt", "a");
	//fprintf(fp, "%d\t", count);
	for (i = 0; i < count; i++) {
		if (lrf_data[i] < 50 || lrf_data[i] > 400000) {
			x[i] = ERR;
			y[i] = ERR;
		}
		else {
			x[i] = lrf_data[i] * sin(lrf_theta[i]);
			y[i] = lrf_data[i] * cos(lrf_theta[i]);

		}
		//fprintf(fp, "%lf\t%lf\t", x[i], y[i]);
		tcount++;
	}
	//fprintf(fp, "\n");
	//fclose(fp);

	if (robot->slam_count != 0) {
		map_mating(robot, tcount, omap);

		m_x += robot->gen_m[robot->best_gene][0];
		m_y += robot->gen_m[robot->best_gene][1];
		robot->dangle += robot->gen_m[robot->best_gene][2];
		s_x = 0, s_y = 0, s_r = 0;
		s_x = robot->gen_m[robot->best_gene][0];
		s_y = robot->gen_m[robot->best_gene][1];
		s_r = robot->gen_m[robot->best_gene][2];

		robot->real_x += s_x * cos(robot->dangle * PI / 180) - s_y * sin(robot->dangle * PI / 180);
		robot->real_y += s_y * cos(robot->dangle * PI / 180) + s_x * sin(robot->dangle * PI / 180);

		robot->rangle += robot->dangle - pre_angle;

		robot->map_x = CENTER + (int)robot->real_x / MAPRATE;
		robot->map_y = CENTER - (int)robot->real_y / MAPRATE;

		double err_a;
		if (fabs(robot->rangle) > 180) {
			err_a = fabs(robot->rangle) - 180.0;
			if (robot->rangle > 0) {
				robot->rangle = -180.0;
				robot->rangle += err_a;
			}
			else {
				robot->rangle = 180.0;
				robot->rangle -= err_a;
			}
		}
	}

	//for (int i = MAPSIZE / 2 - 10; i < MAPSIZE / 2 + 10; i++) {
	//	for (int j = MAPSIZE / 2 - 10; j < MAPSIZE / 2 + 10; j++) {
	//		printf("%d, ", omap[i][j]);
	//	}
	//	printf("\n");
	//	
	//}

	robot->slam_count++;
	return tcount;
}