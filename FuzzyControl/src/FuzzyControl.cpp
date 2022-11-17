// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  FuzzyControl.cpp
 * @brief FuzzyControlComponent
 *
 */
// </rtc-template>

#include "FuzzyControl.h"
#include "controller.h"
#include "slam.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const fuzzycontrol_spec[] =
#else
static const char* fuzzycontrol_spec[] =
#endif
  {
    "implementation_id", "FuzzyControl",
    "type_name",         "FuzzyControl",
    "description",       "FuzzyControlComponent",
    "version",           "1.0.0",
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
FuzzyControl::FuzzyControl(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_obstacleDistanceIn("obstacleDistance", m_obstacleDistance),
    m_localTargetIn("localTarget", m_localTarget),
    m_RobotPositionIn("RobotPosition", m_RobotPosition),
    m_GoalPositionIn("GoalPosition", m_GoalPosition),
    m_wheelLeftOut("wheelLeft", m_wheelLeft),
    m_wheelRightOut("wheelRight", m_wheelRight)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
FuzzyControl::~FuzzyControl()
{
}



RTC::ReturnCode_t FuzzyControl::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("obstacleDistance", m_obstacleDistanceIn);
  addInPort("localTarget", m_localTargetIn);
  addInPort("RobotPosition", m_RobotPositionIn);
  addInPort("GoalPosition", m_GoalPositionIn);
  
  // Set OutPort buffer
  addOutPort("wheelLeft", m_wheelLeftOut);
  addOutPort("wheelRight", m_wheelRightOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FuzzyControl::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t FuzzyControl::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControl::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}

//変数の定義

static bool init_flag;
double outp_obstacle[2];
double outp_tracking[2];
double toutp[2][2];
double outp_fin[2];
int move_flag = 0;

//障害物回避用変数
long Lrf_data[8192];
double Lrf_theta[8192];
static size_t datasize = 768;

//目標追従用変数
double det; //次のノード距離
double relative_angle;  //ゴール-ロボット相対角度

//多目的行動調停用変数
double distance;    //ゴール-ロボット間距離

double min; //最も近い障害物距離
double min_num; //minの方向
double wgt2[BEHVNUM];   //各ルールの重み
double si[BEHVNUM]; //環境知覚変数
double tx;  //次のノードx座標
double ty;  //次のノードy座標
double gx; //ゴールのx座標
double gy; //ゴールのy座標
double rx; //ロボットの現在位置
double ry; //ロボットの現在位置
double rh; //ロボットの現在位置
double lx; //目標追従のx座標
double ly; //目標追従のy座標
int goflag[5] = { 0,0,0,0,0 };
static int goalflag = 0;
static int roboflag = 0;
static int localflag = 0;
static int obstaflag = 0;

RTC::ReturnCode_t FuzzyControl::onActivated(RTC::UniqueId /*ec_id*/)
{

    static bool init_flag = true;
    tx = 0;
    ty = 0;
    gx = 0;
    gy = 0;
    rx = 0;
    ry = 0;
    lx = 0;
    ly = 0;
    goalflag = 0;


    for (int i = 0; i < datasize; i++) {
        Lrf_theta[i] = (-(double)(i - datasize / 2) * 0.351562);
    }

    //重みの初期化
    init_mobc();

  return RTC::RTC_OK;
}


RTC::ReturnCode_t FuzzyControl::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}



RTC::ReturnCode_t FuzzyControl::onExecute(RTC::UniqueId /*ec_id*/)
{
    int roboflag = 0;
    int localflag = 0;
    int obstaflag = 0;
    int min_dis[MEMF] = { MAXRANGE,MAXRANGE,MAXRANGE,MAXRANGE,MAXRANGE };

    if (m_GoalPositionIn.isNew()) {
        m_GoalPositionIn.read();
        gx = m_GoalPosition.data.position.x;
        gy = m_GoalPosition.data.position.y;

        if (gx > 10000) {
            gx -= 10000;
            gy -= 10000;
        }

        //実空間座標に変換
        gx = (-CENTER + gx) * MAPRATE;
        gy = (CENTER - gy) * MAPRATE;
        goalflag = 1;
        goflag[0] = 1;
    }

    if (m_RobotPositionIn.isNew()) {
        m_RobotPositionIn.read();
        rx = m_RobotPosition.data.position.x;
        ry = m_RobotPosition.data.position.y;
        rh = m_RobotPosition.data.heading;
        roboflag = 1;
        goflag[1] = 1;
    }

    if (m_localTargetIn.isNew()) {
        m_localTargetIn.read();
        lx = m_localTarget.data.position.x;
        ly = m_localTarget.data.position.y;

        //実空間座標に変換
        tx = (-CENTER + lx) * MAPRATE;
        ty = (CENTER - ly) * MAPRATE;
        localflag = 1;
        goflag[2] = 1;
    }

    if (m_obstacleDistanceIn.isNew()) {
        m_obstacleDistanceIn.read();
        
        // m_obstacleDataをfuzzy関数の変数に変換
        int s_pos = 0;
        for (int pos = 0; pos < datasize; ++pos) {
            if (pos >= 44 && pos <= 725) {
                Lrf_data[pos] = (long)(m_obstacleDistance.ranges[s_pos] * 1000.0);
                s_pos++;
            }
            else {
                Lrf_data[pos] = -1;
            }
        }
        obstaflag = 1;
        goflag[3] = 1;
    }

    if (goflag[0] == 1 && goflag[1] == 1 && goflag[2] == 1 && goflag[3] == 1) {
        goflag[4] = 1;
    }


    move_flag = 1;

    //-----障害物回避ここから-----
    //min_disを計算
    for (int pos = 0; pos < (int)datasize; ++pos) {

        const int ROBOT_RADIUS = 170;
        if ((Lrf_theta[pos] < 27 && Lrf_theta[pos] > -27) && Lrf_data[pos] - ROBOT_RADIUS >= 0) {
            if (Lrf_data[pos] - ROBOT_RADIUS < min_dis[2]) {
                min_dis[2] = Lrf_data[pos] - ROBOT_RADIUS;
            }
        }
        else if (28 < Lrf_theta[pos] && Lrf_theta[pos] < 81 && Lrf_data[pos] - ROBOT_RADIUS >= 0) {
            if (Lrf_data[pos] - ROBOT_RADIUS < min_dis[3]) {
                min_dis[3] = Lrf_data[pos] - ROBOT_RADIUS;
            }
        }
        else if (81 < Lrf_theta[pos] && Lrf_theta[pos] < 135 && Lrf_data[pos] - ROBOT_RADIUS >= 0) {
            if (Lrf_data[pos] - ROBOT_RADIUS < min_dis[4]) {
                min_dis[4] = Lrf_data[pos] - ROBOT_RADIUS;
            }
        }
        else if (-135 < Lrf_theta[pos] && Lrf_theta[pos] < -81 && Lrf_data[pos] - ROBOT_RADIUS >= 0) {
            if (Lrf_data[pos] - ROBOT_RADIUS < min_dis[0]) {
                min_dis[0] = Lrf_data[pos] - ROBOT_RADIUS;
            }
        }
        else if (-81 < Lrf_theta[pos] && Lrf_theta[pos] < -27 && Lrf_data[pos] - ROBOT_RADIUS >= 0) {
            if (Lrf_data[pos] - ROBOT_RADIUS < min_dis[1]) {
                min_dis[1] = Lrf_data[pos] - ROBOT_RADIUS;
            }
        }
    }
    //障害物回避の出力を計算
    fuzzy_inf(min_dis, outp_obstacle);

    //----目標追従ここから----

    //relative_angleを計算
    relative_angle = 180.0 * atan2(ty - ry, tx - rx) / M_PI - 90.0;
    relative_angle = transformation_angle(relative_angle);
    relative_angle = -relative_angle + rh;
    relative_angle = transformation_angle(relative_angle);

    //detを計算
    det = sqrt((tx - rx) * (tx - rx) + (ty - ry) * (ty - ry));
    if (fabs(relative_angle) > 90)
        det *= -1.0;

    fuzzyinf3(det, relative_angle, outp_tracking);

    //----多目的行動調停ここから----
    // distanceを計算
    distance = (gx - rx) * (gx - rx) + (gy - ry) * (gy - ry);
    distance = sqrt(distance);

    //minを計算
    min = min_dis[0];
    for (int i = 0; i < 5; i++) {
        if (min > min_dis[i]) {
            min = min_dis[i];
            min_num = i;
        }
    }
    min /= MAXRANGE;

    //siを計算
    if (min < 0.3) {
        si[0] = 0.3 / min - 1; //Min=min/1000;障害物までの最小距離(m)0-1
        si[1] = fabs(relative_angle) / 180;
        if (si[0] > 1) {
            si[0] = 1;
        }
    }
    else {
        si[0] = 0;
        si[1] = 1;
    }

    if (fabs(distance) <= 600) {
        si[0] = 0;
        si[1] = 1;
    }
    //ゴール至近距離でストップ
    if (fabs(distance) < 300) {
        move_flag = 0;
    }

    //各ルールの重みを計算
    Update_wgt(wgt2, si);

    //各ルールの出力をtoutpに代入
    for (int i = 0; i < CTRLNUM; i++) {
        toutp[0][i] = outp_obstacle[i];//障害物回避
        toutp[1][i] = outp_tracking[i];//目的地追従
    }
    //最終出力を計算
    cal_outp(outp_fin, toutp);

    if (move_flag == 0) {
        outp_fin[0] = 0.0;
        outp_fin[1] = 0.0;
        goalflag = 0;
        goflag[0] = 0;
        goflag[4] = 0;
    }

    printf("===================================");
    printf("move_flag is %d\n", move_flag);
    printf("output_fin[0] is %f\n", outp_fin[0]);
    printf("output_fin[1] is %f\n", outp_fin[1]);

    //出力データを入力
    m_wheelLeft.data.vx = outp_fin[1];
    m_wheelRight.data.vx = outp_fin[0];

    // 出力データをOutPortから出力する
    m_wheelLeftOut.write();
    m_wheelRightOut.write();

    return RTC::RTC_OK;
}


//RTC::ReturnCode_t FuzzyControl::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControl::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControl::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControl::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControl::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void FuzzyControlInit(RTC::Manager* manager)
  {
    coil::Properties profile(fuzzycontrol_spec);
    manager->registerFactory(profile,
                             RTC::Create<FuzzyControl>,
                             RTC::Delete<FuzzyControl>);
  }
  
}
