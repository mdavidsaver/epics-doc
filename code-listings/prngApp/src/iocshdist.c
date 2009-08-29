#include <stdlib.h>
#include <string.h>

#include <epicsExport.h>
#include <errlog.h>
#include <iocsh.h>
#include <registryDriverSupport.h>
#include <ellLib.h>

#include "drvprngdist.h"

static ELLLIST devices={{NULL,NULL},0};

static const char dpref[]="drvPrng";

void
createPrng(int id,int seed,const char* dist)
{
  unsigned int s=(unsigned int)seed;
  char* dname=NULL;
  size_t dlen;
  struct instancePrng* inst=malloc(sizeof(struct instancePrng));

  if(!inst){
    epicsPrintf("Out of Memory\n");
    goto error;
  }

  /* sizeof(dpreg)==strlen(dpref)+1 */
  dlen=sizeof(dpref)+strlen(dist);
  
  dname=malloc(dlen);
  if(!dname){
    epicsPrintf("Out of Memory\n");
    goto error;
  }
  
  strcpy(dname,dpref);
  strcat(dname,dist);

  inst->table=(struct drvPrngDist*)registryDriverSupportFind(dname);
  if(!inst->table){
    epicsPrintf("Unknown Distribution type\n");
    goto error;
  }

  inst->token=( *(create_prng)inst->table->create_prng )(s);
  if(!inst->token){
    epicsPrintf("Failed to create PRNG.\n");
    goto error;
  }

  inst->id=id;
  ellAdd(&devices,&inst->node);

  return;

error:
  free(dname);
  free(inst);
  return;
}

struct instancePrng* lookupPrng(short N)
{
  ELLNODE* node;
  struct instancePrng* ent;
  
  for(node=ellFirst(&devices); node; node=ellNext(node)){
    ent=(struct instancePrng*)node; /* Use CONTAINER() in 3.14.11 */
    if(ent->id==N)
      return ent;
  }
  
  return NULL;
}


static const iocshArg createPrngArg0 = { "id#", iocshArgInt };
static const iocshArg createPrngArg1 = { "Random Seed", iocshArgInt };
static const iocshArg createPrngArg2 = { "Distribution", iocshArgString };
static const iocshArg * const createPrngArgs[3] = 
{ &createPrngArg0, &createPrngArg1, &createPrngArg2 };
static const iocshFuncDef createPrngFuncDef =
{ "createPrng", 3, createPrngArgs };
static void createPrngCallFunc(const iocshArgBuf *args)
{
  createPrng(args[0].ival,args[1].ival,args[2].sval);
}

void prngDist(void)
{
  iocshRegister(&createPrngFuncDef, createPrngCallFunc);
}
epicsExportRegistrar(prngDist);
