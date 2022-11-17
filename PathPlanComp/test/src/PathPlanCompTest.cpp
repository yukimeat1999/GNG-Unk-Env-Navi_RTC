// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  PathPlanCompTest.cpp
 * @brief wanna play Apex (test code)
 *
 */
// </rtc-template>

#include "PathPlanCompTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* const pathplancomp_spec[] =
  {
    "implementation_id", "PathPlanCompTest",
    "type_name",         "PathPlanCompTest",
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
PathPlanCompTest::PathPlanCompTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_GoalOut("Goal", m_Goal),
    m_PositionOut("Position", m_Position),
    m_TopoMapOut("TopoMap", m_TopoMap),
    m_DirectionIn("Direction", m_Direction)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
PathPlanCompTest::~PathPlanCompTest()
{
}



RTC::ReturnCode_t PathPlanCompTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("Direction", m_DirectionIn);
  
  // Set OutPort buffer
  addOutPort("Goal", m_GoalOut);
  addOutPort("Position", m_PositionOut);
  addOutPort("TopoMap", m_TopoMapOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t PathPlanCompTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t PathPlanCompTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanCompTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t PathPlanCompTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t PathPlanCompTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t PathPlanCompTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t PathPlanCompTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanCompTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanCompTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanCompTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t PathPlanCompTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool PathPlanCompTest::runTest()
{
    return true;
}


extern "C"
{
 
  void PathPlanCompTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(pathplancomp_spec);
    manager->registerFactory(profile,
                             RTC::Create<PathPlanCompTest>,
                             RTC::Delete<PathPlanCompTest>);
  }
  
}
