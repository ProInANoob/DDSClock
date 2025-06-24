#ifndef DEVICEINFO_HH
#define DEVICEINFO_HH

#include <map>
#include <utility>

#include "robobrawl.hh"
#include "context.hh"
// -------------------------------------------------------
// -------------------------------------------------------
class DeviceInfoClient{
public:
  virtual void handle_device_info(const DeviceInfo & data) = 0;
};


class DeviceInfoReader : public dds::sub::NoOpDataReaderListener<DeviceInfo>
{

public:
  // -------------------------------------------------------
  DeviceInfoReader() : topic(dds::core::null),
                       dr(dds::core::null) {}

  // -------------------------------------------------------
  ~DeviceInfoReader()
  {
  }

  // -------------------------------------------------------
  void
  init( DeviceInfoClient * di_client,
        Context &context)
  {
    this->di_client = di_client;

    topic = dds::topic::Topic<DeviceInfo>(context.participant(), "device_info");
    dds::sub::qos::DataReaderQos dr_qos;

    dr = dds::sub::DataReader(context.subscriber(), topic, dr_qos /*, listener, DDS_DATA_AVAILABLE_STATUS */);
  }

  // -------------------------------------------------------
  void clear()
  {
    dr = dds::core::null;
    topic = dds::core::null;
  }

  // -------------------------------------------------------
    void on_data_available(dds::sub::DataReader<DeviceInfo> &reader)
    {
      dds::sub::LoanedSamples<DeviceInfo> samples = reader.take();
      for (const auto &sample : samples)
      {
        if (sample.info().valid())
        {
          if (di_client)
          {
            di_client->handle_device_info(sample.data());
          }
        }
      }
    }

  // -------------------------------------------------------

protected:
  dds::topic::Topic<DeviceInfo>    topic;
  dds::sub::DataReader<DeviceInfo> dr;
  DeviceInfoClient               * di_client;
};

#endif
