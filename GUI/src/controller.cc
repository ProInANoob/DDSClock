#include "controller.hh"

extern std::vector<System> activeSystems; // activated ones
extern std::map<std::string, System> systems; // all of them 



void Controller::handle_button_data(const ButtonData & data){
    // update button states. 
}

void Controller::handle_heartbeat( const Heartbeat & data ){
    // update livelinesss.
}