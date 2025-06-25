#ifndef CC_HH
#define CC_HHH


#include "robobrawl.hh"
#include "context.hh"

class ClockCommandWriter{
public:
    ClockCommandWriter() :
      dw( dds::core::null ) { }

    // -------------------------------------------------------
    ~ClockCommandWriter() {
    }
  
    // -------------------------------------------------------
    void
    init(   Context                          & context,
            const std::string                & deviceId )
    {
      auto topic = dds::topic::Topic<ClockCommand>( context.participant(), "ClockCommand" );
      dw = dds::pub::DataWriter<ClockCommand>( context.publisher(), topic );
      devId = deviceId;

      dds::pub::qos::DataWriterQos dw_qos = dw.qos();
      dw_qos <<  dds::core::policy::UserData(dds::core::ByteSeq( deviceId.begin(), deviceId.end() ) );
      dw.qos( dw_qos );
    }

    // -------------------------------------------------------
    void clear() {
      dw = dds::core::null;
    }
  
  
    // -------------------------------------------------------
    void publish(ClockCommand & command) {
      this->dw.write( command );
    }



protected:
    dds::pub::DataWriter< ClockCommand >      dw;
    std::string                               devId;

};



#endif