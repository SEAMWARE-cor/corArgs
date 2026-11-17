#ifndef CORARGS_CORARGSDURATION_H_
#define CORARGS_CORARGSDURATION_H_

//
// FILE            corArgsDuration.h
//
// Copyright 2026 Seamware
// SPDX-License-Identifier: Apache-2.0
//
#include <stdbool.h>                   // bool



// -----------------------------------------------------------------------------
//
// corArgsDurationParse - a CorArgDuration option's text as seconds: plain seconds ("3600") or an ISO 8601
// duration - P[nW][nD][T[nH][nM][nS]]: "PT30S", "PT15M", "PT12H", "P1D", "P7D", "P1DT12H", "P2W".
//
// Whole numbers only, the designators in that order, upper or lower case. Years and months are refused - they
// have no fixed length in seconds. false: not a duration, *whyP says why.
//
extern bool corArgsDurationParse(const char* text, long long* secondsP, const char** whyP);



// -----------------------------------------------------------------------------
//
// corArgsDurationText - seconds as an ISO 8601 duration, in buf: 86400 "P1D", 43200 "PT12H", 5400 "PT1H30M",
// 0 "PT0S". A negative number: its seconds ("-5"). Returns buf.
//
extern char* corArgsDurationText(long long seconds, char* buf, int bufSize);

#endif  // CORARGS_CORARGSDURATION_H_
