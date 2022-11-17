// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  LocalizationTest.cpp
 * @brief SLAM and Localization (test code)
 *
 * @author Y.Fujii
 *
 */
// </rtc-template>

#include "LocalizationTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* const localization_spec[] =
  {
    "implementation_id", "LocalizationTest",
    "type_name",         "LocalizationTest",
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
LocalizationTest::LocalizationTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_rangeOut("range", m_range),
    m_omapIn("omap", m_omap),
    m_robot_positionIn("robot_position", m_robot_position)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
LocalizationTest::~LocalizationTest()
{
}



RTC::ReturnCode_t LocalizationTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("omap", m_omapIn);
  addInPort("robot_position", m_robot_positionIn);
  
  // Set OutPort buffer
  addOutPort("range", m_rangeOut);
  
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
RTC::ReturnCode_t LocalizationTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t LocalizationTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t LocalizationTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t LocalizationTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t LocalizationTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t LocalizationTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t LocalizationTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t LocalizationTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t LocalizationTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t LocalizationTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t LocalizationTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool LocalizationTest::runTest()
{
    return true;
}


extern "C"
{
 
  void LocalizationTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(localization_spec);
    manager->registerFactory(profile,
                             RTC::Create<LocalizationTest>,
                             RTC::Delete<LocalizationTest>);
  }
  
}
