/*
 *  gng.h
 *  Cluster
 *
 *  Created by Yuichiro Toda on 12/05/18.
 *  Copyright 2012 首都大学東京. All rights reserved.
 *
 */
#pragma once
#define GNGN 500	//最大ノード数
#define DIM 3		//ベクトルの次元数

struct gng {
	double node[GNGN][DIM];		//ノード用配列
	int edge[GNGN][GNGN];		//エッジ用配列
	int cedge[GNGN][GNGN];		//エッジ用配列
	int age[GNGN][GNGN];		//エッジの年齢用配列
	int node_n;					//現在のノード数
	int edge_ct[GNGN];        //エッジの個数カウント用
	int cluster[GNGN][GNGN];
	int cluster_num[GNGN];
	int cluster_ct;
	double gng_err[GNGN];		//積算誤差用配列

	double weight[DIM];     //関連重要度

	int contour[GNGN];        //輪郭 0115

	int property;
};

struct gng* init_gng();

