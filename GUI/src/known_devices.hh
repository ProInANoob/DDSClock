

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

    std::map<std::string, std::map<DeviceRole, std::vector<std::string>*>> getOrg(){
      return(org);
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
      //okay add to our org... 
      //look fro sys, 
      //yes:: find lists and  addd the id
      //NO:: make empty and add
      if(org.find(devinfo.sysName()) != org.end()){
        // has it ?? 
        org[devinfo.sysName()][devinfo.role()]->push_back(devinfo.deviceId());
      } else {
        //no sysname of hta found
        std::map<DeviceRole, std::vector<std::string>*> tempMap;
        //inset vevctors into it?? 
        tempMap[DeviceRole::ROLE_BUTTON] = new std::vector<std::string>;
        tempMap[DeviceRole::ROLE_CLOCK]  = new std::vector<std::string>;
        
        tempMap[devinfo.role()]->push_back(devinfo.deviceId());

        org.insert(std::make_pair(devinfo.sysName(), tempMap));
      
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
    
    std::map<std::string, std::map<DeviceRole, std::vector<std::string>*>> org;
    // so org[sysName][deviceRole] -> list of those device's ids in that system to then look at known devices fro more info if needed;
  
  };
};

#endif
