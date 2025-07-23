#include "controller.hh"


void Controller::handle_button_data(const ButtonData & data){
    // update button states. 
    std::cout << " oohohoh a button data" << std::endl;

    //oka soooooooo 
    if(!data.mainPressed() && !data.tapoutPressed() ){
        // no action happened - likey uninteded message - bug in button .
    }
    else
    {
        // I have system and devId - so look at blue / orange, need a system state I think... so I can change that and update the clock state
        
        if(data.mainPressed() != 0){
            // switch over this system state..
            switch(systemStates[data.sysName()]){
                // 0: idle, 
                case 0:
                    if(known_devices.get_device_role(data.deviceId( )) == DeviceRole::ROLE_BUTTON_BLUE){
                        systemStates[data.sysName()] = 2; 
                    } else {
                        //orange button
                        systemStates[data.sysName()] = 1; 
                    }
                    break;
                case 1:
                    if(known_devices.get_device_role(data.deviceId( )) == DeviceRole::ROLE_BUTTON_BLUE){
                        systemStates[data.sysName()] = 3;
                    } else{
                        //orange repeat press...
                    }
                    break;
                case 2:
                    if(known_devices.get_device_role(data.deviceId( )) == DeviceRole::ROLE_BUTTON_ORANGE){
                        systemStates[data.sysName()] = 3;
                    } else{
                        //blue repeat press...
                    }
                    break;
                case 3:
                    //dont care
                    break;
                case 4:
                    //dont care
                    break;
                case 5:
                case 6:
                case 7:
                case 8:
                    //dont care
                    break;
                default:
                    //unknown state???????? wth. 
                    break;
            } 
        } else if( data.tapoutPressed() != 0){
            switch(systemStates[data.sysName()]){
                // 0: idle, 1: orangReady, 2: blueReady, 3: goTime/Running, 4: doneIdle, 5: orangeWInn, 6: BlueWin
                case 0:
                    // im gonna ignore this here.. .
                    break;
                case 1:
                    // and here
                    break;
                case 2:
                case 3:

                    // and here
                    break;
                case 5:
                    if(known_devices.get_device_role(data.deviceId( )) == DeviceRole::ROLE_BUTTON_BLUE){
                        systemStates[data.sysName()] = 5; 
                    } else {
                        //orange button
                        systemStates[data.sysName()] = 6; 
                    }
                    break;
                case 4:
                    //ignoreing here too.
                    break;
                
                case 6:
                case 7:
                case 8:
                    //and here... 
                    break;
                default:
                    //unknown state???????? wth. 
                    break;
            }   
        }
    }

}

void Controller::handle_heartbeat( const Heartbeat & data ){
    // update livelinesss.
    std::cout << "wowsa\n";
    // I will actualy need to make an alg for this.... huhhhhhhhhhh I dont wannnnnannnananannan ahhhhhh
}