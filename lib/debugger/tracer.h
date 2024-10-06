/*
 * Copyright (c) 2015 Balabit
 * Copyright (c) 2015 Balázs Scheidler
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As an additional exemption you are allowed to compile & link against the
 * OpenSSL libraries as published by the OpenSSL project. See the file
 * COPYING for details.
 *
 */
#ifndef DEBUGGER_TRACER_H_INCLUDED
#define DEBUGGER_TRACER_H_INCLUDED 1

#include "syslog-ng.h"

typedef struct _Tracer Tracer;

/* struct to track the invocation of a breakpoint, we have an instance for each thread */
typedef struct _BreakpointSite
{
  gboolean resume_requested;
  LogMessage *msg;
  LogPipe *pipe;
  gboolean drop;
} BreakpointSite;


void tracer_stop_on_breakpoint(Tracer *self, BreakpointSite *breakpoint_site);
gboolean tracer_wait_for_breakpoint(Tracer *self, BreakpointSite **breakpoint_site);
void tracer_resume_after_breakpoint(Tracer *self, BreakpointSite *breakpoint_site);
void tracer_cancel(Tracer *self);

Tracer *tracer_new(GlobalConfig *cfg);
void tracer_free(Tracer *self);

#endif
