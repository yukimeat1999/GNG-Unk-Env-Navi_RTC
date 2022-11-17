/*
 *  gng.c
 *  Claster
 *
 *  Created by Yuichiro Toda on 12/05/18.
 *  Copyright 2012 ��s��w����. All rights reserved.
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

void bubble_sort_ang(double array[], const int vct)	//0115
{
	int i, j;
	double temp;

	for (i = 0; i < vct - 1; i++) {
		for (j = vct - 1; j > i; j--) {
			if (array[j] < array[j - 1]) {
				temp = array[j];
				array[j] = array[j - 1];
				array[j - 1] = temp;
			}
		}
	}
}

int judge_contour(double vx[], double vy[], double ang[], double pos[], int vct)	//0115
{
	int i;
	double sx, sy;
	double ta;

	sx = pos[0], sy = pos[1];
	for (i = 0; i < vct; i++) {
		ta = atan2((double)(vy[i] - sy), (double)(vx[i] - sx));
		ang[i] = (ta / M_PI) * 180.0;
		if (ang[i] < 0)
			ang[i] = 360.0 - fabs(ang[i]);
	}

	bubble_sort_ang(ang, vct);

	double judge = fabs(360 - ang[vct - 1] + ang[0]);
	if (judge >= 135.0) {
		return  1;
	}
	for (int k = 0; k < vct - 1; k++) {
		judge = fabs(ang[k + 1] - ang[k]);
		if (judge >= 135.0) {
			return 1;
		}
	}

	return 0;
}

void shuffle(int* ary, int size)
{
	int i, j;
	int temp;

	for (i = 0; i < size; i++)
	{
		j = rand() % size;
		temp = ary[i];
		ary[i] = ary[j];
		ary[j] = temp;
	}
}

struct gng* init_gng()
{
	int i, j;
	struct gng* net = NULL;
	if (net == NULL)
		net = (struct gng*)malloc(sizeof(struct gng));

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

	return net;
}

void gng_clustering(struct gng* net)
{
	int i, j, k, n;
	int sflag = 0;
	int flag[GNGN];
	int c = 0;
	int c_n = 0;
	int sum = 0;
	for (i = 0; i < net->node_n; i++)
		flag[i] = 0;

	net->cluster[c][c_n] = 0;
	flag[0] = 1;
	c_n++;

	while (sum != net->node_n) {
		for (i = 0; i < c_n; i++) {
			n = net->cluster[c][i];
			for (j = 0; j < net->node_n; j++) {
				if (n != j && net->cedge[n][j] == 1) {
					sflag = 0;
					for (k = 0; k < c_n; k++)
						if (j == net->cluster[c][k])
							sflag = 1;

					if (sflag == 0) {
						net->cluster[c][c_n] = j;
						flag[j] = 1;
						c_n++;
					}
				}
			}
		}

		sum += c_n;
		net->cluster_num[c] = c_n;
		c_n = 0;
		c++;
		for (i = 0; i < net->node_n; i++) {
			if (flag[i] == 0) {
				net->cluster[c][c_n] = i;
				flag[i] = 1;
				c_n++;
				break;
			}
		}
	}

	net->cluster_ct = c;
}



void gng_asignedge(struct gng* net, struct gng* cnet)
{
	int i, j, k, l;

	for (i = 0; i < net->node_n; i++) {
		for (j = 0; j < net->node_n; j++) {
			if (i == j || net->edge[i][j] == 0) {
				net->cedge[i][j] = 0;
				net->cedge[j][i] = 0;
				continue;
			}
			double mindis = 0;
			for (k = 2; k < DIM; k++)
				mindis += (net->node[i][k] - cnet->node[0][k]) * (net->node[i][k] - cnet->node[0][k]);

			double dis = mindis;
			int s1 = 0;

			double mindis2 = 0;
			for (k = 2; k < DIM; k++)
				mindis2 += (net->node[j][k] - cnet->node[0][k]) * (net->node[j][k] - cnet->node[0][k]);

			double dis2 = mindis2;
			int s2 = 0;

			for (k = 1; k < cnet->node_n; k++) {
				dis = 0;
				for (l = 2; l < DIM; l++)
					dis += (net->node[i][l] - cnet->node[k][l]) * (net->node[i][l] - cnet->node[k][l]);

				if (dis < mindis) {
					mindis = dis;
					s1 = k;
				}

				dis2 = 0;
				for (l = 2; l < DIM; l++)
					dis2 += (net->node[j][l] - cnet->node[k][l]) * (net->node[j][l] - cnet->node[k][l]);

				if (dis2 < mindis2) {
					mindis2 = dis2;
					s2 = k;
				}
			}
			dis = 0;
			for (l = 2; l < DIM; l++)
				dis += (cnet->node[s1][l] - cnet->node[s2][l]) * (cnet->node[s1][l] - cnet->node[s2][l]);

			if (dis < 0.1 * 0.1) {
				net->cedge[i][j] = 1;
				net->cedge[j][i] = 1;
			}
			else {
				net->cedge[i][j] = 0;
				net->cedge[j][i] = 0;
			}
		}
	}
}


void discount_err_gng(struct gng* net)
{
	int i;
	//�S�Ẵm�[�h�̐ώZ�덷�̌���
	for (i = 0; i < net->node_n; i++) {
		net->gng_err[i] -= 0.0005 * net->gng_err[i];

		if (net->gng_err[i] < 0)
			net->gng_err[i] = 0.0;
	}
}

int node_delete(struct gng* net, int i)
{
	int j, k, l;

	//�אڊ֌W�������Ȃ��m�[�hi�̍폜
	for (j = i; j < net->node_n; j++) {

		net->gng_err[j] = net->gng_err[j + 1];
		for (l = 0; l < DIM; l++)
			net->node[j][l] = net->node[j + 1][l];
		net->edge_ct[j] = net->edge_ct[j + 1];

		net->contour[j] = net->contour[j + 1];	//0115

		for (k = 0; k < net->node_n; k++) {
			if (k < i) {
				net->age[j][k] = net->age[j + 1][k];
				net->age[k][j] = net->age[k][j + 1];
				net->edge[j][k] = net->edge[j + 1][k];
				net->edge[k][j] = net->edge[k][j + 1];

				net->cedge[j][k] = net->cedge[j + 1][k];
				net->cedge[k][j] = net->cedge[k][j + 1];
			}
			else {
				net->age[j][k] = net->age[j + 1][k + 1];
				net->edge[j][k] = net->edge[j + 1][k + 1];

				net->cedge[j][k] = net->cedge[j + 1][k + 1];
			}
		}
	}

	net->node_n--;
	return net->node_n;
}

int node_add_gng(struct gng* net, double total_err, int flag)
{
	int i;
	double max_err;
	double min_err;
	int q;
	int f;
	int r;
	int q2;

	//�ő�ώZ�덷�����m�[�h�̌���
	max_err = net->gng_err[0];
	min_err = net->gng_err[0];
	q = 0;
	q2 = 0;
	for (i = 1; i < net->node_n; i++) {
		if (max_err < net->gng_err[i]) {
			max_err = net->gng_err[i];
			q = i;
		}

		if (min_err > net->gng_err[i]) {
			min_err = net->gng_err[i];
			q2 = i;
		}
	}

	if (flag == 1) {
		if (net->node_n > 10 && min_err < NDEN) {	//MAPRATE*x=300���炢�ɂȂ�悤��(379�s�ڂ�)
			node_delete(net, q2);
		}

		return net->node_n;
	}

	//�m�[�hq�Ɨאڊ֌W�ɂ���m�[�h�̒��ōł��ώZ�덷���傫���m�[�h�̌���
	max_err = 0;
	f = GNGN;
	for (i = 0; i < net->node_n; i++) {
		if (net->edge[q][i] == 1 && q != i) {
			if (net->gng_err[i] >= max_err) {
				max_err = net->gng_err[i];
				f = i;
			}
		}
	}

	//�m�[�hr�̒ǉ�
	r = net->node_n;
	for (i = 0; i < DIM; i++)
		net->node[r][i] = 0.5 * (net->node[q][i] + net->node[f][i]);

	net->edge[q][f] = 0;
	net->edge[f][q] = 0;

	net->cedge[q][f] = 0;
	net->cedge[f][q] = 0;

	net->age[q][f] = 0;
	net->age[f][q] = 0;

	net->edge[q][r] = 1;
	net->edge[r][q] = 1;

	net->contour[r] = 0; //0115

	double dis = 0.0;
	for (i = 2; i < DIM; i++) {
		dis += (net->node[q][i] - net->node[r][i]) * (net->node[q][i] - net->node[r][i]);
	}

	if (dis < 0.2 * 0.2) {
		net->cedge[q][r] = 1;
		net->cedge[r][q] = 1;
	}

	net->age[q][r] = 0;
	net->age[r][q] = 0;

	net->edge[r][f] = 1;
	net->edge[f][r] = 1;

	dis = 0.0;
	for (i = 2; i < DIM; i++) {
		dis += (net->node[f][i] - net->node[r][i]) * (net->node[f][i] - net->node[r][i]);
	}

	if (dis < 0.2 * 0.2) {
		net->cedge[f][r] = 1;
		net->cedge[r][f] = 1;
	}

	net->age[r][f] = 0;
	net->age[f][r] = 0;

	net->gng_err[q] -= 0.5 * net->gng_err[q];
	net->gng_err[f] -= 0.5 * net->gng_err[f];

	net->gng_err[r] = 0.5 * (net->gng_err[q] + net->gng_err[f]);

	net->edge_ct[r] = 2;

	net->node_n++;

	if (net->node_n > 10 && min_err < NDEN) {
		node_delete(net, q2);
	}

	return 1;
}

int calc_age(int s1, int s2, struct gng* net)
{
	int i, j;
	int d_flag = 0;
	const int MAX_AGE = 88; //�N��̂������l

	/*0115*/
	double vx_s1[100], vy_s1[100];
	double pos_s1[2];
	int ct_s1 = 0;
	int cct_s1 = 0;
	double vx_s2[100], vy_s2[100];
	double pos_s2[2];
	int ct_s2 = 0;
	int cct_s2 = 0;
	double ang[100];
	double vx[100], vy[100];
	double pose[2];
	/*0115*/

	//��1���ҁC��2���҃m�[�h�Ɍ����֌W���Ȃ��ꍇ�́C�G�b�W��ǉ�����
	if (net->edge[s1][s2] == 0) {
		net->edge[s1][s2] = 1;
		net->edge[s2][s1] = 1;
		net->edge_ct[s1]++;
		net->edge_ct[s2]++;
	}

	double dis = 0.0;
	for (i = 2; i < DIM; i++) {
		dis += (net->node[s1][i] - net->node[s2][i]) * (net->node[s1][i] - net->node[s2][i]);
	}
	if (dis < 0.2 * 0.2) {
		net->cedge[s1][s2] = 1;
		net->cedge[s2][s1] = 1;
	}
	else {
		net->cedge[s2][s1] = 0;
		net->cedge[s1][s2] = 0;
	}

	/*0115*/
	pos_s1[0] = net->node[s1][0];
	pos_s1[1] = net->node[s1][1];
	pos_s2[0] = net->node[s2][0];
	pos_s2[1] = net->node[s2][1];

	for (i = 0; i < net->node_n; i++) {
		if (net->edge[s1][i] == 1) {
			vx_s1[ct_s1] = net->node[i][0];
			vy_s1[ct_s1] = net->node[i][1];
			if (net->contour[i] == 0)
				cct_s1++;
			ct_s1++;
		}
		if (net->edge[s2][i] == 1) {
			vx_s2[ct_s2] = net->node[i][0];
			vy_s2[ct_s2] = net->node[i][1];
			if (net->contour[i] == 0)
				cct_s2++;
			ct_s2++;
		}
	}

	net->contour[s1] = judge_contour(vx_s1, vy_s1, ang, pos_s1, ct_s1);
	net->contour[s2] = judge_contour(vx_s2, vy_s2, ang, pos_s2, ct_s2);
	if (cct_s1 == ct_s1)
		net->contour[s1] = 0;
	if (cct_s2 == ct_s2)
		net->contour[s2] = 0;
	/*0115*/

	//��1���ҁC��2���҃m�[�h�̌����֌W�̔N���0�ɂ���
	net->age[s1][s2] = 0;
	net->age[s2][s1] = 0;

	//��1���҃m�[�h�Ɨאڊ֌W�����G�b�W�̔N����C���N�������g����
	for (i = 0; i < net->node_n; i++) {
		if (i != s1 && i != s2) {
			if (net->edge[s1][i] == 1) {
				net->age[s1][i]++;
				net->age[i][s1]++;

				//�N�臒l���z�����ꍇ�͗אڊ֌W���폜����
				if (net->age[s1][i] > MAX_AGE) {
					net->age[s1][i] = 0;
					net->age[i][s1] = 0;
					net->edge[s1][i] = 0;
					net->edge[i][s1] = 0;

					net->cedge[s1][i] = 0;
					net->cedge[i][s1] = 0;

					net->edge_ct[s1]--;
					net->edge_ct[i]--;

					//�m�[�hi���אڊ֌W�������Ă��邩�ǂ����̔�����s��
					if (net->edge_ct[i] == 0) {
						net->node_n = node_delete(net, i);
						if (s1 > i)
							s1--;
						if (s2 > i)
							s2--;
					}
				}
				/*0115*/
				pose[0] = net->node[i][0];
				pose[1] = net->node[i][1];
				int ct = 0;
				int cct = 0;
				for (j = 0; j < net->node_n; j++) {
					if (net->edge[i][j] == 1) {
						vx[ct] = net->node[j][0];
						vy[ct] = net->node[j][1];
						if (net->contour[j] == 0)
							cct++;
						ct++;
					}
				}

				net->contour[i] = judge_contour(vx, vy, ang, pose, ct);
				if (cct == ct)
					net->contour[i] = 0;
				/*0115*/
			}
		}
	}

	return net->node_n;
}

