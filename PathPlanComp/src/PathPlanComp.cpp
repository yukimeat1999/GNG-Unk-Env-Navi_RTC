// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  PathPlanComp.cpp
 * @brief wanna play Apex
 *
 */
// </rtc-template>

#include "PathPlanComp.h"
#include <Windows.h>
using namespace cv;

// Module specification
// <rtc-template block="module_spec">
static const char* const pathplancomp_spec[] =
  {
    "implementation_id", "PathPlanComp",
    "type_name",         "PathPlanComp",
    "description",       "wanna play Apex",
    "version",           "1.1.1",
    "vendor",            "VenderName",
    "category",          "Controller",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
PathPlanComp::PathPlanComp(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_GoalIn("Goal", m_Goal),
    m_PositionIn("Position", m_Position),
    m_TopoMapIn("TopoMap", m_TopoMap),
    m_DirectionOut("Direction", m_Direction)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PathPlanComp::~PathPlanComp()
{
}

//グローバル変数の定義
Mat gngImage;

int GoalSet = 0;
int PosSet = 0;
int MapSet = 0;
int targetflag = 1;
int move_flag;
int isUnknown = 0;
struct gng* net1 = NULL;
struct robot_position* robot_pos1;

RTC::ReturnCode_t PathPlanComp::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("Goal", m_GoalIn);
  addInPort("Position", m_PositionIn);
  addInPort("TopoMap", m_TopoMapIn);
  
  // Set OutPort buffer
  addOutPort("Direction", m_DirectionOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  GoalSet = 0;
  PosSet = 0;
  MapSet = 0;
  targetflag = 1;
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PathPlanComp::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t PathPlanComp::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanComp::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}

//funcions----------------------------------------------------------------------------------
double DIST[GNGN][GNGN];
double COST[GNGN];
int PATH[GNGN];
char USED[GNGN];
double route[GNGN][3];
int nodenum[GNGN];
int map[MAPSIZE][MAPSIZE];
double occ[GNGN];



int mtx, mty;   //map position of target
int tx, ty;     //real-space position of the target
int nextnode;
int r1, g1;
double pathdis = 0;	//計画した経路距離
int fpath[1000][2];
int path_ct;
int pathcount = 0;
double nodedis;
double a = 1.0;
int b = 0;
int c = 0;
int rposcount = 0;
cv::Point rpt4, rpt5, rpt8;

void Draw_GNG(Mat frame, struct gng* net)
{
    int i, j;
    int node_num = 0;
    double rgb;
    cv::Point rpt, rpt2, rpt3;
    for (i = 0; i < net->node_n; i++) {
        rpt.x = net->node[i][0];
        rpt.y = net->node[i][1];
        rgb = 1.0 - 0.5 * (net->node[i][2] + 1.0);
        for (j = 0; j < net->node_n; j++) {

            if (net->edge[i][j] == 1 && net->edge[j][i] == 1) {

                rpt2.x = net->node[j][0];
                rpt2.y = net->node[j][1];
                cv::line(frame, rpt, rpt2, Scalar((int)(255.0 * rgb), (int)(255.0 * rgb), (int)(255.0 * rgb)), 1, 8, 0);

            }

        }
        if (net->contour[i] == 0)
            cv::circle(frame, rpt, 2, Scalar((int)(255.0 * rgb), (int)(255.0 * rgb), (int)(255.0 * rgb)), -1, 8, 0);
        else
            cv::circle(frame, rpt, 2, Scalar(0, 255, 0), -1, 8, 0);

    }
}


int dijkstra(int start, int goal)
{
    int min, target = 0;

    COST[start] = 0;

    while (1) {

        /* 未確定の中から距離が最も小さい地点を選んで、その地点の最小距離として確定 */
        min = 99999.0;
        for (int i = 0; i < net1->node_n; i++) {
            if (!USED[i] && min > COST[i]) {
                min = COST[i];
                target = i;
            }
        }

        /* 全ての地点の最短経路が確定 */
        if (target == goal)
            return COST[goal];

        /* 今確定した場所から「直接つながっている」かつ「未確定の」地点に関して、
        今確定した場所を経由した場合の距離を計算し、今までの距離よりも小さければ書き直します。 */

        for (int j = 0; j < net1->node_n; j++) {
            if (COST[j] > DIST[target][j] + COST[target]) {
                COST[j] = DIST[target][j] + COST[target];
                PATH[j] = target;
            }
        }

        USED[target] = TRUE;
    }
}

void search_path2(int s, int g)
{
    int y = PATH[g];
    int prex;

    double x1, y1, xx, yy;
    cv::Point rpt6, rpt7, rpt8;

    rpt6.x = net1->node[g][0];
    rpt6.y = net1->node[g][1];
    rpt7.x = net1->node[y][0];
    rpt7.y = net1->node[y][1];

    x1 = (float)(-CENTER + net1->node[g][0]) * MAPRATE;
    y1 = (float)(CENTER - net1->node[g][1]) * MAPRATE;
    xx = (float)(-CENTER + net1->node[y][0]) * MAPRATE;
    yy = (float)(CENTER - net1->node[y][1]) * MAPRATE;

    pathdis = sqrt((xx - x1) * (xx - x1) + (yy - y1) * (yy - y1));

    if (y == s) {
        nextnode = g;
    }

    cv::line(gngImage, rpt6, rpt7, cv::Scalar(0, 0, 255), 4, 8);

    route[0][0] = net1->node[g][0];
    route[0][1] = net1->node[g][1];
    route[0][2] = net1->node[g][2];
    nodenum[0] = g1;
    route[1][0] = net1->node[y][0];
    route[1][1] = net1->node[y][1];
    route[1][2] = net1->node[y][2];
    nodenum[1] = y;

    pathcount = 2;

    prex = y;

    while (y != s)
    {
        y = PATH[y];

        if (PATH[prex] == s)
        {
            nextnode = prex;
            rpt8.x = net1->node[nextnode][0];
            rpt8.y = net1->node[nextnode][1];
            cv::circle(gngImage, rpt8, 8, CV_RGB(255, 0, 0), -1, 8, 0);
        }

        rpt6.x = net1->node[prex][0];
        rpt6.y = net1->node[prex][1];
        rpt7.x = net1->node[y][0];
        rpt7.y = net1->node[y][1];

        line(gngImage, rpt6, rpt7, cv::Scalar(0, 0, 255), 4, 8);

        x1 = (float)(-CENTER + net1->node[prex][0]) * MAPRATE;
        y1 = (float)(CENTER - net1->node[prex][1]) * MAPRATE;
        xx = (float)(-CENTER + net1->node[y][0]) * MAPRATE;
        yy = (float)(CENTER - net1->node[y][1]) * MAPRATE;

        pathdis += sqrt((xx - x1) * (xx - x1) + (yy - y1) * (yy - y1));

        route[pathcount][0] = net1->node[y][0];
        route[pathcount][1] = net1->node[y][1];
        route[pathcount][2] = net1->node[y][2];
        nodenum[pathcount] = y;

        robot_pos1->path[pathcount][0] = route[pathcount][0];
        robot_pos1->path[pathcount][1] = route[pathcount][1];

        prex = y;
        pathcount++;
    }
    route[pathcount][0] = net1->node[s][0];
    route[pathcount][1] = net1->node[s][1];
    route[pathcount][2] = net1->node[s][2];
    nodenum[pathcount] = s;
    robot_pos1->path_ct = pathcount;
}

void calc_dijkstra(int r1, int g1, double a)
{

    static int t = 0;
    double risk = 0;
    double risk_i = 0;
    double risk_j = 0;

    for (int i = 0; i < net1->node_n; i++) {
        COST[i] = 99999.0;
        USED[i] = FALSE;
        PATH[i] = -1;
        for (int j = 0; j < net1->node_n; j++)
            DIST[i][j] = 99999.0;
    }

    if (t == 0) {

        // 初期値(エッジの重み)設定
        for (int i = 0; i < net1->node_n; i++) {
            for (int j = 0; j < net1->node_n; j++) {

                if (i == j) {
                    DIST[i][j] = 0;
                }
                else if (net1->edge[i][j] == 1) {

                    b = 2;
                    c = 1;
                    risk = 0;
                    risk_i = 0;
                    risk_j = 0;


                    for (int k = 0; k < net1->node_n; k++) {
                        if (net1->edge[i][k] == 1 && k != j) {	//i近傍の占有度の和
                            risk_i += occ[k];
                            b++;
                        }
                        if (net1->edge[j][k] == 1 && k != i) {	//j近傍の占有度の和
                            risk_j += occ[k];
                            b++;
                        }
                    }

                    nodedis = sqrt((net1->node[j][0] - net1->node[i][0]) * (net1->node[j][0] - net1->node[i][0]) + (net1->node[j][1] - net1->node[i][1]) * (net1->node[j][1] - net1->node[i][1]));

                    risk = occ[j] + occ[i] + risk_j + risk_i;


                    DIST[i][j] = a * risk / b + nodedis;

                }
                else {
                    DIST[i][j] = 99999.0;
                }
            }
        }

        dijkstra(r1, g1);
    }
    search_path2(r1, g1);
}
void select_node(int mmtx, int mmty)
{

    //ロボット位置のノード選択
    int robotpos[DIM] = { 0 };
    int targetpos[DIM] = { 0 };

    double mindis, mindis2, dis[GNGN];
    int i, j;

    robotpos[0] = robot_pos1->map_x;
    robotpos[1] = robot_pos1->map_y;
    mindis = 0;
    for (i = 0; i < 2; i++)
        mindis += (net1->node[0][i] - robotpos[i]) * (net1->node[0][i] - robotpos[i]);
    dis[0] = mindis;
    r1 = 0;

    for (i = 1; i < net1->node_n; i++) {
        dis[i] = 0;
        for (j = 0; j < 2; j++)
            dis[i] += (net1->node[i][j] - robotpos[j]) * (net1->node[i][j] - robotpos[j]);

        if (dis[i] < mindis) {
            mindis = dis[i];
            r1 = i;
        }
    }

    rpt4.x = net1->node[r1][0];
    rpt4.y = net1->node[r1][1];
    circle(gngImage, rpt4, 8, CV_RGB(0, 255, 255), -1, 8, 0);

    //目的地のノード選択

    if (isUnknown == 0)
    {

        targetpos[0] = mmtx;
        targetpos[1] = mmty;
        mindis = 0;
        for (i = 0; i < 2; i++)
            mindis += (net1->node[0][i] - targetpos[i]) * (net1->node[0][i] - targetpos[i]);
        dis[0] = mindis;
        g1 = 0;

        for (i = 1; i < net1->node_n; i++) {
            dis[i] = 0;
            for (j = 0; j < 2; j++)
                dis[i] += (net1->node[i][j] - targetpos[j]) * (net1->node[i][j] - targetpos[j]);

            if (dis[i] < mindis) {
                mindis = dis[i];
                g1 = i;
            }
        }
    }
    else if (isUnknown == 1) {
        targetpos[0] = mmtx;
        targetpos[1] = mmty;
        mindis = 99999.0;
        mindis2 = 99999.0;
        g1 = -1;


        for (i = 0; i < net1->node_n; i++) {
            dis[i] = 0;
            for (j = 0; j < 2; j++)
                dis[i] += (net1->node[i][j] - targetpos[j]) * (net1->node[i][j] - targetpos[j]);

            if (dis[i] < mindis2 && net1->node[i][2] < -0.8) {
                mindis2 = dis[i];
            }
            if (net1->contour[i] == 1 && net1->node[i][2] < -0.8) {
                if (dis[i] < mindis) {
                    mindis = dis[i];
                    g1 = i;
                }
            }
        }
        if (mindis2 < 6.0 * 6.0 && g1 != -1) {
            isUnknown = 0;
        }
    }

    if (g1 != -1) {
        rpt5.x = net1->node[g1][0];
        rpt5.y = net1->node[g1][1];
        circle(gngImage, rpt5, 8, CV_RGB(0, 255, 0), -1, 8, 0);
    }

}

//-------------------------------------------------------------------------

RTC::ReturnCode_t PathPlanComp::onActivated(RTC::UniqueId /*ec_id*/)
{
    m_TopoMap.node_num = 0;
    m_TopoMap.node.length(GNGN * 3);
    m_TopoMap.edge.length(GNGN * (GNGN + 1));

    net1 = init_gng();
    robot_pos1 = initialize_RobotPosition(robot_pos1, 0);

    //cv::Matの初期化
    gngImage = cv::Mat::zeros(MAPSIZE, MAPSIZE, CV_8UC3);
    rectangle(gngImage, cv::Point(0, 0), cv::Point(MAPSIZE, MAPSIZE), Scalar(125, 125, 125), -1, 8, 0);
    
    return RTC::RTC_OK;
}


RTC::ReturnCode_t PathPlanComp::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t PathPlanComp::onExecute(RTC::UniqueId /*ec_id*/)
{
    //get coordinates of the goal
    if (m_GoalIn.isNew())
    {
        m_GoalIn.read();
        mtx = m_Goal.data.position.x;
        mty = m_Goal.data.position.y;
        if (mtx > 10000) {
            mtx -= 10000;
            mty -= 10000;
            isUnknown = 1;
        }
        else {
            isUnknown = 0;
        }
        tx = (float)(-CENTER + mtx) * MAPRATE;
        ty = (float)(CENTER - mty) * MAPRATE;

        GoalSet = 1;

    }

    if (m_TopoMapIn.isNew())
    {
        m_TopoMapIn.read();
        rectangle(gngImage, cv::Point(0, 0), cv::Point(MAPSIZE, MAPSIZE), Scalar(125, 125, 125), -1, 8, 0);
        net1->node_n = m_TopoMap.node_num;
        for (int i = 0; i < net1->node_n; i++)  //read node number
        {
            occ[i] = m_TopoMap.node[i] + 1;

        }

        int k = 0;
        for (int i = 0; i < net1->node_n; i++)  //read edge
        {
            for (int j = 0; j < net1->node_n; j++)
            {
                net1->edge[i][j] = m_TopoMap.edge[k];
                k++;
            }
            net1->contour[i] = m_TopoMap.edge[GNGN * (GNGN - 1) + i];
        }
        k = 0;

        for (int i = 0; i < net1->node_n; i++)  //read edge
        {
            for (int j = 0; j < 3; j++)
            {
                net1->node[i][j] = m_TopoMap.node[k];
                k++;
            }
        }
        MapSet = 1;
        Draw_GNG(gngImage, net1);

    }


    if (m_PositionIn.isNew())
    {
        m_PositionIn.read();
        robot_pos1->map_x = CENTER + (int)m_Position.data.position.x / MAPRATE;
        robot_pos1->map_y = CENTER - (int)m_Position.data.position.y / MAPRATE;
        PosSet = 1;

    }



    if (GoalSet * PosSet * MapSet == 1)
    {

        select_node(mtx, mty); //calculate r1g1
        if (targetflag == 1)
        {
            static int stop_num = 0;
            if (g1 != -1)
            {
                if (r1 != g1)
                    calc_dijkstra(r1, g1, a);
                else if (r1 == g1)
                    pathdis = 0;
                stop_num = 0;
            }
            else
            {
                stop_num++;
                if (stop_num > 2)
                {
                    targetflag = 0;
                    move_flag = 0;
                }
            }
        }


    }


    //目標位置追従ここから
    if (GoalSet == 1)
    {

        double xem, yem;
        double txm, tym;
        double targetx, targety;

        txm = (float)(-CENTER + mtx) * MAPRATE;
        tym = (float)(CENTER - mty) * MAPRATE;
        tx = (-CENTER + net1->node[nextnode][0]) * MAPRATE;
        ty = (CENTER - net1->node[nextnode][1]) * MAPRATE;

        targetx = net1->node[nextnode][0];
        targety = net1->node[nextnode][1];

        if (nextnode == g1)
        {
            tx = (float)(-CENTER + mtx) * MAPRATE;
            ty = (float)(CENTER - mty) * MAPRATE;
            targetx = mtx;
            targety = mty;
        }

        m_Direction.data.position.x = targetx;
        m_Direction.data.position.y = targety;
        m_DirectionOut.write();
    }



    imshow("GNG_PathPlan", gngImage);
    waitKey(1);
    
    return RTC::RTC_OK;
}


//RTC::ReturnCode_t PathPlanComp::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanComp::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanComp::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanComp::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanComp::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void PathPlanCompInit(RTC::Manager* manager)
  {
    coil::Properties profile(pathplancomp_spec);
    manager->registerFactory(profile,
                             RTC::Create<PathPlanComp>,
                             RTC::Delete<PathPlanComp>);
  }
  
}
