//implementaitons. 
#include <dds/dds.hpp>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <thread>
#include "robobrawlClock.hh"

extern int roboClock::domain_id;
extern dds::domain::DomainParticipant dp;
extern dds::pub::Publisher pub;
extern dds::sub::Subscriber sub;
extern std::string roboClock::deviceId;

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
        roboClock::done = 1;
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
                             "d:i:-")) != -1)
    {
        switch (opt)
        {

        case 'd': /* domain id */
            roboClock::domain_id = atoi(toc_optarg);
            break;
        case 'i':
            roboClock::deviceId = atoi(toc_optarg);
            break;


        }

    }
    return 0;
}



void roboClock::init(int argc, char *argv[]){
    install_sig_handlers();

    /* parse command line options */
    if (parse_commandline(argc, argv) < 0)
    {
        exit(0);
    }

    //setup ds stuff 
    if ( DDS_DomainParticipantFactory_get_instance() == NULL )
    {
      fprintf( stderr, "Failed to initialize CoreDX DDS: missing license?\n" );
      exit( 1 );
    }    

    context.init(domain_id);
    
    // reader/writer innits
    if(deviceId == ""){
        //no deviceId set so do default
        deviceId = "Control-001";
    }
    control.init(context, deviceId);


    gui::init_backend("Robobrawl Control");
    gui::init_imgui();

    gui::init_renderer();
    gui::set_icon();
}


