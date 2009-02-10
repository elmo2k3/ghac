#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

#include "ghac.h"
#include "graph_view.h"
#include "libhagraph.h"
#include "data.h"
#include "config.h"
#include <libhac/libhac.h>

	
#define SECONDS_PER_DAY (60*60*24)


pthread_t update_thread, network_thread;

GladeXML *xml;

GtkAdjustment *adjusts_rgb[3];

static uint8_t relaisState;
	
GtkWidget *errorPopup;
GtkWidget *widget;

struct _graph_data graph;
static int yet_drawed = 0;

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
	closeLibHac();
	gtk_main_quit();
}

void updateTemperatures()
{
	float temperature_outside,
	      temperature_wohnzimmer;

	gchar label_buffer[20];
	
	
	if(getTemperature(3,1,&temperature_outside) < 0)
	{
		gtk_widget_show_all(GTK_WIDGET(errorPopup));
	}
	if(getTemperature(3,0, &temperature_wohnzimmer) < 0)
	{
		gtk_widget_show_all(GTK_WIDGET(errorPopup));
	}

	sprintf(label_buffer,"%3.2f°C", temperature_outside);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_outside")), label_buffer);
	sprintf(label_buffer,"%3.2f°C", temperature_wohnzimmer);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_wohnzimmer")), label_buffer);
}

void updateThermostat()
{
	int16_t tempis, tempset, voltage;
	int8_t valve, mode;

	gchar label_buffer[20];
	
	hr20GetStatus(&tempis, &tempset, &valve, &voltage, &mode);

	sprintf(label_buffer,"%3.2f°C", (float)tempis/100.0);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_t_is")), label_buffer);
//	sprintf(label_buffer,"%3.2f°C", (float)tempset/100.0);
//	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_t_set")), label_buffer);
	if((tempset/50-10) >= -1)
		gtk_combo_box_set_active(GTK_COMBO_BOX(glade_xml_get_widget(xml,"combobox_temperature")),tempset/50 - 10);
	sprintf(label_buffer,"%d%%", valve);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_valve")), label_buffer);
	sprintf(label_buffer,"%1.3fV", (float)voltage/1000.0);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_bat")), label_buffer);
	gtk_combo_box_set_active(GTK_COMBO_BOX(glade_xml_get_widget(xml,"combobox_mode")),(gint)mode-1);
}

G_MODULE_EXPORT gint thermostat_set_mode(GtkWidget *widget)
{
	gint mode = (gint)(gtk_combo_box_get_active(GTK_COMBO_BOX(glade_xml_get_widget(xml,"combobox_mode"))))+1;
	setHr20Mode((int)mode);
	return 0;
}

G_MODULE_EXPORT gint thermostat_set_temperature(GtkWidget *widget)
{
	int16_t temperature;

	temperature = (int16_t)(gtk_combo_box_get_active(GTK_COMBO_BOX(glade_xml_get_widget(xml,"combobox_temperature"))) + 10) * 5;


	printf("%d\n",temperature);
	setHr20Temperature(temperature);
	return 0;
}

void updateVoltage()
{
	float voltage;
	gchar label_buffer[20];

	getVoltage(3,&voltage);
	sprintf(label_buffer,"%3.2fV",voltage);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_voltage")), label_buffer);
}
	

void updateRelais()
{
	relaisState = getRelaisState();

	if(relaisState & 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton1")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton1")), 0);
	if(relaisState & 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton2")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton2")), 0);
	if(relaisState & 4)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton3")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton3")), 0);
	if(relaisState & 8)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton4")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton4")), 0);
	if(relaisState & 16)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton5")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton5")), 0);
	if(relaisState & 32)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton6")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton6")), 0);

}

void updateModules()
{
	if(getLedmatrixState())
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"ledmatrix_button")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"ledmatrix_button")), 0);
	if(getScrobblerState())
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"scrobbler_button")), 1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"scrobbler_button")), 0);
}


