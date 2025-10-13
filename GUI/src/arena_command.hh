#ifndef AC_HH
#define AC_HH


#include "robobrawl.hh"
#include "context.hh"

class ArenaCommandWriter{
public:
    ArenaCommandWriter() :
      dw( dds::core::null ) { }

    // -------------------------------------------------------
    ~ArenaCommandWriter() {
    }
  
    // -------------------------------------------------------
    void
    init(   Context                          & context,
            const std::string                & deviceId )
    {
      auto topic = dds::topic::Topic<ArenaCommand>( context.participant(), "ArenaCommand" );
      dw = dds::pub::DataWriter<ArenaCommand>( context.publisher(), topic );
      

      dds::pub::qos::DataWriterQos dw_qos = dw.qos();
      dw_qos <<  dds::core::policy::UserData(dds::core::ByteSeq( deviceId.begin(), deviceId.end() ) );
      dw.qos( dw_qos );

    }

    // -------------------------------------------------------
    void clear() {
      dw = dds::core::null;
    }
  
  
    // -------------------------------------------------------
    void publish(ArenaCommand & command) {
      std::cout << "ArenaCommand Sent. \n";
      this->dw.write( command );
    }



protected:
    dds::pub::DataWriter< ArenaCommand >      dw;

};



#endif