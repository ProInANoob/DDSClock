#include "controller.hh"


void Controller::handle_button_data(const ButtonData & data){
    // update button states. 
    std::cout << " oohohoh a button data" << std::endl;
}

void Controller::handle_heartbeat( const Heartbeat & data ){
    // update livelinesss.
    std::cout << "wowsa\n";
}