#ifndef CORARGS_CORARGTYPE_H_
#define CORARGS_CORARGTYPE_H_

// -----------------------------------------------------------------------------
//
// FILE                  CorArgType.h - CorArgType enum for CorArg and CorArgInfo structs
//
// AUTHOR                Ken Zangelin
//
// Copyright 2024 Ken Zangelin
//
// SPDX-License-Identifier: Apache-2.0
//



// -----------------------------------------------------------------------------
//
// ValueFrom -
//
typedef enum CorArgValueFrom
{
  CorArgFromDefaultValue,
  CorArgFromConfigFile,
  CorArgFromEnvVar,
  CorArgFromShortOption,
  CorArgFromLongOption
} CorArgValueFrom;



// -----------------------------------------------------------------------------
//
// CorArgType -
//
typedef enum CorArgType
{
  CorArgBool,
  CorArgFloat,
  CorArgString,
  CorArgChar,
  CorArgInt8    = CorArgChar,
  CorArgUChar,
  CorArgUInt8   = CorArgUChar,
  CorArgShort,
  CorArgInt16   = CorArgShort,
  CorArgUShort,
  CorArgUInt16  = CorArgUShort,
  CorArgInt,
  CorArgInt32   = CorArgInt,
  CorArgUInt,
  CorArgUInt32  = CorArgUInt,
  CorArgLong,
  CorArgInt64   = CorArgLong,
  CorArgULong,
  CorArgUInt64  = CorArgULong,
  CorArgSeparator,
  CorArgEnd,

  //
  // After CorArgEnd, so no value above changes (a plugin built against the earlier header keeps working)
  //
  CorArgDuration    // an int of seconds - given as seconds ("3600") or as ISO 8601 ("PT1H", "P1D", "P7D")
} CorArgType;

#endif  // CORARGS_CORARGTYPE_H_
