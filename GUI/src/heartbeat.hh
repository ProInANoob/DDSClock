
#ifndef HEARTBEAT_HH
#define HEARTBEAT_HH

#include <map>
#include <utility>
#include "robobrawl.hh"
#include "context.hh"

class HeartbeatClient{
public:
  virtual void handle_heartbeat(const Heartbeat & data) = 0;
};


class HeartbeatWriter : public dds::sub::NoOpDataReaderListener<Heartbeat>
{

  public:
  
    // -------------------------------------------------------
    HeartbeatWriter() :
      dw( dds::core::null ),
      dr( dds::core::null ) { }

    // -------------------------------------------------------
    ~HeartbeatWriter() {
    }
  
    // -------------------------------------------------------
    void
    init(   HeartbeatClient             * hb_client,
            Context                     & context,
	        const std::string           & deviceId )
    {
      auto topic = dds::topic::Topic<Heartbeat>( context.participant(), "Heartbeat" );
      dw = dds::pub::DataWriter<Heartbeat>( context.publisher(), topic );
      hb.deviceId( deviceId );
      hb.sequenceNumber( 0 );
      
      dds::pub::qos::DataWriterQos dw_qos = dw.qos();
      dw_qos <<  dds::core::policy::UserData(dds::core::ByteSeq( deviceId.begin(), deviceId.end() ) );
      dw.qos( dw_qos );
      dds::sub::qos::DataReaderQos dr_qos;

      dr = dds::sub::DataReader(context.subscriber(), topic, dr_qos);
      dr.listener(this, dds::core::status::StatusMask::all());
    }

    // -------------------------------------------------------
    void clear() {
      dw    = dds::core::null;
      dr    = dds::core::null;
    }
  
    // -------------------------------------------------------
    void publish() {
      this->dw.write( hb );
      hb.sequenceNumber( hb.sequenceNumber() + 1 );
    }

    // -------------------------------------------------------
    void on_data_available(dds::sub::DataReader<Heartbeat> &reader)
    {
      dds::sub::LoanedSamples<Heartbeat> samples = reader.take();
      for (const auto &sample : samples)
      {
        if (sample.info().valid())
        {
          if (hb_client)
          {
            hb_client->handle_heartbeat(sample.data());
          }
        }
      }
    }
  
  protected:
    dds::pub::DataWriter< Heartbeat >      dw;
    dds::sub::DataReader< Heartbeat >      dr;
    HeartbeatClient                      * hb_client;
    Heartbeat                              hb;
  };


  #endif