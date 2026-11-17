#!/bin/bash
#
# FILE            durationTest.sh
#
# AUTHOR          Ken Zangelin
#
# Copyright 2026 Seamware
# SPDX-License-Identifier: Apache-2.0
#
# A CorArgDuration option (test/durationTest.c is the program):
#   - seconds as before ("86400"), or ISO 8601: P[nW][nD][T[nH][nM][nS]] - PT12H, P1D, P7D, P1DT12H30M15S
#   - from the command line and the environment, the same
#   - years and months refused (no fixed length), fractions refused, the designators in their order
#   - its limits checked and said as durations ("Allowed range: PT1M-P365D")
#   - the extended usage: the type Duration, the default as a duration
#
#   durationTest.sh <path to durationTest>
#
P=${1:?usage: durationTest.sh <path to durationTest>}
failures=0

#
# check <description> <expected exit code> <grep -E pattern the output must match, or !pattern it must not> <command...>
#
check() {
  local what=$1 rcWanted=$2 pattern=$3
  shift 3

  local out rc
  out=$("$@" 2>&1)
  rc=$?

  local ok=1
  [ "$rc" = "$rcWanted" ] || ok=0
  if [ "${pattern:0:1}" = '!' ]; then
    command grep -q -E -- "${pattern:1}" <<< "$out" && ok=0
  else
    command grep -q -E -- "$pattern" <<< "$out" || ok=0
  fi

  if [ $ok = 1 ]; then
    echo "OK      $what"
  else
    echo "FAILED  $what (exit code $rc, expected $rcWanted; output must match '$pattern'):"
    sed 's/^/          /' <<< "$out"
    failures=$((failures + 1))
  fi
}

unset DURATIONTEST_RENEW

check "the default: an hour"                        0 '^OK renew 3600 PT1H$'                       "$P"
check "plain seconds, as before"                    0 '^OK renew 86400 P1D$'                       "$P" --renew 86400
check "PT12H"                                       0 '^OK renew 43200 PT12H$'                     "$P" --renew PT12H
check "P1D"                                         0 '^OK renew 86400 P1D$'                       "$P" --renew P1D
check "P7D"                                         0 '^OK renew 604800 P7D$'                      "$P" -r P7D
check "P2W"                                         0 '^OK renew 1209600 P14D$'                    "$P" --renew P2W
check "P1DT12H30M15S"                               0 '^OK renew 131415 P1DT12H30M15S$'            "$P" --renew P1DT12H30M15S
check "PT90M - more than 59 in a field"             0 '^OK renew 5400 PT1H30M$'                    "$P" --renew PT90M
check "lower case"                                  0 '^OK renew 900 PT15M$'                       "$P" --renew pt15m
check "from the environment"                        0 '^OK renew 604800 P7D$'                      env DURATIONTEST_RENEW=P7D "$P"
check "the limits: P365D taken"                     0 '^OK renew 31536000 P365D$'                  "$P" --renew P365D
check "above the max: refused, said as durations"   3 "value P366D for option '--renew' exceeds its upper limit. Allowed range: PT1M-P365D" "$P" --renew P366D
check "below the min: refused, said as durations"   3 "value PT30S for option '--renew' is inferior to its lower limit. Allowed range: PT1M-P365D" "$P" --renew PT30S
check "years: refused - no fixed length"            3 "no years or months"                         "$P" --renew P1Y
check "P1M (a month): refused, PT1M said"           3 "PT1M for a minute"                          "$P" --renew P1M
check "a fraction: refused"                         3 "whole numbers only"                         "$P" --renew PT1.5H
check "hours before T: refused"                     3 "come after T"                               "$P" --renew P12H
check "out of order: refused"                       3 "in order"                                   "$P" --renew PT1M2H
check "P alone: refused"                            3 "P alone"                                    "$P" --renew P
check "T with nothing after: refused"               3 "nothing after T"                            "$P" --renew P1DT
check "seconds with a letter: refused"              3 "digits only"                                "$P" --renew 12h
check "a bad value from the environment: refused"   3 "invalid value 'P1Y'"                        env DURATIONTEST_RENEW=P1Y "$P"
check "the usage: the type and the default"         0 "Duration.*PT1H"                             "$P" -U
check "text: 0"                                     0 '^PT0S$'                                     "$P" text 0
check "text: 59"                                    0 '^PT59S$'                                    "$P" text 59
check "text: 90061"                                 0 '^P1DT1H1M1S$'                               "$P" text 90061

if [ $failures != 0 ]; then
  echo "durationTest: $failures FAILED"
  exit 1
fi

echo "durationTest: all OK"
