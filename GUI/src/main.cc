#include <dds/dds.h>

#include <vector>
#include <map>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <array>
#include <iostream>

#include <thread>
#include "types.hh"
#include "md.hh"
#include "gui.hh"
#include "discovery.hh" 

bool MD::done = false;
DDS_DomainParticipant dp;
int domain_id = 0; // think ill just pull em all in this...
DDS_InstanceHandle_t     my_handle = DDS_HANDLE_NIL; 

// so a clock list probably... Id like to orginize the buttons a little more than just a list, but idk how

std::vector<ClockInfo*> clocks;

std::map<std::array<unsigned char, 12>, std::vector<ButtonInfo*>*> matchedButtons; // this IG? keyed by clock key 
//and 
std::vector<ButtonInfo*> unknownButtons;


#if defined(_WIN32)
// ---------------------------------------------------------------
static int
install_sig_handlers()
{
    return 0;
}
#else
// ---------------------------------------------------------------
void handle_sig(int sig)
{
    if (sig == SIGINT)
        MD::done = 1;
}

// ---------------------------------------------------------------
static int
install_sig_handlers()
{
    struct sigaction int_action;
    int_action.sa_handler = handle_sig;
    sigemptyset(&int_action.sa_mask);
    sigaddset(&int_action.sa_mask, SIGINT);
    int_action.sa_flags = 0;

    sigaction(SIGINT, &int_action, NULL);
    return 0;
}
#endif

/*********************************************************************
 */
int parse_commandline(int argc, char *argv[])
{
    int opt;
    while ((opt = toc_getopt(argc, argv,
                             "d")) != -1)
    {
        switch (opt)
        {

        case 'd': /* domain id */
            domain_id = atoi(toc_optarg);
            break;
        }
    }
    return 0;
}


#pragma region GUI
// ---------------------------------------------------------------
void MD::init(int argc, char *argv[])
{
    install_sig_handlers();

    /* parse command line options */
    if (parse_commandline(argc, argv) < 0)
    {
        exit(0);
    }

    DDS_DomainParticipantQos dp_qos;
    DDS_DomainParticipantFactory_get_default_participant_qos(&dp_qos);
    dp = DDS_DomainParticipantFactory_create_participant(domain_id, &dp_qos, NULL, 0);
    
    my_handle = DDS_DomainParticipant_get_instance_handle(dp); 
    add_discovery_listeners(dp);

    gui::init_backend("MLS TMG Dashboard");
    gui::init_imgui();

    gui::init_renderer();
    gui::set_icon();

}

// ---------------------------------------------------------------
void MD::main_loop()
{
    while (!done)
    {
        gui::loop(domain_id);

        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

// ---------------------------------------------------------------
void MD::cleanup()
{
}

#pragma endregion




// ---------------------------------------------------------------
int main(int argc, char *argv[])
{
    MD::init(argc, argv);
    MD::main_loop();
    MD::cleanup();

    return 0;
}