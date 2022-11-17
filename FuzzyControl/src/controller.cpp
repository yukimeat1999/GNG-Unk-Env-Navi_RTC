/*
*  controller.cpp
*  slam
*
*  Created by Yuichiro Toda on 16/07/12 and modified by Wei Hong on 12/7/17.
*  Copyright 2012 首都大学東京. All rights reserved.
*
*/

#include "controller.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>


/*ファジィ推論IF-THENルール↓*/
double cal_weight(int r_num, double mu[]) {
	const int frif[RULENUM][MEMNUM] = { { 0,0,0,0,0 }/*1*/,{ 0,0,1,1,0 }/*2*/ ,{ 0,1,1,0,0 }/*3*/ ,{ 0,0,1,0,0 }/*4*/,{ 0,1,0,1,0 }/*5*/,{ 0,0,0,1,0 }/*6*/ ,{ 0,1,0,0,0 }/*7*/ ,{ 1,1,0,1,1 }/*8*/, 
										{ 1,1,0,0,0 }/*9*/, { 0,0,0,1,1 }/*10*/, { 1,0,0,0,0 }/*11*/, { 0,0,0,0,1 }/*12*/ ,{ 1,0,0,0,1 }/*13*/,{ 1,1,0,0,1 }/*14*/ ,{ 1,0,0,1,1 }/*15*/, { 0,1,1,1,0 }/*16*/ };
	



	double tlw = 1.0;//total weight 総合の重み(初期化)
	double x[MEMNUM];//メンバシップ関数の適応度
	for (int i = 0; i<MEMNUM; i++)
	{
		x[i] = 0;//各センサの適応度
		if (frif[r_num][i] == 0)
			x[i] = mu[i] + 0.001;
		else
			x[i] = 1.0 - mu[i] + 0.001;
		tlw *= x[i];//発火度 どんどん掛け算してくれる(総合の重み)
	}
	return tlw;
}

void fuzzy_inf(int min[], double outp[])//ファジィ推論
{


	const double frth[RULENUM][CTRLNUM] = { { 1.0,1.0 }/*1*/,{ 1.2,-0.5}/*2*/,{ -0.5,1.2}/*3*/,{ 0.6,0.8 }/*4*/,{ 1.0,1.0 }/*5*/,{ 1.0,0.2 }/*6*/,{ 0.2,1.0 }/*7*/,{ 1.0,1.0 }/*8*/,
											{ 0.4,0.8 }/*9*/,{ 0.8,0.4}/*10*/, { 1.0 ,0.8 }/*11*/,{ 0.8, 1.0 }/*12*/,{ 1.0,1.0 }/*13*/,{ 0.2,0.7}/*14*/,{ 0.7,0.2 }/*15*/,{ -0.5, -0.5 }/*16*/ };//fuzzy reference for文
	
	double f[RULENUM];
	double tlw = 0.0;
	double mu[5];//μ
	outp[0] = 0;//出力初期か
	outp[1] = 0;//出力初期か
	for (int i = 0; i<5; i++) {
		
		if (min[i] > MAXRANGE)
			mu[i] = 1.0;
		else
			mu[i] = (double)min[i] / (double)MAXRANGE;
	}

	for (int i = 0; i<RULENUM; i++) {
		f[i] = cal_weight(i, mu);
		tlw += f[i];
	}

	for (int i = 0; i<CTRLNUM; i++) {
		for (int j = 0; j<RULENUM; j++) {
			outp[i] += f[j] * frth[j][i];
		}
	}

	for (int i = 0; i<CTRLNUM; i++) {
		if (tlw > 0)
			outp[i] = outp[i] / tlw;
	}
}

/*ファジィ推論IF-THENルール↓*/

