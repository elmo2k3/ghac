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

