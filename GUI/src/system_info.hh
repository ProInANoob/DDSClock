#include <map>
#include <string>
#include "robobrawl.hh"
#include "timer.hh"

//setttings definitiaons. 
#define DO_3_SEC_COUNTDOWN 0x0001 << 0



struct SystemInfo
{

    std::map<DeviceRole, std::vector<std::string>> devices;

    float durationSec;
    NewTimer timer;
    uint32_t settings;
    int state;


    /* data */
};
 