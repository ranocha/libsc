/*
  This file is part of the SC Library, version 3.
  The SC Library provides support for parallel scientific applications.

  Copyright (C) 2019 individual authors

  The SC Library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  The SC Library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with the SC Library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
  02110-1301, USA.
*/

/** \file sc3_error.h
 */

#ifndef SC3_ERROR_H
#define SC3_ERROR_H

#include <sc3.h>

#ifdef __cplusplus
extern              "C"
{
#if 0
}
#endif
#endif

typedef struct sc3_error sc3_error_t;
typedef struct sc3_error_args sc3_error_args_t;

typedef enum sc3_error_severity
{
  SC3_RUNTIME,
  SC3_WARNING,
  SC3_FATAL,
  SC3_ERROR_SEVERITY_LAST
}
sc3_error_severity_t;

typedef enum sc3_error_sync
{
  SC3_LOCAL,
  SC3_SYNCED,
  SC3_DISAGREE,
  SC3_ERROR_SYNC_LAST
}
sc3_error_sync_t;

/*** TODO pass counting memory allocator to constructor */

/*** TODO implement reference counting */

sc3_error_args_t   *sc3_error_args_new (void);
void                sc3_error_args_set_from (sc3_error_args_t * ea,
                                             sc3_error_t from);
void                sc3_error_args_set_severity (sc3_error_args_t * ea,
                                                 sc3_error_severity_t sev);
void                sc3_error_args_set_sync (sc3_error_args_t * ea,
                                             sc3_error_sync_t syn);
void                sc3_error_args_set_msg (sc3_error_args_t * ea,
                                            const char *errmsg);
void                sc3_error_args_set_msgf (sc3_error_args_t * ea,
                                             const char *errfmt, ...);
void                sc3_error_args_destroy (sc3_error_args_t * ea);

sc3_error_t        *sc3_error_new (sc3_error_args_t ea);

sc3_error_t        *sc3_error_new_new (sc3_error_severity_t sev,
                                       sc3_error_sync_t syn,
                                       const char *errmsg);
sc3_error_t        *sc3_error_new_from (sc3_error_t * from,
                                        const char *errmsg);

/*** TODO need a bunch of _get_ and/or _is_ functions ***/

void                sc3_error_destroy (sc3_error_t * e);

#ifdef __cplusplus
#if 0
{
#endif
}
#endif

#endif /* !SC3_ERROR_H */