G_MODULE_EXPORT void updateGraph(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	char date[20];
	char from[13], to[13];
	int view;
	int modul[6], sensor[6], numGraphs=0;
	unsigned int day, month, year;

	if(graph_bo_out)
	{
		modul[numGraphs] = 3;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(graph_bo_wohn)
	{
		modul[numGraphs] = 3;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(graph_oe_out)
	{
		modul[numGraphs] = 4;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(graph_oe_vor)
	{
		modul[numGraphs] = 2;
		sensor[numGraphs] = 0;
		numGraphs++;
	}
	if(graph_oe_rueck)
	{
		modul[numGraphs] = 2;
		sensor[numGraphs] = 1;
		numGraphs++;
	}
	if(graph_oe_wohn)
	{
		modul[numGraphs] = 4;
		sensor[numGraphs] = 1;
		numGraphs++;
	}

	if(!yet_drawed)
	{
		int i;
		gtk_calendar_get_date(GTK_CALENDAR(glade_xml_get_widget(xml,"calendar")),
					&year, &month, &day);
		if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"radio_day"))))
			view = TB_DAY;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"radio_week"))))
			view = TB_WEEK;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"radio_month"))))
			view = TB_MONTH;
		else if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"radio_year"))))
			view = TB_YEAR;

		sprintf(date,"%d-%02d-%02d",year,month+1,day);
		transformDate(from,to,date,view);

		printf("from: %s to: %s view: %d\n",from,to,view);
		initGraph(&graph, from, to);
		for(i=0; i< numGraphs; i++)
		{
			addGraphData(&graph, modul[i], sensor[i]);
		}
		drawGraphGtk(widget, &graph); 
//		drawGraphPng("foo.png", &graph, 2000,800); 
		yet_drawed = 1;
	}
	else
		drawGraphGtk(widget, &graph);
}

G_MODULE_EXPORT void on_button_draw_clicked(GtkButton *button)
{
	freeGraph(&graph);
	yet_drawed = 0;
//	setDrawGraph();
	gtk_widget_queue_draw(glade_xml_get_widget(xml,"drawingarea2"));
	//updateGraph();
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
		gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,slider_name)),hadState.rgbModuleValues[i].red);
		g_sprintf(slider_name,"vscale_green%d",i);
		gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,slider_name)),hadState.rgbModuleValues[i].green);
		g_sprintf(slider_name,"vscale_blue%d",i);
		gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,slider_name)),hadState.rgbModuleValues[i].blue);
		gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness")),smoothness_buf);
	}
	
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

	setRgbValueModul(0x10, (int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red0"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green0"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue0"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness"))))
		    );
	setRgbValueModul(0x11, (int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red1"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green1"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue1"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness"))))
		    );
	setRgbValueModul(0x12, (int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red2"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green2"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue2"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness"))))
		    );

}

G_MODULE_EXPORT void on_button_config_set_clicked(GtkWidget *widget)
{
	int tempint;
	strncpy(config.had_ip,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_had_ip"))), 15);
	config.had_ip[15] = 0;
	config.had_port = atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_had_port"))));
	config.graph_port = atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_port"))));
	strncpy(config.graph_database,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_database"))), 99);
	config.graph_database[99] = 0;
	strncpy(config.graph_user,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_user"))), 99);
	config.graph_user[99] = 0;
	strncpy(config.graph_password,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_password"))), 99);
	config.graph_password[99] = 0;
	strncpy(config.graph_database_ws2000,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_ws2000"))), 99);
	config.graph_database_ws2000[99] = 0;
	strncpy(config.graph_host,gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_host"))), 99);
	config.graph_host[99] = 0;
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_had_activated"))); 
	if(tempint != config.had_activated)
	{
		if(tempint)
		{
			initLibHac(config.had_ip);
			if(config.had_control_activated)
				gtk_widget_show(GTK_WIDGET(glade_xml_get_widget(xml,"hbox4")));
			if(config.thermostat_activated)
				gtk_widget_show(GTK_WIDGET(glade_xml_get_widget(xml,"fixed2")));
		}
		else
		{
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"hbox4")));
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"fixed2")));
			closeLibHac();
		}
		config.had_activated = tempint;
	}
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_graph_activated"))); 
	if(tempint != config.graph_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(glade_xml_get_widget(xml,"vbox2")));
		else
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"vbox2")));
		config.graph_activated = tempint;
	}
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_control_activated"))); 
	if(tempint != config.had_control_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(glade_xml_get_widget(xml,"hbox4")));
		else
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"hbox4")));
		config.had_control_activated = tempint;
	}
	tempint = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_thermostat_activated"))); 
	if(tempint != config.thermostat_activated)
	{
		if(tempint)
			gtk_widget_show(GTK_WIDGET(glade_xml_get_widget(xml,"fixed2")));
		else
			gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"fixed2")));
		config.thermostat_activated = tempint;
	}
}

