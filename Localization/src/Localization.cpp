// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Localization.cpp
 * @brief SLAM and Localization
 *
 * @author Y.Fujii
 *
 */
// </rtc-template>

#include "Localization.h"

//slam.h
#include "slam.h"

extern double MAPRATE;
extern int T2, GANM;

// Module specification
// <rtc-template block="module_spec">
static const char* const localization_spec[] =
  {
    "implementation_id", "Localization",
    "type_name",         "Localization",
    "description",       "SLAM and Localization",
    "version",           "1.0.5",
    "vendor",            "Y. Fujii",
    "category",          "Navigation",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.MAPRATE", "50",
    "conf.default.GANM", "600",
    "conf.default.T2", "2400",

    // Widget
    "conf.__widget__.MAPRATE", "text",
    "conf.__widget__.GANM", "text",
    "conf.__widget__.T2", "text",
    // Constraints

    "conf.__type__.MAPRATE", "int",
    "conf.__type__.GANM", "int",
    "conf.__type__.T2", "int",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Localization::Localization(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rangeIn("range", m_range),
    m_omapOut("omap", m_omap),
    m_robot_positionOut("robot_position", m_robot_position)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Localization::~Localization()
{
}



RTC::ReturnCode_t Localization::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("range", m_rangeIn);
  
  // Set OutPort buffer
  addOutPort("omap", m_omapOut);
  addOutPort("robot_position", m_robot_positionOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("MAPRATE", m_MAPRATE, "50");
  bindParameter("GANM", m_GANM, "600");
  bindParameter("T2", m_T2, "2400");
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Localization::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Localization::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Localization::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Localization::onActivated(RTC::UniqueId /*ec_id*/)
{
    // Prtの初期化
    m_robot_position.data.position.x = 0;
    m_robot_position.data.position.y = 0;
    m_robot_position.data.heading = 0;
    m_omap.data.length(MAPSIZE * MAPSIZE);
    //m_omap.data = { 0 };

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Localization::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Localization::onExecute(RTC::UniqueId /*ec_id*/)
{
    MAPRATE = m_MAPRATE;
    GANM = m_GANM;
    T2 = m_T2;

    // 宣言
    static struct robot_position* robot_pos = NULL;
    long Lrf_data[8192] = { 0 };
    static double Lrf_theta[8192] = { 0 };
    static double input_vec_e[MAPSIZE * MAPSIZE][DIM];
    static double input_vec_o[MAPSIZE * MAPSIZE][DIM];
    static int dct[2] = { 0 };
    static size_t count = 768;
    static int omap[MAPSIZE][MAPSIZE];
    static int amap[MAPSIZE][MAPSIZE];

    if (robot_pos == NULL) {
        int datasize = count;
        robot_pos = (struct robot_position*)malloc(sizeof(struct robot_position));
        initialize_RobotPosition(robot_pos, 0);
        for (int i = 0; i < datasize; i++) {
            Lrf_theta[i] = (-(double)(i - datasize / 2) * 0.351562 * M_PI) / 180.0;
        }
    }

    // 新しいデータのチェック
    if (m_rangeIn.isNew()) {
        // InPortデータの読み込み
        m_rangeIn.read();
        // m_rangeをslam関数の変数に変換
        int s_pos = 0;
        for (int pos = 0; pos < count; ++pos) {
            if (pos >= 44 && pos <= 725) {
                Lrf_data[pos] = (long)(m_range.ranges[s_pos] * 1000.0);
                s_pos++;
            }else
                Lrf_data[pos] = 0;
        }
        // SLAM
        int tcount = slam(robot_pos, Lrf_data, Lrf_theta, count, omap);
        //printf("==========================");
        // omapを二次元配列から一次元配列に変換
        int k = 0;
        for (int i = 0; i < MAPSIZE; ++i) {
            for (int j = 0; j <MAPSIZE; j++) {
                m_omap.data[k] =omap[i][j];
                k++;
            }
        }

        // Localization
        map_building2(robot_pos, count, input_vec_e, input_vec_o, dct, omap, amap);
        // test print to console
        //printf("(x,y,theta) = (%.2f, %.2f, %.2f)\n", robot_pos->real_x, robot_pos->real_y, robot_pos->rangle);

        // Write to robot_position
        m_robot_position.data.position.x = robot_pos->real_x;
        m_robot_position.data.position.y = robot_pos->real_y;
        m_robot_position.data.heading = robot_pos->rangle;

        // OutPortから出力する
        m_robot_positionOut.write();
        m_omapOut.write();
    }

    return RTC::RTC_OK;
}


//RTC::ReturnCode_t Localization::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Localization::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Localization::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Localization::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Localization::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void LocalizationInit(RTC::Manager* manager)
  {
    coil::Properties profile(localization_spec);
    manager->registerFactory(profile,
                             RTC::Create<Localization>,
                             RTC::Delete<Localization>);
  }
  
}
