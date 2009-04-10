#include <stdlib.h>
#include <epicsExport.h>
#include <dbAccess.h>
#include <devSup.h>
#include <recGbl.h>

#include <aiRecord.h>

static long init_record(aiRecord *pao);
static long read_ai(aiRecord *pao);

struct prngState {
  unsigned int seed;
};

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

static long init_record(aiRecord *pao)
{
  struct prngState* priv;
  unsigned long start;

  priv=malloc(sizeof(struct prngState));
  if(!priv){
    recGblRecordError(S_db_noMemory, (void*)pao,
      "devAoTimebase failed to allocate private struct");
    return S_db_noMemory;
  }

  recGblInitConstantLink(&pao->inp,DBF_ULONG,&start);

  priv->seed=start;
  pao->dpvt=priv;

  return 0;
}

static long read_ai(aiRecord *pao)
{
  struct prngState* priv=pao->dpvt;

  pao->rval=rand_r(&priv->seed);

  return 0;
}


