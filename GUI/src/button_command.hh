#ifndef BC_HH
#define BC_HHH


#include "robobrawl.hh"
#include "context.hh"

class ButtonCommandWriter{
public:
    ButtonCommandWriter() :
      dw( dds::core::null ) { }

    // -------------------------------------------------------
    ~ButtonCommandWriter() {
    }
  
    // -------------------------------------------------------
    void
    init(   Context                          & context,
            const std::string                & deviceId )
    {
      auto topic = dds::topic::Topic<ButtonCommand>( context.participant(), "ButtonCommand" );
      dw = dds::pub::DataWriter<ButtonCommand>( context.publisher(), topic );
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
    void publish(ButtonCommand & command) {
      this->dw.write( command );
    }



protected:
    dds::pub::DataWriter< ButtonCommand >      dw;
    std::string                               devId;

};



#endif