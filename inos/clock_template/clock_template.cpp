


  #include <string.h>
  #include <sys/param.h>
  #include <string> 
  #include <vector> 
  #include <map> 
  #include "freertos/FreeRTOS.h"
  #include "freertos/task.h"
  #include "freertos/event_groups.h"
  #include "esp_system.h"
  #include "esp_event.h"
  #include "esp_log.h"
  #include "nvs_flash.h"
  #include "esp_netif.h"
  #include "protocol_examples_common.h"

  #include "lwip/err.h"
  #include "lwip/sockets.h"
  #include "lwip/sys.h"
  #include <lwip/netdb.h>

  #include <dds/dds.h>
  #include <dds/toc_util.h>
  #include "example.h"
  #include "exampleTypeSupport.h"
  #include "exampleDataWriter.h"
  #include "exampleDataReader.h"

  #include "led_strip.h"

  static const char *TAG = "example";
  // static const char *payload = "Message from ESP32 ";
  
  // How many leds in your strip?
  #define NUM_LEDS 95 

  
  /*************************************************************/
  #define CDX_HEAP_SIZE (1024 * 1024) /* 1 MB */
  // ESP has 160K limit on DRAM (global variables)
  // however, we can use malloc() to get a larger buffer...
  
  // unsigned char cdx_heap[CDX_HEAP_SIZE];
  
  /*************************************************************/
  
  
  DDS_DomainParticipant dp = NULL;
  DDS_DomainParticipantQos dp_qos;
  DDS_Publisher pub = NULL;
  DDS_Subscriber sub = NULL;
  DDS_Topic DeviceInfo_topic;    // discovery data
  DDS_Topic SysName_topic;       // sub, for cheinging system allocaiton
  DDS_Topic ClockCommand_topic; // sub, for commands, duh
  DeviceInfoDataWriter di_dw = NULL;
  SysNameDataReader sn_dr = NULL;
  ClockCommandDataReader bc_dr = NULL;
  DDS_DataWriterQos dw_qos;
  DDS_DataReaderQos dr_qos;
  DeviceInfo devInfo; 
  ClockCommand clockState; 


  // leds config
  // LED strip common configuration
  led_strip_config_t strip_config = {
      .strip_gpio_num = BLINK_GPIO,  // The GPIO that connected to the LED strip's data line
      .max_leds = 1,                 // The number of LEDs in the strip,
      .led_model = LED_MODEL_WS2812, // LED strip model, it determines the bit timing
      .color_component_format = LED_STRIP_COLOR_COMPONENT_FMT_GRB, // The color component format is G-R-B
      .flags = {
          .invert_out = false, // don't invert the output signal
      }
  };
  
  /// RMT backend specific configuration
  led_strip_rmt_config_t rmt_config = {
      .clk_src = RMT_CLK_SRC_DEFAULT,    // different clock source can lead to different power consumption
      .resolution_hz = 10 * 1000 * 1000, // RMT counter clock frequency: 10MHz
      .mem_block_symbols = 64,           // the memory size of each RMT channel, in words (4 bytes)
      .flags = {
          .with_dma = true, // DMA feature is available on chips like ESP32-S3/P4
      }
  };

  led_strip_handle_t led_strip;



  //dev  Config.
  char * deviceId = "TEST_DEVICE_CLOCK";
  int devRole = ROLE_CLOCK;
  char * displayName = "TEST_DEVICE_CLOCK"; //not  implemented on gui  probably
  char * sysName_default  = "TEST";
  char sysName[100];


  bool dds_created = false; 

  static uint
  coredx_logio_routine(const char *cbuf, size_t size)
  {
    char *buf = (char *)cbuf;
    if ((size > 0) && (buf[size - 1] == '\n'))
      buf[size - 1] = '\0';

    ESP_LOGI(TAG, "%s", buf);
    // printf("%s", buf);
    return 0;
  }

  /*************************************************************/
  extern "C" int device_coredx_init(uint32_t ticks_per_sec)
  {
    toc_set_logio_routine(coredx_logio_routine);
    // CoreDX_DDS_set_ipaddr( (unsigned char *)&ipaddr); /* 4 byte IPv4 addr */
    unsigned char *cdx_heap = (unsigned char *)malloc(CDX_HEAP_SIZE);
    if (cdx_heap != NULL)
    {
      CoreDX_DDS_heap_init(cdx_heap, CDX_HEAP_SIZE);
    }
    else
    {
      ESP_LOGE(TAG, "Unable to allocate heap for CoreDX DDS...");
    }

    //toc_set_ticks_per_sec(ticks_per_sec);
    return 0;
  }

  /*****************************************************
   */
  int dds_work(DDS_DomainParticipant dp, int msec)
  {
    static int counter = 0;
    static int heap_max = 0;

    DDS_Duration_t work_time;
    DDS_ReturnCode_t rval;

    work_time.sec = msec / MSEC_PER_SEC;
    while (msec >= MSEC_PER_SEC)
      msec = msec - MSEC_PER_SEC;
    work_time.nanosec = msec * NSEC_PER_MSEC;
    rval = DDS_DomainParticipant_do_work(dp, &work_time);
    if (rval != DDS_RETCODE_OK)
      return -1;

    /* sometimes, print stats on CoreDX DDS memory utilization
    */
    counter++;
    if (counter >= 10)
    {
      uint32_t heap_current = CoreDX_DDS_current_allocation();
      uint32_t heap_peak = CoreDX_DDS_max_allocation();
      if (heap_peak > heap_max)
        heap_max = heap_peak;
      float current_p = heap_current * 100.0 / CDX_HEAP_SIZE;
      float peak_p = heap_peak * 100.0 / CDX_HEAP_SIZE;
      float max_p = heap_max * 100.0 / CDX_HEAP_SIZE;
      ESP_LOGI(TAG, "heap current: 0x%08x : %0.2f%% (peak: 0x%08x : %0.2f%%) (all time max: 0x%08x : %0.2f%%)",

              (unsigned)heap_current, current_p,
              (unsigned)heap_peak, peak_p,
              (unsigned)heap_max, max_p);
      counter = 0;
    }

    return 0;
  }

  void readSysName()
  {
    SysNamePtrSeq samples;
    DDS_SampleInfoSeq samples_info;
    DDS_ReturnCode_t retval;
    DDS_SampleStateMask ss = DDS_ANY_SAMPLE_STATE;
    DDS_ViewStateMask vs = DDS_ANY_VIEW_STATE;
    DDS_InstanceStateMask is = DDS_ANY_INSTANCE_STATE;

    INIT_SEQ(samples);
    INIT_SEQ(samples_info);

    retval = SysNameDataReader_take(sn_dr, &samples, &samples_info,
                                    DDS_LENGTH_UNLIMITED,
                                    ss,
                                    vs,
                                    is);

    if (retval == DDS_RETCODE_OK)
    {
      unsigned int i;

      /* iterrate through the samples */
      for (i = 0; i < samples._length; i++)
      {
        SysName *smsg = samples._buffer[i];
        DDS_SampleInfo *si = samples_info._buffer[i];

        /* If this sample does not contain valid data,
        * it is a dispose or other non-data command,
        * and, accessing any member from this sample
        * would be invalid.
        */
        if (si->valid_data){
          printf("Sample Received (SN):  msg %d = %s\n", i, smsg->sysName);

          if(strcmp(smsg->sysName, sysName) != 0){
            // new sys Name.. 
            strcpy( sysName, smsg->sysName); 
            devInfo.sysName = sysName; 
            DeviceInfoDataWriter_write(di_dw, &devInfo, DDS_HANDLE_NIL); // update network of new sys namem.
            dds_work(dp, 100); 
          }


        }
      }
      //fflush(stdout);

      /* read() and take() always "loan" the data, we need to
      * return it so CoreDX can release resources associated
      * with it.
      */
      retval = SysNameDataReader_return_loan(sn_dr,
                                              &samples, &samples_info);
      if (retval != DDS_RETCODE_OK)
        printf("ERROR (%s): unable to return loan of samples\n",
              DDS_error(retval));
    }
    else
    {
      //printf("ERROR (%s) taking samples from DataReader\n", DDS_error(retval));
    }
  }

  void readClockCommand(){
    ClockCommandPtrSeq samples;
    DDS_SampleInfoSeq samples_info;
    DDS_ReturnCode_t retval;
    DDS_SampleStateMask ss = DDS_ANY_SAMPLE_STATE;
    DDS_ViewStateMask vs = DDS_ANY_VIEW_STATE;
    DDS_InstanceStateMask is = DDS_ANY_INSTANCE_STATE;

    INIT_SEQ(samples);
    INIT_SEQ(samples_info);

    retval = ClockCommandDataReader_take(bc_dr, &samples, &samples_info,
                                    DDS_LENGTH_UNLIMITED,
                                    ss,
                                    vs,
                                    is);

    if (retval == DDS_RETCODE_OK)
    {
      unsigned int i;

      /* iterrate through the samples */
      for (i = 0; i < samples._length; i++)
      {
        ClockCommand *smsg = samples._buffer[i];
        DDS_SampleInfo *si = samples_info._buffer[i];

        /* If this sample does not contain valid data,
        * it is a dispose or other non-data command,
        * and, accessing any member from this sample
        * would be invalid.
        */
        if (si->valid_data){
          printf("Sample Received (BC):  msg %d  \n", i);
          // do things.... 
          ClockCommand_copy(clockState, smsg);


        }
      }
      //fflush(stdout);

      /* read() and take() always "loan" the data, we need to
      * return it so CoreDX can release resources associated
      * with it.
      */
      retval = ClockCommandDataReader_return_loan(bc_dr,
                                              &samples, &samples_info);
      if (retval != DDS_RETCODE_OK)
        printf("ERROR (%s): unable to return loan of samples\n",
              DDS_error(retval));
    }
    else
    {
      //printf("ERROR (%s) taking samples from DataReader\n", DDS_error(retval));
    }
  }


  // CLOCK stuff ----------------------------------------------

  std::vector<std::string> mapNames = {"dig1", "dig2", "dig3", "dig4", "colonUp", "colonDown", "decimal", "gear1", "gear2" }; 
  std::map<Colors, std::tuple<int>> colorToRgb;

  void initMap() {
                        // 6, 1, 2, 4, 5, 7, 3
    TEST_MAP["dig1"] = {20, 5, 8, 14, 17, 23, 11};
    TEST_MAP["dig2"] = {41, 26, 29, 35, 38, 44, 32};
    TEST_MAP["dig3"] = {65, 50, 53, 59, 62, 68, 56};
    TEST_MAP["dig4"] = {86, 71, 74, 80, 83, 89, 77};
    TEST_MAP["colonUp"] = {48}; //upper colon
    TEST_MAP["colonDown"] = {47}; //lower colon
    TEST_MAP["decimal"] = {46}; //period 
    TEST_MAP["gear1"] = {0, 1, 2, 3};
    TEST_MAP["gear2"] = {92, 93, 94, 95};

    colorToRgb[Colors::COLOR_RED] = std::make_tuple(225, 0, 0);
    colorToRgb[Colors::COLOR_GREEN] = std::make_tuple(0, 255, 0);
    colorToRgb[Colors::COLOR_BLUE] = std::make_tuple(0, 0, 255);
    colorToRgb[Colors::COLOR_BLACK] = std::make_tuple(0, 0, 0);
    colorToRgb[Colors::COLOR_YELLOW] = std::make_tuple(225, 0xC4, 0);
    colorToRgb[Colors::COLOR_ORANGE] = std::make_tuple(0xF3, 0x80, 0x22);

  }

  // Color to RGB 


  void writeSolid(Colors color){

    auto colorVals = colorToRgb[color];

    for(std::string name : mapNames ){
        if(name != "gear1" && name != "gear2"){ // let the grars be probably... 
          for(auto num : TEST_MAP[name]){
            led_strip_set_pixel(led_strip, num, std::get<0>(colorVals), std::get<0>(colorVals), std::get<0>(colorVals) );
          }
        }
       
    }
  }

  void writeGear(Colors color, bool leftGear){
    auto colorVals = colorToRgb[color];
    std::string gear = "gear1"; // no clue witch one is left / right, or blue / orange. 
    if(leftGear){
      gear = "gear2";
    }
    for(auto num : TEST_MAP[gear]){
      led_strip_set_pixel(led_strip, num, std::get<0>(colorVals), std::get<0>(colorVals), std::get<0>(colorVals) );
    }
  }







  static void dds_example_task(void *pvParameters)
  {

    ESP_LOGI(TAG, "DDS example, here we go...");

    ESP_LOGI(TAG, "TICK_RATE (Hz): %d", configTICK_RATE_HZ);
    ESP_LOGI(TAG, "TICK_PERIOD/MS: %ld", portTICK_PERIOD_MS);
    device_coredx_init(configTICK_RATE_HZ);

    DDS_DomainParticipantFactory_get_default_participant_qos(&dp_qos);
    // dp_qos.logging.flags += 64; // transport logging
    dp_qos.discovery.dpd_lease_duration.sec = 20;
    dp_qos.discovery.dpd_lease_duration.nanosec = 0;
    dp = DDS_DomainParticipantFactory_create_participant(0, &dp_qos, NULL, 0);
    DDS_DomainParticipantQos_clear(&dp_qos);

    if (dp)
    {
      ESP_LOGI(TAG, "Created DomainParticipant...");

      
      
      

      pub = DDS_DomainParticipant_create_publisher(dp, DDS_PUBLISHER_QOS_DEFAULT, NULL, 0);
      if (pub)
      {
        DDS_Publisher_get_default_datawriter_qos(pub, &dw_qos);
        
        DeviceInfoTypeSupport_register_type(dp, "DeviceInfo");
        DeviceInfo_topic = DDS_DomainParticipant_create_topic(dp, "DeviceInfo", "DeviceInfo", DDS_TOPIC_QOS_DEFAULT, NULL, 0);
        di_dw = DDS_Publisher_create_datawriter(pub, DeviceInfo_topic, &dw_qos, NULL, 0);
        ESP_LOGI(TAG, "Created DeviceInfo dataWriter...");

        

        DDS_DataWriterQos_clear(&dw_qos);
      }
      sub = DDS_DomainParticipant_create_subscriber(dp, DDS_SUBSCRIBER_QOS_DEFAULT, NULL, 0);
      if (sub)
      {
        DDS_Subscriber_get_default_datareader_qos(sub, &dr_qos);

        dr_qos.reliability.kind = DDS_RELIABLE_RELIABILITY_QOS;

        SysNameTypeSupport_register_type(dp, "SysName");
        SysName_topic = DDS_DomainParticipant_create_topic(dp, "SysName", "SysName", DDS_TOPIC_QOS_DEFAULT, NULL, 0);
        sn_dr = DDS_Subscriber_create_datareader(sub, DDS_Topic_TopicDescription(SysName_topic), &dr_qos, NULL, 0);
        ESP_LOGI(TAG, "Created SysName DataReader...");

        ClockCommandTypeSupport_register_type(dp, "ClockCommand");
        ClockCommand_topic = DDS_DomainParticipant_create_topic(dp, "ClockCommand", "ClockCommand", DDS_TOPIC_QOS_DEFAULT, NULL, 0);
        bc_dr = DDS_Subscriber_create_datareader(sub, DDS_Topic_TopicDescription(ClockCommand_topic), &dr_qos, NULL, 0);
        ESP_LOGI(TAG, "Created ClockCommand DataReader...");

        DDS_DataReaderQos_clear(&dr_qos);
      }
      if (di_dw) // probably made all of them....
      {
        
        DeviceInfo_init(&devInfo);
        devInfo.deviceId = deviceId;
        devInfo.role = devRole;
        devInfo.displayName = displayName;
        devInfo.sysName = sysName;

        // create other strucs
        
        

        dds_work(dp, 1000); 
        dds_work(dp, 1000); 
        dds_work(dp, 1000); 



        
        for(int i = 0; i < 15; i++){

          DeviceInfoDataWriter_write(di_dw, &devInfo, DDS_HANDLE_NIL); // write inint message, would prefer nto to do this on loop but..
          dds_work(dp, 100); 
        }
          
        printf("Finnished writing deviecinfos\n");
        
        dds_created = true;
        
        //DDS_SubscriptionMatchedStatus  status;
        while (1)
        {
          // so heres my const / loop for dds side, Ill have button polling and writing elsewhere... (main loop)

          // reading polling -
          
          readSysName();
          readClockCommand();


          devInfo.sysName = sysName;

          DeviceInfoDataWriter_write(di_dw, &devInfo, DDS_HANDLE_NIL); 
          //DDS_DataReader_get_subscription_matched_status(sn_dr, &status);
          //printf("Subscition matched count: %ld\n", status.total_count);


          // write and increment hearbeat.

          dds_work(dp, 1000); // do DDS work for 100ms -> ~10 per sec
        }
      }
      else
      {
        ESP_LOGE(TAG, "Unable to create DataWriter...");
      }
    }
    else
    {
      ESP_LOGE(TAG, "Unable to create DomainParticipant...");
    }

    vTaskDelete(NULL);
  }


  static void device_task(void *pvParameters){
    initMap();
    while( 1 ){
      vTaskDelay(200 / portTICK_PERIOD_MS);

      //TODO: 
      //  FILLCOLOR(Colors color)
      //  SetGears / set GRaer Left or someithng. 
      //  Write Time
      //  others? 
      
      if(!clockState.isOff){
        if(clockState.doDisplayTime){
          // run some code to display a time some kinda displayTime{int, int, color}     
          
        }else{
          // solid color Was the intentaion I believe.
          
        }
      }
      else {
        //clock is off. 
        writeSolid(Colors::COLOR_BLACK);

      }

    }
  }


  extern "C" void app_main(void)
  {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    strcpy(sysName, sysName_default);

    /* This helper function configures Wi-Fi or Ethernet, as selected in menuconfig.
    * Read "Establishing Wi-Fi or Ethernet Connection" section in
    * examples/protocols/README.md for more information about this function.
    */
    ESP_ERROR_CHECK(example_connect());

    xTaskCreatePinnedToCore(dds_example_task, "dds_readers", 16384, NULL, 5, NULL, 1);
    xTaskCreatePinnedToCore(device_task, "device_stuff", 16384, NULL, 5, NULL, 0);


    
  }
