#ifndef OptFmc_hh
#define OptFmc_hh

#include <stdint.h>

namespace Pds {
  namespace HSD {
    class OptFmc {
    public:
      uint32_t fmc;
      uint32_t qsfp;
      uint32_t clks[7];
      uint32_t adcOutOfRange[10];
      uint32_t phaseCount_0;
      uint32_t phaseValue_0;
      uint32_t phaseCount_1;
      uint32_t phaseValue_1;
    public:
      void resetPgp();
    };
  };
};

#endif
