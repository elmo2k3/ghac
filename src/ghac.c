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
#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <glib/gprintf.h>
#include <pthread.h>
#include <time.h>
#include <string.h>
#include <config.h>
#include <stdint.h>

#ifdef ENABLE_LIBHAC
#include <libhac/libhac.h>
#endif
#ifdef ENABLE_LIBHAGRAPH
#include <libhagraph/libhagraph.h>
#endif

#include "ghac.h"
#include "graph_view.h"
#include "configfile.h"
#include "../version.h"

GtkStatusIcon *trayIcon=NULL;

GtkBuilder *builder;

GtkAdjustment *adjusts_rgb[3];

static uint8_t relaisState;
	
GtkWidget *errorPopup;
GtkWidget *widget;

#ifdef ENABLE_LIBHAGRAPH
struct _graph_data graph;
#endif
static int yet_drawed = 0;

static void ghac_initLibHac(char *server_ip, GtkWindow *parent);
void ghac_error_dialog(const char *window, gchar *string);
static int ghacUpdater();

gint exit_handler(GtkWidget *widget, GdkEvent *event, gpointer data)
{
//	closeLibHac();
	return FALSE; // darf beenden
}

G_MODULE_EXPORT void ghac_end(GtkWidget *widget, gpointer daten)
{
	char *home = getenv("HOME");
	char location[1024];
	sprintf(location,GHAC_CONFIG,home);
	saveConfig(location);
#ifdef ENABLE_LIBHAC
	closeLibHac();
#endif
	if(trayIcon)
	{
		g_object_unref(trayIcon);
	}
	gtk_main_quit();
}

#ifdef ENABLE_LIBHAC

static gboolean updateTemperatures()
{
	float temperature_outside,
	      temperature_wohnzimmer;

	gchar label_buffer[2000];
	
	
	getTemperature(3,1,&temperature_outside);
	getTemperature(3,0, &temperature_wohnzimmer);

	sprintf(label_buffer,"%3.2f°C", temperature_outside);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_outside")), label_buffer);
	sprintf(label_buffer,"%3.2f°C", temperature_wohnzimmer);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_wohnzimmer")), label_buffer);
//	getLastValueTable(label_buffer, config.graph_host, config.graph_user,
//		config.graph_password, config.graph_database, config.graph_database_ws2000);
//	gtk_status_icon_set_tooltip(trayIcon, label_buffer);
	return 1;
}

static gboolean updateThermostat()
{
	int16_t tempis, tempset, voltage;
	int8_t valve, mode;
	struct _hr20info hr20info;

	gchar label_buffer[20];
	
	hr20GetStatus(&hr20info);
	sprintf(label_buffer,"%3.2f°C", (float)hr20info.tempis/100.0);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_t_is")), label_buffer);
	if((hr20info.tempset/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(
			gtk_builder_get_object(builder,"combobox_temperature")),hr20info.tempset/50 - 10);
	if((hr20info.auto_temperature[0]/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(
			gtk_builder_get_object(builder,"combobox_temp_frost")),hr20info.auto_temperature[0]/50 - 10);
	if((hr20info.auto_temperature[1]/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(
			gtk_builder_get_object(builder,"combobox_temp_save")),hr20info.auto_temperature[1]/50 - 10);
	if((hr20info.auto_temperature[2]/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(
			gtk_builder_get_object(builder,"combobox_temp_comfort")),hr20info.auto_temperature[2]/50 - 10);
	if((hr20info.auto_temperature[3]/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(
			gtk_builder_get_object(builder,"combobox_temp_super_comfort")),hr20info.auto_temperature[3]/50 - 10);
	sprintf(label_buffer,"%d%%", hr20info.valve);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_valve")), label_buffer);
	sprintf(label_buffer,"%1.3fV", (float)hr20info.voltage/1000.0);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_bat")), label_buffer);
	gtk_combo_box_set_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_mode")),(gint)hr20info.mode-1);
	return 1;
}

G_MODULE_EXPORT gint thermostat_set_mode(GtkWidget *widget)
{
	gint mode = (gint)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_mode"))))+1;
	setHr20Mode((int)mode);
	return 0;
}

G_MODULE_EXPORT gint thermostat_set_temperature(GtkWidget *widget)
{
	int16_t temperature;

	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_temperature"))) + 10) * 5;


	setHr20Temperature(temperature);
	return 0;
}

G_MODULE_EXPORT gint on_combobox_temp_save_changed(GtkWidget *widget)
{
	int16_t temperature;
	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_temp_save"))) + 10) * 5;
	setHr20AutoTemperature(1, temperature);
	return 0;
}

G_MODULE_EXPORT gint on_combobox_temp_frost_changed(GtkWidget *widget)
{
	int16_t temperature;
	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_temp_frost"))) + 10) * 5;
	setHr20AutoTemperature(0, temperature);
	return 0;
}

G_MODULE_EXPORT gint on_combobox_temp_comfort_changed(GtkWidget *widget)
{
	int16_t temperature;
	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_temp_comfort"))) + 10) * 5;
	setHr20AutoTemperature(2, temperature);
	return 0;
}

G_MODULE_EXPORT gint on_combobox_temp_super_comfort_changed(GtkWidget *widget)
{
	int16_t temperature;
	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(gtk_builder_get_object(builder,"combobox_temp_super_comfort"))) + 10) * 5;
	setHr20AutoTemperature(3, temperature);
	return 0;
}

static gboolean updateVoltage()
{
	float voltage;
	gchar label_buffer[20];

	getVoltage(3,&voltage);
	sprintf(label_buffer,"%3.2fV",voltage);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_voltage")), label_buffer);
	return 1;
}
	

static gboolean updateRelais()
{
	relaisState = getRelaisState();

	if(relaisState & 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton1")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton1")), 0);
	if(relaisState & 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton2")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton2")), 0);
	if(relaisState & 4)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton3")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton3")), 0);
	if(relaisState & 8)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton4")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton4")), 0);
	if(relaisState & 16)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton5")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton5")), 0);
	if(relaisState & 32)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton6")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"relaisbutton6")), 0);
	return 1;

}

