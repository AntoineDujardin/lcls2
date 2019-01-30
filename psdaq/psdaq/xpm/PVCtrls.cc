#include "psdaq/xpm/PVCtrls.hh"
#include "psdaq/xpm/Module.hh"
#include "psdaq/service/Semaphore.hh"

#include <sstream>

#include <stdio.h>

//#define SET_PLL

using Pds_Epics::EpicsPVA;
using Pds_Epics::PVMonitorCb;

namespace Pds {
  namespace Xpm {

#define Q(a,b)      a ## b
#define PV(name)    Q(name, PV)

    //#define PRT(value)  printf("%60.60s: %32.32s: 0x%02x\n", __PRETTY_FUNCTION__, _channel.epicsName(), value)
#define PRT(value)  {}

#define PVG(i)      PRT(getScalarAs<unsigned>()); _ctrl.module().i;
#define GPVG(i)   {                                        \
      _ctrl.sem().take();                                  \
      PRT(getScalarAs<unsigned>()); _ctrl.module();        \
      _ctrl.sem().give(); }
#define PVP(i)      PRT( _ctrl.module().i );  putFrom<unsigned>(_ctrl.module().i);
#define GPVP(i) {                                               \
    _ctrl.sem().take();                                         \
    PRT( _ctrl.module().i );  putFrom<unsigned>(_ctrl.module().i);  \
    _ctrl.sem().give();                                         \
    PRT( ( _ctrl.module().i ) ); }

#define CPV(name, updatedBody, connectedBody)                           \
                                                                        \
    class PV(name) : public EpicsPVA,                                    \
                     public PVMonitorCb                                 \
    {                                                                   \
    public:                                                             \
      PV(name)(PVCtrls& ctrl, const char* pvName, unsigned idx = 0) :   \
        EpicsPVA(pvName, this),                                          \
        _ctrl(ctrl),                                                    \
        _idx(idx) {}                                                    \
      virtual ~PV(name)() {}                                            \
    public:                                                             \
      void updated();                                                   \
      void onConnect();                                             \
    private:                                                            \
      PVCtrls& _ctrl;                                                   \
      unsigned _idx;                                                    \
    };                                                                  \
    void PV(name)::updated()                                            \
    {                                                                   \
      updatedBody                                                       \
    }                                                                   \
    void PV(name)::onConnect()                                    \
    {                                                                   \
      connectedBody                                                     \
    }

    CPV(LinkTxDelay,    { GPVG(linkTxDelay  (_idx, getScalarAs<unsigned>()));      },
                        { GPVP(linkTxDelay  (_idx));                   })
    CPV(LinkPartition,  { GPVG(linkPartition(_idx, getScalarAs<unsigned>()));      },
                        { GPVP(linkPartition(_idx));                   })
    CPV(LinkTrgSrc,     { GPVG(linkTrgSrc   (_idx, getScalarAs<unsigned>()));      },
                        { GPVP(linkTrgSrc   (_idx));                   })
    CPV(LinkLoopback,   { GPVG(linkLoopback (_idx, getScalarAs<unsigned>() != 0)); },
                        { GPVP(linkLoopback (_idx));                   })
    CPV(TxLinkReset,    { if (getScalarAs<unsigned>()!=0)
                            GPVG(txLinkReset  (_idx));                 },
                        {                                              })
    CPV(RxLinkReset,    { if (getScalarAs<unsigned>()!=0)
                            GPVG(rxLinkReset  (_idx));                 },
                        {                                              })
    CPV(RxLinkDump ,    { if (getScalarAs<unsigned>()!=0)
                            GPVG(rxLinkDump   (_idx));                 },
                        {                                              })
    CPV(LinkEnable,     { GPVG(linkEnable(_idx, getScalarAs<unsigned>() != 0));    },
                        { GPVP(linkEnable(_idx));                      })

#if 0
    CPV(LinkRxReady,    { },
                        { GPVP(linkRxReady(_idx));                     })
    CPV(LinkTxReady,    { },
                        { GPVP(linkTxReady(_idx));                     })
    CPV(LinkRxResetDone,{ },
                        { GPVP(linkRxResetDone(_idx));                 })
    CPV(LinkTxResetDone,{ },
                        { GPVP(linkTxResetDone(_idx));                 })
    CPV(LinkIsXpm,      { },
                        { GPVP(linkIsXpm(_idx));                       })
    CPV(LinkRxErr,      { },
                        { GPVP(linkRxErr(_idx));                       })
