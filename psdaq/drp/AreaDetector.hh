#pragma once

#include "drp.hh"
#include "XpmDetector.hh"
#include "xtcdata/xtc/Xtc.hh"
#include "xtcdata/xtc/NamesLookup.hh"

namespace Drp {

class AreaDetector : public XpmDetector
{
public:
    AreaDetector(Parameters* para, MemPool* pool);
    unsigned configure(const std::string& config_alias, XtcData::Xtc& xtc) override;
    void event(XtcData::Dgram& dgram, PGPEvent* event) override;
private:
    enum {RawNamesIndex, FexNamesIndex};
    XtcData::NamesLookup m_namesLookup;
    unsigned m_evtcount;
};

}
