/* Bjoern Biesenbach <bjoern@bjoern-b.de>
 */


#include <stdio.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#include <cairo/cairo.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <unistd.h>

#include "libhagraph.h"

#define MYSQL_SERVER    "192.168.2.1"
#define MYSQL_USER      "home_automation"
#define MYSQL_PASS      "rfm12"
#define MYSQL_DB        "home_automation"
#define MYSQL_DB_WS2000	"wetterstation"

#define IMG_WIDTH_STD 800
#define IMG_HEIGHT_STD 600

#define X1_SKIP 40
#define X2_SKIP 40
#define Y1_SKIP 40
#define Y2_SKIP 40
#define X1_TO_TEXT 25
#define X1_TO_TEXT2 5
#define Y1_TO_TEXT 20
#define TICK_OFFSET 10

#define DRAW_VERTICAL_GRID
#define DRAW_HORIZONTAL_GRID

#define SECONDS_PER_DAY (60*60*24)
#define SECONDS_PER_WEEK (SECONDS_PER_DAY*7)
#define SECONDS_PER_MONTH (SECONDS_PER_DAY*31)
#define SECONDS_PER_YEAR (SECONDS_PER_DAY*366)
	
#define WIDTH_FOR_ONE_HOUR ((IMG_WIDTH-X1_SKIP-X2_SKIP)/24)
#define WIDTH_FOR_ONE_DAY_IN_WEEK ((IMG_WIDTH-X1_SKIP-X2_SKIP)/7)
#define WIDTH_FOR_ONE_DAY_IN_MONTH ((IMG_WIDTH-X1_SKIP-X2_SKIP)/31)
#define WIDTH_FOR_ONE_DAY_IN_YEAR ((IMG_WIDTH-X1_SKIP-X2_SKIP)/366)

#define TB_DAY 1
#define TB_WEEK 2
#define TB_MONTH 3
#define TB_YEAR 4

struct _graph_data
{
	double **points; /**< will be the huge array holding the xy-points */
	long int num_points;
	float max;
	float min;
}*pgraph_data;

int num_graphs;

static int IMG_WIDTH, IMG_HEIGHT;

static void drawXLegend(cairo_t *cr, char timebase, int color, unsigned char *title);
static void getMaxMinValues(MYSQL *mysql_connection, const char *time_from, const char *time_to, float *max, int *sec_max, float *min, int modul, int sensor);
static int transformY(float temperature, float max, float min);
static void addGraph(cairo_t *cr, MYSQL *mysql_connection, int color, const char *time_from, const char *time_to, char timebase, int modul, int sensor, float temp_max, float temp_min);
static void drawYLegend(cairo_t *cr, float temp_max, float temp_min, int color);
static int decideView(char *time_from, char *time_to);

int createGraph(GtkWidget *widget, int width, int heigth, const char *time_from,
		const char *time_to, int *modul, int *sensor, int numGraphs)
{
	IMG_HEIGHT = heigth;
	IMG_WIDTH = width;

	cairo_t *cr;

	cr = gdk_cairo_create(widget->window);

	int white, black, red, green, blue, purple, orange;
	int sec_max;
	int c;
	float temp_max = 0.0,
	      temp_min = 0.0;

	int view;
	
	view = decideView(time_from, time_to);

	view = TB_DAY;
	MYSQL *mysql_connection;

	mysql_connection = mysql_init(NULL);
	if (!mysql_real_connect(mysql_connection, MYSQL_SERVER, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_connection));
		return -1;
	}
	mysql_connection->reconnect=1;
	
	
//	int colors[]={red,blue,green,purple,orange};

	for(c=0; c< numGraphs; c++)
	{
		getMaxMinValues(mysql_connection, time_from, time_to, &temp_max, &sec_max, &temp_min, modul[c],sensor[c]);
	}
	
	temp_max = ceil(temp_max/10)*10;
	temp_min = floor(temp_min/10)*10;
	
	for(c=0;c < numGraphs;c++)
	{
		addGraph(cr, mysql_connection, 0, time_from, time_to, view, modul[c], sensor[c], temp_max, temp_min);
	}
	
	drawXLegend(cr, view, black, (unsigned char*)time_from);
	drawYLegend(cr, temp_max, temp_min, black);
	
	cairo_destroy(cr);

	mysql_close(mysql_connection);
	return 0;
}

