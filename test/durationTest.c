//
// FILE            durationTest.c
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
// A CorArgDuration option (test/durationTest.sh runs it): seconds or ISO 8601, its limits, its default in
// the usage, from the command line and from the environment.
//
//   durationTest [--renew <duration>] [-u | -U]       prints "OK renew <seconds> <as text>"
//   durationTest text <seconds>                       corArgsDurationText
//
#include <stdio.h>                               // printf
#include <stdlib.h>                              // atoll
#include <string.h>                              // strcmp

#include "corArgs/corArgs.h"                     // corArgsInit, corArgsParse, corArgsDurationText
#include "corArgs/CorArg.h"                      // CorArg



static int renew;

static CorArg options[] =
{
  { "--renew", "-r", CorArgDuration, &renew, CorArgOpt, (void*) 3600, (void*) 60, (void*) 31536000, "how often - PT1M to P365D" },
  CORARGS_END
};



// -----------------------------------------------------------------------------
//
// main -
//
int main(int argC, char* argV[])
{
  char buf[64];

  if ((argC == 3) && (strcmp(argV[1], "text") == 0))
  {
    printf("%s\n", corArgsDurationText(atoll(argV[2]), buf, sizeof(buf)));
    return 0;
  }

  if (corArgsInit("durationTest", options, "DURATIONTEST") != CorArgsOk)
    return 2;

  if (corArgsParse(argC, argV) != CorArgsOk)
    return 3;

  printf("OK renew %d %s\n", renew, corArgsDurationText(renew, buf, sizeof(buf)));
  return 0;
}
