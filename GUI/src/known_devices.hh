

#ifndef DEVICES_HH
#define DEVICES_HH

#include <map>
#include "robobrawl.hh"
#include "context.hh"

namespace roboClock
{
  class KnownDevices : public dds::sub::NoOpDataReaderListener<DeviceInfo>
  {

  public:
    // -------------------------------------------------------
    KnownDevices() : dr(dds::core::null),
                     known_devices()
    {
    }

    // -------------------------------------------------------
    ~KnownDevices()
    {
    }

    // -------------------------------------------------------
    void
    init(Context &context)
    {
      auto topic = dds::topic::Topic<DeviceInfo>(context.participant(), "DeviceInfo");
      dr = dds::sub::DataReader<DeviceInfo>(context.subscriber(), topic);
      dr.listener(this, dds::core::status::StatusMask::all());
    }

    // -------------------------------------------------------
    void clear()
    {
      dr.listener(NULL, dds::core::status::StatusMask::all());
      dr = dds::core::null;
    }

    // -------------------------------------------------------
    std::vector<std::string> known_device_ids()
    {
      std::vector<std::string> ids;
      for (auto const &imap : known_devices)
      {
        ids.push_back(imap.first);
      }
      return ids;
    }

    // -------------------------------------------------------
    const DeviceInfo &get_deviceinfo(std::string devId)
    {
      auto search = known_devices.find(devId);
      if (search != known_devices.end())
        return search->second;
      else
        throw std::runtime_error("unknown");
    }

    // -------------------------------------------------------
    DeviceRole get_device_role(std::string devId)
    {
      auto search = known_devices.find(devId);
      if (search != known_devices.end())
        return search->second.role();
      else
        return DeviceRole::ROLE_UNKNOWN;
    }

    // -------------------------------------------------------
    void handle_deviceinfo(const DeviceInfo &devinfo)
    {
      auto result = known_devices.insert(std::make_pair(devinfo.deviceId(), devinfo));
      // const auto [it, added] =
      auto it = result.first;
      auto added = result.second;
      if (!added)
      {
        it->second = devinfo;
      }
      else
      {
        fprintf(stderr, "ADD  DEVICE: '%s'\n", devinfo.deviceId().c_str());
      }
    }

    // -------------------------------------------------------
    void remove_deviceinfo(const DeviceInfo &devinfo)
    {
      fprintf(stderr, "DEAD DEVICE: '%s'\n", devinfo.deviceId().c_str());
      known_devices.erase(devinfo.deviceId());
    }

    // -------------------------------------------------------
    void on_data_available(dds::sub::DataReader<DeviceInfo> &reader)
    {
      dds::sub::LoanedSamples<DeviceInfo> samples = reader.take();
      for (const auto &sample : samples)
      {
        if (sample.info().valid())
        {
          handle_deviceinfo(sample.data());
        }
        else
        {
          remove_deviceinfo(sample.data()); // key is valid, but no other fields
        }
      }
    }

  protected:
    dds::sub::DataReader<DeviceInfo> dr;
    std::map<std::string, DeviceInfo> known_devices;
  };
};

#endif
