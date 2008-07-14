#include <stdlib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>

#include "ghac.h"
#include <libhac.h>
	
GladeXML *xml;

GtkAdjustment *adjusts_rgb[3];

gint exit_handler(GtkWidget *widget, GdkEvent *event, gpointer data)
{
//	closeLibHac();
	return FALSE; // darf beenden
}

void ghac_end(GtkWidget *widget, gpointer daten)
{
	gtk_main_quit();
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
GtkWidget *widget;

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
	GtkWidget *errorPopup;

	gint red, green, blue;
	gint smoothness;

	gchar smoothness_buf[2];


	gtk_init(&argc, &argv);

	xml = glade_xml_new("/home/bjoern/Projekte_git/home-automation/ghac/glade/ghac.glade", NULL, NULL);
	trayIcon = gtk_status_icon_new_from_file("/usr/share/icons/crystalsvg/64x64/apps/colors.png");

	widget = glade_xml_get_widget(xml, "mainWindow");
	errorPopup = glade_xml_get_widget(xml,"errorDialog");

	glade_xml_signal_autoconnect(xml);

	g_signal_connect(widget, "delete-event",
			G_CALLBACK(exit_handler), NULL);

	g_signal_connect(widget, "destroy",
			G_CALLBACK(ghac_end), NULL);
	
	g_signal_connect(trayIcon, "activate", 
			G_CALLBACK(trayIconClicked), NULL);

//	if(initLibHac(HAD_HOST) < 0)
//	{
//		gtk_widget_show_all(GTK_WIDGET(errorPopup));
//		gtk_main();
//	}

	gtk_status_icon_set_visible(trayIcon, TRUE);



	getRgbValues(&red, &green, &blue, &smoothness);
	g_sprintf(smoothness_buf,"%d",smoothness);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red")),red);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green")),green);
	gtk_range_set_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue")),blue);
	gtk_entry_set_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness")),smoothness_buf);

	gtk_widget_show_all(GTK_WIDGET(widget));
	gtk_main();

	return 0;
}
