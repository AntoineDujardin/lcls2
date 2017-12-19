#ifndef EVENT_BUILDER_H
#define EVENT_BUILDER_H

#include "xtcdata/xtc/Xtc.hh"
#include "xtcdata/xtc/Dgram.hh"
#include "psdaq/eb/BatchManager.hh"
#include "psdaq/eb/EbFtClient.hh"
#include "psdaq/eb/EbFtServer.hh"

// these parameters must agree with the server side
unsigned maxBatches = 1000; // size of the pool of batches
unsigned maxEntries = 10; // maximum number of events in a batch
unsigned BatchSizeInPulseIds = 8; // age of the batch. should never exceed maxEntries above, must be a power of 2

unsigned EbId = 0; // from 0-63, maximum number of event builders
unsigned ContribId = 0; // who we are

class TheSrc : public XtcData::Src
{
public:
    TheSrc(XtcData::Level::Type level, unsigned id) :
        XtcData::Src(level)
    {
        _log |= id;
    }
};

class MyDgram : public XtcData::Dgram {
public:
    MyDgram(unsigned pulseId, uint64_t val);
private:
    uint64_t _data;
};

size_t maxSize = sizeof(MyDgram);

class MyBatchManager: public Pds::Eb::BatchManager {
public:
    MyBatchManager(Pds::Eb::EbFtClient& ebFtClient) :
        Pds::Eb::BatchManager(BatchSizeInPulseIds, maxBatches, maxEntries, maxSize),
        _ebFtClient(ebFtClient)
    {}
    void post(Pds::Eb::Batch* batch) {
        _ebFtClient.post(batch->buffer(), batch->extent(), EbId, batch->index() * maxBatchSize());
    }
private:
    Pds::Eb::EbFtClient& _ebFtClient;
};

void eb_rcvr(MyBatchManager& myBatchMan);

#endif // EVENT_BUILDER_H
