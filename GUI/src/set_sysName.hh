#ifndef SSN_HH
#define SSN_HH


#include "robobrawl.hh"
#include "context.hh"

class SetSysName : public dds::pub::NoOpDataWriterListener<SysName>{
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
      auto topic = dds::topic::Topic<SysName>( context.participant(), "SysName" );
      dw = dds::pub::DataWriter<SysName>( context.publisher(), topic );
      dw.listener(this, dds::core::status::StatusMask::publication_matched() | dds::core::status::StatusMask::offered_incompatible_qos() );
      devId = deviceId;

      dds::pub::qos::DataWriterQos dw_qos = dw.qos();
      dw_qos <<  dds::core::policy::UserData(dds::core::ByteSeq( deviceId.begin(), deviceId.end() ) );
      dw.qos( dw_qos );

    }

    // -------------------------------------------------------
    void clear() {
      dw.listener(NULL, dds::core::status::StatusMask::all());
      dw = dds::core::null;
    }
  
  
    // -------------------------------------------------------
    void publish(SysName & command) {
      this->dw.write( command );
    }


    virtual void on_publication_matched(
        dds::pub::DataWriter<SysName>& writer,
        const dds::core::status::PublicationMatchedStatus& status)  {
        std::cout << "Publication matched. Total count: " << status.total_count() << std::endl;
    }

    virtual void on_offered_incompatible_qos(
        dds::pub::DataWriter<SysName>& writer,
        const dds::core::status::OfferedIncompatibleQosStatus& status)  {
        std::cout << "Offered incompatible QoS. Total count: " << status.total_count() << std::endl;
    }


protected:
    dds::pub::DataWriter< SysName >           dw;
    std::string                               devId;

};



#endif