#ifndef Pds_Eb_EbContribution_hh
#define Pds_Eb_EbContribution_hh

#include <stdint.h>

#include "xtcdata/xtc/Dgram.hh"


namespace Pds {
  namespace Eb {

    class EbContribution : public XtcData::Dgram
    {
    public:
      EbContribution();
      ~EbContribution();
    public:
      unsigned  payloadSize()   const;
      unsigned  number()        const;
      uint64_t  numberAsMask()  const;
      uint64_t  retire()        const;
    };
  };
};

/*
** ++
**
**   Constructor
**
** --
*/

inline Pds::Eb::EbContribution::EbContribution()
{
}

/*
** ++
**
**   Destructor
**
** --
*/

inline Pds::Eb::EbContribution::~EbContribution()
{
}

/*
** ++
**
**   Return the size (in bytes) of the contribution's payload
**
** --
*/

inline unsigned Pds::Eb::EbContribution::payloadSize() const
{
  return xtc.sizeofPayload();
}

/*
** ++
**
**    Return the source ID of the contributor which sent this packet...
**
** --
*/

inline unsigned Pds::Eb::EbContribution::number() const
{
  return xtc.src.value();
}

/*
** ++
**
**   Return the source ID of the contributor which sent this packet as a mask...
**
** --
*/

inline uint64_t Pds::Eb::EbContribution::numberAsMask() const
{
  return 1ul << number();
}

/*
** ++
**
**   Return the (complemented) value of the source ID.  The value is used by
**   "EbEvent" to strike down its "remaining" field which in turn signifies its
**   remaining contributors.
**
** --
*/

inline uint64_t Pds::Eb::EbContribution::retire() const
{
  return ~numberAsMask();
}

#endif
