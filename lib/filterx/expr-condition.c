/*
 * Copyright (c) 2023 shifter <shifter@axoflow.com>
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

#include "filterx/expr-condition.h"
#include "filterx/object-primitive.h"
#include "scratch-buffers.h"

typedef struct _FilterXConditional FilterXConditional;

struct _FilterXConditional
{
  FilterXExpr super;
  FilterXExpr *condition;
  FilterXExpr *true_branch;
  FilterXExpr *false_branch;
};

static gboolean
_init(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXConditional *self = (FilterXConditional *) s;

  if (!filterx_expr_init(self->condition, cfg))
    return FALSE;

  if (!filterx_expr_init(self->true_branch, cfg))
    {
      filterx_expr_deinit(self->condition, cfg);
      return FALSE;
    }

  if (!filterx_expr_init(self->false_branch, cfg))
    {
      filterx_expr_deinit(self->condition, cfg);
      filterx_expr_deinit(self->true_branch, cfg);
      return FALSE;
    }

  return filterx_expr_init_method(s, cfg);
}


static void
_deinit(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXConditional *self = (FilterXConditional *) s;

  filterx_expr_deinit(self->condition, cfg);
  filterx_expr_deinit(self->true_branch, cfg);
  filterx_expr_deinit(self->false_branch, cfg);
  filterx_expr_deinit_method(s, cfg);
}

static void
_free(FilterXExpr *s)
{
  FilterXConditional *self = (FilterXConditional *) s;

  filterx_expr_unref(self->condition);
  filterx_expr_unref(self->true_branch);
  filterx_expr_unref(self->false_branch);
  filterx_expr_free_method(s);
}

static FilterXObject *
_eval(FilterXExpr *s)
{
  FilterXConditional *self = (FilterXConditional *) s;
  FilterXObject *condition_value = filterx_expr_eval(self->condition);
  FilterXObject *result = NULL;

  if (!condition_value)
    return NULL;

  if (trace_flag)
    {
      ScratchBuffersMarker mark;
      GString *buf = scratch_buffers_alloc_and_mark(&mark);

      if (!filterx_object_repr(condition_value, buf))
        {
          LogMessageValueType t;
          if (!filterx_object_marshal(condition_value, buf, &t))
            g_assert_not_reached();
        }

      msg_trace(filterx_object_truthy(condition_value) ? "FILTERX CONDT" : "FILTERX CONDF",
                filterx_expr_format_location_tag(self->condition),
                evt_tag_mem("value", buf->str, buf->len),
                evt_tag_int("truthy", filterx_object_truthy(condition_value)),
                evt_tag_str("type", condition_value->type->name));
      scratch_buffers_reclaim_marked(mark);
    }

  if (filterx_object_truthy(condition_value))
    {
      if (self->true_branch)
        result = filterx_expr_eval(self->true_branch);
      else
        result = filterx_object_ref(condition_value);
    }
  else
    {
      if (self->false_branch)
        result = filterx_expr_eval(self->false_branch);
      else
        result = filterx_boolean_new(TRUE);
    }

  filterx_object_unref(condition_value);
  return result;
}

void
filterx_conditional_set_true_branch(FilterXExpr *s, FilterXExpr *true_branch)
{
  FilterXConditional *self = (FilterXConditional *) s;

  filterx_expr_unref(self->true_branch);
  self->true_branch = true_branch;
}

void
filterx_conditional_set_false_branch(FilterXExpr *s, FilterXExpr *false_branch)
{
  FilterXConditional *self = (FilterXConditional *) s;

  filterx_expr_unref(self->false_branch);
  self->false_branch = false_branch;
}

FilterXExpr *
filterx_conditional_new(FilterXExpr *condition)
{
  FilterXConditional *self = g_new0(FilterXConditional, 1);
  filterx_expr_init_instance(&self->super);
  self->super.eval = _eval;
  self->super.init = _init;
  self->super.deinit = _deinit;
  self->super.free_fn = _free;
  self->super.suppress_from_trace = TRUE;
  self->condition = condition;
  return &self->super;
}

FilterXExpr *
filterx_conditional_find_tail(FilterXExpr *s)
{
  /* check if this is a FilterXConditional instance */
  if (s->eval != _eval)
    return NULL;

  FilterXConditional *self = (FilterXConditional *) s;
  if (self->false_branch)
    {
      FilterXConditional *tail = (FilterXConditional *) filterx_conditional_find_tail(self->false_branch);
      if (tail)
        {
          g_assert(tail->false_branch == NULL);
          return &tail->super;
        }
    }
  return s;
}
