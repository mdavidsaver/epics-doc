#include <stdlib.h>
#include <dbAccess.h>
#include <devSup.h>
#include <drvSup.h>
#include <recGbl.h>
#include <alarm.h>

#include <aiRecord.h>

#include <devLib.h> /* for noDevice error code */

#include "drvprngdist.h"

#include <epicsExport.h>

static long init_record(aiRecord *prec)
{
  struct instancePrng* priv;

  priv=lookupPrng(prec->inp.value.vmeio.card);
  if(!priv){
    recGblRecordError(S_dev_noDevice, (void*)prec,
      "Not a valid device id code");
    return S_dev_noDevice;
  }

  prec->dpvt=priv;

  return 0;
}

static long read_ai(aiRecord *prec)
{
  struct instancePrng* priv=prec->dpvt;
  if(!priv) {
    (void)recGblSetSevr(prec, COMM_ALARM, INVALID_ALARM);
    return 0;
  }

  prec->rval=priv->table->read_prng(priv->token);

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