int createGraphPng(const char *filename, int width, int heigth, const char *time_from,
		const char *time_to, int *modul, int *sensor, int numGraphs)
{
	IMG_HEIGHT = heigth;
	IMG_WIDTH = width;

	cairo_surface_t *surface;
	cairo_t *cr;

	surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, IMG_WIDTH, IMG_HEIGHT);
	
	cr = cairo_create(surface);

	int white, black, red, green, blue, purple, orange;
	int sec_max;
	int c;
	float temp_max = 0.0,
	      temp_min = 0.0;

	int view;
	
	view = decideView(time_from, time_to);

	view = TB_DAY;
	MYSQL *mysql_connection;

	mysql_connection = mysql_init(NULL);
	if (!mysql_real_connect(mysql_connection, MYSQL_SERVER, MYSQL_USER, MYSQL_PASS, MYSQL_DB, 0, NULL, 0))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_connection));
		return -1;
	}
	mysql_connection->reconnect=1;
	
	
//	int colors[]={red,blue,green,purple,orange};

	for(c=0; c< numGraphs; c++)
	{
		getMaxMinValues(mysql_connection, time_from, time_to, &temp_max, &sec_max, &temp_min, modul[c],sensor[c]);
	}
	
	temp_max = ceil(temp_max/10)*10;
	temp_min = floor(temp_min/10)*10;
	
	for(c=0;c < numGraphs;c++)
	{
		addGraph(cr, mysql_connection, 0, time_from, time_to, view, modul[c], sensor[c], temp_max, temp_min);
	}
	
	drawXLegend(cr, view, black, (unsigned char*)time_from);
	drawYLegend(cr, temp_max, temp_min, black);
	
	cairo_surface_write_to_png(surface, "test.png");

	cairo_destroy(cr);
	cairo_surface_destroy(surface);

	mysql_close(mysql_connection);
	return 0;
}


/* 
 * X-Achse zeichnen
 * Möglichkeiten für timebase: TB_DAY, TB_WEEK, TB_MONTH, TB_YEAR
 * 
 */
static void drawXLegend(cairo_t *cr, char timebase, int color, unsigned char *title)
{
	int width;
	int i,p;
	char time[200];
	
	cairo_set_line_width(cr, 2);
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_move_to(cr, X1_SKIP-5, IMG_HEIGHT-Y1_SKIP);
	cairo_line_to(cr, IMG_WIDTH-X2_SKIP+5, IMG_HEIGHT-Y1_SKIP);
	cairo_stroke(cr);

	switch(timebase)
	{
		case TB_DAY: 	if(IMG_WIDTH<2000)
				{
					width = WIDTH_FOR_ONE_HOUR*2;i=0,p=13;
				}
				else
				{
					width = WIDTH_FOR_ONE_HOUR; i=0; p=25;
				}
				cairo_move_to(cr, IMG_WIDTH/2,5);
				cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
				cairo_set_font_size(cr, 9.0);
				cairo_show_text(cr, title);
	//			gdImageString(im,gdFontGetLarge(), IMG_WIDTH/2, 5, title,color); 
				break;
		case TB_WEEK: 	width = WIDTH_FOR_ONE_DAY_IN_WEEK; i=0; p=8; break;
		case TB_MONTH: 	width = WIDTH_FOR_ONE_DAY_IN_MONTH; i=0; p=32; break;
		case TB_YEAR: 	width = WIDTH_FOR_ONE_DAY_IN_YEAR; i=0; p=367; break;
	}
		
	for(;i<p;i++)
	{
#ifdef DRAW_VERTICAL_GRID
//		cairo_move_to(cr, i*width+X1_SKIP, Y2_SKIP);
//		cairo_line_to(cr, i*width+X1_SKIP, IMG_HEIGHT-Y1_SKIP+TICK_OFFSET);	
//		gdImageDashedLine(im, i*width+X1_SKIP, Y2_SKIP, i*width+X1_SKIP, IMG_HEIGHT-Y1_SKIP+TICK_OFFSET, color);
#endif
		cairo_set_line_width(cr, 1);
		cairo_move_to(cr, i*width+X1_SKIP, Y2_SKIP);
		cairo_line_to(cr, i*width+X1_SKIP, IMG_HEIGHT-Y1_SKIP+TICK_OFFSET);	
		cairo_stroke(cr);
		cairo_set_line_width(cr, 1);
//		gdImageLine(im, i*width+X1_SKIP,IMG_HEIGHT-Y1_SKIP, i*width+X1_SKIP, IMG_HEIGHT -Y1_SKIP, color);
		switch(timebase)
		{
			case TB_DAY: 	if(IMG_WIDTH<2000)
							sprintf(time,"%02d:00:00",i*2); 
					else
							sprintf(time,"%02d:00:00",i);
							break;
			case TB_WEEK: 	if(i<7) sprintf(time,"%d",i+1); else strcpy(time,"\0");  break;
			case TB_MONTH: 	if(i<31) sprintf(time,"%d",i+1); else strcpy(time,"\0"); break;
			case TB_YEAR: 	sprintf(time,"%d",i+1); break;
		}
		cairo_move_to(cr, i*width+X1_SKIP-X1_TO_TEXT2, IMG_HEIGHT - Y1_TO_TEXT);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, 9.0);
		cairo_show_text(cr, time);
//		gdImageString(im,gdFontGetSmall(), i*width+X1_SKIP-X1_TO_TEXT2,IMG_HEIGHT-Y1_TO_TEXT, time,color);
	}
	
}

