/*
 *  Copyright (c) 2020-2022 Thakee Nathees
 *  Copyright (c) 2021-2022 Pocketlang Contributors
 *  Distributed Under The MIT License
 */

#include <time.h>

#ifndef PK_AMALGAMATED
#include "libs.h"
#endif

#ifdef _WIN32
  #include <windows.h>
#endif

#if !defined(_MSC_VER) && !(defined(_WIN32) && defined(__TINYC__))
  #include <unistd.h> // usleep
#endif

DEF(_timeEpoch,
  "time() -> Number",
  "Returns the number of seconds since the Epoch, 1970-01-01 "
  "00:00:00 +0000 (UTC).") {
  pkSetSlotNumber(vm, 0, (double) time(NULL));
}

DEF(_timeClock,
  "clock() -> Number",
  "Returns the number of clocks passed divied by CLOCKS_PER_SEC.") {
  pkSetSlotNumber(vm, 0, (double) clock() / CLOCKS_PER_SEC);
}

DEF(_timeSleep,
    "sleep(t:num) -> Number",
    "Sleep for [t] milliseconds.") {

  double t;
  pkValidateSlotNumber(vm, 1, &t);

#if defined(_MSC_VER) || (defined(_WIN32) && defined(__TINYC__))
  // Sleep(milli seconds)
  Sleep((DWORD) t);

#else // usleep(micro seconds)
  usleep(((useconds_t) (t)) * 1000);
#endif
}

DEF(_timeMonotime,
  "monotime() -> Number",
  "Returns monotonic timestamps.") {

  #if defined(_WIN32) || defined(__NT__)
    uint64_t ticks, freq;
    QueryPerformanceCounter((LARGE_INTEGER*) &ticks);
    QueryPerformanceFrequency((LARGE_INTEGER*) &freq);
    pkSetSlotNumber(vm, 0, (double) ticks * (1000000000 / freq));
  #elif defined(__linux__)
    struct timespec tt;
    clock_gettime(CLOCK_MONOTONIC, &tt);
    pkSetSlotNumber(vm, 0, (double) tt.tv_sec * 1000000000 + tt.tv_nsec);
  #else
  #endif
}

/*****************************************************************************/
/* MODULE REGISTER                                                           */
/*****************************************************************************/

void registerModuleTime(PKVM* vm) {
  PkHandle* time = pkNewModule(vm, "time");

  REGISTER_FN(time, "epoch", _timeEpoch, 0);
  REGISTER_FN(time, "sleep", _timeSleep, 1);
  REGISTER_FN(time, "clock", _timeClock, 0);
  REGISTER_FN(time, "monotime", _timeMonotime, 0);

  pkRegisterModule(vm, time);
  pkReleaseHandle(vm, time);
}

