#ifndef __HAGRAPH_H__
#define __HAGRAPH_H__

#include <gtk/gtk.h>

extern int createGraph(GtkWidget *widget, int width, int heigth, const char *time_from,
		const char *time_to, int *modul, int *sensor, int numGraphs);

extern int createGraphPng(const char *filename, int width, int heigth, const char *time_from,
		const char *time_to, int *modul, int *sensor, int numGraphs);


#endif

