/*
 * Copyright (c) 2024 Axoflow
 * Copyright (c) 2023 Balazs Scheidler <balazs.scheidler@axoflow.com>
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

#include "filterx/filterx-expr.h"
#include "cfg-source.h"
#include "messages.h"
#include "mainloop.h"

void
filterx_expr_set_location_with_text(FilterXExpr *self, CfgLexer *lexer, CFG_LTYPE *lloc, const gchar *text)
{
  self->lloc = *lloc;
  if (debug_flag)
    self->expr_text = g_strdup(text);
}

void
filterx_expr_set_location(FilterXExpr *self, CfgLexer *lexer, CFG_LTYPE *lloc)
{
  self->lloc = *lloc;
  if (debug_flag)
    {
      GString *res = g_string_sized_new(0);
      cfg_source_extract_source_text(lexer, lloc, res);
      self->expr_text = g_string_free(res, FALSE);
    }
}

EVTTAG *
filterx_expr_format_location_tag(FilterXExpr *self)
{
  if (self)
    return evt_tag_printf("expr", "%s:%d:%d|\t%s",
                          self->lloc.name, self->lloc.first_line, self->lloc.first_column,
                          self->expr_text ? : "n/a");
  else
    return evt_tag_str("expr", "n/a");
}

gboolean
filterx_expr_init_method(FilterXExpr *self, GlobalConfig *cfg)
{
  return TRUE;
}

void
filterx_expr_deinit_method(FilterXExpr *self, GlobalConfig *cfg)
{
}

void
filterx_expr_free_method(FilterXExpr *self)
{
  g_free(self->expr_text);
}

void
filterx_expr_init_instance(FilterXExpr *self)
{
  self->ref_cnt = 1;
  self->init = filterx_expr_init_method;
  self->deinit = filterx_expr_deinit_method;
  self->free_fn = filterx_expr_free_method;
}

FilterXExpr *
filterx_expr_new(void)
{
  FilterXExpr *self = g_new0(FilterXExpr, 1);
  filterx_expr_init_instance(self);
  return self;
}

FilterXExpr *
filterx_expr_ref(FilterXExpr *self)
{
  main_loop_assert_main_thread();

  if (!self)
    return NULL;

  self->ref_cnt++;
  return self;
}

void
filterx_expr_unref(FilterXExpr *self)
{
  main_loop_assert_main_thread();

  if (!self)
    return;

  if (--self->ref_cnt == 0)
    {
      self->free_fn(self);
      g_free(self);
    }
}

gboolean
filterx_unary_op_init_method(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXUnaryOp *self = (FilterXUnaryOp *) s;

  if (!filterx_expr_init(self->operand, cfg))
    return FALSE;

  return filterx_expr_init_method(s, cfg);
}

void
filterx_unary_op_deinit_method(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXUnaryOp *self = (FilterXUnaryOp *) s;

  filterx_expr_deinit(self->operand, cfg);
  filterx_expr_deinit_method(s, cfg);
}

void
filterx_unary_op_free_method(FilterXExpr *s)
{
  FilterXUnaryOp *self = (FilterXUnaryOp *) s;

  filterx_expr_unref(self->operand);
  filterx_expr_free_method(s);
}

void
filterx_unary_op_init_instance(FilterXUnaryOp *self, FilterXExpr *operand)
{
  filterx_expr_init_instance(&self->super);
  self->super.init = filterx_unary_op_init_method;
  self->super.deinit = filterx_unary_op_deinit_method;
  self->super.free_fn = filterx_unary_op_free_method;
  self->operand = operand;
}

void
filterx_binary_op_free_method(FilterXExpr *s)
{
  FilterXBinaryOp *self = (FilterXBinaryOp *) s;

  filterx_expr_unref(self->lhs);
  filterx_expr_unref(self->rhs);
  filterx_expr_free_method(s);
}

gboolean
filterx_binary_op_init_method(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXBinaryOp *self = (FilterXBinaryOp *) s;

  if (!filterx_expr_init(self->lhs, cfg))
    return FALSE;

  if (!filterx_expr_init(self->rhs, cfg))
    return FALSE;

  return filterx_expr_init_method(s, cfg);
}

void
filterx_binary_op_deinit_method(FilterXExpr *s, GlobalConfig *cfg)
{
  FilterXBinaryOp *self = (FilterXBinaryOp *) s;

  filterx_expr_deinit(self->lhs, cfg);
  filterx_expr_deinit(self->rhs, cfg);
  filterx_expr_deinit_method(s, cfg);
}

void
filterx_binary_op_init_instance(FilterXBinaryOp *self, FilterXExpr *lhs, FilterXExpr *rhs)
{
  filterx_expr_init_instance(&self->super);
  self->super.init = filterx_binary_op_init_method;
  self->super.deinit = filterx_binary_op_deinit_method;
  self->super.free_fn = filterx_binary_op_free_method;
  g_assert(lhs);
  g_assert(rhs);
  self->lhs = lhs;
  self->rhs = rhs;
}