static void drawYLegend(cairo_t *cr, float temp_max, float temp_min, int color)
{
	float range = temp_max - temp_min;
	int one_degree_height = (IMG_HEIGHT-Y1_SKIP-Y2_SKIP)/10;
	int i;
	char tstring[10];
	
	static const double dash[] = {1.0};

	for(i=0;i<10;i++)
	{
#ifdef DRAW_HORIZONTAL_GRID
		//gdImageDashedLine(im,X1_SKIP-TICK_OFFSET,one_degree_height*i+Y2_SKIP ,IMG_WIDTH-X2_SKIP,one_degree_height*i+Y2_SKIP ,color);
#endif
//		cairo_set_dash(cr, dash, 1, 0);
		cairo_move_to(cr, X1_SKIP-TICK_OFFSET, one_degree_height*i+Y2_SKIP);
		cairo_line_to(cr, IMG_WIDTH-X2_SKIP ,one_degree_height*i+Y2_SKIP);
		cairo_stroke(cr);

		
		sprintf(tstring,"%d",(int)(temp_max-(range/10)*i));
		cairo_move_to(cr ,X1_SKIP-X1_TO_TEXT, one_degree_height*i+Y2_SKIP);
		cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_size(cr, 9.0);
		cairo_show_text(cr, tstring);
//		gdImageString(im,gdFontGetSmall(), X1_SKIP-X1_TO_TEXT,one_degree_height*i+Y2_SKIP, tstring,color);
	}
}

