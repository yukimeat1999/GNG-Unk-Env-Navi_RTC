// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Roomba_OutPort.cpp
 * @brief ${rtcParam.description}
 *
 * @author Y. Fujii
 *
 */
// </rtc-template>

#include "Roomba_OutPort.h"

//Serial.h
#include "Serial.h"

//----------Global-----------------
extern const char* port_num;
int roomba_flag = 0;
char port_num_str[20];


// Module specification
// <rtc-template block="module_spec">
static const char* const roomba_outport_spec[] =
  {
    "implementation_id", "Roomba_OutPort",
    "type_name",         "Roomba_OutPort",
    "description",       "${rtcParam.description}",
    "version",           "1.0.0",
    "vendor",            "Y. Fujii",
    "category",          "Robot",
    "activity_type",     "PERIODIC",
    "kind",              "DataFlowComponent",
    "max_instance",      "1",
    "language",          "C++",
    "lang_type",         "compile",
    // Configuration variables
    "conf.default.port_name", "COM6",

    // Widget
    "conf.__widget__.port_name", "text",
    // Constraints

    "conf.__type__.port_name", "string",

    ""
  };
// </rtc-template>

/*!
 * @brief constructor
 * @param manager Maneger Object
 */
Roomba_OutPort::Roomba_OutPort(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_wheel_lIn("wheel_l", m_wheel_l),
    m_wheel_rIn("wheel_r", m_wheel_r)
    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Roomba_OutPort::~Roomba_OutPort()
{
}



RTC::ReturnCode_t Roomba_OutPort::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("wheel_l", m_wheel_lIn);
  addInPort("wheel_r", m_wheel_rIn);
  
  // Set OutPort buffer

  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("port_name", m_port_name, "COM6");
  // </rtc-template>

  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Roomba_OutPort::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Roomba_OutPort::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPort::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Roomba_OutPort::onActivated(RTC::UniqueId /*ec_id*/)
{

    port_num = m_port_name.c_str();

    sprintf(port_num_str, "\\\\.\\%s", port_num);

    roomba_flag = iRobotInitialize(port_num_str);

    if (roomba_flag != 0) {
        printf("ルンバの初期化に成功しました。\n");
    }
    else {
        printf("ルンバの初期化に失敗しました。\n");
    }

    start();    //Activate roomba
    Sleep(50);
    safe();     //Set roomba to safe mode

    /*
    
    Sleep(500);
    controlMotor(100, 100);
    Sleep(1000);
    controlMotor(-100, -100);
    Sleep(1000);
    controlMotor(-100, 100);
    Sleep(500);
    controlMotor(100, -100);
    Sleep(500);
    controlMotor(0, 0);
    */

    printf("Used Port Name is \"%s\"\n", port_num_str);

    return RTC::RTC_OK;
}


RTC::ReturnCode_t Roomba_OutPort::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Roomba_OutPort::onExecute(RTC::UniqueId /*ec_id*/)
{
  double fin_outp[2];

  if (m_wheel_lIn.isNew() && m_wheel_rIn.isNew()) {

      // InPortデータの読み込み
      m_wheel_lIn.read();
      m_wheel_rIn.read();

      // InPortデータの代入
      fin_outp[0] = m_wheel_r.data.vx * MAX_OUTPUT;
      fin_outp[1] = m_wheel_l.data.vx * MAX_OUTPUT;

      printf("%f\t%f\n", fin_outp[0], fin_outp[1]);

      if (fin_outp[0] == 0 && fin_outp[1] == 0) {
          fin_outp[0] = 0;
          fin_outp[1] = 0;
          controlMotor(0, 0);
      }
      else {
          controlMotor(fin_outp[0], fin_outp[1]);
      }
      
  }

  return RTC::RTC_OK;
}


//RTC::ReturnCode_t Roomba_OutPort::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPort::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPort::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPort::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPort::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}



extern "C"
{
 
  void Roomba_OutPortInit(RTC::Manager* manager)
  {
    coil::Properties profile(roomba_outport_spec);
    manager->registerFactory(profile,
                             RTC::Create<Roomba_OutPort>,
                             RTC::Delete<Roomba_OutPort>);
  }
  
}
