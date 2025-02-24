#ifndef Pds_Eb_MebContributor_hh
#define Pds_Eb_MebContributor_hh

#include "eb.hh"
#include "EbLfClient.hh"

#include <cstdint>
#include <memory>
#include <vector>


class MetricExporter;

namespace XtcData {
  class Dgram;
};

namespace Pds {
  namespace Eb {

    class EbLfLink;

    class MebContributor
    {
    public:
      MebContributor(const MebCtrbParams&, std::shared_ptr<MetricExporter>&);
    public:
      int  connect(const MebCtrbParams&, void* region, size_t size);
      void shutdown();
    public:
      int  post(const XtcData::Dgram* dataDatagram); // Transitions
      int  post(const XtcData::Dgram* dataDatagram,
                uint32_t              destination);  // L1Accepts
    private:
      size_t                 _maxEvSize;
      size_t                 _maxTrSize;
      size_t                 _trSize;
      EbLfClient             _transport;
      std::vector<EbLfLink*> _links;
      unsigned               _id;
      unsigned               _verbose;
    private:
      uint64_t               _eventCount;
    };
  };
};

#endif