static void addGraph(cairo_t *cr, MYSQL *mysql_connection, int color, const char *time_from, const char *time_to, char timebase, int modul, int sensor, float temp_max, float temp_min)
{
	char query[255];
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	MYSQL *mysql_helper_connection;
	
	float seconds[2];		// vorgänger und aktueller wert
	int day_of_week, day_of_month, day_of_year;
	float temperature[2];
	float x_div;
	int x1,y1,x2,y2;

	static int line_color = 0;
	
	mysql_helper_connection = mysql_connection;
	if(modul==4)
	{
		mysql_connection = mysql_init(NULL);
		if (!mysql_real_connect(mysql_connection, MYSQL_SERVER, MYSQL_USER, MYSQL_PASS, MYSQL_DB_WS2000, 0, NULL, 0))
		{
			fprintf(stderr, "%s\n", mysql_error(mysql_connection));
			exit(0);
		}
		sprintf(query,"SELECT TIME_TO_SEC(time), DAYOFWEEK(date), DAYOFMONTH(date), DAYOFYEAR(date), T_1 FROM sensor_1_8 WHERE date>='%s' AND date<'%s' AND ok_1='0' ORDER BY date,time asc",time_from, time_to);
	}
	else
		sprintf(query,"SELECT TIME_TO_SEC(CONVERT_TZ(date,'UTC','MET')), DAYOFWEEK(date), DAYOFMONTH(date), DAYOFYEAR(date), temperature FROM temperatures WHERE modul_id='%d' AND sensor_id='%d' AND CONVERT_TZ(date,'UTC','MET')>'%s' AND date<'%s' ORDER BY date asc", modul, sensor, time_from, time_to);
	if(mysql_query(mysql_connection,query))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_connection));
		exit(0);
	}
	mysql_res = mysql_use_result(mysql_connection);
	int i=0;
	
	cairo_set_line_width(cr, 1);

	if(line_color == 0)
	{
		cairo_set_source_rgb(cr, 0, 0, 255); //blue
		line_color = 1;
	}
	else if(line_color == 1)
	{
		cairo_set_source_rgb(cr, 0, 255, 0); //green
		line_color = 2;
	}
	else if(line_color == 2)
	{
		cairo_set_source_rgb(cr, 255, 0, 0); //red
		line_color = 3;
	}
	else if(line_color == 3)
	{
		cairo_set_source_rgb(cr, 255, 0, 255); //violett
		line_color = 4;
	}

	while((mysql_row = mysql_fetch_row(mysql_res)))
	{
		
		if(!mysql_row)
		{	
			fprintf(stderr, "%s\n", mysql_error(mysql_connection));
			exit(0);
		}
		
		if(mysql_row[0]) seconds[1]	= atoi(mysql_row[0]);
		else seconds[1]	= 0;
		
		if(strcmp(mysql_row[1],"0.0")) temperature[1] = atof(mysql_row[4]);
		else temperature[1] = 0;
		
		day_of_week = atoi(mysql_row[1]) -2;	// MYSQL gibt Sonntag = 1... zurück
		if(day_of_week == -1) day_of_week = 6;		// jetzt Montag=1, Sonntag=7
		day_of_month = atoi(mysql_row[2]) -1;
		day_of_year = atoi(mysql_row[3]) -1;
	
		switch(timebase)
		{
			case TB_DAY: 	x_div = SECONDS_PER_DAY; break;
			case TB_WEEK: 	x_div = SECONDS_PER_WEEK;
							seconds[1] += SECONDS_PER_DAY*day_of_week;
							break;
			case TB_MONTH: 	x_div = SECONDS_PER_MONTH;
							seconds[1] += SECONDS_PER_DAY*day_of_month;
							break; 
			case TB_YEAR: 	x_div = SECONDS_PER_YEAR;
							seconds[1] += SECONDS_PER_DAY*day_of_year;
							break; 
		}
		if(i) // erst ab dem zweiten Durchlauf
		{
			x1 = seconds[0]/x_div*(IMG_WIDTH-X1_SKIP-X2_SKIP)+X1_SKIP;
			x2 = seconds[1]/x_div*(IMG_WIDTH-X1_SKIP-X2_SKIP)+X1_SKIP;
			y1 = transformY(temperature[0],temp_max,temp_min);
			y2 = transformY(temperature[1],temp_max,temp_min);
			cairo_move_to(cr, x1, y1);
			cairo_line_to(cr, x2, y2);
			cairo_stroke(cr);
//			gdImageLine(im,x1,y1,x2,y2,color);
		}
		i++;
		temperature[0]=temperature[1];
		seconds[0]=seconds[1];
	}
	if(modul==4)
		mysql_close(mysql_connection);
	mysql_connection = mysql_helper_connection;

}

/* 
 * Gives back the absolute position for the temperature in the picture
 */
static int transformY(float temperature, float max, float min)
{
	const float range = max - min;
	return (1-((temperature-min)/range))*(IMG_HEIGHT-80)+40;
}

