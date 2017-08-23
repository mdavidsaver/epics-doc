#include <stdlib.h>
#include <stdio.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>
#include <dbScan.h>
#include <dbDefs.h>
#include <ellLib.h>
#include <cantProceed.h>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <initHooks.h>
#include <callback.h>
#include <epicsVersion.h>

#include <aiRecord.h>

#include <epicsExport.h>

#ifdef EPICS_VERSION_INT
#if EPICS_VERSION_INT>=VERSION_INT(3,16,0,0)
#  define USE_COMPLETE
#endif
#endif

#ifndef callbackGetPriority
#define callbackGetPriority(PRIORITY, PCALLBACK) \
    ( (PRIORITY) = (PCALLBACK)->priority )
#endif

static ELLLIST allprngs = ELLLIST_INIT;

struct prngState {
  ELLNODE node;
  unsigned int seed;
  unsigned int lastnum;
  epicsMutexId lock;
  epicsEventId nextnum;
  IOSCANPVT scan;
  epicsThreadId generator;
  unsigned waitfor;
#ifndef USE_COMPLETE
  CALLBACK done[NUM_CALLBACK_PRIORITIES];
#endif
};

static void start_workers(initHookState state);

static long init(int phase)
{
  if(phase==0)
    initHookRegister(&start_workers);
  return 0;
}

static void worker(void* raw);

#ifdef USE_COMPLETE
static
void prioComplete(void *usr, IOSCANPVT scan, int prio);
#else
static
void prioComplete(CALLBACK*);
#endif

static long init_record(aiRecord *prec)
{
  struct prngState* priv;
  unsigned long start;

  priv=callocMustSucceed(1,sizeof(*priv),"prngintrrate");

  recGblInitConstantLink(&prec->inp,DBF_ULONG,&start);

  priv->seed=start;
  scanIoInit(&priv->scan);
  priv->lock = epicsMutexMustCreate();
  priv->nextnum = epicsEventMustCreate(epicsEventEmpty);
  priv->generator = NULL;
  ellAdd(&allprngs, &priv->node);
  prec->dpvt=priv;

#ifdef USE_COMPLETE
  scanIoSetComplete(priv->scan, prioComplete, priv);
#else
  {
    unsigned i;
    for(i=0; i<NUM_CALLBACK_PRIORITIES; i++) {
      callbackSetPriority(i, &priv->done[i]);
      callbackSetCallback(prioComplete, &priv->done[i]);
      callbackSetUser(priv, &priv->done[i]);
    }
  }
#endif

  return 0;
}

static void start_workers(initHookState state)
{
  ELLNODE *cur;
  if(state!=initHookAfterInterruptAccept)
    return;
  for(cur=ellFirst(&allprngs); cur; cur=ellNext(cur)) {
    struct prngState *priv = CONTAINER(cur, struct prngState, node);
    priv->generator = epicsThreadMustCreate("prngintrrate",
                                            epicsThreadPriorityMedium,
                                            epicsThreadGetStackSize(epicsThreadStackSmall),
                                            &worker, priv);
  }
}

#ifdef USE_COMPLETE
static
void prioComplete(void *usr, IOSCANPVT scan, int prio)
{
    int dowake;
    struct prngState* priv=usr;
#else
static
void prioComplete(CALLBACK* pcb)
{
    int dowake, prio;
    struct prngState* priv;
    callbackGetUser(priv, pcb);
    callbackGetPriority(prio, pcb);
#endif /* USE_COMPLETE */

    epicsMutexMustLock(priv->lock);
    priv->waitfor &= ~(1<<prio);
    dowake = (priv->waitfor==0);
    epicsMutexUnlock(priv->lock);
    if(dowake)
        epicsEventSignal(priv->nextnum);
}

static void worker(void* raw)
{
  struct prngState* priv=raw;
  while(1) {
    unsigned needwait;
    printf("Rate limited worker running %p\n", priv);

    epicsMutexMustLock(priv->lock);
    assert(priv->waitfor==0);

    priv->lastnum = rand_r(&priv->seed);

#ifdef USE_COMPLETE
    priv->waitfor |= scanIoRequest(priv->scan);
#else
    scanIoRequest(priv->scan);
    {
        unsigned i;
        for(i=0; i<NUM_CALLBACK_PRIORITIES; i++)
            callbackRequest(&priv->done[i]);
    }
    priv->waitfor |= (1<<NUM_CALLBACK_PRIORITIES)-1;
#endif
    needwait = priv->waitfor!=0;
    
    epicsMutexUnlock(priv->lock);

    if(needwait) {
        epicsEventMustWait(priv->nextnum);
    } else {
        /* No I/O Intr records to wait for, slow down arbitraily */
        epicsThreadSleep(1.0);
    }
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

  /* arbitraily slow things down.
   * Remove this for full speed (and log spam)
   */
  epicsThreadSleep(1.0);

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
} devAiPrngIntrRate = {
  6, /* space for 6 functions */
  NULL,
  init,
  init_record,
  get_ioint_info,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiPrngIntrRate);