static gboolean updateModules()
{
	if(getLedmatrixState())
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"ledmatrix_button")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"ledmatrix_button")), 0);
	if(getScrobblerState())
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"scrobbler_button")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"scrobbler_button")), 0);
	return 1;
}

G_MODULE_EXPORT void on_ledmatrix_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		setLedmatrixOn();
	else
		setLedmatrixOff();
}

G_MODULE_EXPORT void on_scrobbler_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		setScrobblerOn();
	else
		setScrobblerOff();
}


G_MODULE_EXPORT void on_relaisbutton1_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 1;
	else
		relaisState &= ~1;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_relaisbutton2_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 2;
	else
		relaisState &= ~2;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_relaisbutton3_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 4;
	else
		relaisState &= ~4;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_relaisbutton4_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 8;
	else
		relaisState &= ~8;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_relaisbutton5_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 16;
	else
		relaisState &= ~16;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_relaisbutton6_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 32;
	else
		relaisState &= ~32;

	setRelais(relaisState);
}

G_MODULE_EXPORT void on_button_send_clicked(GtkWidget *widget)
{
	gint rgb_modul;
	gint smoothness;
	
	rgb_modul = 3;
	smoothness = 5;

	setRgbValueModul(0x10, (int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_red0"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_green0"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_blue0"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_smoothness"))))
		    );
	setRgbValueModul(0x11, (int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_red1"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_green1"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_blue1"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_smoothness"))))
		    );
	setRgbValueModul(0x12, (int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_red2"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_green2"))),
			(int)gtk_range_get_value(GTK_RANGE(gtk_builder_get_object(builder,"vscale_blue2"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_smoothness"))))
		    );

}

static gboolean updateRgb()
{
	int i;
	gchar smoothness_buf[2];
	gchar slider_name[64];

	struct _hadState hadState;

	getHadState(&hadState);
	
	for(i=0;i<3;i++)
	{
		g_sprintf(smoothness_buf,"%d",hadState.rgbModuleValues[i].smoothness);
		g_sprintf(slider_name,"vscale_red%d",i);
		gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder,slider_name)),hadState.rgbModuleValues[i].red);
		g_sprintf(slider_name,"vscale_green%d",i);
		gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder,slider_name)),hadState.rgbModuleValues[i].green);
		g_sprintf(slider_name,"vscale_blue%d",i);
		gtk_range_set_value(GTK_RANGE(gtk_builder_get_object(builder,slider_name)),hadState.rgbModuleValues[i].blue);
		gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_smoothness")),smoothness_buf);
	}
	
	return 1;
}

#endif //ENABLE_LIBHAC