void updater()
{
	static int counter=10;

	while(1)
	{
		updateRgb();
		updateRelais();
		updateModules();
		updateVoltage();
		updateTemperatures();

		if(counter++ == 10)
		{
			updateThermostat();
			printf("update temperature\n");
			counter=0;
		}
		g_usleep(10000000);
		printf("update\n");
	}
}

G_MODULE_EXPORT void trayIconClicked(GtkWidget *foo, gpointer data)
{
	if(GTK_WIDGET_VISIBLE(widget))
		gtk_widget_hide(GTK_WIDGET(widget));
	else
	{
		gtk_window_deiconify(GTK_WINDOW(widget));
		gtk_widget_show_all(GTK_WIDGET(widget));
	}
}

/*G_MODULE_EXPORT void on_drawingarea1_expose_event(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
	cairo_t *cr;

	cr = gdk_cairo_create(widget->window);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
	cairo_set_font_size(cr, 40.0);
	cairo_move_to(cr, 10.0, 50.0);
	cairo_show_text(cr, "elmo");
	
	cairo_set_source_rgb(cr, 0, 0, 50);
	cairo_move_to(cr, 30.0, 50.0);
	cairo_show_text(cr, "2k3");

	cairo_destroy(cr);

	return FALSE;
}
*/
void trayIconPopup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}

G_MODULE_EXPORT void loadConfigToGui()
{
	gchar entry[100];
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_had_ip")),config.had_ip);
	g_sprintf(entry,"%d",config.had_port);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_had_port")),entry);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_database")),config.graph_database);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_ws2000")),config.graph_database_ws2000);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_host")),config.graph_host);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_user")),config.graph_user);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_password")),config.graph_password);
	g_sprintf(entry,"%d",config.graph_port);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_graph_port")),entry);
	if(config.had_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_had_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_had_activated")),0);
	if(config.graph_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_graph_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_graph_activated")),0);
	if(config.thermostat_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_thermostat_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_thermostat_activated")),0);
	if(config.had_control_activated)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_control_activated")),1);
	else
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_control_activated")),0);
}
	

int main(int argc, char *argv[])
{
	GtkStatusIcon *trayIcon;
	time_t rawtime;
	struct tm *today;

	char *server_ip = getenv("HAD_HOST");
	char *home = getenv("HOME");
	char location[1024];
	sprintf(location,GHAC_CONFIG,home);
	
	loadConfig(location);
	if(!server_ip)
	{
		server_ip = config.had_ip;
	}

	graph_bo_out = 1;
	graph_oe_out = 1;
	
	time(&rawtime);
	today = localtime(&rawtime);

	if(config.had_activated)
		initLibHac(server_ip);

	gtk_init(&argc, &argv);
#ifdef _WIN32
	xml = glade_xml_new("C:\\Programme\\ghac\\ghac.glade", NULL, NULL);
	trayIcon = gtk_status_icon_new_from_file("C:\\Programme\\ghac\\gnome-color-browser.png");
#else
	sprintf(location,"%s/.ghac/ghac.glade",home);
	xml = glade_xml_new(location, NULL, NULL);
	sprintf(location,"%s/.ghac/gnome-color-browser.png",home);
	trayIcon = gtk_status_icon_new_from_file(location);
#endif

	widget = glade_xml_get_widget(xml, "mainWindow");
	errorPopup = glade_xml_get_widget(xml,"errorDialog");
	

	g_signal_connect(widget, "delete-event",
			G_CALLBACK(exit_handler), NULL);

	g_signal_connect(widget, "destroy",
			G_CALLBACK(ghac_end), NULL);
	
	g_signal_connect(trayIcon, "activate", 
			G_CALLBACK(trayIconClicked), NULL);

	
	gtk_status_icon_set_visible(trayIcon, TRUE);
	glade_xml_signal_autoconnect(xml);
	
	gtk_calendar_select_month(GTK_CALENDAR(glade_xml_get_widget(xml,"calendar")),
			today->tm_mon, today->tm_year+1900);
	gtk_calendar_select_day(GTK_CALENDAR(glade_xml_get_widget(xml,"calendar")),
			today->tm_mday);
	
	gtk_widget_show_all(GTK_WIDGET(widget));
	if(!config.had_control_activated)
		gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"hbox4")));
	if(!config.thermostat_activated)
		gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"fixed2")));
	if(!config.graph_activated)
		gtk_widget_hide(GTK_WIDGET(glade_xml_get_widget(xml,"vbox2")));
	loadConfigToGui();
	pthread_create(&update_thread, NULL, (void*)&updater, NULL);
	gtk_main();

	return 0;
}
