#ifndef __HAGRAPH_H__
#define __HAGRAPH_H__

#include <gtk/gtk.h>

void setDrawGraph(void);

int createGraph(GtkWidget *widget, int width, int heigth, const char *time_from,
		const char *time_to, int *modul, int *sensor, int numGraphs);


#endif

