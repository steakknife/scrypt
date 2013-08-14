/*
 * Author:  David Robert Nadeau
 * Site:    http://NadeauSoftware.com/
 * License: Creative Commons Attribution 3.0 Unported License
 *          http://creativecommons.org/licenses/by/3.0/deed.en_US
 */

#include "precise_time.h"

#define NS_PER_S 1000000000.0
#define US_PER_S 1000000.0

#ifdef WINDOWS
#include <Windows.h>
#elif defined(UNIX) || defined(APPLE) 
#include <unistd.h> /* POSIX flags */
#include <time.h>
#include <sys/time.h>   /* gethrtime(), gettimeofday() */

#if defined(APPLE)
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

#else
#error "Unable to define getprecisetime() for an unknown OS."
#endif

#ifdef __MINGW32__
struct timespec {
    long tv_sec;
    long tv_nsec;
};
#endif

/**
 * Returns the real time, in seconds, or -1.0 if an error occurred.
 *
 * Time is measured since an arbitrary and OS-dependent start time.
 * The returned real time is only useful for computing an elapsed time
 * between two calls to this function.
 */
int
getprecisetime(precise_time_t *result) {
#ifdef WINDOWS
    FILETIME tm;
    ULONGLONG t;
#ifdef WINDOWS8
    /* Windows 8, Windows Server 2012 and later. ---------------- */
    GetSystemTimePreciseAsFileTime(&tm);
#else
    /* Windows 2000 and later. ---------------------------------- */
    GetSystemTimeAsFileTime(&tm);
#endif
    t = ((ULONGLONG)tm.dwHighDateTime << 32) | (ULONGLONG)tm.dwLowDateTime;
    *result = (precise_time_t)t / NS_PER_S;
    return 0;

#elif defined(HPUX) || defined(SUN) || defined(SVR4)
    /* HP-UX, Solaris. ------------------------------------------ */
    *result = (precise_time_t)gethrtime() / NS_PER_S;
    return 0;

#elif defined(APPLE)
    /* OSX. ----------------------------------------------------- */
    precise_time_t timeConvert;
    mach_timebase_info_data_t timeBase;

    mach_timebase_info(&timeBase);
    timeConvert = (precise_time_t)timeBase.numer / (precise_time_t)timeBase.denom / NS_PER_S;
    *result = (precise_time_t)mach_absolute_time() * timeConvert;
    return 0;

#elif defined(_POSIX_VERSION)
    /* POSIX. --------------------------------------------------- */
#if defined(_POSIX_TIMERS) && (_POSIX_TIMERS > 0)
    {
        struct timespec ts;
#if defined(CLOCK_MONOTONIC_PRECISE)
        /* BSD. --------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_PRECISE;
#elif defined(CLOCK_MONOTONIC_RAW)
        /* Linux. ------------------------------------------- */
        const clockid_t id = CLOCK_MONOTONIC_RAW;
#elif defined(CLOCK_HIGHRES)
        /* Solaris. ----------------------------------------- */
        const clockid_t id = CLOCK_HIGHRES;
#elif defined(CLOCK_MONOTONIC)
        /* AIX, BSD, Linux, POSIX, Solaris. ----------------- */
        const clockid_t id = CLOCK_MONOTONIC;
#elif defined(CLOCK_REALTIME)
        /* AIX, BSD, HP-UX, Linux, POSIX. ------------------- */
        const clockid_t id = CLOCK_REALTIME;
#else
        const clockid_t id = (clockid_t)-1; /* Unknown. */
#endif /* CLOCK_* */
        if ((id != (clockid_t)-1) && (clock_gettime(id, &ts) != -1)) {
            * result = (precise_time_t)ts.tv_sec +
                (precise_time_t)ts.tv_nsec / NS_PER_S;
        }
        /* Fall thru. */
    }
#endif /* _POSIX_TIMERS */

    /* AIX, BSD, Cygwin, HP-UX, Linux, OSX, POSIX, Solaris. ----- */
    struct timeval tm;
    gettimeofday(&tm, NULL);
    *result = (precise_time_t)tm.tv_sec + (precise_time_t)tm.tv_usec / US_PER_S;
    return 0;
#else
    return 1;        /* Failed. */
#endif
}


int
getprecisetimeres(precise_time_resolution_t *resd) {
#if (_POSIX_TIMERS > 0)
    struct timespec res;

    /*
     * Try clocks in order of preference until we find one which works.
     * (We assume that if clock_getres works, clock_gettime will, too.)
     * The use of if/else/if/else/if/else rather than if/elif/elif/else
     * is ugly but legal, and allows us to #ifdef things appropriately.
     */
#ifdef CLOCK_VIRTUAL
    if (clock_getres(CLOCK_VIRTUAL, &res) == 0) {
    } else
#endif
#ifdef CLOCK_MONOTONIC
    if (clock_getres(CLOCK_MONOTONIC, &res) == 0) {
    } else
#endif
    if (clock_getres(CLOCK_REALTIME, &res) == 0) {
    } else {
#ifdef DEBUG
        fprintf(stderr, "clock_getres failed\n");
#endif
        return -1;
    }

    /* Convert clock resolution to a double. */
    *resd = (precise_time_resolution_t)res.tv_sec + (precise_time_resolution_t)res.tv_nsec * NS;
#elif defined(APPLE)
    precise_time_t timeConvert;
    mach_timebase_info_data_t timeBase;

    mach_timebase_info(&timeBase);
    *resd = (precise_time_resolution_t)timeBase.numer / (precise_time_resolution_t)timeBase.denom / NS_PER_S;
#else
    *resd = 1.0 / CLOCKS_PER_SEC;
#endif
    return 0;
}

int
getprecisetimediff(precise_time_t previous, precise_time_duration_t * diffd) {
    precise_time_t now;

    if (getprecisetime(&now)) {
        return 1;
    }
    *diffd = (precise_time_duration_t)(now - previous);
    if (*diffd <= 0.0) {
        return 1;
    }
    return 0;
}
