#include <string>
#include <sstream>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <signal.h>

#include <cpsw_error.h>

#include "psdaq/cphw/Reg.hh"

#include "psdaq/xpm/Module.hh"
#include "psdaq/xpm/PVStats.hh"
#include "psdaq/xpm/PVCtrls.hh"
#include "psdaq/xpm/PVPStats.hh"
#include "psdaq/xpm/PVPCtrls.hh"
#include "psdaq/xpm/XpmSequenceEngine.hh"

#include "psdaq/epicstools/EpicsPVA.hh"
#include "psdaq/epicstools/PVMonitorCb.hh"

#include "psdaq/service/Routine.hh"
#include "psdaq/service/Semaphore.hh"
#include "psdaq/service/Task.hh"
#include "psdaq/service/Timer.hh"

#include <cpsw_error.h>  // To catch a CPSW exception and continue

using Pds_Epics::EpicsPVA;
using Pds_Epics::PVMonitorCb;
using Pds::Xpm::CoreCounts;
using Pds::Xpm::L0Stats;
using std::string;

extern int optind;

namespace Pds {
  namespace Xpm {

    //class StatsTimer;

    //static StatsTimer* theTimer = NULL;

    static void sigHandler( int signal )
    {
      //if (theTimer)  theTimer->cancel();

      ::exit(signal);
    }

    class StatsTimer : public Timer {
    public:
      StatsTimer(Module& dev);
      ~StatsTimer() { _task->destroy(); }
    public:
      void allocate(const char* module_prefix,
                    const char* partition_prefix,
                    unsigned    shelf);
      void start   ();
      void cancel  ();
      void expired ();
      Task* task() { return _task; }
      //      unsigned duration  () const { return 1000; }
      unsigned duration  () const { return 1010; }  // 1% error on timer
      unsigned repetitive() const { return 1; }
    public:  // PVMonitorCb interface
      void     updated   ();
    private:
      void     _allocate();
    private:
      Module&    _dev;
      Semaphore  _sem;
      Task*      _task;
      string     _module_prefix;
      string     _partition_prefix;
      unsigned   _shelf;
      EpicsPVA*   _partPV;
      EpicsPVA*    _paddrPV;
      EpicsPVA*    _fwBuildPV;
      timespec   _t;
      CoreCounts _c;
      LinkStatus _links[32];
      L0Stats    _s    [Pds::Xpm::Module::NPartitions];
      PVPStats*  _pvps [Pds::Xpm::Module::NPartitions];
      PVStats    _pvs;
      PVCtrls    _pvc;
      PVPCtrls*  _pvpc [Pds::Xpm::Module::NPartitions];
      friend class PvAllocate;
    };

    class PvAllocate : public Routine {
    public:
      PvAllocate(StatsTimer& parent) : _parent(parent) {}
    public:
      void routine() { _parent._allocate(); delete this; }
    private:
      StatsTimer& _parent;
    };

  };
};

using namespace Pds;
using namespace Pds::Xpm;

StatsTimer::StatsTimer(Module& dev) :
  _dev       (dev),
  _sem       (Semaphore::FULL),
  _task      (new Task(TaskObject("PtnS"))),
  _pvc       (dev,_sem)
{
}

void StatsTimer::allocate(const char* module_prefix,
                          const char* partition_prefix,
                          unsigned    shelf)
{
  _module_prefix    = module_prefix;
  _partition_prefix = partition_prefix;
  _shelf            = shelf;

  _task->call(new PvAllocate(*this));
}

void StatsTimer::_allocate()
{
  _pvs.allocate(_module_prefix);
  _pvc.allocate(_module_prefix);

  for(unsigned i=0; i<Pds::Xpm::Module::NPartitions; i++) {
    std::stringstream ostr,dtstr;
    ostr  << _partition_prefix << ":" << i;
    dtstr << _module_prefix << ":PART:" << i;
    _pvps[i] = new PVPStats(_dev,i);
    _pvps[i]->allocate(ostr.str(),dtstr.str());
    _pvpc[i] = new PVPCtrls(_dev,_sem,*_pvps[i],_shelf,i);
    _pvpc[i]->allocate(ostr.str());
  }

  { std::stringstream ostr;
    ostr << _module_prefix << ":PAddr";
    _paddrPV = new EpicsPVA(ostr.str().c_str()); }

#if 1
  { std::stringstream ostr;
    ostr << _module_prefix << ":FwBuild";
    printf("fwbuildpv: %s\n", ostr.str().c_str());
    _fwBuildPV = new EpicsPVA(ostr.str().c_str(),256);  }
#endif

}

void StatsTimer::start()
{
  //theTimer = this;

  _dev._usTiming.resetStats();
  _dev._cuTiming.resetStats();

  //  _pvs.begin();
  Timer::start();
}

void StatsTimer::cancel()
{
  Timer::cancel();
  expired();
}

