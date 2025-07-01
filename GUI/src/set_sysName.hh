#ifndef SSN_HH
#define SSN_HH


#include "robobrawl.hh"
#include "context.hh"

class SetSysName{
public:
    SetSysName() :
      dw( dds::core::null ) { }

    // -------------------------------------------------------
    ~SetSysName() {
    }
  
    // -------------------------------------------------------
    void
    init(   Context                          & context,
            const std::string                & deviceId )
    {
      auto topic = dds::topic::Topic<SysName>( context.participant(), "SetSysName" );
      dw = dds::pub::DataWriter<SysName>( context.publisher(), topic );
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
    void publish(SysName & command) {
      this->dw.write( command );
    }



protected:
    dds::pub::DataWriter< SysName >      dw;
    std::string                               devId;

};



#endif