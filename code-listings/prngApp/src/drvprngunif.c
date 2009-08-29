
#include <stdlib.h>
#include <drvSup.h>
#include <epicsExport.h>

#include "drvprngdist.h"

struct uniform {
  unsigned int state;
};

static
void* create(unsigned int seed)
{
  struct uniform* priv=malloc(sizeof(struct uniform));
  
  if(!priv)
    return NULL;
  
  priv->state=seed;
  
  return priv;
}

static
int read(void* tok)
{
  struct uniform* priv=tok;
  
  return rand_r(&priv->state);
}

static
struct drvPrngDist drvPrngUniform = {
  { 4,
    NULL,
    NULL,
  },
  create,
  read,
};
epicsExportAddress(drvet,drvPrngUniform);
