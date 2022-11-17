// -*- C++ -*-
// <rtc-template block="description">
/*!
 * @file  Destination_genTest.cpp
 * @brief Destination generation (test code)
 *
 * @author Y.Fujii
 *
 */
// </rtc-template>

#include "Destination_genTest.h"

// Module specification
// <rtc-template block="module_spec">
static const char* const destination_gen_spec[] =
  {
    "implementation_id", "Destination_genTest",
    "type_name",         "Destination_genTest",
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
Destination_genTest::Destination_genTest(RTC::Manager* manager)
    // <rtc-template block="initializer">
  : RTC::DataFlowComponentBase(manager),
    m_omapOut("omap", m_omap),
    m_dest_genIn("dest_gen", m_dest_gen)

    // </rtc-template>
{
}

/*!
 * @brief destructor
 */
Destination_genTest::~Destination_genTest()
{
}



RTC::ReturnCode_t Destination_genTest::onInitialize()
{
  // Registration: InPort/OutPort/Service
  // <rtc-template block="registration">
  // Set InPort buffers
  addInPort("dest_gen", m_dest_genIn);
  
  // Set OutPort buffer
  addOutPort("omap", m_omapOut);
  
  // Set service provider to Ports
  
  // Set service consumers to Ports
  
  // Set CORBA Service Ports
  
  // </rtc-template>

  // <rtc-template block="bind_config">
  // </rtc-template>
  
  return RTC::RTC_OK;
}

/*
RTC::ReturnCode_t Destination_genTest::onFinalize()
{
  return RTC::RTC_OK;
}
*/


//RTC::ReturnCode_t Destination_genTest::onStartup(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_genTest::onShutdown(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


RTC::ReturnCode_t Destination_genTest::onActivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Destination_genTest::onDeactivated(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


RTC::ReturnCode_t Destination_genTest::onExecute(RTC::UniqueId /*ec_id*/)
{
  return RTC::RTC_OK;
}


//RTC::ReturnCode_t Destination_genTest::onAborting(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_genTest::onError(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_genTest::onReset(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_genTest::onStateUpdate(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


//RTC::ReturnCode_t Destination_genTest::onRateChanged(RTC::UniqueId /*ec_id*/)
//{
//  return RTC::RTC_OK;
//}


bool Destination_genTest::runTest()
{
    return true;
}


extern "C"
{
 
  void Destination_genTestInit(RTC::Manager* manager)
  {
    coil::Properties profile(destination_gen_spec);
    manager->registerFactory(profile,
                             RTC::Create<Destination_genTest>,
                             RTC::Delete<Destination_genTest>);
  }
  
}
