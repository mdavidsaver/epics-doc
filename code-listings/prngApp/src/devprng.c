#include <stdlib.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>

#include <aiRecord.h>

#include <epicsExport.h>

struct prngState {
  unsigned int seed;
};

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

  recGblInitConstantLink(&prec->inp,DBF_ULONG,&start);

  priv->seed=start;
  prec->dpvt=priv;

  return 0;
}

static long read_ai(aiRecord *prec)
{
  struct prngState* priv=prec->dpvt;

  prec->rval=rand_r(&priv->seed);

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
} devAiPrng = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  init_record,
  NULL,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiPrng);