static int getModuleSensorArrays(int *modul, int *sensor)
{
	int numGraphs = 0;
	if(config.graph_bo_out)
	{
		modul[numGraphs] = 3;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_bo_wohn)
	{
		modul[numGraphs] = 3;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(config.graph_oe_out)
	{
		modul[numGraphs] = 4;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(config.graph_oe_vor)
	{
		modul[numGraphs] = 2;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(config.graph_oe_rueck)
	{
		modul[numGraphs] = 2;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_oe_wohn)
	{
		modul[numGraphs] = 4;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_bo_hk_soll)
	{
		modul[numGraphs] = 5;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_bo_hk_ist)
	{
		modul[numGraphs] = 5;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(config.graph_bo_hk_ventil)
	{
		modul[numGraphs] = 5;
		sensor[numGraphs] = 2;
		numGraphs++;
	}
	if(config.graph_bo_hk_spannung)
	{
		modul[numGraphs] = 5;
		sensor[numGraphs] = 3;
		numGraphs++;
	}
	if(config.graph_oe_hk_soll)
	{
		modul[numGraphs] = 6;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_oe_hk_ist)
	{
		modul[numGraphs] = 6;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(config.graph_oe_hk_ventil)
	{
		modul[numGraphs] = 6;
		sensor[numGraphs] = 2;
		numGraphs++;
	}
	if(config.graph_oe_hk_spannung)
	{
		modul[numGraphs] = 6;
		sensor[numGraphs] = 3;
		numGraphs++;
	}
	if(config.graph_bo_door)
	{
		modul[numGraphs] = 7;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(config.graph_bo_window)
	{
		modul[numGraphs] = 7;
		sensor[numGraphs] = 2;
		numGraphs++;
	}
	if(config.graph_oe_dachboden)
	{
		modul[numGraphs] = 8;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	return numGraphs;
}


G_MODULE_EXPORT void updateGraph(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	char date[20];
	char from[13], to[13];
	int view;
	int modul[15], sensor[15], numGraphs;
	unsigned int day, month, year;
	gchar message[1024];
	int error;
	
	numGraphs = getModuleSensorArrays(modul, sensor);
#ifdef ENABLE_LIBHAGRAPH
	if(!yet_drawed)
	{
		int i;
		gtk_calendar_get_date(GTK_CALENDAR(gtk_builder_get_object(builder,"calendar")),
					&year, &month, &day);
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"radio_day"))))
			view = TB_DAY;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"radio_week"))))
			view = TB_WEEK;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"radio_month"))))
			view = TB_MONTH;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"radio_year"))))
			view = TB_YEAR;

		sprintf(date,"%d-%02d-%02d",year,month+1,day);
		transformDate(from,to,date,view);

		initGraph(&graph, from, to);
		for(i=0; i< numGraphs; i++)
		{
			if(error = addGraphData(&graph, modul[i], sensor[i],
				config.graph_host,
				config.graph_user,
				config.graph_password,
				config.graph_database,
				config.graph_database_ws2000))
			{
				switch(error)
				{
					case -1: g_stpcpy(message,"could not draw graph: could not connect to database");
							break;
					case -2: g_stpcpy(message,"could not draw graph: empty dataset");
							break;
					default: g_sprintf(message,"could not draw graph: unknown error %d",error);
							break;
				}
				ghac_error_dialog("mainWindow",message);
				break;
			}
		}
		yet_drawed = 1;

	}
	
	drawGraphGtk(widget, &graph); 
#endif //ENABLE_LIBHAGRAPH
}

void ghac_error_dialog(const char *window, gchar *string)
{
	g_assert(window);
	g_assert(string);

	GtkWidget *error_dialog = gtk_message_dialog_new(
		GTK_WINDOW(gtk_builder_get_object(builder,window)),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_MESSAGE_ERROR,
		GTK_BUTTONS_CLOSE,
		"%s",
		string);
	gtk_dialog_run(GTK_DIALOG(error_dialog));
	gtk_widget_destroy(error_dialog);
}

G_MODULE_EXPORT void save_graph(GtkWidget *widget, GData *data)
{
	char *filename = (char*)gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gtk_builder_get_object(builder,"button_save_graph")));
	if(!filename)
		return;
	strncpy(config.last_graph_filename, filename, 4095);
	g_free(filename);
	config.last_graph_filename[4095] = '\0';
	if(!strstr(config.last_graph_filename, ".png") && strlen(config.last_graph_filename) < 4092)
		strcat(config.last_graph_filename, ".png");

	config.last_graph_width = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gtk_builder_get_object(builder,"spinbutton_width")));
	config.last_graph_height = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gtk_builder_get_object(builder,"spinbutton_height")));
	if(!yet_drawed)
		gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder,"drawingarea2")));
	drawGraphPng(config.last_graph_filename, &graph, config.last_graph_width, config.last_graph_height);	
}

G_MODULE_EXPORT gboolean on_button_draw_clicked(GtkButton *button)
{
#ifdef ENABLE_LIBHAGRAPH
	freeGraph(&graph);
#endif
	yet_drawed = 0;
	gtk_widget_queue_draw(GTK_WIDGET(gtk_builder_get_object(builder,"drawingarea2")));
	return 1;
}


