
#include <stdlib.h>
#include <drvSup.h>
#include <epicsExport.h>

#include "drvprngdist.h"

struct gaussian {
  unsigned int state;
};

static
void* create(unsigned int seed)
{
  struct gaussian* priv=malloc(sizeof(struct gaussian));
  
  if(!priv)
    return NULL;
  
  priv->state=seed;
  
  return priv;
}

static
int read(void* tok)
{
  struct gaussian* priv=tok;
  int ret=0, i=8;
  
  while(i--)
    ret+=rand_r(&priv->state)/8;

  return ret;
}

static
struct drvPrngDist drvPrngGaussian = {
  { 4,
    NULL,
    NULL,
  },
  create,
  read,
};
epicsExportAddress(drvet,drvPrngGaussian);
