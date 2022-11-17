// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  GNG_componentTest.cpp
 * @brief ModuleDescription (test code)
 *
 */
// </rtc-template>

#include "GNG_componentTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* const gng_component_spec[] =
  {
    "implementation_id", "GNG_componentTest",
    "type_name",         "GNG_componentTest",
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
GNG_componentTest::GNG_componentTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_input_vecOut("input_vec", m_input_vec),
    m_topomapIn("topomap", m_topomap)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
GNG_componentTest::~GNG_componentTest()
{
}



RTC::ReturnCode_t GNG_componentTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("topomap", m_topomapIn);
  
  // Set OutPort buffer
  addOutPort("input_vec", m_input_vecOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t GNG_componentTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t GNG_componentTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_componentTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t GNG_componentTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t GNG_componentTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t GNG_componentTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t GNG_componentTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_componentTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_componentTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_componentTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t GNG_componentTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool GNG_componentTest::runTest()
{
    return true;
}


extern "C"
{
 
  void GNG_componentTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(gng_component_spec);
    manager->registerFactory(profile,
                             RTC::Create<GNG_componentTest>,
                             RTC::Delete<GNG_componentTest>);
  }
  
}
