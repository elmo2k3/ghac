#include <stdlib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>

#include "main.h"
#include <libhac.h>
	
GladeXML *xml;

GtkAdjustment *adjusts_rgb[3];

gint exit_handler(GtkWidget *widget, GdkEvent *event, gpointer data)
{
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


int main(int argc, char *argv[])
{
	GtkWidget *widget;

	gint red, green, blue;
	gint smoothness;

	gchar smoothness_buf[2];

	gtk_init(&argc, &argv);
	xml = glade_xml_new("/home/bjoern/Projekte_git/home-automation/ghac/glade/ghac.glade", NULL, NULL);

	
	widget = glade_xml_get_widget(xml, "window1");

	glade_xml_signal_autoconnect(xml);

	g_signal_connect(widget, "delete-event",
			G_CALLBACK(exit_handler), NULL);

	g_signal_connect(widget, "destroy",
			G_CALLBACK(ghac_end), NULL);


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
