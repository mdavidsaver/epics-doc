#include <stdlib.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recSup.h>
#include <recGbl.h>
#include <callback.h>

#include <aiRecord.h>

#include <epicsExport.h>

struct prngState {
  unsigned int seed;
  CALLBACK cb; /* New */
};

void prng_cb(CALLBACK* cb);

static long init_record(aiRecord *prec)
{
  struct prngState* priv;
  unsigned long start;

  priv=malloc(sizeof(struct prngState));
  if(!priv){
    recGblRecordError(S_db_noMemory, (void*)prec,
      "devAoTimebase failed to allocate private struct");
    return S_db_noMemory;
  }

  /* New */
  callbackSetCallback(prng_cb,&priv->cb);
  callbackSetPriority(priorityLow,&priv->cb);
  callbackSetUser(prec,&priv->cb);
  priv->cb.timer=NULL;

  recGblInitConstantLink(&prec->inp,DBF_ULONG,&start);

  priv->seed=start;
  prec->dpvt=priv;

  return 0;
}

static long read_ai(aiRecord *prec)
{
  struct prngState* priv=prec->dpvt;

  if( ! prec->pact ){
    /* start async operation */
    prec->pact=TRUE;
    callbackSetUser(prec,&priv->cb);
    callbackRequestDelayed(&priv->cb,0.1);
    return 0;
  }else{
    /* complete operation */
    prec->pact=FALSE;
    return 0;
  }
}

void prng_cb(CALLBACK* cb)
{
  aiRecord* prec;
  struct prngState* priv;
  rset* prset;
  epicsInt32 raw;

  callbackGetUser(prec,cb);
  prset=(rset*)prec->rset;
  priv=prec->dpvt;

  raw=rand_r(&priv->seed);

  dbScanLock((dbCommon*)prec);
  prec->rval=raw;
  (*prset->process)((dbCommon*)prec);
  dbScanUnlock((dbCommon*)prec);
}

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiPrngAsync = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  init_record,
  NULL,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiPrngAsync); /* change name */

