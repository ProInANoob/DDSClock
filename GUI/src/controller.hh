//aight
#include "context.hh"
#include <string>

#include "button_data.hh"
#include "button_command.hh"
#include "clock_command.hh"
#include "heartbeat.hh"

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

    std::string devId;

public:
    Controller(/* args */){};
    ~Controller(){};

    void init(Context &context, std::string deviceId){
        this->devId = deviceId;
        
        buttonData.init( this, context );

        heartbeat.init( this, context, deviceId);

        buttonWriter.init( context, devId );
        clockWriter.init( context, devId );


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

    void writeHeartbeat( ){
        heartbeat.publish( );
    }

    
    virtual void handle_button_data ( const ButtonData & data );
    virtual void handle_heartbeat   ( const Heartbeat  & data );


};


