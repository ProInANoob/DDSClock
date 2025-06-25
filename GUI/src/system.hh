#ifndef SYS_HH
#define SYS_HH
#include "robobrawl.hh"

// need:
// writer for SYS_CLOCK and SYS_BUTTON



class System 
{
private:
    /* data */
public:
    System(std::string name){
        this->name = name;
    };
    //just pushback the lists. idc that  much
    ~System(){    };

    //other functions... 
    void init(){
        //startup the writer (Button reader?) need instruction topic.. 
        //check in loops or at end of deviceinfo for filled systems - clock + 2 buttons, and have a gui manual startup.. allow multiple clocks?
    }

    std::string name;
    std::vector<ClockInfo *> clocks; 
    std::vector<ButtonInfo*> buttons;

};





#endif