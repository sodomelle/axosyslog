/*
 * Copyright (c) 2024 Attila Szakacs
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

#include "filterx/func-len.h"
#include "filterx/object-primitive.h"
#include "filterx/filterx-eval.h"

#define FILTERX_FUNC_LEN_USAGE "Usage: len(object)"

FilterXObject *
filterx_simple_function_len(FilterXExpr *s, FilterXObject *args[], gsize args_len)
{
  if (args == NULL || args_len != 1)
    {
      filterx_simple_function_argument_error(s, "Requires exactly one argument", FALSE);
      return NULL;
    }

  FilterXObject *object = args[0];

  guint64 len;
  gboolean success = filterx_object_len(object, &len);
  if (!success)
    {
      filterx_eval_push_error("Object does not support len()", s, object);
      return NULL;
    }

  return filterx_integer_new((gint64) MIN(len, G_MAXINT64));
}