#ifndef HSD_FmcCoreVoid_hh
#define HSD_FmcCoreVoid_hh

#include <stdint.h>

namespace Pds {
  namespace HSD {
    class FmcCoreVoid {
    public:
      bool   present  () const { return false; }
      bool   powerGood() const { return false; }
      void   selectClock(unsigned) {}
      double clockRate() const { return 0; }
      void   cal_enable () {}
      void   cal_disable() {}
    private:
      uint32_t _rsvd[0x100];
    };
  };
};

#endif
