#include <stdlib.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>
#include <pthread.h>
#include <time.h>

#include "ghac.h"
#include "graph_view.h"
#include <libhac/libhac.h>
#include <libhagraph/libhagraph.h>
	
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

void ghac_end(GtkWidget *widget, gpointer daten)
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

	if(getRelaisState((uint8_t*)&relaisState) < 0)
	{
		gtk_widget_show_all(GTK_WIDGET(errorPopup));
//		gtk_main();
	}

	if(relaisState & 1)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton1")), 1);
	if(relaisState & 2)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton2")), 1);
	if(relaisState & 4)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton3")), 1);
	if(relaisState & 8)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton4")), 1);
	if(relaisState & 16)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton5")), 1);
	if(relaisState & 32)
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(glade_xml_get_widget(xml,"relaisbutton6")), 1);

}

void updateGraph()
{
	int modul[6], sensor[6], numGraphs=0;

	gchar *time_from,
	      *time_to;

	const char *tmpfilename = "tempgraph.png";

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

	time_from = gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_from")));
	time_to= gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_to")));

	createGraph(tmpfilename, 800, 400, time_from, time_to, (int*)&modul, (int*)&sensor, numGraphs);
	
	gtk_image_set_from_file(GTK_IMAGE(glade_xml_get_widget(xml,"image_graph")), tmpfilename);
	
	unlink(tmpfilename);

}

void on_button_draw_clicked(GtkButton *button)
{
	updateGraph();
}

static gboolean updateRgb()
{
	gint red, green, blue;
	gint smoothness;

	gchar smoothness_buf[2];

	if((getRgbValues(&red, &green, &blue, &smoothness) < 0))
	{
		gtk_widget_show_all(GTK_WIDGET(errorPopup));
//		gtk_main();
	}
	g_sprintf(smoothness_buf,"%d",smoothness);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red")),red);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green")),green);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue")),blue);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness")),smoothness_buf);
	
	return TRUE;
}


void on_relaisbutton1_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 1;
	else
		relaisState &= ~1;

	setRelais(relaisState);
}

void on_relaisbutton2_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 2;
	else
		relaisState &= ~2;

	setRelais(relaisState);
}

void on_relaisbutton3_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 4;
	else
		relaisState &= ~4;

	setRelais(relaisState);
}

void on_relaisbutton4_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 8;
	else
		relaisState &= ~8;

	setRelais(relaisState);
}

void on_relaisbutton5_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 16;
	else
		relaisState &= ~16;

	setRelais(relaisState);
}

void on_relaisbutton6_toggled(GtkToggleButton *toggle_button)
{
	if(gtk_toggle_button_get_active(toggle_button))
		relaisState |= 32;
	else
		relaisState &= ~32;

	setRelais(relaisState);
}

void on_button_send_clicked(GtkWidget *widget)
{
	gint rgb_modul;
	gint smoothness;
	
	rgb_modul = 3;
	smoothness = 5;

	setRgbValues( (int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue"))),
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
		updateTemperatures();
		updateVoltage();
		if(counter++ == 600)
		{
			updateGraph();
			counter=0;
		}
		sleep(1);
	}
}

void trayIconClicked(GtkWidget *foo, gpointer data)
{
	if(GTK_WIDGET_VISIBLE(widget))
		gtk_widget_hide(GTK_WIDGET(widget));
	else
		gtk_widget_show_all(GTK_WIDGET(widget));
}

void trayIconPopup(GtkStatusIcon *status_icon, guint button, guint32 activate_time, gpointer popUpMenu)
{
	gtk_menu_popup(GTK_MENU(popUpMenu), NULL, NULL, gtk_status_icon_position_menu, status_icon, button, activate_time);
}


int main(int argc, char *argv[])
{
	GtkStatusIcon *trayIcon;
	time_t rawtime;
	struct tm *today;

	gchar time_from[11], time_to[11];

	graph_bo_out = 1;
	graph_bo_wohn = 1;
	
	
	time(&rawtime);
	today = gmtime(&rawtime);
	strftime (time_from,255,"%Y-%m-%d",today);
	rawtime += SECONDS_PER_DAY; // jetzt ist morgen heute
	today = gmtime(&rawtime);
	strftime (time_to,255,"%Y-%m-%d",today);
	
	initLibHac("192.168.0.2");

	gtk_init(&argc, &argv);

	xml = glade_xml_new("/home/bjoern/Projekte/home-automation/ghac/glade/ghac.glade", NULL, NULL);
	trayIcon = gtk_status_icon_new_from_file("/usr/share/icons/crystalsvg/64x64/apps/colors.png");

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
