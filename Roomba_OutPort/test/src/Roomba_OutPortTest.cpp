// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Roomba_OutPortTest.cpp
 * @brief ${rtcParam.description} (test code)
 *
 * @author Y. Fujii
 *
 */
// </rtc-template>

#include "Roomba_OutPortTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* const roomba_outport_spec[] =
  {
    "implementation_id", "Roomba_OutPortTest",
    "type_name",         "Roomba_OutPortTest",
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
    "conf.default.port_name", "COM1",

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
Roomba_OutPortTest::Roomba_OutPortTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_wheel_lOut("wheel_l", m_wheel_l),
    m_wheel_rOut("wheel_r", m_wheel_r)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Roomba_OutPortTest::~Roomba_OutPortTest()
{
}



RTC::ReturnCode_t Roomba_OutPortTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  
  // Set OutPort buffer
  addOutPort("wheel_l", m_wheel_lOut);
  addOutPort("wheel_r", m_wheel_rOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // Bind variables and configuration variable
  bindParameter("port_name", m_port_name, "COM1");
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Roomba_OutPortTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Roomba_OutPortTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPortTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Roomba_OutPortTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Roomba_OutPortTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Roomba_OutPortTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t Roomba_OutPortTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPortTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPortTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPortTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Roomba_OutPortTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool Roomba_OutPortTest::runTest()
{
    return true;
}


extern "C"
{
 
  void Roomba_OutPortTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(roomba_outport_spec);
    manager->registerFactory(profile,
                             RTC::Create<Roomba_OutPortTest>,
                             RTC::Delete<Roomba_OutPortTest>);
  }
  
}
