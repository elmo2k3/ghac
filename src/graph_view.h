/*
 * Copyright (C) 2007-2009 Bjoern Biesenbach <bjoern@bjoern-b.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */
#ifndef __GRAPH_VIEW_H__
#define __GRAPH_VIEW_H__

#include <gtk/gtk.h>

void on_checkbutton_bo_out_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_bo_wohn_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_out_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_wohn_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_vor_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_rueck_toggled(GtkToggleButton *toggle_button);

#endif

