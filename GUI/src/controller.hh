//aight
#include "context.hh"
#include <string>

#include "device_info.hh"
class Controller : 
        public DeviceInfoClient
{
protected:
    /* data */
    DeviceInfoReader deviceInfo;


public:
    Controller(/* args */){};
    ~Controller(){};

    void init(Context &context){
        deviceInfo.init( this, context );
    };

    
    virtual void handle_device_info(const DeviceInfo & data);

};


