#ifndef SCRYPT_PLATFORM_H
#define SCRYPT_PLATFORM_H

#ifdef _WIN32
#define WINDOWS
#if defined(NTDDI_WIN8) && NTDDI_VERSION >= NTDDI_WIN8
#define WINDOWS8
#endif
#endif

#if defined(__sun__) || defined(__sun) || defined(sun)
#define SUN
#endif


#if defined(__unix__) || defined(__unix) || defined(unix)
#define UNIX
#endif

#if defined(__hpux) || defined(hpux)
#define HPUX
#endif

#if defined(__SVR4) || defined(__svr4__)
#define SVR4
#endif

#if defined(__APPLE__) && defined(__MACH__)
#define APPLE
#endif

#endif
