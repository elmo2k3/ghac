#ifndef __GRAPH_VIEW_H__
#define __GRAPH_VIEW_H__

#include <gtk/gtk.h>

void on_checkbutton_bo_out_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_bo_wohn_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_out_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_wohn_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_vor_toggled(GtkToggleButton *toggle_button);
void on_checkbutton_oe_rueck_toggled(GtkToggleButton *toggle_button);

extern gboolean graph_oe_out,
     graph_oe_wohn,
     graph_bo_out,
     graph_bo_wohn,
     graph_oe_vor,
     graph_oe_rueck;

#endif

