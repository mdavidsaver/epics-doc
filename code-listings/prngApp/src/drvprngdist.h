
#ifndef DRVPRNGDIST_H
#define DRVPRNGDIST_H 1

#include <ellLib.h>
#include <drvSup.h>

/*
 * Define the Driver Support interface.
 */

/* Create and seed a new PRNG and return a token
 * which will be used in future calls.
 */
typedef void* (*create_prng_fun)(unsigned int seed);

/* Read a random number
 */
typedef int (*read_prng_fun)(void* tok);

struct drvPrngDist {
  drvet base;
  create_prng_fun create_prng;
  read_prng_fun read_prng;
};

/* Everything about an instance of a PRNG
 */
struct instancePrng {
  ELLNODE node; /* must be first */
  struct drvPrngDist* table;
  void* token;
  int id;
};

/* Find the PRNG instance which has been associated
 * with the key N by the createPrng() IOCSH function
 */
struct instancePrng* lookupPrng(short N);

#endif /* DRVPRNGDIST_H */
