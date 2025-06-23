#include <dds/dds.h>
#include <dds/dds_builtin.h>
#include <dds/dds_builtinDataReader.h>

#include <stdio.h>
#include <iostream>
#include <map>
#include <string>
#include <vector> // For std::stringstream
#include <array>
#include <cstring>

#include "extended_discovery_types.h"
#include "types.hh"
// #include "gui.hh"



/*****************************************************************
 *
 *****************************************************************/


extern DDS_InstanceHandle_t my_handle;


extern std::vector<ClockInfo*> clocks;

extern std::map<std::array<unsigned char, 12>, std::vector<ButtonInfo*>*> matchedButtons; // this IG? keyed by clock key 
//and 
extern std::vector<ButtonInfo*> unknownButtons;




void bi_dpd_data_available(DDS_DataReader dr)
{
    DDS_DCPSParticipantDataReader bi_dr = (DDS_DCPSParticipantDataReader)dr;
    DDS_ReturnCode_t retval;
    DDS_DCPSParticipantPtrSeq samples;
    DDS_SampleInfoSeq sample_infos;

    INIT_SEQ(samples);
    INIT_SEQ(sample_infos);

    retval = DDS_DCPSParticipantDataReader_read(bi_dr,
                                                &samples,
                                                &sample_infos,
                                                DDS_LENGTH_UNLIMITED,
                                                DDS_NOT_READ_SAMPLE_STATE,
                                                DDS_ANY_VIEW_STATE,
                                                DDS_ANY_INSTANCE_STATE);
    if (retval == DDS_RETCODE_OK)
    {
        uint32_t i;
        for (i = 0; i < samples._length; i++)
        {
            
            //if (sample_infos._buffer[i]->instance_handle == my_handle){
            //    
            //    continue;
            //}
              
           // printf("%x\n", sample_infos._buffer[i]->instance_handle);

            if (sample_infos._buffer[i]->valid_data)
            {
                unsigned char *key;
                struct list_head *plh;
                RTPS_Locator *l;
                char lbuf[80];
                char vendor_id[30];
                char prod_id[64];
                uint32_t epoint;
                unsigned int j;
                if ((sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE))
                {
                    // DDS_DCPSParticipant *dpd = samples._buffer[i];
                    // key = (unsigned char *)&dpd->key;
                    // std::cout << key << std::endl;
                    RTPS_SPDPdiscoveredParticipantData *dpd = (RTPS_SPDPdiscoveredParticipantData *)samples._buffer[i];

                    key = (unsigned char *)&dpd->key; /* (unsigned char *)&samples._buffer[i]->key; */
                    printf("-----------------------------------------------------------------\n");
                    printf("TIME: %d.%09d\n", 
                           sample_infos._buffer[i]->reception_timestamp.sec,
                           sample_infos._buffer[i]->reception_timestamp.nanosec);
                    printf("%s PARTICIPANT      : %.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x\n",
                           (sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE ? "NEW   " : "UPDATE"),
                           key[0], key[1], key[2], key[3],
                           key[4], key[5], key[6], key[7],
                           key[8], key[9], key[10], key[11]);


                    // create domain data struct. 

                


                    printf("   domain_id            : 0x%08x\n", dpd->domain_id);
                    printf("   domain_tag           : '%s'\n", dpd->domain_tag ? dpd->domain_tag : "<null>");
                    printf("   entity_name          : '%s'\n", dpd->entity_name ? dpd->entity_name : "<null>");

                    if ((dpd->vendorId.vendorId[0] == 1) && (dpd->vendorId.vendorId[1] == 6))
                        sprintf(vendor_id, "%s", "CoreDX DDS");
                    else if ((dpd->vendorId.vendorId[0] == 1) && (dpd->vendorId.vendorId[1] == 1))
                        sprintf(vendor_id, "%s", "RTI DDS");
                    else if ((dpd->vendorId.vendorId[0] == 1) && (dpd->vendorId.vendorId[1] == 2))
                        sprintf(vendor_id, "%s", "OpenSplice");
                    else
                        sprintf(vendor_id, "%d.%d", dpd->vendorId.vendorId[0], dpd->vendorId.vendorId[1]);

                    printf("   vendor id            : %s\n", vendor_id);
                    snprintf(prod_id, 64, "%d.%d.%d",
                             dpd->productVersion[0],
                             dpd->productVersion[1],
                             dpd->productVersion[2]);
                    printf("   product version      : %s\n", prod_id);

                    /* print participant QoS in 'dpd' and highlight any changes from p->dpd */
                    /*diff = compare_userdata(&p->dpd.user_data.value, &dpd->user_data.value); */
                    printf("   user_data  [%.5d]   : ", dpd->user_data.value._length);
                    if (dpd->user_data.value._length)
                        ;
                    // print_hex("", dpd->user_data.value._buffer, MIN(dpd->user_data.value._length, 16));
                    else
                        printf("\n");

                    printf("   lease_duration       : %d.%.9d sec\n",
                           dpd->leaseDuration.sec,
                           dpd->leaseDuration.nanosec);

                    epoint = dpd->availableBuiltinEndpoints;
                    for (j = 0; j < 32; j++)
                    {
                        lbuf[j] = (epoint & 0x80000000) ? '1' : '0';
                        epoint = epoint << 1;
                    }
                    lbuf[32] = '\0';

                    printf("   builtin endpoints    : %s :: ", lbuf);
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER)
                        printf("DPD-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR)
                        printf("DPD-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER)
                        printf("DWD-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR)
                        printf("DWD-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER)
                        printf("DRD-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR)
                        printf("DRD-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_TOPIC_ANNOUNCER)
                        printf("TOP-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_TOPIC_DETECTOR)
                        printf("TOP-rd ");
                    if (dpd->availableBuiltinEndpoints & BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER)
                        printf("PM-wr ");
                    if (dpd->availableBuiltinEndpoints & BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER)
                        printf("PM-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATIONS_SECURE_WRITER)
                        printf("SDWD-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PUBLICATIONS_SECURE_READER)
                        printf("SEC_DWD-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTIONS_SECURE_WRITER)
                        printf("SEC_DRD-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_SUBSCRIPTIONS_SECURE_READER)
                        printf("SEC_DRD-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_SECURE_WRITER)
                        printf("SEC_PM-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_SECURE_READER)
                        printf("SEC_PM-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_STATELESS_SECURE_WRITER)
                        printf("SEC_PMS-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_STATELESS_SECURE_READER)
                        printf("SEC_PMS-rd ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_VOLATILE_MESSAGE_SECURE_WRITER)
                        printf("SEC_PVM-wr ");
                    if (dpd->availableBuiltinEndpoints & DISC_BUILTIN_ENDPOINT_PARTICIPANT_VOLATILE_MESSAGE_SECURE_READER)
                        printf("SEC_PVM-rd ");

                    printf("\n");
                    
                    j = 0;
                    list_for_each_entry(l, RTPS_Locator, &dpd->metatrafficUnicastLocatorList, list, plh) j++ ;
                    std::cout << "   Meta Unicast size    : " << j << " " << std::endl;
                    //    printf("   Locator              : meta unicast [%d] %s\n", j++, locatorToString(l, lbuf, 80));
                    //
                    j = 0;
                    list_for_each_entry(l, RTPS_Locator, &dpd->metatrafficMulticastLocatorList, list, plh){
                        j++;
                    } 
                        
                    std::cout << "   Meta Multicast size  : " << j << " " << std::endl;

                    //    printf("   Locator              : meta multicast [%d] %s\n", j++, locatorToString(l, lbuf, 80));
                    //
                    j = 0;
                    list_for_each_entry(l, RTPS_Locator, &dpd->defaultUnicastLocatorList, list, plh) j++;
                    std::cout << "   Default Unicast size : " << j << " " << std::endl;

                    //    printf("   Locator              : user unicast [%d] %s\n", j++, locatorToString(l, lbuf, 80));
                    //
                    j = 0;
                    list_for_each_entry(l, RTPS_Locator, &dpd->defaultMulticastLocatorList, list, plh);
                    std::cout << "   Def. Multicast size  : " << j << " " << std::endl;
                    
                    //    printf("   Locator              : user multicast [%d] %s\n", j++, locatorToString(l, lbuf, 80));

                    
                    


                   if (dpd->properties.value._length)
                    { 
                        printf("   properties [%.5d]   : \n", dpd->properties.value._length);
                        for (j = 0; j < dpd->properties.value._length; j++)
                        {
                            DDS_Property_t *prop = &dpd->properties.value._buffer[j];
                            printf("              [%.5d]   : %40s :: '%s'\n",
                                   j, prop->name, prop->value);
                        }
                    }
                    else
                        printf("   properties [%.5d]   : \n", dpd->properties.value._length);
#if defined(DDS_SECURITY) && (DDS_SECURITY)
                    /* security: identity token & permissions token */
                    if (dpd->identity_token)
                    {
                        printf("   identity_token       : \n");
                        DDS_Security_DataHolder_print(stdout, "                        : ", dpd->identity_token);
                        printf("\n");
                    }
                    else
                        printf("  identity_token        : NULL\n");

                    if (dpd->permissions_token)
                    {
                        printf("   permissions_token    : \n");
                        DDS_Security_DataHolder_print(stdout, "                        : ", dpd->identity_token);
                        printf("\n"); 
                    }
                    else
                        printf("  permissions_token     : NULL\n");
#endif

                }
            }
            else
            {
                /* DEAD */
                /* TODO: get 'key' from handle ?  */
                unsigned char *key = (unsigned char *)&samples._buffer[i]->key;
                printf("-----------------------------------------------------------------\n");
                printf("TIME: %d.%09d\n",
                       sample_infos._buffer[i]->reception_timestamp.sec,
                       sample_infos._buffer[i]->reception_timestamp.nanosec);
                 
            }
        }
        DDS_DCPSParticipantDataReader_return_loan(bi_dr, &samples, &sample_infos);
    }
}


void bi_dwd_data_available( DDS_DataReader dr){
    // uh ?
    DDS_DCPSParticipantDataReader bi_dr = (DDS_DCPSParticipantDataReader)dr;
    DDS_ReturnCode_t retval;
    DDS_DCPSParticipantPtrSeq samples;
    DDS_SampleInfoSeq sample_infos;

    INIT_SEQ(samples);
    INIT_SEQ(sample_infos);

    retval = DDS_DCPSParticipantDataReader_read(     bi_dr,
                                                &samples,
                                                &sample_infos,
                                                DDS_LENGTH_UNLIMITED,
                                                DDS_NOT_READ_SAMPLE_STATE,
                                                DDS_ANY_VIEW_STATE,
                                                DDS_ANY_INSTANCE_STATE);
    
    if (retval == DDS_RETCODE_OK)
    {
        uint32_t i;
        for (i = 0; i < samples._length; i++)
        {
            // if (sample_infos._buffer[i]->instance_handle == my_handle)
            //   continue;

            if (sample_infos._buffer[i]->valid_data)
            {
                unsigned char *participantKey;
                unsigned char *key;
                char * topic;
                char * type;
                struct list_head *plh;
                RTPS_Locator *l;

                char * entityName;


                unsigned int j;
                if ((sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE))
                {

                    RTPS_DiscoveredWriterData *dwd = (RTPS_DiscoveredWriterData *)samples._buffer[i];


                    participantKey = (unsigned char *)&dwd->participant_key;
                    key = (unsigned char *)&dwd->key;
                    topic = dwd->topic_name;
                    type = dwd->type_name;


                    // soo here we find the buttons... I think ... well idk. I coul dlook for one that matches name and 
                    if(topic[0] == "B"[0]){
                        //es un buttontont
                        ButtonInfo * bi = new ButtonInfo;

                        memcpy(&bi->dpKey, participantKey, sizeof(unsigned char) * 12);

                        // BUTTON_DATA_***** <- clock name. sooo button name = what? 
                        strcpy(bi->dataTopicName, topic); 

                        bi->buttonName =  &bi->dataTopicName[12];// ill want to append _orange, or _blue when I gui it, but for now... 

                        //can I do others?. 
                        if(!strcmp(bi->buttonName, "UNKNOWN")){
                            //should have a clock for it.. 
                            //look in the clocks first, then in the map with the dpKey we get.. if in map add to vec in map, else make map entry. 
                            bool matched = 0;
                            for(std::vector<ClockInfo*>::iterator it = clocks.begin(); it != clocks.end(); it++){
                                if(strcmp((*it)->clockName, bi->buttonName )){
                                    //has clock with same name
                                    bool foundInMap = 0;
                                    for (const auto& pair : matchedButtons) {
                                        if(pair.first == (*it)->dpKey){
                                            foundInMap = 1;
                                            pair.second->push_back(bi);
                                            matched = 1;
                                            continue;
                                        }
                                    }
                                    if(!foundInMap){
                                        //add to map
                                        std::vector<ButtonInfo*> * temp = new std::vector<ButtonInfo*>;
                                        temp->push_back(bi);
                                        matched = 1;
                                        matchedButtons[(*it)->dpKey] = temp; 
                                    }
                                }
                            }
                            if(! matched){
                                unknownButtons.push_back(bi);
                            }
                        }
                        else {
                            unknownButtons.push_back(bi);
                        }


                    }
                    
                   


                    printf("------------------------------------------------------------------\n");
                    printf(" %s Writer\n", (sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE ? "NEW   " : "UPDATE"));
                    printf(" PARTICIPANT KEY  : %.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x\n",
                           participantKey[0], participantKey[1], participantKey[2], participantKey[3],
                           participantKey[4], participantKey[5], participantKey[6], participantKey[7],
                           participantKey[8], participantKey[9], participantKey[10], participantKey[11]);
                    printf(" TOPIC NAME       : %s \n", topic);
                    printf(" TYPE NAME        : %s \n", type);

                }

            }
            else
            {
                /* DEAD */
                ///* TODO: get 'key' from handle ?  */
                unsigned char *key = (unsigned char *)&samples._buffer[i]->key;
                printf("-----------------------------------------------------------------\n");
                printf("TIME: %d.%09d\n",
                       sample_infos._buffer[i]->reception_timestamp.sec,
                       sample_infos._buffer[i]->reception_timestamp.nanosec);
                 
            }
        }
        DDS_DCPSParticipantDataReader_return_loan(bi_dr, &samples, &sample_infos);
    }
    
}



void bi_drd_data_available( DDS_DataReader dr){
    // uh ?

    DDS_DCPSParticipantDataReader bi_dr = (DDS_DCPSParticipantDataReader)dr;
    DDS_ReturnCode_t retval;
    DDS_DCPSParticipantPtrSeq samples;
    DDS_SampleInfoSeq sample_infos;

    INIT_SEQ(samples);
    INIT_SEQ(sample_infos);

    retval = DDS_DCPSParticipantDataReader_read(     bi_dr,
                                                &samples,
                                                &sample_infos,
                                                DDS_LENGTH_UNLIMITED,
                                                DDS_NOT_READ_SAMPLE_STATE,
                                                DDS_ANY_VIEW_STATE,
                                                DDS_ANY_INSTANCE_STATE);
    if (retval == DDS_RETCODE_OK)
    {
        uint32_t i;
        for (i = 0; i < samples._length; i++)
        {
            // if (sample_infos._buffer[i]->instance_handle == my_handle)
            //   continue;

            if (sample_infos._buffer[i]->valid_data)
            {
                unsigned char *participantKey;
                unsigned char *key;
                char * topic;
                char * type;
                struct list_head *plh;
                RTPS_Locator *l;

                char * entityName;


                unsigned int j;
                if ((sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE))
                {
                    
                    RTPS_DiscoveredReaderData *drd = (RTPS_DiscoveredReaderData *)samples._buffer[i];


                    participantKey = (unsigned char *)&drd->participant_key;
                    key = (unsigned char *)&drd->key;
                    topic = drd->topic_name;
                    type = drd->type_name;


                    // soo here we look for the clock, need a CLOCK_STATE or ajacent topic, to tell clock what time and whatever to display, work on truct in a sec, take care of timeing in here, not on esp
                    // do I need to specify, oh its on robobrawl_clock_state, opposed to dpd_clock_state.... I might. I think I will. 
                    // orrrr cool thing, make it incredably dynamic, have a writer on "awaiting instruction" topic, witch this then sends hey your a button for this clock, get on these topics. 
                    //will be hard to figure out witch buttons go to witch clocks and stuff... could do a matching system, turn a clock bright color, b elike touch the orange button for this one, then the blue one. and we just do that.... 
                    //get the message on button press and attach that kay to a message that says "hey go over to this/these topics". 
                    // or I put a bit_idle, dpd_idle and stuff for the buttond to go into auto, then asign those once I have a clock. becuase I can make the clocks hardcoded rorbo clock and dpd clock. 

                    // could add a unknown clock topuic for expandabiliy, have user input what kind of clock it is, and asign that topic, might be a nice feature to have even when the clocks are 'locked' ( put a go to this topic in the clcok contol type ).
                    // i could do the matching system(once I have some buttons on the network), tehat then writes it into memory of the esp, to not have to do it on every startup, only when its nessisary. 

                    // I thnk I like ^^^ that one. 
                    //use default qos fro all of these. the types will just be copied for every button / clcok network. 
                    // NOTE: I need asignable times for the clocks, not all the 5 or wahteer robobrawl is.  

                    // need a consistaint style for these: CLOCK_STATE_*******, *** -> UNKNOWN, ROBOBRAWL, ect.
                    //buttons will be some kind of BUTTON_CLOCKNAME_STATES or something( not starting with a C)

                    //sooo
                    if(drd->topic_name[0] == 'C'){
                        // then its a clock
                        ClockInfo* cd = new ClockInfo;

                        memcpy(&(cd->dpKey), participantKey, sizeof(unsigned char) * 12); // copy the dpKey, probably. 
                        cd->topicName = topic;
                        cd->clockName = &(drd->topic_name[12]);// fetch the end of the topic name ( if I can count)

                        clocks.push_back(cd);
                        
                    }


                    printf("------------------------------------------------------------------\n");
                    printf(" %s Reader\n", (sample_infos._buffer[i]->view_state == DDS_NEW_VIEW_STATE ? "NEW   " : "UPDATE"));
                    printf(" PARTICIPANT KEY  : %.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x.%.2x%.2x%.2x%.2x\n",
                        participantKey[0], participantKey[1], participantKey[2], participantKey[3],
                        participantKey[4], participantKey[5], participantKey[6], participantKey[7],
                        participantKey[8], participantKey[9], participantKey[10], participantKey[11]);
                    
                    printf(" TOPIC NAME       : %s \n", topic);
                    printf(" TYPE NAME        : %s \n", type);

                }

            }
            else
            {
                /* DEAD */
                /* TODO: get 'key' from handle ?  */
                unsigned char *key = (unsigned char *)&samples._buffer[i]->key;
                printf("-----------------------------------------------------------------\n");
                printf("TIME: %d.%09d\n",
                       sample_infos._buffer[i]->reception_timestamp.sec,
                       sample_infos._buffer[i]->reception_timestamp.nanosec);
                 
            }
        }
        DDS_DCPSParticipantDataReader_return_loan(bi_dr, &samples, &sample_infos);
    }


}


static DDS_DataReaderListener bi_dpd_listener =
    {
        /* .on_requested_deadline_missed  */ NULL,
        /* .on_requested_incompatible_qos */ NULL,
        /* .on_sample_rejected            */ NULL,
        /* .on_liveliness_changed         */ NULL,
        /* .on_data_available             */ bi_dpd_data_available,
        /* .on_subscription_matched       */ NULL,
        /* .on_sample_lost                */ NULL};


static DDS_DataReaderListener bi_dwd_listener =
    {
        /* .on_requested_deadline_missed  */ NULL,
        /* .on_requested_incompatible_qos */ NULL,
        /* .on_sample_rejected            */ NULL,
        /* .on_liveliness_changed         */ NULL,
        /* .on_data_available             */ bi_dwd_data_available,
        /* .on_subscription_matched       */ NULL,
        /* .on_sample_lost                */ NULL};


static DDS_DataReaderListener bi_drd_listener =
    {
        /* .on_requested_deadline_missed  */ NULL,
        /* .on_requested_incompatible_qos */ NULL,
        /* .on_sample_rejected            */ NULL,
        /* .on_liveliness_changed         */ NULL,
        /* .on_data_available             */ bi_drd_data_available,
        /* .on_subscription_matched       */ NULL,
        /* .on_sample_lost                */ NULL};

void add_discovery_listeners(DDS_DomainParticipant dp)
{
    DDS_Subscriber bi_sub = DDS_DomainParticipant_get_builtin_subscriber(dp);

    DDS_DataReader bi_dr_dpd = DDS_Subscriber_lookup_datareader(bi_sub, "DCPSParticipant");
    DDS_DataReader bi_dr_dwd = DDS_Subscriber_lookup_datareader(bi_sub, "DCPSPublication");
    DDS_DataReader bi_dr_drd = DDS_Subscriber_lookup_datareader(bi_sub, "DCPSSubscription");

    //DDS_DataReader_set_listener(bi_dr_dpd, &bi_dpd_listener, DDS_DATA_AVAILABLE_STATUS);
    DDS_DataReader_set_listener(bi_dr_dwd, &bi_dwd_listener, DDS_DATA_AVAILABLE_STATUS);
    DDS_DataReader_set_listener(bi_dr_drd, &bi_drd_listener, DDS_DATA_AVAILABLE_STATUS);
}