G_MODULE_EXPORT void on_button_config_set_clicked(GtkWidget *widget)
{
	int tempint;
	strncpy(config.had_ip,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_ip"))), 15);
	config.had_ip[15] = 0;
	strncpy(config.had_password,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_password"))), 127);
	config.had_password[127] = '\0';
	config.had_port = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_port"))));
	config.graph_port = atoi(gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_port"))));
	strncpy(config.graph_database,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_database"))), 99);
	config.graph_database[99] = 0;
	strncpy(config.graph_user,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_user"))), 99);
	config.graph_user[99] = 0;
	strncpy(config.graph_password,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_password"))), 99);
	config.graph_password[99] = 0;
	strncpy(config.graph_database_ws2000,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_ws2000"))), 99);
	config.graph_database_ws2000[99] = 0;
	strncpy(config.graph_host,gtk_entry_get_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_host"))), 99);
	config.graph_host[99] = 0;
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_had_activated"))); 
#ifdef ENABLE_LIBHAC
	if(tempint != config.had_activated)
	{
		if(tempint)
		{
			if(config.had_control_activated)
				gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder,"hbox4")));
			if(config.thermostat_activated)
				gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder,"fixed2")));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"hbox4")));
			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"fixed2")));
			closeLibHac();
		}
	}
	if(config.had_activated)
	{
		closeLibHac();
		ghac_initLibHac(config.had_ip, 
			GTK_WINDOW(gtk_builder_get_object(builder,"mainWindow")));
	}
#endif
	config.had_activated = tempint;
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_graph_activated"))); 
	if(tempint != config.graph_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder,"vbox2")));
		else
			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"vbox2")));
		config.graph_activated = tempint;
	}
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_control_activated"))); 
	if(tempint != config.had_control_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder,"hbox4")));
		else
			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"hbox4")));
		config.had_control_activated = tempint;
	}
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_thermostat_activated"))); 
	if(tempint != config.thermostat_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(gtk_builder_get_object(builder,"fixed2")));
		else
			gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"fixed2")));
		config.thermostat_activated = tempint;
	}
}

G_MODULE_EXPORT void trayIconClicked(GtkWidget *foo, gpointer data)
{
	if(GTK_WIDGET_VISIBLE(widget))
		gtk_widget_hide(GTK_WIDGET(widget));
	else
	{
		gtk_window_deiconify(GTK_WINDOW(widget));
		gtk_widget_show(GTK_WIDGET(widget));
	}
}

void trayIconPopup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}

G_MODULE_EXPORT void loadConfigToGui()
{
	gchar entry[100];
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_ip")),config.had_ip);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_password")),config.had_password);
	g_sprintf(entry,"%d",config.had_port);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_had_port")),entry);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_database")),config.graph_database);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_ws2000")),config.graph_database_ws2000);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_host")),config.graph_host);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_user")),config.graph_user);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_password")),config.graph_password);
	g_sprintf(entry,"%d",config.graph_port);
	gtk_entry_set_text(GTK_ENTRY(gtk_builder_get_object(builder,"entry_graph_port")),entry);
	if(config.had_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_had_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_had_activated")),0);
	if(config.graph_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_graph_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_graph_activated")),0);
	if(config.thermostat_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_thermostat_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_thermostat_activated")),0);
	if(config.had_control_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_control_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_control_activated")),0);
	if(config.graph_oe_out)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_out")),1);
	if(config.graph_oe_wohn)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_wohn")),1);
	if(config.graph_bo_out)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_out")),1);
	if(config.graph_bo_wohn)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_wohn")),1);
	if(config.graph_oe_vor)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_vor")),1);
	if(config.graph_oe_rueck)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_rueck")),1);
	if(config.graph_bo_hk_soll)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_hk_soll")),1);
	if(config.graph_bo_hk_ist)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_hk_ist")),1);
	if(config.graph_bo_hk_ventil)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_hk_ventil")),1);
	if(config.graph_bo_hk_spannung)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_bo_hk_spannung")),1);
	if(config.graph_oe_hk_soll)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_hk_soll")),1);
	if(config.graph_oe_hk_ist)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_hk_ist")),1);
	if(config.graph_oe_hk_ventil)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_hk_ventil")),1);
	if(config.graph_oe_hk_spannung)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_hk_spannung")),1);
	if(config.graph_oe_dachboden)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gtk_builder_get_object(builder,"checkbutton_oe_dachboden")),1);

	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(builder,"spinbutton_width")), (gdouble)config.last_graph_width);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(gtk_builder_get_object(builder,"spinbutton_height")), (gdouble)config.last_graph_height);
}
	

