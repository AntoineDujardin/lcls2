#pragma once

#include "drp.hh"
#include "FileWriter.hh"
#include "psdaq/service/json.hpp"
#include "psdaq/eb/TebContributor.hh"
#include "psdaq/eb/MebContributor.hh"
#include "psdaq/eb/EbCtrbInBase.hh"
#include "psdaq/service/Collection.hh"
#include "psdaq/service/MetricExporter.hh"

namespace Drp {

#pragma pack(push, 4)
class MyDgram : public XtcData::Dgram {
public:
    MyDgram(XtcData::Dgram& dgram, uint64_t val, unsigned contributor_id);
private:
    uint64_t _data;
};
#pragma pack(pop)

class EbReceiver : public Pds::Eb::EbCtrbInBase
{
public:
    EbReceiver(const Parameters& para, Pds::Eb::TebCtrbParams& tPrms, MemPool& pool,
               ZmqSocket& inprocSend, Pds::Eb::MebContributor* mon,
               std::shared_ptr<MetricExporter>& exporter);
    void process(const XtcData::Dgram* result, const void* input) override;
private:
    MemPool& m_pool;
    Pds::Eb::MebContributor* m_mon;
    BufferedFileWriter m_fileWriter;
    SmdWriter m_smdWriter;
    bool m_writing;
    ZmqSocket& m_inprocSend;
    static const int m_size = 100;
    uint32_t m_indices[m_size];
    int m_count;
    uint32_t lastIndex;
    uint32_t lastEvtCounter;
    uint64_t lastPid;
    XtcData::TransitionId::Value lastTid;
    uint64_t m_offset;
    unsigned m_nodeId;
};

class DrpBase
{
public:
    DrpBase(Parameters& para, ZmqContext& context);
    void shutdown();
    nlohmann::json connectionInfo();
    std::string connect(const nlohmann::json& msg, size_t id);
    std::string disconnect(const nlohmann::json& msg);
    Pds::Eb::TebContributor& tebContributor() const {return *m_tebContributor;}
    prometheus::Exposer* exposer() {return m_exposer.get();}
    unsigned nodeId() const {return m_nodeId;}
    MemPool pool;
private:
    void parseConnectionParams(const nlohmann::json& body, size_t id);
    Parameters& m_para;
    unsigned m_nodeId;
    Pds::Eb::TebCtrbParams m_tPrms;
    Pds::Eb::MebCtrbParams m_mPrms;
    std::unique_ptr<Pds::Eb::TebContributor> m_tebContributor;
    std::unique_ptr<Pds::Eb::MebContributor> m_meb;
    std::unique_ptr<EbReceiver> m_ebRecv;
    std::unique_ptr<prometheus::Exposer> m_exposer;
    std::shared_ptr<MetricExporter> m_exporter;
    ZmqSocket m_inprocSend;
};

}
