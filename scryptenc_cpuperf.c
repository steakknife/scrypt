/*-
 * Copyright 2009 Colin Percival
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file was originally written by Colin Percival as part of the Tarsnap
 * online backup system.
 */

#include <sys/time.h>

#include <stdint.h>
#include <stdio.h>

#include "crypto_scrypt.h"

#include "scryptenc_cpuperf.h"


#include "precise_time.h"


/**
 * scryptenc_cpuperf(opps):
 * Estimate the number of salsa20/8 cores which can be executed per second,
 * and return the value via opps.
 */
int
scryptenc_cpuperf(double * opps)
{
    precise_time_t start;
    precise_time_resolution_t resolution;
    precise_time_duration_t elapsed = 0.0;
	uint64_t i = 0;

	/* Get the clock resolution. */
    if (getprecisetimeres(&resolution)) {
#ifdef DEBUG
	fprintf(stderr, "get precise time resolution failed\n");
#endif
		return 2;
    }

#ifdef DEBUG
	fprintf(stderr, "Clock resolution is %0.12f\n", resolution);
#endif

	/* Loop until the clock ticks. */
	if (getprecisetime(&start)) {
#ifdef DEBUG
    	fprintf(stderr, "get precise time failed\n");
#endif
		return 2;
    }
	do {
		/* Do an scrypt. */
		if (crypto_scrypt(NULL, 0, NULL, 0, 16, 1, 1, NULL, 0)) {
			return 3;
        }

		/* Has the clock ticked? */
		if (getprecisetimediff(start, &elapsed)) {
#ifdef DEBUG
        	fprintf(stderr, "get precise time diff failed\n");
#endif
			return 2;
        }
		if (elapsed > 0) {
			break;
        }
	} while (1);

	/* Could how many scryps we can do before the next tick. */
	if (getprecisetime(&start)) {
#ifdef DEBUG
    	fprintf(stderr, "get precise time failed 2\n");
#endif
		return 2;
    }
	do {
		/* Do an scrypt. */
		if (crypto_scrypt(NULL, 0, NULL, 0, 128, 1, 1, NULL, 0)) {
			return 3;
        }

		/* We invoked the salsa20/8 core 512 times. */
		i += 512;

		/* Check if we have looped for long enough. */
		if (getprecisetimediff(start, &elapsed)) {
#ifdef DEBUG
        	fprintf(stderr, "get precise time diff failed 2\n");
#endif
			return 2;
        }
		if (elapsed > 1000*resolution) {
			break;
        }
	} while (1);

#ifdef DEBUG
	fprintf(stderr, "%ju salsa20/8 cores performed in %f seconds\n",
			(uintmax_t)i, elapsed);
#endif

	/* We can do approximately i salsa20/8 cores per diffd seconds. */
	*opps = i / elapsed;
	return 0;
}
