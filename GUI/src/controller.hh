//aight
#include "context.hh"
#include <string>

#include "button_data.hh"
#include "button_command.hh"
#include "clock_command.hh"
#include "heartbeat.hh"
#include "set_sysName.hh"
#include "known_devices.hh"

class Controller : 
        public ButtonDataClient,
        public HeartbeatClient
{
protected:
    /* data */
    ButtonDataReader buttonData;

    ButtonCommandWriter buttonWriter;
    ClockCommandWriter clockWriter;
    HeartbeatWriter heartbeat;
    SetSysName setSysName;

    std::string devId;

public:

    // keyeeeesss? 
    //states: idle, before ready type thing, then the readys - a blu eready and orang and a both-> auto transitioj to running or just the running
    // running -> contians all the paused, resume, running ect. 
    // some kind of doneIdle -> at 0000 no decition tho (diff than Idle? )
    // winner states.  
    // 0: idle, 1: orangReady, 2: blueReady, 3: goTime/Running, 4: doneIdle, 5: orangeWInn, 6: BlueWin
    std::map<std::string, int> systemStates;
    KnownDevices known_devices;

    Controller(/* args */){};
    ~Controller(){};



    void init(Context &context, std::string deviceId){
        this->devId = deviceId;
        
        known_devices.init( context );

        buttonData.init( this, context );

        heartbeat.init( this, context, deviceId);


        buttonWriter.init( context, devId );
        clockWriter.init( context, devId );
        setSysName.init( context, deviceId);


    };

    void startup(){
        //device info, idk what else.
    }

    void writeToClock( ClockCommand & command ){
        clockWriter.publish( command );
    }

    void writeToButton( ButtonCommand & command ){
        buttonWriter.publish( command );
    }

    void writeSysName( SysName & command ){
        setSysName.publish( command );
    }

    void writeHeartbeat( ){
        heartbeat.publish( );
    }

    
    virtual void handle_button_data ( const ButtonData & data );
    virtual void handle_heartbeat   ( const Heartbeat  & data );


};


