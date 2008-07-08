#include <stdlib.h>
#include <gtk/gtk.h>
#include <glade/glade.h>
#include <glib/gprintf.h>

#include "main.h"
	
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
	gchar program[255];
	
	rgb_modul = 3;
	smoothness = 5;


	g_sprintf(program,"%s %s %d %d %d %d %d",
			HAC,
			HAD_HOST,
			1,
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness"))))
		 );
#ifdef _DEBUG
	g_printf("Executing: %s\n",program);
#endif

	system(program);
	
	g_sprintf(program,"%s %s %d %d %d %d %d",
			HAC,
			HAD_HOST,
			3,
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_red"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_green"))),
			(int)gtk_range_get_value(GTK_RANGE(glade_xml_get_widget(xml,"vscale_blue"))),
			atoi(gtk_entry_get_text(GTK_ENTRY(glade_xml_get_widget(xml,"entry_smoothness"))))
		);

#ifdef _DEBUG
	g_printf("Executing: %s\n",program);
#endif

	system(program);

}


int main(int argc, char *argv[])
{
	GtkWidget *widget;

	gtk_init(&argc, &argv);
	xml = glade_xml_new("glade_test/test.glade", NULL, NULL);


	widget = glade_xml_get_widget(xml, "window1");

	glade_xml_signal_autoconnect(xml);

	g_signal_connect(widget, "delete-event",
			G_CALLBACK(exit_handler), NULL);

	g_signal_connect(widget, "destroy",
			G_CALLBACK(ghac_end), NULL);

	gtk_widget_show_all(GTK_WIDGET(widget));

	gtk_main();

	return 0;
}
