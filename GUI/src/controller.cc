#include "controller.hh"


void Controller::handle_button_data(const ButtonData & data){
    // update button states. 
}

void Controller::handle_heartbeat( const Heartbeat & data ){
    // update livelinesss.
    std::cout << "wowsa\n";
}