int main(int argc, char *argv[])
{
	time_t rawtime;
	struct tm *today;

	char *server_ip = getenv("HAD_HOST");
	char *home = getenv("HOME");
	char location[2048];
	int error = 0;
	sprintf(location,GHAC_CONFIG,home);
	
	loadConfig(location);
	if(!server_ip)
	{
		server_ip = config.had_ip;
	}
	
	time(&rawtime);
	today = localtime(&rawtime);
	
	gtk_init(&argc, &argv);
	builder = gtk_builder_new();
#ifdef _WIN32
	gtk_builder_add_from_file(builder, "C:\\Programme\\ghac\\ghac.ui", NULL);
	trayIcon = gtk_status_icon_new_from_file("C:\\Programme\\ghac\\gnome-color-browser.png");
#else
	sprintf(location,"%s/.ghac/ghac.ui",home);
	if(!gtk_builder_add_from_file(builder, location, NULL))
	{
		fprintf(stderr,"Could not import %s\n",location);
		exit(1);
	}
	sprintf(location,"%s/.ghac/gnome-color-browser.png",home);
	trayIcon = gtk_status_icon_new_from_file(location);
#endif

	widget = GTK_WIDGET(gtk_builder_get_object(builder, "mainWindow"));
	
#ifdef ENABLE_LIBHAC
	if(config.had_activated)
	{
		ghac_initLibHac(server_ip, GTK_WINDOW(widget));
	}
#endif

	g_signal_connect(widget, "delete-event",
			G_CALLBACK(exit_handler), NULL);

	g_signal_connect(widget, "destroy",
			G_CALLBACK(ghac_end), NULL);
	
	g_signal_connect(trayIcon, "activate", 
			G_CALLBACK(trayIconClicked), NULL);

	
	gtk_status_icon_set_visible(trayIcon, TRUE);
	gtk_builder_connect_signals(builder, NULL);

	gtk_calendar_select_month(GTK_CALENDAR(gtk_builder_get_object(builder,"calendar")),
			today->tm_mon, today->tm_year+1900);
	gtk_calendar_select_day(GTK_CALENDAR(gtk_builder_get_object(builder,"calendar")),
			today->tm_mday);
	
	gtk_widget_show_all(GTK_WIDGET(widget));
	if(!config.had_control_activated)
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"hbox4")));
	if(!config.thermostat_activated)
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"fixed2")));
	if(!config.graph_activated)
		gtk_widget_hide(GTK_WIDGET(gtk_builder_get_object(builder,"vbox2")));
	loadConfigToGui();
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_version_ghac")), GHAC_VERSION);
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_version_libhac")), libhacVersion());
	gtk_label_set_text(GTK_LABEL(gtk_builder_get_object(builder,"label_version_libhagraph")), libhagraphVersion());
#ifdef ENABLE_LIBHAC
	g_timeout_add(500, (GSourceFunc)ghacUpdater, NULL);
#endif
#ifdef ENABLE_LIBHAGRAPH
	g_timeout_add_seconds(300, (GSourceFunc)on_button_draw_clicked, NULL);
	memset(location,0,sizeof(location));
//	getLastValueTable(location, config.graph_host, config.graph_user,
//		config.graph_password, config.graph_database, config.graph_database_ws2000);
//	gtk_status_icon_set_tooltip(trayIcon, location);
#endif
	gtk_main();
	return 0;
}

static int ghacUpdater()
{
	updateThermostat();
	updateRgb();
	updateRelais();
	updateModules();
	updateVoltage();
	updateTemperatures();
	g_timeout_add_seconds(10, (GSourceFunc)updateRgb, NULL);
	g_timeout_add_seconds(10, (GSourceFunc)updateRelais, NULL);
	g_timeout_add_seconds(10, (GSourceFunc)updateModules, NULL);
	g_timeout_add_seconds(10, (GSourceFunc)updateVoltage, NULL);
	g_timeout_add_seconds(10, (GSourceFunc)updateTemperatures, NULL);
	g_timeout_add_seconds(60, (GSourceFunc)updateThermostat, NULL);
	return 0;
}

static void ghac_initLibHac(char *server_ip, GtkWindow *parent)
{
	int error = 0;
	gchar message[1024];
	if(error = initLibHac(server_ip, config.had_password))
	{
		if(error == LIBHAC_WRONG_PASSWORD)
			g_sprintf(message, "could not connect to had: wrong password");
		else
			g_sprintf(message, "could not connect to had: %s", g_strerror(error));
		ghac_error_dialog("mainWindow",message);
	}
}
