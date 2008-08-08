#include "graph_view.h"

gboolean graph_oe_out,
     graph_oe_wohn,
     graph_bo_out,
     graph_bo_wohn,
     graph_oe_vor,
     graph_oe_rueck;

void on_checkbutton_bo_out_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_bo_out = 1;
	else
		graph_bo_out = 0;
}

void on_checkbutton_bo_wohn_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_bo_wohn = 1;
	else
		graph_bo_wohn = 0;
}

void on_checkbutton_oe_out_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_oe_out = 1;
	else
		graph_oe_out = 0;
}
	
void on_checkbutton_oe_wohn_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_oe_wohn = 1;
	else
		graph_oe_wohn = 0;
}

void on_checkbutton_oe_vor_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_oe_vor = 1;
	else
		graph_oe_vor = 0;
}
	
void on_checkbutton_oe_rueck_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		graph_oe_rueck = 1;
	else
		graph_oe_rueck = 0;
}

