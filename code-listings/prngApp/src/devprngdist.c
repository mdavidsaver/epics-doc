#include <stdlib.h>
#include <epicsExport.h>
#include <dbAccess.h>
#include <devSup.h>
#include <drvSup.h>
#include <recGbl.h>

#include <aiRecord.h>

#include <devLib.h> /* for noDevice error code */

#include "drvprngdist.h"

static long init_record(aiRecord *pao);
static long read_ai(aiRecord *pao);

struct {
  long num;
  DEVSUPFUN  report;
  DEVSUPFUN  init;
  DEVSUPFUN  init_record;
  DEVSUPFUN  get_ioint_info;
  DEVSUPFUN  read_ai;
  DEVSUPFUN  special_linconv;
} devAiPrngDist = {
  6, /* space for 6 functions */
  NULL,
  NULL,
  init_record,
  NULL,
  read_ai,
  NULL
};
epicsExportAddress(dset,devAiPrngDist);

static long init_record(aiRecord *pao)
{
  struct instancePrng* priv;

  priv=lookupPrng(pao->inp.value.vmeio.card);
  if(!priv){
    recGblRecordError(S_dev_noDevice, (void*)pao,
      "Not a valid device id code");
    return S_dev_noDevice;
  }

  pao->dpvt=priv;

  return 0;
}

static long read_ai(aiRecord *pao)
{
  struct instancePrng* priv=pao->dpvt;

  pao->rval=priv->table->read_prng(priv->token);

  return 0;
}
