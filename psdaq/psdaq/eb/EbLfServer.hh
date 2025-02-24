#ifndef Pds_Eb_EbLfServer_hh
#define Pds_Eb_EbLfServer_hh

#include "EbLfLink.hh"

#include <stdint.h>
#include <cstddef>
#include <string>
#include <unordered_map>

struct fi_cq_data_entry;

namespace Pds {

  namespace Fabrics {
    class PassiveEndpoint;
    class CompletionQueue;
  };

  namespace Eb {

    using LinkMap = std::unordered_map<fid_ep*, EbLfLink*>;

    class EbLfServer
    {
    public:
      EbLfServer(unsigned verbose);
    public:
      int  initialize(const std::string& addr,    // Interface to use
                      const std::string& port,    // Port to listen on
                      unsigned           nLinks); // Number of links to expect
      int  connect(EbLfLink**, int msTmo = -1);
      int  shutdown(EbLfLink*);
      void shutdown();
      int  pend(fi_cq_data_entry*, int msTmo);
      int  pend(void** context, int msTmo);
      int  pend(uint64_t* data, int msTmo);
      int  poll(uint64_t* data);
      int  pollEQ();
    public:
      const uint64_t& pending() const { return _pending; }
    private:
      int _poll(fi_cq_data_entry*, uint64_t flags);
    private:                            // Arranged in order of access frequency
      Fabrics::EventQueue*      _eq;      // Event Queue
      Fabrics::CompletionQueue* _rxcq;    // Receive Completion Queue
      int                       _tmo;     // Timeout for polling or waiting
      unsigned                  _verbose; // Print some stuff if set
    private:
      uint64_t                  _pending; // Flag set when currently pending
      uint64_t                  _unused;  // Bit list of IDs currently posting
    private:
      Fabrics::PassiveEndpoint* _pep;     // EP for establishing connections
      LinkMap                   _linkByEp;// Map to retrieve link given raw EP
    };
  };
};

inline
int Pds::Eb::EbLfServer::_poll(fi_cq_data_entry* cqEntry, uint64_t flags)
{
  ssize_t rc;

  // Polling favors latency, waiting favors throughput
  if (!_tmo)
  {
    rc = _rxcq->comp(cqEntry, 1); // Uses much less kernel time than comp_wait() with tmo = 0
  }
  else
  {
    rc = _rxcq->comp_wait(cqEntry, 1, _tmo);
    if (rc > 0)  _tmo = 0;     // Switch to polling after successful completion
  }

#ifdef DBG
  if (rc > 0)
  {
    if ((cqEntry->flags & flags) != flags)
    {
      fprintf(stderr, "%s:\n  Expected   CQ entry:\n"
                      "  count %zd, got flags %016lx vs %016lx, data = %08lx\n"
                      "  ctx   %p, len %zd, buf %p\n",
              __PRETTY_FUNCTION__, rc, cqEntry->flags, flags, cqEntry->data,
              cqEntry->op_context, cqEntry->len, cqEntry->buf);
    }
  }
#endif

  return rc;
}

inline
int Pds::Eb::EbLfServer::pend(void** ctx, int msTmo)
{
  fi_cq_data_entry cqEntry;

  int rc = pend(&cqEntry, msTmo);
  *ctx = cqEntry.op_context;

  return rc;
}

inline
int Pds::Eb::EbLfServer::pend(uint64_t* data, int msTmo)
{
  fi_cq_data_entry cqEntry;

  int rc = pend(&cqEntry, msTmo);
  *data = cqEntry.data;

  return rc;
}

inline
int Pds::Eb::EbLfServer::poll(uint64_t* data)
{
  if (_rxcq)                            // Revisit: why is this here?
  {
    const uint64_t   flags = FI_MSG | FI_RECV | FI_REMOTE_CQ_DATA;
    fi_cq_data_entry cqEntry;

    int rc = _poll(&cqEntry, flags);
    *data = cqEntry.data;

    return rc;
  }
  return -1;
}

#endif
