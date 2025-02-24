#include "Decide.hh"
#include "eb.hh"
#include "utilities.hh"
#include "xtcdata/xtc/Dgram.hh"
#include "xtcdata/xtc/Damage.hh"

#include "rapidjson/document.h"

#include <cstdint>
#include <stdio.h>

using namespace XtcData;
using namespace Pds::Eb;
using namespace rapidjson;

using json = nlohmann::json;


namespace Pds {
  namespace Eb {

    class DecideImpl : public Decide
    {
    public:
      int    configure(const json& msg, const std::string& connect_json);
      Damage configure(const Dgram* dgram);
      Damage event(const Dgram* ctrb, uint32_t* result, size_t sizeofPayload);
    private:
      uint32_t _triggerVal;
      uint32_t _monitorVal;
    };
  };
};


int Pds::Eb::DecideImpl::configure(const json& msg, const std::string& connect_json)
{
  Document    top;
  const char* detName = "tmoteb";
  int         rc      = fetchFromCfgDb(detName, top, connect_json);
  if (!rc)
  {
    const char* key = "triggerVal";
    if (top.HasMember(key))  _triggerVal = top[key].GetUint();
    else rc = fprintf(stderr, "%s:\n  Key '%s' not found in Document %s\n",
                      __PRETTY_FUNCTION__, key, detName);

    key = "monitorVal";
    if (top.HasMember(key))  _monitorVal = top[key].GetUint();
    else rc = fprintf(stderr, "%s:\n  Key '%s' not found in Document %s\n",
                      __PRETTY_FUNCTION__, key, detName);
  }
  else rc = fprintf(stderr, "%s:\n  Failed to retrieve Document '%s' from ConfigDb\n",
                    __PRETTY_FUNCTION__, detName);

  return rc;
}

Damage Pds::Eb::DecideImpl::configure(const Dgram* dgram)
{
  return 0;
}

Damage Pds::Eb::DecideImpl::event(const Dgram* ctrb, uint32_t* result, size_t sizeofPayload)
{
  if (result)
  {
    const uint32_t* input = reinterpret_cast<uint32_t*>(ctrb->xtc.payload());

    result[WRT_IDX] |= input[WRT_IDX] == _triggerVal ? 1 : 0;
    result[MON_IDX] |= input[MON_IDX] == _monitorVal ? 1 : 0;

    //static unsigned lastIndex[] = { 0, 0, 0 };
    //unsigned id = ctrb->xtc.src.value();
    //unsigned index = input[MON_IDX];
    //if (index != ((lastIndex[id] + 1) & (131072 - 1))) {
    //  printf("Index jump:  id %d  index %u  last %u\n", id, index, lastIndex[id]);
    //}
    //lastIndex[id] = index;
  }
  return 0;
}


// The class factory

extern "C" Pds::Eb::Decide* create()
{
  return new DecideImpl;
}
