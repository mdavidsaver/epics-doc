#include <stdlib.h>

#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>
#include <dbScan.h>
#include <dbDefs.h>
#include <ellLib.h>
#include <cantProceed.h>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <initHooks.h>
#include <callback.h>
#include <epicsVersion.h>

#include <aiRecord.h>

#include <epicsExport.h>

static ELLLIST allprngs = ELLLIST_INIT;

struct prngState {
  ELLNODE node;
  unsigned int seed;
  unsigned int lastnum;
  epicsMutexId lock;
  IOSCANPVT scan;
  epicsThreadId generator;
};

static void start_workers(initHookState state);

static long init(int phase)
{
  if(phase==0)
    initHookRegister(&start_workers);
  return 0;
}

static void worker(void* raw);

static long init_record(aiRecord *prec)
{
  struct prngState* priv;
  unsigned long start;

  priv=callocMustSucceed(1,sizeof(*priv),"prngintr");

  recGblInitConstantLink(&prec->inp,DBF_ULONG,&start);

  priv->seed=start;
  scanIoInit(&priv->scan);
  priv->lock = epicsMutexMustCreate();
  priv->generator = NULL;
  ellAdd(&allprngs, &priv->node);
  prec->dpvt=priv;

  return 0;
}

static void start_workers(initHookState state)
{
  ELLNODE *cur;
  if(state!=initHookAfterInterruptAccept)
    return;
  for(cur=ellFirst(&allprngs); cur; cur=ellNext(cur)) {
    struct prngState *priv = CONTAINER(cur, struct prngState, node);
    priv->generator = epicsThreadMustCreate("prngworker",
                                            epicsThreadPriorityMedium,
                                            epicsThreadGetStackSize(epicsThreadStackSmall),
                                            &worker, priv);
  }
}

static void worker(void* raw)
{
  struct prngState* priv=raw;
  while(1) {
    epicsMutexMustLock(priv->lock);
    priv->lastnum = rand_r(&priv->seed);
    epicsMutexUnlock(priv->lock);

#ifdef EPICS_VERSION_INT
#if EPICS_VERSION_INT>=VERSION_INT(3,16,0,0)
#  define USE_IMMEDIATE
#endif
#endif

#ifdef USE_IMMEDIATE
    scanIoImmediate(priv->scan, priorityHigh);
    scanIoImmediate(priv->scan, priorityMedium);
    scanIoImmediate(priv->scan, priorityLow);
#else
    scanIoRequest(priv->scan);
#endif

    epicsThreadSleep(1.0);
  }
}

static long get_ioint_info(int dir,dbCommon* prec,IOSCANPVT* io)
{
  struct prngState* priv=prec->dpvt;
  
  *io = priv->scan;
  return 0;
}

static long read_ai(aiRecord *prec)
{
  struct prngState* priv=prec->dpvt;

  epicsMutexMustLock(priv->lock);
  prec->rval = priv->lastnum;
  epicsMutexUnlock(priv->lock);

  return 0;
}

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiPrngIntr = {
  6, /* space for 6 functions */
  NULL,
  init,
  init_record,
  get_ioint_info,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiPrngIntr);
