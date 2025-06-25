/*****************************************************************
 * 
 *****************************************************************
 *
 *   Copyright(C) 2024      Twin Oaks Computing, Inc
 *   All rights reserved.   Castle Rock, CO 80108
 *
 *****************************************************************/

#ifndef TMS_CONTEXT_HH
#define TMS_CONTEXT_HH

#include <mutex>

#include <dds/pub/ddspub.hpp>
//#include <dds/sub/ddssub.hpp>
#include <dds/coredx_udp_transport.h>

#include "robobrawl.hh"


  class Context {
  
  public:
    // ---------------------------------------------------------------
    Context() :
      participant_( dds::core::null ), 
      publisher_( dds::core::null ),
      subscriber_( dds::core::null ) { }

    // ---------------------------------------------------------------
    ~Context() {
    }
  
    // ---------------------------------------------------------------
    void
    init( int                 domain_id )
    {
      // make sure the DomainParticipant is not enabled initially
      // so we can install a specially configured UDP Transport
      dds::domain::qos::DomainParticipantFactoryQos dpfqos;
      dpfqos << dds::core::policy::EntityFactory::ManuallyEnable();
      dds::domain::DomainParticipant::participant_factory_qos(dpfqos);

      // make sure the DomainParticipant doesn't enable entities that
      // it creates, so we can get everything created and configued first 
      dds::domain::qos::DomainParticipantQos dpqos = dds::domain::DomainParticipant::default_participant_qos();
      dpqos << dds::core::policy::EntityFactory::ManuallyEnable();
      dds::domain::DomainParticipant::default_participant_qos( dpqos );
    
        
      participant_ = dds::domain::DomainParticipant( domain_id );
      // configure CoreDX DDS's udp transport:
      //   - enable IPv6 (not enabled by default)
      {
	CoreDX_UdpTransportConfig udpTransportConfig;
	CoreDX_UdpTransport_get_default_config (&udpTransportConfig);
	CoreDX_UdpTransport_get_env_config (&udpTransportConfig);
	udpTransportConfig.use_ipv6 = 1;
	CoreDX_Transport *transport = CoreDX_UdpTransport_create_transport (&udpTransportConfig);
	participant_->add_transport( transport );
	CoreDX_UdpTransport_clear_config( &udpTransportConfig );
      }    
      // enable the participant now, so we can go on to create other things
      participant_.enable();
    
      publisher_   = dds::pub::Publisher( this->participant_ );
      subscriber_  = dds::sub::Subscriber( this->participant_ );

      publisher_.enable();
      subscriber_.enable();

    }

    // ---------------------------------------------------------------
    void
    clear()
    {
      subscriber_  = dds::core::null;
      publisher_   = dds::core::null;
      participant_ = dds::core::null;
    }
  
    // ---------------------------------------------------------------
    dds::domain::DomainParticipant participant()   { return participant_; }
    dds::pub::Publisher            publisher()     { return publisher_; }
    dds::sub::Subscriber           subscriber()    { return subscriber_; }
  
    
  
  protected:
  
    dds::domain::DomainParticipant participant_;
    dds::pub::Publisher            publisher_;
    dds::sub::Subscriber           subscriber_;
  
  
    std::mutex                     mtx;

  };


#endif