#endif

    CPV(PLL_BW_Select,  { GPVG(pllBwSel  (_idx, getScalarAs<unsigned>()));         },
                        { GPVP(pllBwSel  (_idx));                      })
    CPV(PLL_FreqTable,  { GPVG(pllFrqTbl (_idx, getScalarAs<unsigned>()));         },
                        { GPVP(pllFrqTbl (_idx));                      })
    CPV(PLL_FreqSelect, { GPVG(pllFrqSel (_idx, getScalarAs<unsigned>()));         },
                        { GPVP(pllFrqSel (_idx));                      })
    CPV(PLL_Rate,       { GPVG(pllRateSel(_idx, getScalarAs<unsigned>()));         },
                        { GPVP(pllRateSel(_idx));                      })
    CPV(PLL_PhaseInc,   { GPVG(pllPhsInc (_idx));                      },
                        {                                              })
    CPV(PLL_PhaseDec,   { GPVG(pllPhsDec (_idx));                      },
                        {                                              })
    CPV(PLL_Bypass,     { GPVG(pllBypass (_idx, getScalarAs<unsigned>()));         },
                        { GPVP(pllBypass (_idx));                      })
    CPV(PLL_Reset,      { GPVG(pllReset  (_idx));                      },
                        {                                              })
    CPV(PLL_Skew,       { GPVG(pllSkew   (_idx, getScalarAs<unsigned>()));         },
                        {                                              })
    CPV(PLL_LOS,        {                                              },
                        { GPVP(pllStatus0(_idx));                      })
    CPV(PLL_LOL,        {                                              },
                        { GPVP(pllStatus1(_idx));                      })

    CPV(ModuleInit,     { GPVG(init     ());     }, { })
    CPV(DumpPll,        { GPVG(dumpPll  (_idx)); }, { })
    CPV(DumpTiming,     { PVG(dumpTiming(_idx)); }, { })

    PVCtrls::PVCtrls(Module& m, Semaphore& sem) : _pv(0), _m(m), _sem(sem) {}
    PVCtrls::~PVCtrls() {}

    void PVCtrls::allocate(const std::string& title)
    {
      for(unsigned i=0; i<_pv.size(); i++)
        delete _pv[i];
      _pv.resize(0);

      std::ostringstream o;
      o << title << ":";
      std::string pvbase = o.str();

#define NPV(name)  _pv.push_back( new PV(name)(*this, (pvbase+#name).c_str()) )
#define NPVN(name, n)                                                        \
      for (unsigned i = 0; i < n; ++i) {                                     \
        std::ostringstream str;  str << i;  std::string idx = str.str();     \
        _pv.push_back( new PV(name)(*this, (pvbase+#name+idx).c_str(), i) ); \
      }

      NPV ( ModuleInit                              );
      NPVN( DumpPll,            Module::NAmcs       );
      NPVN( DumpTiming,         2                   );

      NPVN( LinkTxDelay,        24    );
      NPVN( LinkPartition,      24    );
      NPVN( LinkTrgSrc,         24    );
      NPVN( LinkLoopback,       24    );
      NPVN( TxLinkReset,        24    );
      NPVN( RxLinkReset,        24    );
      NPVN( RxLinkDump,         Module::NDSLinks    );
      NPVN( LinkEnable,         24    );
#if 0
      NPVN( LinkRxReady,        32);
      NPVN( LinkTxReady,        32);
      NPVN( LinkIsXpm,          32);
      NPVN( LinkRxErr,          32);
#endif
      NPVN( PLL_BW_Select,      Module::NAmcs       );
      NPVN( PLL_FreqTable,      Module::NAmcs       );
      NPVN( PLL_FreqSelect,     Module::NAmcs       );
      NPVN( PLL_Rate,           Module::NAmcs       );
      NPVN( PLL_PhaseInc,       Module::NAmcs       );
      NPVN( PLL_PhaseDec,       Module::NAmcs       );
      NPVN( PLL_Bypass,         Module::NAmcs       );
#ifdef SET_PLL
      //  This makes the timing disappear
      NPVN( PLL_Reset,          Module::NAmcs       );
#endif
      NPVN( PLL_Skew,           Module::NAmcs       );
      NPVN( PLL_LOS,            Module::NAmcs       );
      NPVN( PLL_LOL,            Module::NAmcs       );
    }

    void PVCtrls::dump() const
    {
      _sem.take();
      unsigned enable=0;
      for(unsigned i=0; i<16; i++)
        if (_m.linkEnable(i))
          enable |= (1<<i);
      _sem.give();
    }

    Module& PVCtrls::module() { return _m; }
    Semaphore& PVCtrls::sem() { return _sem; }

  };
};
