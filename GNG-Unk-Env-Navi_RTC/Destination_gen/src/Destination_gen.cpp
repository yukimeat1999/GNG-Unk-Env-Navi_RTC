// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Destination_gen.cpp
 * @brief Destination generation
 *
 * @author Y.Fujii
 *
 */
// </rtc-template>

#include "Destination_gen.h"

#include <iostream>
using namespace std;
using namespace cv;

int touch_x, touch_y;
int touch_flag = 0;

#define MAPSIZE 500
#define MAPRATE 50 

void mouse_callback(int event, int x, int y, int flags, void* userdata)
{
    if (event == EVENT_LBUTTONDOWN) {
        touch_x = x;
        touch_y = y;
        touch_flag = 1;
    }
}

double cal_probability(int k)
{
    double e;
    e = tanh((double)k / 10.0);
    return e;
}

void draw_map(int k, int l, int map_value, Mat omap_Image)
{
    double p;

    if (map_value > 0) {
        p = cal_probability(map_value);
        if (p >= 0.8) {
            omap_Image.at<Vec3b>(l, k)[2] = 0;
            omap_Image.at<Vec3b>(l, k)[1] = 0;
            omap_Image.at<Vec3b>(l, k)[0] = 0;
        }
        else {
            omap_Image.at<Vec3b>(l, k)[2] = 125 - (int)(125 * p);
            omap_Image.at<Vec3b>(l, k)[1] = 125 - (int)(125 * p);
            omap_Image.at<Vec3b>(l, k)[0] = 125 - (int)(125 * p);
        }
    }
    else {

        p = cal_probability(fabs(map_value));
        if (p >= 0.8) {
            omap_Image.at<Vec3b>(l, k)[2] = 255;
            omap_Image.at<Vec3b>(l, k)[1] = 255;
            omap_Image.at<Vec3b>(l, k)[0] = 255;
        }
        else {
            omap_Image.at<Vec3b>(l, k)[2] = 125 + (int)(125 * p);
            omap_Image.at<Vec3b>(l, k)[1] = 125 + (int)(125 * p);
            omap_Image.at<Vec3b>(l, k)[0] = 125 + (int)(125 * p);
        }
    }
}

// Module specification
// <rtc-template block="module_spec">
static const char* const destination_gen_spec[] =
  {
    "implementation_id", "Destination_gen",
    "type_name",         "Destination_gen",
    "description",       "Destination generation",
    "version",           "1.0.0",
    "vendor",            "Y.Fujii",
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
Destination_gen::Destination_gen(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_omapIn("omap", m_omap),
    m_dest_genOut("dest_gen", m_dest_gen)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Destination_gen::~Destination_gen()
{
}



RTC::ReturnCode_t Destination_gen::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("omap", m_omapIn);
  
  // Set OutPort buffer
  addOutPort("dest_gen", m_dest_genOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Destination_gen::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Destination_gen::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_gen::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Destination_gen::onActivated(RTC::UniqueId /*ec_id*/)
{
    // Prtの初期化
    m_dest_gen.data.position.x = 0;
    m_dest_gen.data.position.y = 0;
    m_dest_gen.data.heading = 0;
    m_omap.data.length(MAPSIZE * MAPSIZE);

    touch_flag = 0;

    slamImage = cv::Mat::zeros(MAPSIZE, MAPSIZE, CV_8UC3);
    rectangle(slamImage, cv::Point(0, 0), cv::Point(MAPSIZE, MAPSIZE), Scalar(125, 125, 125), -1, 8, 0);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Destination_gen::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Destination_gen::onExecute(RTC::UniqueId /*ec_id*/)
{
    // 宣言
   // static int omap[MAPSIZE][MAPSIZE];

    // 新しいデータのチェック
    if (m_omapIn.isNew()) {
        // InPortデータの読み込み
        m_omapIn.read();

        // m_omapを一次元配列から二次元配列に変換
        int k = 0;
        for (int i = 0; i < MAPSIZE; i++) {
            for (int j = 0; j < MAPSIZE; j++) {
                //omap[i][j] = m_omap.data[k];
                draw_map(i, j, m_omap.data[k], slamImage);
                k++;
            }
        }

        // omapから地図を描画
        /*for (int i = 0; i < MAPSIZE; i++)
            for (int j = 0; j < MAPSIZE; j++)
                draw_map(i, j, omap[i][j], slamImage);
                */
        setMouseCallback("Map", mouse_callback);
        imshow("Map", slamImage);
        waitKey(1);
        if (touch_flag == 1) {

            // dest_genの変換
            //touch_x = touch_x - (MAPSIZE / 2) * MAPRATE;
            //touch_y = touch_y - (MAPSIZE / 2) * MAPRATE;

            printf("touch_x is %d\n", touch_x);
            printf("touch_y is %d\n", touch_y);

            // Write to dest_gen
            if((int)slamImage.at<Vec3b>(touch_y, touch_x)[2] > 150){
                m_dest_gen.data.position.x = touch_x;
                m_dest_gen.data.position.y = touch_y;
            }
            else if ((int)slamImage.at<Vec3b>(touch_y, touch_x)[2] == 125) {
                m_dest_gen.data.position.x = touch_x+10000;
                m_dest_gen.data.position.y = touch_y+10000;
            }

            // OutPortから出力する
            m_dest_genOut.write();

            touch_flag = 0;
        }
    }
    
    return RTC::RTC_OK;
}


//RTC::ReturnCode_t Destination_gen::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_gen::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_gen::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_gen::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_gen::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void Destination_genInit(RTC::Manager* manager)
  {
    coil::Properties profile(destination_gen_spec);
    manager->registerFactory(profile,
                             RTC::Create<Destination_gen>,
                             RTC::Delete<Destination_gen>);
  }
  
}