void gng_learn(struct gng* net, int s1, int s2, double v[], double e1, double e3, int flag)
{
	int i, j;
	double e2 = e1 / 100.0, e4 = e3 / 100.0;     //�w�K�W��
	int d_flag = 0;
	const int MAX_AGE = 250; //�N��̂������l

	/*0115*/
	double vx_s1[100], vy_s1[100];
	double pos_s1[2];
	int ct_s1 = 0;
	int cct_s1 = 0;
	double vx_s2[100], vy_s2[100];
	double pos_s2[2];
	int ct_s2 = 0;
	int cct_s2 = 0;
	double ang[100];
	double vx[100], vy[100];
	double pose[2];
	/*0115*/

	//��1���ҁC��2���҃m�[�h�Ɍ����֌W���Ȃ��ꍇ�́C�G�b�W��ǉ�����
	if (net->edge[s1][s2] == 0) {
		net->edge[s1][s2] = 1;
		net->edge[s2][s1] = 1;
		net->edge_ct[s1]++;
		net->edge_ct[s2]++;
	}

	double dis = 0.0;
	for (i = 2; i < DIM; i++) {
		dis += (net->node[s1][i] - net->node[s2][i]) * (net->node[s1][i] - net->node[s2][i]);
	}
	if (dis < 0.2 * 0.2) {
		net->cedge[s1][s2] = 1;
		net->cedge[s2][s1] = 1;
	}
	else {
		net->cedge[s2][s1] = 0;
		net->cedge[s1][s2] = 0;
	}

	/*0115*/
	pos_s1[0] = net->node[s1][0];
	pos_s1[1] = net->node[s1][1];
	pos_s2[0] = net->node[s2][0];
	pos_s2[1] = net->node[s2][1];

	for (i = 0; i < net->node_n; i++) {
		if (net->edge[s1][i] == 1) {
			vx_s1[ct_s1] = net->node[i][0];
			vy_s1[ct_s1] = net->node[i][1];
			if (net->contour[i] == 0)
				cct_s1++;
			ct_s1++;
		}
		if (net->edge[s2][i] == 1) {
			vx_s2[ct_s2] = net->node[i][0];
			vy_s2[ct_s2] = net->node[i][1];
			if (net->contour[i] == 0)
				cct_s2++;
			ct_s2++;
		}
	}


	net->contour[s1] = judge_contour(vx_s1, vy_s1, ang, pos_s1, ct_s1);
	net->contour[s2] = judge_contour(vx_s2, vy_s2, ang, pos_s2, ct_s2);
	if (cct_s1 == ct_s1)
		net->contour[s1] = 0;
	if (cct_s2 == ct_s2)
		net->contour[s2] = 0;

	/*0115*/

	//��1���ҁC��2���҃m�[�h�̌����֌W�̔N���0�ɂ���
	net->age[s1][s2] = 0;
	net->age[s2][s1] = 0;

	//��1���ҁC��2���҃m�[�h�̍X�V���s��
	for (i = 0; i < 2; i++) {
		net->node[s1][i] += e1 * (v[i] - net->node[s1][i]);
	}

	for (j = 2; j < DIM; j++) {
		if (v[j] < 0)
			net->node[s1][j] += e1 * (v[j] - net->node[s1][j]);
		else
			net->node[s1][j] += e3 * (v[j] - net->node[s1][j]);
	}
	//��1���҃m�[�h�Ɨאڊ֌W�����m�[�h�̍X�V���s��
	for (i = 0; i < net->node_n; i++) {
		if (flag == 1) {
			net->gng_err[i] -= 0.0005 * net->gng_err[i];

			if (net->gng_err[i] < 0)
				net->gng_err[i] = 0.0;
		}
		if (net->edge[s1][i] == 1) {
			for (j = 0; j < 2; j++)
				net->node[i][j] += e2 * (v[j] - net->node[i][j]);

			if (net->cedge[s1][i] == 1) {
				for (j = 2; j < DIM; j++) {
					if (v[j] < 0)
						net->node[s1][j] += e2 * (v[j] - net->node[s1][j]);
					else
						net->node[s1][j] += e4 * (v[j] - net->node[s1][j]);
				}
			}
			net->age[s1][i]++;
			net->age[i][s1]++;

			//�N�臒l���z�����ꍇ�͗אڊ֌W���폜����
			if (net->age[s1][i] > MAX_AGE) {
				net->age[s1][i] = 0;
				net->age[i][s1] = 0;
				net->edge[s1][i] = 0;
				net->edge[i][s1] = 0;

				net->cedge[s1][i] = 0;
				net->cedge[i][s1] = 0;

				net->edge_ct[s1]--;
				net->edge_ct[i]--;

				//�m�[�hi���אڊ֌W�������Ă��邩�ǂ����̔�����s��
				if (net->edge_ct[i] == 0) {
					net->node_n = node_delete(net, i);
					if (s1 > i)
						s1--;
					if (s2 > i)
						s2--;
				}
			}
			/*0115*/
			pose[0] = net->node[i][0];
			pose[1] = net->node[i][1];
			int ct = 0;
			int cct = 0;
			for (j = 0; j < net->node_n; j++) {
				if (net->edge[i][j] == 1) {
					vx[ct] = net->node[j][0];
					vy[ct] = net->node[j][1];
					if (net->contour[j] == 0)
						cct++;
					ct++;
				}
			}

			net->contour[i] = judge_contour(vx, vy, ang, pose, ct);
			if (cct == ct)
				net->contour[i] = 0;
			/*0115*/
		}

		if (net->cedge[s1][i] == 1) {
			for (j = 2; j < DIM; j++) {
				if (v[j] < 0)
					net->node[i][j] += e2 * (v[j] - net->node[i][j]);
				else
					net->node[i][j] += e4 * (v[j] - net->node[i][j]);
			}
		}
	}
}