double calmemf3(int r_num, double det, double t_theta) {
	double tlw = 1.0;//total weight 総合の重み(初期化)
	int i;
	double ptrg = 0.3,
		ptrg2 = 1.0;
	double t_rad;

	const int frif1[RULENUM1][MEMNUM1] = {
		{ 0,0 }, // rule 0	Negative & S(目標が後ろにある時)
		{ 0,1 }, // rule 1	N & MS(Sより)
		{ 0,2 }, // rule 2	N & M
		{ 0,3 }, // rule 3	N & ML
		{ 0,4 }, // rule 4	N & L(近くにある時)
		{ 1,0 }, // rule 5	P & S
		{ 1,1}, // rule 6	P & MS
		{ 1,2 }, // rule 7	P & M
		{ 1,3 }, // rule 8	P & ML
		{ 1,4 }
	};//rule 9	P & L          fuzzy reference if文


	  // angle ------------------------------------------------------------
	  // Negative
	i = 0;
	t_rad = M_PI*t_theta / 180.0;
	if (frif1[r_num][i] == 0) {
		if (t_rad <-M_PI / 2)
			tlw *= 1.0;
		else
			tlw *= exp((-(t_rad + M_PI / 2)*(t_rad + M_PI / 2)) / 2.0);
	}
	// Positive
	else if (frif1[r_num][i] == 1) {
		if (t_rad > M_PI / 2)
			tlw *= 1.0;
		else
			tlw *= exp((-(t_rad - M_PI / 2)*(t_rad - M_PI / 2)) / 2.0);
	}


	// distance ---------------------------------------------------------
	// S
	i = 1;
	if (frif1[r_num][i] == 0) {
		if (det>-ptrg2)
			tlw *= exp(-(det + ptrg2)*(det + ptrg2) / 0.0045);	// mobir 0.0045
																//else if(det>0)
	}
	// MS
	else if (frif1[r_num][i] == 1) {
		tlw *= exp(-(det + ptrg)*(det + ptrg) / 0.002);
	}
	// M
	else if (frif1[r_num][i] == 2) {
		tlw *= exp(-det*det / 0.0002);
	}
	// ML
	else if (frif1[r_num][i] == 3) {
		tlw *= exp(-(det - ptrg)*(det - ptrg) / 0.002);
	}
	// L
	else if (frif1[r_num][i] == 4) {
		if (det< ptrg2)
			tlw *= exp(-(det - ptrg2)*(det - ptrg2) / 0.0045);
	}

	return tlw;
}

void fuzzyinf3(double det, double t_theta, double outp[])//ファジィ推論 OK
{
	double f[RULENUM1];
	int i, j;

	const double frth1[RULENUM1][CTRLNUM] = {
		{ 0.3,-0.3 }, // rule 0	N & S
		{0.7,-0.7}, // rule 1	N & MS
		{ 0,0 },   // rule 2	N & M
		{ 0.9, 0.3}, // rule 3	N & ML
		{ 0.9, 0.7 },// rule 4	N & L

		{ -0.3,0.3 }, // rule 5	P & S
		{-0.7,0.7},// rule 6	P & MS
		{ 0,0 },// rule 7	P & M
		{ 0.3, 0.9},// rule 8	P & ML
		{ 0.7,0.9} 
	};// rule 9	P & L  //fuzzy reference for文 {右車輪, 左車輪}
	for (i = 0; i<CTRLNUM; i++) {//for (ic=0;ic<cntl;ic++)
		outp[i] = 0;//outp[it][ib][ic]=0;	// init outputs
	}

	double tlw = 0.0;

	for (j = 0; j<RULENUM1; j++) {//for (ir=0;ir<10;ir++){
		f[j] = calmemf3(j, det, t_theta);//各ルールの適応度 f[ir]= calmemf3(it,ib,ir);
		for (i = 0; i<CTRLNUM; i++)
			outp[i] += f[j] * frth1[j][i];
		tlw += f[j];
	}

	for (i = 0; i<CTRLNUM; i++) {//for (ic=0;ic<cntl;ic++)
		if (tlw > 0)
			outp[i] = /*MAX_OUTPUT*/outp[i] / tlw;//total weightで正規化 outp[it][ib][ic] = outp[it][ib][ic]/tlw;
		//printf("%lf,", outp[i]);
	}
	//printf("\n");
}

