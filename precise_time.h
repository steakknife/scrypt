#ifndef PRECISE_CLOCK_H
#define PRECISE_CLOCK_H

#include "scrypt_platform.h"

#ifdef WINDOWS
#include <Windows.h>
#else
#include <time.h>
#endif


typedef double precise_time_t; // seconds
typedef double precise_time_duration_t; // seconds
typedef double precise_time_resolution_t; // seconds

/**
 * Returns the real time, in seconds, or -1.0 if an error occurred.
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 */
int
getprecisetime(precise_time_t *result);

int
getprecisetimeres(precise_time_resolution_t *resd);

int
getprecisetimediff(precise_time_t previous, precise_time_duration_t *diffd);

#endif

