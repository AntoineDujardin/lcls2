#ifndef HSD_Module134_hh
#define HSD_Module134_hh

#include "psdaq/hsd/EnvMon.hh"
#include "psdaq/hsd/Globals.hh"
#include "psdaq/hsd/I2cSwitch.hh"
#include "psdaq/service/Semaphore.hh"
#include <stdint.h>
#include <stdio.h>
#include <vector>

namespace Pds {
  namespace Mmhw {
    class AxiVersion;
    class Jtag;
  };
  namespace HSD {
    class TprCore;
    class Fmc134Ctrl;
    class FexCfg;
    class Mmcm;
    class Pgp;
    class Jesd204b;
    class I2c134;
    class ChipAdcCore;
    class OptFmc;

    //
    //  High level API
    //
    class Module134 {
    public:
      static Module134* create(int fd);
      static Module134* create(void*);

      ~Module134();

      const Pds::Mmhw::AxiVersion& version() const;
      I2c134&                      i2c    ();
      ChipAdcCore&                 chip   (unsigned ch);
      Fmc134Ctrl&                  jesdctl();
      Mmcm&                        mmcm   ();

      std::vector<Pgp*>            pgp    ();
      Jesd204b&                    jesd   (unsigned ch);
      OptFmc&                      optfmc ();
      void*                        reg    ();
 
      //  Accessors
      uint64_t device_dna() const;

      void     setup_timing();
      void     setup_jesd  ();
      void     board_status();

      void     set_local_id(unsigned bus);

      void     dumpRxAlign     () const;
      void     dumpPgp         () const;
      void     dumpMap         () const;

      //  Monitoring
      void     mon_start();
      EnvMon   mon() const;

      void     i2c_lock  (I2cSwitch::Port) const;
      void     i2c_unlock() const;
    private:
      Module134() : _sem_i2c(Semaphore::FULL) {}

      class PrivateData;
      PrivateData*      p;

      int               _fd;
      mutable Semaphore _sem_i2c;
    };
  };
};

#endif
