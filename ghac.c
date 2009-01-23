#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>
#include <pthread.h>
#include <time.h>

#include "ghac.h"
#include "graph_view.h"
#include "libhagraph.h"
#include <libhac/libhac.h>

	
#define SECONDS_PER_DAY (60*60*24)


pthread_t update_thread, network_thread;

GladeXML *xml;

GtkAdjustment *adjusts_rgb[3];

static uint8_t relaisState;
	
GtkWidget *errorPopup;
GtkWidget *widget;

gint exit_handler(GtkWidget *widget, GdkEvent *event, gpointer data)
{
//	closeLibHac();
	return FALSE; // darf beenden
}

G_MODULE_EXPORT void ghac_end(GtkWidget *widget, gpointer daten)
{
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
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"scale_t_set")),(double)tempset/100.0);
	sprintf(label_buffer,"%d%%", valve);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_valve")), label_buffer);
	sprintf(label_buffer,"%1.3fV", (float)voltage/1000.0);
	gtk_label_set_text(GTK_LABEL(glade_xml_get_widget(xml,"label_bat")), label_buffer);
	if(mode == 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"checkbutton_manual")), 1);
}

G_MODULE_EXPORT gint thermostat_set_temperature(GtkWidget *widget)
{
	double temperature;
	int t;

	temperature = gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"scale_t_set"))) * 10 / 5;

	t = (int)temperature*5;
//	printf("t_set = %d\n",t);
	temperature = (double)t/10.0;

	setHr20Temperature(t);
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
	int modul[6], sensor[6], numGraphs=0;

	const gchar *time_from = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_from")));
	const gchar *time_to= gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_to")));

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

	
	gint x,y;
	GdkWindow *window = widget->window;
	gdk_window_get_geometry(window,0,0,&x,&y,0);
#ifdef _DEBUG	
	printf("Width: %d Height: %d\n",x,y);
#endif
	createGraph2(widget, x,y, time_from, time_to, (int*)&modul, (int*)&sensor, numGraphs);	
}

G_MODULE_EXPORT void on_button_draw_clicked(GtkButton *button)
{
	setDrawGraph();
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

void updater()
{
	static int counter=600;

	while(1)
	{
		updateRgb();
		updateRelais();
		updateModules();
		updateTemperatures();
		updateVoltage();
		updateThermostat();

		if(counter++ == 600)
		{
#ifdef _WIN32
#endif
			//updateGraph();
			counter=0;
		}
		gtk_widget_queue_draw(glade_xml_get_widget(xml,"scrobbler_button"));
#ifdef _WIN32
		g_usleep(5000000);
#else
		sleep(10);
#endif
	}
}

G_MODULE_EXPORT void trayIconClicked(GtkWidget *foo, gpointer data)
{
	if(GTK_WIDGET_VISIBLE(widget))
		gtk_widget_hide(GTK_WIDGET(widget));
	else
		gtk_widget_show_all(GTK_WIDGET(widget));
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


int main(int argc, char *argv[])
{
	GtkStatusIcon *trayIcon;
	time_t rawtime;
	struct tm *today;

	char *server_ip = getenv("HAD_HOST");

#ifdef _WIN32
	if(!server_ip)
		server_ip = "192.168.0.29";
#else
	if(!server_ip)
		server_ip = "192.168.0.2";
#endif

	gchar time_from[11], time_to[11];

	graph_bo_out = 1;
	graph_oe_out = 1;
	
	time(&rawtime);
	today = localtime(&rawtime);
	strftime (time_from,255,"%Y-%m-%d",today);
	rawtime += SECONDS_PER_DAY; // jetzt ist morgen heute
	today = localtime(&rawtime);
	strftime (time_to,255,"%Y-%m-%d",today);
	
	initLibHac(server_ip);

	gtk_init(&argc, &argv);
#ifdef _WIN32
	xml = glade_xml_new("C:\\Programme\\ghac\\ghac.glade", NULL, NULL);
	trayIcon = gtk_status_icon_new_from_file("C:\\Programme\\ghac\\gnome-color-browser.png");
#else
	xml = glade_xml_new("/home/bjoern/Projekte/home-automation/ghac/glade/ghac.glade", NULL, NULL);
	trayIcon = gtk_status_icon_new_from_file("/usr/share/pixmaps/gnome-color-browser.png");
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

	
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_from")),time_from);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_to")),time_to);


	gtk_widget_show_all(GTK_WIDGET(widget));

	pthread_create(&update_thread, NULL, (void*)&updater, NULL);

	gtk_main();

	return 0;
}