//
//  Update EPICS PVs
//
void StatsTimer::expired()
{
  try {
    timespec t; clock_gettime(CLOCK_REALTIME,&t);
    CoreCounts c = _dev.counts();
    LinkStatus links[32];
    _dev.linkStatus(links);
    unsigned bpClk  = _dev._monClk[0]&0x1fffffff;
    unsigned fbClk  = _dev._monClk[1]&0x1fffffff;
    unsigned recClk = _dev._monClk[2]&0x1fffffff;
    double dt = double(t.tv_sec-_t.tv_sec)+1.e-9*(double(t.tv_nsec)-double(_t.tv_nsec));
    _sem.take();
    try {
      _pvs.update(c,_c,links,_links,recClk,fbClk,bpClk,dt);
    } catch (CPSWError& e) {
      printf("Caught exception %s\n", e.what());
    }
    _sem.give();
    _c=c;
    std::copy(links,links+32,_links);
    _t=t;

    for(unsigned i=0; i<Pds::Xpm::Module::NPartitions; i++) {
      if (_pvpc[i]->enabled()) {
        _sem.take();
        try {
          _dev.setPartition(i);
          _pvps[i]->update();
        } catch (CPSWError& e) {
          printf("Caught exception %s\n", e.what());
        }
        _sem.give();
      }
    }
  } catch (CPSWError& e) { printf("cpsw exception %s\n",e.what()); }

    _pvc.dump();

    if (_paddrPV->connected()) {
    _paddrPV->putFrom<unsigned>(_dev._paddr);
    }
    else
      printf("paddrpv not connected\n");

#if 1
    if (_fwBuildPV && _fwBuildPV->connected()) {
      std::string bld = _dev._version.buildStamp();
      printf("fwBuild: %s\n",bld.c_str());
    _fwBuildPV->putFrom<std::string>(bld.c_str());
      _fwBuildPV = 0;
    }
#endif
}


void usage(const char* p) {
  printf("Usage: %s [options]\n",p);
  printf("Options: -a <IP addr> (default: 10.0.2.102)\n"
         "         -p <port>    (default: 8193)\n"
         "         -P <prefix>  (default: DAQ:LAB2\n");
}

int main(int argc, char** argv)
{
  extern char* optarg;

  int c;
  bool lUsage = false;

  const char* ip = "10.0.2.102";
  unsigned short port = 8193;
  const char* prefix = "DAQ:LAB2";
  char module_prefix[64];
  char partition_prefix[64];

  while ( (c=getopt( argc, argv, "a:p:P:h")) != EOF ) {
    switch(c) {
    case 'a':
      ip = optarg;
      break;
    case 'p':
      port = strtoul(optarg,NULL,0);
      break;
    case 'P':
      prefix = optarg;
      break;
    case '?':
    default:
      lUsage = true;
      break;
    }
  }

  if (optind < argc) {
    printf("%s: invalid argument -- %s\n",argv[0], argv[optind]);
    lUsage = true;
  }

  if (lUsage) {
    usage(argv[0]);
    exit(1);
  }

  unsigned shelf = strtoul(strchr(strchr(ip,'.')+1,'.')+1,NULL,10);
  //  Crate number is "c" in IP a.b.c.d
  sprintf(module_prefix   ,"%s:XPM:%u" ,
          prefix,
          shelf);
  sprintf(partition_prefix,"%s:PART",prefix);

  Pds::Cphw::Reg::set(ip, port, 0);

  Module* m = Module::locate();
  m->init();

#if 0
  //
  // Program sequencer
  //
  XpmSequenceEngine& engine = m->sequenceEngine();
  engine.verbosity(2);
  // Setup a 22 pulse sequence to repeat 40000 times each second
  const unsigned NP = 22;
  std::vector<TPGen::Instruction*> seq;
  seq.push_back(new TPGen::FixedRateSync(6,1));  // sync start to 1Hz
  for(unsigned i=0; i<NP; i++) {
    unsigned bits=0;
    for(unsigned j=0; j<16; j++)
      if ((i*(j+1))%NP < (j+1))
        bits |= (1<<j);
    seq.push_back(new TPGen::ExptRequest(bits));
    seq.push_back(new TPGen::FixedRateSync(0,1)); // next pulse
  }
  seq.push_back(new TPGen::Branch(1, TPGen::ctrA,199));
  //  seq.push_back(new TPGen::Branch(1, TPGen::ctrB, 99));
  seq.push_back(new TPGen::Branch(1, TPGen::ctrB,199));
  seq.push_back(new TPGen::Branch(0));
  int rval = engine.insertSequence(seq);
  if (rval < 0)
    printf("Insert sequence failed [%d]\n", rval);
  engine.dump  ();
  engine.enable(true);
  engine.setAddress(rval,0,0);
  engine.reset ();
#endif

  StatsTimer* timer = new StatsTimer(*m);

  Task* task = new Task(Task::MakeThisATask);

  ::signal( SIGINT, sigHandler );

  timer->allocate(module_prefix,
                  partition_prefix,
                  shelf);
  timer->start();

  task->mainLoop();

  sleep(1);                    // Seems to help prevent a crash in cpsw on exit

  return 0;
}
