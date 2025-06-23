#include <array>


//okay soo o


struct ClockInfo
{
    //sooo dp key(dr/dw are transient with restart so dont use those..), topic / clock type (adjustable by gui, puts on new topic), internal ID?   

    std::array<unsigned char, 12> dpKey;

    char * topicName;

    char* displayName;

    char * clockName;

    //self created id?
    

};


struct ButtonInfo
{
    // soo dpKay, atached clock in some way(dpKey, struct*, ect), topicnames(read/write), button Name, orange/blue

    bool isOrange;

    std::array<unsigned char, 12> dpKey;

    std::array<unsigned char, 12> atatchedClockKey;

    char* stateTopicName; // as in the state of the lights, this app writes here.

    char* dataTopicName; // as in teh button's data, this app reads this.

    char* buttonName;

    char* displayName;


    //self created id?


};
