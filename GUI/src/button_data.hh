#ifndef BD_HH
#define BD_HH

#include <map>
#include <utility>

#include "robobrawl.hh"
#include "context.hh"
// -------------------------------------------------------
// -------------------------------------------------------
class ButtonDataClient{
public:
  virtual void handle_button_data(const ButtonData & data) = 0;
};


class ButtonDataReader : public dds::sub::NoOpDataReaderListener<ButtonData>
{

public:
  // -------------------------------------------------------
  ButtonDataReader() : topic(dds::core::null),
                       dr(dds::core::null) {}

  // -------------------------------------------------------
  ~ButtonDataReader()
  {
  }

  // -------------------------------------------------------
  void
  init( ButtonDataClient * bd_client,
        Context &context)
  {
    this->bd_client = bd_client;

    topic = dds::topic::Topic<ButtonData>(context.participant(), "ButtonData");
    dds::sub::qos::DataReaderQos dr_qos;

    dr = dds::sub::DataReader(context.subscriber(), topic, dr_qos /*, listener, DDS_DATA_AVAILABLE_STATUS */);
    dr.listener(this, dds::core::status::StatusMask::all());
  }

  // -------------------------------------------------------
  void clear()
  {
    dr = dds::core::null;
    topic = dds::core::null;
  }

  // -------------------------------------------------------
    void on_data_available(dds::sub::DataReader<ButtonData> &reader)
    {
      dds::sub::LoanedSamples<ButtonData> samples = reader.take();
      for (const auto &sample : samples)
      {
        if (sample.info().valid())
        {
          if (bd_client)
          {
            
            bd_client->handle_button_data(sample.data());
          }
        }
      }
    }

  // -------------------------------------------------------

protected:
  dds::topic::Topic<ButtonData>    topic;
  dds::sub::DataReader<ButtonData> dr;
  ButtonDataClient               * bd_client;
};

#endif