static void getMaxMinValues(MYSQL *mysql_connection, const char *time_from, const char *time_to, float *max, int *sec_max, float *min, int modul, int sensor)
{
	MYSQL *mysql_helper_connection;
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	float t_max, s_max, t_min, s_min;
	char query[255];
	
	mysql_helper_connection = mysql_connection;
	if(modul==4)
	{
		mysql_connection = mysql_init(NULL);
		if (!mysql_real_connect(mysql_connection, MYSQL_SERVER, MYSQL_USER, MYSQL_PASS, MYSQL_DB_WS2000, 0, NULL, 0))
		{
			fprintf(stderr, "%s\n", mysql_error(mysql_connection));
			exit(0);
		}
		sprintf(query,"SELECT TIME_TO_SEC(time), T_1 FROM sensor_1_8 WHERE date>='%s' AND date<'%s' AND ok_1='0' ORDER BY T_1 desc LIMIT 1", time_from, time_to);
	}
	else
		sprintf(query,"SELECT TIME_TO_SEC(date), temperature FROM temperatures WHERE modul_id='%d' AND sensor_id='%d' AND date>'%s' AND date<'%s' AND temperature!='85.0' ORDER BY temperature desc LIMIT 1", modul, sensor, time_from, time_to);
		
	if(mysql_query(mysql_connection,query))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_connection));
		exit(0);
	}
	mysql_res = mysql_use_result(mysql_connection);
	if(!(mysql_row = mysql_fetch_row(mysql_res)))
		return;
	if(mysql_row[0]) s_max = atoi(mysql_row[0]);
	else s_max = 0;
	if(mysql_row[1]) t_max = atof(mysql_row[1]);
	else t_max = 0;
	mysql_free_result(mysql_res);
	if(modul==4)
		sprintf(query,"SELECT TIME_TO_SEC(time), T_1 FROM sensor_1_8 WHERE date>='%s' AND date<'%s' AND ok_1='0' ORDER BY T_1 asc LIMIT 1", time_from,  time_to);
	else
		sprintf(query,"SELECT TIME_TO_SEC(date), temperature FROM temperatures WHERE modul_id='%d' AND sensor_id='%d' AND date>'%s' AND date<'%s' ORDER BY temperature asc LIMIT 1", modul, sensor, time_from, time_to);
	if(mysql_query(mysql_connection,query))
	{
		fprintf(stderr, "%s\n", mysql_error(mysql_connection));
		exit(0);
	}
	mysql_res = mysql_use_result(mysql_connection);
	mysql_row = mysql_fetch_row(mysql_res);
	if(mysql_row[0]) s_max = atoi(mysql_row[0]);
	else s_min = 0;
	if(mysql_row[1]) t_min = atof(mysql_row[1]);
	else t_min = 0;
	mysql_free_result(mysql_res);
	//printf("%f %f \n",t_max,t_min);
	if(*max == 0.0 && *min == 0.0)
	{
		*max = t_max;
		*min = t_min;
	}
	else
	{
		if(t_max > *max) *max = t_max;
		if(t_min < *min) *min = t_min;
	}
	if(modul==4)
		mysql_close(mysql_connection);
	mysql_connection = mysql_helper_connection; // wieder herstellen
}
static int decideView(char *time_from, char *time_to)
{
	char c_from[255], c_to[255];
	struct tm from, to;
	
	strcpy(c_from,time_from);
	strcpy(c_to,time_to);
	
	from.tm_year = atoi(strtok(c_from,"-")) -1900;
	from.tm_mon = atoi(strtok(NULL,"-")) - 1;
	from.tm_mday = atoi(strtok(NULL,"-"));
	from.tm_hour = 0;
	from.tm_min = 0;
	from.tm_sec = 0;
	
	to.tm_year = atoi(strtok(c_to,"-")) -1900;
	to.tm_mon = atoi(strtok(NULL,"-")) - 1;
	to.tm_mday = atoi(strtok(NULL,"-"));
	to.tm_hour = 0;
	to.tm_min = 0;
	to.tm_sec = 0;
	
	if(mktime(&to)-mktime(&from) <= SECONDS_PER_DAY)
		return TB_DAY;
	else if(mktime(&to)-mktime(&from) <= SECONDS_PER_WEEK)
		return TB_WEEK;
	else if(mktime(&to)-mktime(&from) <= SECONDS_PER_MONTH)
		return TB_MONTH;
	else
		return TB_YEAR;
}
