// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  GNG_component.cpp
 * @brief ModuleDescription
 *
 */
// </rtc-template>

#include "GNG_component.h"
#include "gng.h"
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//#include <opencv/cxcore.h>

using namespace cv;

//グローバル変数の定義
struct gng* net = NULL;
Mat gngImage;

// Module specification
// <rtc-template block="module_spec">
static const char* const gng_component_spec[] =
  {
    "implementation_id", "GNG_component",
    "type_name",         "GNG_component",
    "description",       "ModuleDescription",
    "version",           "1.1.0",
    "vendor",            "Y.Furuta",
    "category",          "Machine Learning",
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
GNG_component::GNG_component(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_input_vecIn("input_vec", m_input_vec),
    m_topomapOut("topomap", m_topomap)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
GNG_component::~GNG_component()
{
}



RTC::ReturnCode_t GNG_component::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("input_vec", m_input_vecIn);
  
  // Set OutPort buffer
  addOutPort("topomap", m_topomapOut);

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>

  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t GNG_component::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t GNG_component::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_component::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t GNG_component::onActivated(RTC::UniqueId /*ec_id*/)
{
    // Prtの初期化
    m_input_vec.data.length(MAPSIZE * MAPSIZE);
    m_topomap.node_num = 0;
    m_topomap.node.length(GNGN * 3);
    m_topomap.edge.length(GNGN * (GNGN + 1));

    net = init_gng();   //GNGの初期化
    net->weight[0] = 1; //x
    net->weight[1] = 1; //y
    net->weight[2] = 1; //占有度

    //cv::Matの初期化
    gngImage = cv::Mat::zeros(MAPSIZE, MAPSIZE, CV_8UC3);
    rectangle(gngImage, cv::Point(0, 0), cv::Point(MAPSIZE, MAPSIZE), Scalar(125, 125, 125), -1, 8, 0);
    
    return RTC::RTC_OK;
}

//GNGの描画に関する関数
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
        cv::circle(frame, rpt, 2, Scalar((int)(255.0 * rgb), (int)(255.0 * rgb), (int)(255.0 * rgb)), -1, 8, 0);

    }
}

RTC::ReturnCode_t GNG_component::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t GNG_component::onExecute(RTC::UniqueId /*ec_id*/)
{
    //Releaseでビルド

    // 宣言
    static int omap[MAPSIZE][MAPSIZE] = { 0 };
    static double input_vec_e[MAPSIZE * MAPSIZE][DIM];
    static double input_vec_o[MAPSIZE * MAPSIZE][DIM];
    static int dct[2] = { 0 };
    static double topomap_node[GNGN][DIM] = { 0 };
    static int topomap_edge[GNGN][GNGN] = { 0 };

    // 新しいデータのチェック
    if (m_input_vecIn.isNew()) {
        // InPortデータの読み込み
        m_input_vecIn.read();

        int i = 0;
        int j = 0;

        //gngで扱える変数の型に変換（1次元配列から適切な2次元配列へ）
        int num = 0;
        for (i = 0; i < MAPSIZE; i++) {
            for (j = 0; j < MAPSIZE; j++) {
                omap[i][j] = m_input_vec.data[num];
                num++;
            }
        }


        int count = 0; //後に入力のデータサイズとなる
        dct[0] = 0;
        dct[1] = 0;
        for (i = 0; i < MAPSIZE; i++) {
            for (j = 0; j < MAPSIZE; j++) {
                if (omap[i][j] < 0) {       //占有度が低いマス
                    input_vec_e[dct[0]][0] = i;// -MAPSIZE / 2;
                    input_vec_e[dct[0]][1] = j;// -MAPSIZE / 2;
                    input_vec_e[dct[0]][2] = -1;
                    dct[0]++;
                }
                else if (omap[i][j] > 0) {  //占有度が高いマス（何かしら障害物がありそうと判断されたマス）
                    input_vec_o[dct[1]][0] = i;// -MAPSIZE / 2;
                    input_vec_o[dct[1]][1] = j;// -MAPSIZE / 2;
                    input_vec_o[dct[1]][2] = 1;
                    dct[1]++;
                }
                else {

                }

            }
        }

        printf("%d,%d\n", dct[0], dct[1]);

        if (dct[0] != 0 && dct[1] != 0) {
            gng_main(net, input_vec_e, input_vec_o, dct, 0);
        }
        rectangle(gngImage, cv::Point(0, 0), cv::Point(MAPSIZE, MAPSIZE), Scalar(125, 125, 125), -1, 8, 0);
        Draw_GNG(gngImage, net);

        imshow("gng", gngImage);
        waitKey(1);

        //printf("node_x is %f\tnode_y is %f\tnode_occupy is %f\n", net->node[0][0], net->node[0][1], net->node[0][2]);

        //topomapを生成
        //node_num
        m_topomap.node_num = net->node_n;

        //node,edge
        for (i = 0; i < net->node_n; i++) {
            for (j = 0; j < 3; j++) {
                m_topomap.node[i * 3 + j] = net->node[i][j];  //node
            }
            for (j = 0; j < net->node_n; j++) {
                m_topomap.edge[i * net->node_n + j] = net->edge[i][j];  //edge
            }
            m_topomap.edge[(GNGN - 1) * GNGN + i] = net->contour[i];  //edge
        }

        //OutPortから出力する
        m_topomapOut.write();
    }
    
    return RTC::RTC_OK;
}


//RTC::ReturnCode_t GNG_component::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_component::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_component::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_component::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_component::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void GNG_componentInit(RTC::Manager* manager)
  {
    coil::Properties profile(gng_component_spec);
    manager->registerFactory(profile,
                             RTC::Create<GNG_component>,
                             RTC::Delete<GNG_component>);
  }
  
}
