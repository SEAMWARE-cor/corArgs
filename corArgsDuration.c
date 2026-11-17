//
// FILE            corArgsDuration.c
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
// A CorArgDuration option's value: seconds, written as seconds or as an ISO 8601 duration - "P7D" says what
// 604800 hides.
//
#include <stdbool.h>                   // bool
#include <stdio.h>                     // snprintf
#include <string.h>                    // strlen

#include "corArgs/corArgsDuration.h"   // Own interface



//
// A duration longer than this is refused - a hundred years, far beyond any option and far from overflowing
//
#define DURATION_MAX  (100LL * 366 * 86400)



// -----------------------------------------------------------------------------
//
// corArgsDurationParse -
//
bool corArgsDurationParse(const char* text, long long* secondsP, const char** whyP)
{
  const char* s = text;

  *whyP = NULL;

  if ((s == NULL) || (*s == 0))
  {
    *whyP = "empty";
    return false;
  }

  //
  // Plain seconds
  //
  if ((*s >= '0') && (*s <= '9'))
  {
    long long n = 0;

    for (; *s != 0; ++s)
    {
      if ((*s < '0') || (*s > '9'))
      {
        *whyP = "seconds are digits only - or an ISO 8601 duration (PT12H, P1D)";
        return false;
      }

      n = n * 10 + (*s - '0');
      if (n > DURATION_MAX)
      {
        *whyP = "longer than a hundred years";
        return false;
      }
    }

    *secondsP = n;
    return true;
  }

  if ((*s != 'P') && (*s != 'p'))
  {
    *whyP = "seconds, or an ISO 8601 duration - it starts with P (PT12H, P1D)";
    return false;
  }
  ++s;

  //
  // The designators, in their order: W D before T; H M S after it. 'order' is the rank of the last one seen.
  //
  static const char  dateUnits[] = "WD";
  static const char  timeUnits[] = "HMS";
  static const long long dateSecs[] = { 7 * 86400, 86400 };
  static const long long timeSecs[] = { 3600, 60, 1 };

  bool       inTime = false;
  int        order  = -1;
  int        parts  = 0;
  long long  total  = 0;

  while (*s != 0)
  {
    if ((*s == 'T') || (*s == 't'))
    {
      if (inTime == true)
      {
        *whyP = "a second T";
        return false;
      }

      inTime = true;
      order  = -1;
      ++s;

      if (*s == 0)
      {
        *whyP = "nothing after T (PT12H)";
        return false;
      }
      continue;
    }

    if ((*s < '0') || (*s > '9'))
    {
      *whyP = "a number before each designator (P1D, PT12H)";
      return false;
    }

    long long n = 0;
    while ((*s >= '0') && (*s <= '9'))
    {
      n = n * 10 + (*s - '0');
      if (n > DURATION_MAX)
      {
        *whyP = "longer than a hundred years";
        return false;
      }
      ++s;
    }

    if ((*s == '.') || (*s == ','))
    {
      *whyP = "whole numbers only (PT90M, not PT1.5H)";
      return false;
    }

    char        u     = (*s >= 'a' && *s <= 'z') ? (char) (*s - 'a' + 'A') : *s;
    const char* units = (inTime == true) ? timeUnits : dateUnits;
    const char* p     = (u != 0) ? strchr(units, u) : NULL;

    if (p == NULL)
    {
      if (u == 0)
        *whyP = "a number without its designator (P1D, PT12H)";
      else if ((inTime == false) && ((u == 'Y') || (u == 'M')))
        *whyP = "no years or months - their length varies (P30D, or PT1M for a minute)";
      else if ((inTime == false) && ((u == 'H') || (u == 'S')))
        *whyP = "hours, minutes and seconds come after T (PT12H)";
      else
        *whyP = (inTime == true) ? "after T: H, M or S (PT12H)" : "before T: W or D (P1D)";
      return false;
    }

    int rank = (int) (p - units);
    if (rank <= order)
    {
      *whyP = "the designators in order: W D, then T H M S";
      return false;
    }
    order = rank;

    total += n * ((inTime == true) ? timeSecs[rank] : dateSecs[rank]);
    if (total > DURATION_MAX)
    {
      *whyP = "longer than a hundred years";
      return false;
    }

    ++parts;
    ++s;
  }

  if (parts == 0)
  {
    *whyP = "P alone says nothing (PT0S, P1D)";
    return false;
  }

  *secondsP = total;
  return true;
}



// -----------------------------------------------------------------------------
//
// corArgsDurationText -
//
char* corArgsDurationText(long long seconds, char* buf, int bufSize)
{
  if (seconds < 0)
  {
    snprintf(buf, bufSize, "%lld", seconds);
    return buf;
  }

  if (seconds == 0)
  {
    snprintf(buf, bufSize, "PT0S");
    return buf;
  }

  long long d = seconds / 86400;
  long long h = (seconds % 86400) / 3600;
  long long m = (seconds % 3600) / 60;
  long long s = seconds % 60;
  int       n = snprintf(buf, bufSize, "P");

  if ((d > 0) && (n < bufSize))
    n += snprintf(&buf[n], bufSize - n, "%lldD", d);

  if (((h > 0) || (m > 0) || (s > 0)) && (n < bufSize))
  {
    n += snprintf(&buf[n], bufSize - n, "T");
    if ((h > 0) && (n < bufSize)) n += snprintf(&buf[n], bufSize - n, "%lldH", h);
    if ((m > 0) && (n < bufSize)) n += snprintf(&buf[n], bufSize - n, "%lldM", m);
    if ((s > 0) && (n < bufSize)) n += snprintf(&buf[n], bufSize - n, "%lldS", s);
  }

  return buf;
}
