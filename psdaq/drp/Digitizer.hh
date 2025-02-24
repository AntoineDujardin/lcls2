#pragma once

#include <vector>
#include "drp.hh"
#include "Detector.hh"
#include "xtcdata/xtc/Xtc.hh"
#include "xtcdata/xtc/NamesId.hh"

namespace Drp {

class Digitizer : public Detector
{
public:
    Digitizer(Parameters* para, MemPool* pool);
    void connect(const nlohmann::json&, const std::string& collectionId) override;
    unsigned configure(const std::string& config_alias, XtcData::Xtc& xtc) override;
    void event(XtcData::Dgram& dgram, PGPEvent* event) override;
private:
    unsigned _addJson(XtcData::Xtc& xtc, XtcData::NamesId& configNamesId);
    enum {ConfigNamesIndex, EventNamesIndex};
    unsigned          m_evtcount;
    unsigned          m_readoutGroup;
    XtcData::NamesId  m_evtNamesId;
    std::string       m_connect_json;
    std::string       m_epics_name;
};

}
