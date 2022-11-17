// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  FuzzyControlTest.cpp
 * @brief FuzzyControlComponent (test code)
 *
 */
// </rtc-template>

#include "FuzzyControlTest.h"

// Module specification
// <rtc-template block="module_spec">
#if RTM_MAJOR_VERSION >= 2
static const char* const fuzzycontrol_spec[] =
#else
static const char* fuzzycontrol_spec[] =
#endif
  {
    "implementation_id", "FuzzyControlTest",
    "type_name",         "FuzzyControlTest",
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
FuzzyControlTest::FuzzyControlTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_obstacleDistanceOut("obstacleDistance", m_obstacleDistance),
    m_localTargetOut("localTarget", m_localTarget),
    m_RobotPositionOut("RobotPosition", m_RobotPosition),
    m_GoalPositionOut("GoalPosition", m_GoalPosition),
    m_wheelLeftIn("wheelLeft", m_wheelLeft),
    m_wheelRightIn("wheelRight", m_wheelRight)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
FuzzyControlTest::~FuzzyControlTest()
{
}



RTC::ReturnCode_t FuzzyControlTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("wheelLeft", m_wheelLeftIn);
  addInPort("wheelRight", m_wheelRightIn);
  
  // Set OutPort buffer
  addOutPort("obstacleDistance", m_obstacleDistanceOut);
  addOutPort("localTarget", m_localTargetOut);
  addOutPort("RobotPosition", m_RobotPositionOut);
  addOutPort("GoalPosition", m_GoalPositionOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t FuzzyControlTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t FuzzyControlTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControlTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t FuzzyControlTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t FuzzyControlTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t FuzzyControlTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t FuzzyControlTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControlTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControlTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControlTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t FuzzyControlTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool FuzzyControlTest::runTest()
{
    return true;
}


extern "C"
{
 
  void FuzzyControlTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(fuzzycontrol_spec);
    manager->registerFactory(profile,
                             RTC::Create<FuzzyControlTest>,
                             RTC::Delete<FuzzyControlTest>);
  }
  
}