int gng_main(struct gng* net, double ve[][DIM], double vo[][DIM], int dmax[], int tt)
{
	int i, j;
	int s1, s2;
	double mindis, mindis2, dis[GNGN];
	const int lamda = 500;
	int t;
	double total_err = 0.0;
	double v[DIM];

	for (int k = 0; k < lamda; k++) {
		if (rnd() > 0.15) {
			while ((t = (int)((double)dmax[0] * rnd())) == dmax[0]);	//���̓f�[�^�̑I��
			for (i = 0; i < DIM; i++)
				v[i] = ve[t][i];
		}
		else {
			while ((t = (int)((double)dmax[1] * rnd())) == dmax[1]);    //���̓f�[�^�̑I��
			for (i = 0; i < DIM; i++)
				v[i] = vo[t][i];
		}
		//��1���҃m�[�h�̌���
		mindis = 0;

		dis[0] = 0;
		for (i = 0; i < 3; i++)
			dis[0] += (net->node[0][i] - v[i]) * (net->node[0][i] - v[i]);
		dis[1] = 0;
		for (i = 0; i < 3; i++)
			dis[1] += (net->node[1][i] - v[i]) * (net->node[1][i] - v[i]);

		if (dis[0] < dis[1]) {
			mindis = dis[0];
			s1 = 0;
			mindis2 = dis[1];
			s2 = 1;
		}
		else {
			mindis = dis[1];
			s1 = 1;
			mindis2 = dis[0];
			s2 = 0;
		}

		for (i = 2; i < net->node_n; i++) {
			dis[i] = 0;
			for (j = 0; j < 3; j++)
				dis[i] += (net->node[i][j] - v[j]) * (net->node[i][j] - v[j]);

			if (dis[i] < mindis) {
				mindis2 = mindis;
				s2 = s1;
				mindis = dis[i];
				s1 = i;
			}
			else if (dis[i] < mindis2) {
				mindis2 = dis[i];
				s2 = i;
			}
		}

		//�ώZ�덷�̌v�Z
		net->gng_err[s1] += mindis;
		total_err += mindis;
		//�m�[�h�̍X�V
		gng_learn(net, s1, s2, v, 0.05, 0.05, 1);
	}

	total_err /= (double)lamda;
	if (net->node_n < GNGN && total_err > NDEN) {
		node_add_gng(net, total_err, 0);
	}
	else {
		node_add_gng(net, total_err, 1);
	}

	for (int k = 0; k < lamda; k++) {
		if (rnd() > 0.15) {
			while ((t = (int)((double)dmax[0] * rnd())) == dmax[0]);    //���̓f�[�^�̑I��
			for (i = 0; i < DIM; i++)
				v[i] = ve[t][i];
		}
		else {
			while ((t = (int)((double)dmax[1] * rnd())) == dmax[1]);    //���̓f�[�^�̑I��
			for (i = 0; i < DIM; i++)
				v[i] = vo[t][i];
		}
		//��1���҃m�[�h�̌���
		mindis = 0;
		for (i = 0; i < DIM; i++)
			mindis += net->weight[i] * (net->node[0][i] - v[i]) * (net->node[0][i] - v[i]);
		dis[0] = mindis;
		s1 = 0;

		for (i = 1; i < net->node_n; i++) {
			dis[i] = 0;
			for (j = 0; j < DIM; j++)
				dis[i] += net->weight[j] * (net->node[i][j] - v[j]) * (net->node[i][j] - v[j]);

			if (dis[i] < mindis) {
				mindis = dis[i];
				s1 = i;
			}
		}

		//��2���҃m�[�h�̌���
		if (s1 != 0) {
			mindis2 = dis[0];
			s2 = 0;
		}
		else {
			mindis2 = dis[1];
			s2 = 1;
		}

		for (i = 0; i < net->node_n; i++) {
			if (dis[i] < mindis2 && i != s1) {
				mindis2 = dis[i];
				s2 = i;
			}
		}
		gng_learn(net, s1, s2, v, 0.006, 0.006, 0);
	}


	return t;
}