//===================================================================================
double calmemf(int m, double *sdt)   	//  Fuzzy inference of rule m 
{
	int i;
	double	x[MEMF + 1],	// Fitness of membership function
		tlw;		// total weights

	const int frif[RULE + 1][MEMF + 1] = {	// Fuzzy Rules if parts (premise)
											//Collision Avoidance using　LRF
											//0,1,2,3,4
		{ 0,0,0,0,0 },	//Rule1:
		{ 1,0,0,0,0 },	//Rule2:
		{ 0,1,0,0,0 },	//Rule3:
		{ 0,0,1,0,0 },	//Rule4:
		{ 0,0,0,1,0 },	//Rule5:
		{ 0,0,0,0,1 },	//Rule6:
	};


	tlw = 1.0;
	for (i = 0; i<MEMF; i++) {
		x[i] = 0;
		if (frif[m][i] == 0)
			x[i] = sdt[i] / MAX_LRF_DISTANCE;
		else if (frif[m][i] == 1)
			x[i] = (MAX_LRF_DISTANCE - sdt[i]) / MAX_LRF_DISTANCE;
		tlw *= x[i];
	}
	return(tlw);
}

void fuzzyinf(double *sdt, double *outp)   	// Fuzzy inference  
{
	int		i, j;		// h: behavior No.
	double	f[RULE],
		tlw;		// total output, total weight 

	const double frth[RULE + 1][CNTL + 1] = {	// Fuzzy Rules then parts (conclusions)
		{ 0.6,   0.6 },	//Rule1:
		{ 0.8,   0.2 },	//Rule2:
		{ 0.5,  -0.5 },	//Rule3:
		{ -0.5,  -0.5 },	//Rule4:
		{ -0.5,   0.5 },	//Rule5:
		{ 0.2,   0.8 },	//Rule6:
	};

	for (j = 0; j<CNTL; j++)
		outp[j] = 0;	// init outputs

	tlw = 0;

	for (i = 0; i<RULE; i++) {
		f[i] = calmemf(i, sdt);
		tlw += f[i];
	}


	for (j = 0; j<CNTL; j++)
		for (i = 0; i<RULE; i++) {
			outp[j] += f[i] * frth[i][j];
		}

	for (j = 0; j<CNTL; j++) {
		if (tlw>0) {
			outp[j] /= tlw;
		}
		//outp[j] *= MAX_OUTPUT;
		if (outp[j] > MAX_OUTPUT)
			outp[j] = MAX_OUTPUT;
		//printf("%lf,", outp[j]);
	}
	//printf("\n");
}

//=========================================================================
// Multi-objective behaviour control------------------------------
#define BEHVNUM 2 // Number of Behaviour

double	cdng;		// Current degree of danger
double	wgt[BEHVNUM],	// Weight
dw[BEHVNUM][BEHVNUM];	// Update Matrix


void init_mobc()
{
	int ib, is;

	double init_dw[2][2] = {
		// CA   TTT
		{ 1.0,  0.0 },  // si[0]
		{ 0.0,  1.0 }   // si[1]
	};

	

	for (ib = 0; ib<BEHVNUM; ib++)
		for (is = 0; is<BEHVNUM; is++)
			dw[ib][is] = init_dw[ib][is];

	for (ib = 0; ib<BEHVNUM; ib++)
		wgt[ib] = 1 / 2.0;
}
//===================================================================================
void Update_wgt(double wgt2[], double si[BEHVNUM])											//重みアップデート
{
	int ib, is;
	double dwgt, wgtn;

	//更新式計算
	for (ib = 0; ib<BEHVNUM; ib++) {
		dwgt = 0.0;
		for (is = 0; is<BEHVNUM; is++) {
			dwgt += dw[ib][is] * si[is];
		}
		wgt[ib] = wgt[ib] * 1.0 + dwgt;		//係数 = dwgt反映比率調整
	}

	//忘却＋スケール調整

	wgtn = 0.0;
	for (ib = 0; ib<BEHVNUM; ib++)
		wgtn += wgt[ib];
	for (ib = 0; ib < BEHVNUM; ib++) {
		wgt[ib] /= wgtn;
		wgt2[ib] = wgt[ib];
	}
		

}
//===================================================================================
void cal_outp(double outp[CTRLNUM], double toutp[BEHVNUM][CTRLNUM])
{
	int ib, ic;
	double Sw, Swb;

	Sw = 0.0;
	for (ib = 0; ib<BEHVNUM; ib++)
		Sw += wgt[ib];
	for (ic = 0; ic<CTRLNUM; ic++) {
		Swb = 0.0;
		for (ib = 0; ib<BEHVNUM; ib++)
			Swb += wgt[ib] * toutp[ib][ic];
		outp[ic] = Swb / Sw;

	}
}
