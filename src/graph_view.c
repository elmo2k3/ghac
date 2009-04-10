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
#include "graph_view.h"
#include "configfile.h"

G_MODULE_EXPORT void on_checkbutton_bo_out_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_out =1;
	else
		config.graph_bo_out =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_wohn_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_wohn =1;
	else
		config.graph_bo_wohn =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_out_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_out =1;
	else
		config.graph_oe_out =0;
}
	
G_MODULE_EXPORT void on_checkbutton_oe_wohn_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_wohn =1;
	else
		config.graph_oe_wohn =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_vor_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_vor =1;
	else
		config.graph_oe_vor =0;
}
	
G_MODULE_EXPORT void on_checkbutton_oe_rueck_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_rueck =1;
	else
		config.graph_oe_rueck =0;
}
	
G_MODULE_EXPORT void on_checkbutton_bo_hk_soll_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_hk_soll =1;
	else
		config.graph_bo_hk_soll =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_hk_ist_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_hk_ist =1;
	else
		config.graph_bo_hk_ist =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_hk_ventil_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_hk_ventil =1;
	else
		config.graph_bo_hk_ventil =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_hk_spannung_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_hk_spannung =1;
	else
		config.graph_bo_hk_spannung =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_hk_soll_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_hk_soll =1;
	else
		config.graph_oe_hk_soll =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_hk_ist_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_hk_ist =1;
	else
		config.graph_oe_hk_ist =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_hk_ventil_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_hk_ventil =1;
	else
		config.graph_oe_hk_ventil =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_hk_spannung_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_hk_spannung =1;
	else
		config.graph_oe_hk_spannung =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_door_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_door =1;
	else
		config.graph_bo_door =0;
}

G_MODULE_EXPORT void on_checkbutton_bo_window_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_bo_window =1;
	else
		config.graph_bo_window =0;
}

G_MODULE_EXPORT void on_checkbutton_oe_dachboden_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		config.graph_oe_dachboden =1;
	else
		config.graph_oe_dachboden =0;
}
