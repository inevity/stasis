/*
 * tsc.h
 *
 *  Created on: Sep 28, 2010
 *      Author: sears
 */

#ifndef TIME_H_
#define TIME_H_

#include <stasis/common.h>
#include <sys/time.h>

#include "log2.h"

static inline unsigned long long stasis_get_tsc() {
  unsigned long long tsc;
  asm volatile ("rdtsc" : "=A" (tsc));
  return tsc;
}
static inline struct timeval stasis_subtract_timeval(const struct timeval a, const struct timeval b) {
  struct timeval ret = {a.tv_sec - b.tv_sec, a.tv_usec - b.tv_usec};
  if(ret.tv_usec < 0) { ret.tv_usec += 1000000; ret.tv_sec--; }
  return ret;
}
static uint8_t stasis_log_2_timeval(const struct timeval a) {
  return stasis_log_2_64(a.tv_sec * 1000000 + a.tv_usec);
}
#endif /* TIME_H_ */