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

/*!
* \file	config.c
* \brief	config file handling
* \author	Bjoern Biesenbach <bjoern at bjoern-b dot de>
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "config.h"

#define NUM_PARAMS 12
static char *config_params[NUM_PARAMS] = { "had_activated","had_ip","had_port","graph_activated",
		"graph_database","graph_ws2000","graph_host","graph_port","graph_user","graph_password",
		"thermostat_activated","had_control_activated"};

int saveConfig(char *conf)
{
	FILE *config_file = fopen(conf,"w");
	if(!config_file)
		return 0;
	fprintf(config_file,"had_activated = %d\r\n",config.had_activated);
	fprintf(config_file,"had_ip = %s\r\n",config.had_ip);
	fprintf(config_file,"had_port = %d\r\n",config.had_port);
	fprintf(config_file,"had_control_activated = %d\r\n",config.had_control_activated);
	fprintf(config_file,"graph_activated = %d\r\n",config.graph_activated);
	fprintf(config_file,"graph_database = %s\r\n",config.graph_database);
	fprintf(config_file,"graph_ws2000 = %s\r\n",config.graph_database_ws2000);
	fprintf(config_file,"graph_host = %s\r\n",config.graph_host);
	fprintf(config_file,"graph_port = %d\r\n",config.graph_port);
	fprintf(config_file,"graph_user = %s\r\n",config.graph_user);
	fprintf(config_file,"graph_password = %s\r\n",config.graph_password);
	fprintf(config_file,"thermostat_activated = %d\r\n",config.thermostat_activated);

	fclose(config_file);
	return 1;
}

int loadConfig(char *conf)
{
	FILE *config_file;
	char line[120];
	char value[100];
	char *lpos;
	int param;

	config_file = fopen(conf,"r");
	if(!config_file)
	{
		return 0;
	}

	/* set everything to zero */
	memset(&config, 0, sizeof(config));
	
	/* default values */

	/* step through every line */
	while(fgets(line, sizeof(line), config_file) != NULL)
	{
		/* skip comments and empty lines */
		if(line[0] == '#' || line[0] == '\n' || line[0] == '\r')
			continue;
		for(param = 0; param < NUM_PARAMS; param++)
		{
			/* if param name not at the beginning of line */
			if(strstr(line,config_params[param]) != line)
				continue;
			/* go beyond the = */
			if(!(lpos =  strstr(line, "=")))
				continue;
			/* go to the beginning of value 
			 * only whitespaces are skipped, no tabs */
			do
				lpos++;
			while(*lpos == ' ');
			
			strcpy(value, lpos);

			/* throw away carriage return 
			 * might only work for *nix */
			lpos = strchr(value,'\n');
			*lpos = 0;
			if((lpos = strchr(value,'\r')))
				*lpos = 0;

			/* put the value where it belongs */
			switch(param)
			{
				/* had activated */
				case 0: config.had_activated = atoi(value);
					break;
				/* had ip */
				case 1: strcpy(config.had_ip, value);
					break;
				/* had port */
				case 2: config.had_port = atoi(value);
					break;
				/* graph activated */
				case 3: config.graph_activated = atoi(value);
					break;
				/* graph mysql database */
				case 4: strcpy(config.graph_database, value);
					printf("Database = %s\n",value);
					break;
				/* graph mysql database ws2000 */
				case 5: strcpy(config.graph_database_ws2000, value);
					printf("Database ws2000 = %s\n",value);
					break;
				/* graph mysql host */
				case 6: strcpy(config.graph_host, value);
					break;
				/* graph mysql port */
				case 7: config.graph_port = atoi(value);
					break;
				/* graph mysql user */
				case 8: strcpy(config.graph_user, value);
					break;
				/* graph mysql pass */
				case 9: strcpy(config.graph_password, value);
					break;
				/* thermostat activated */
				case 10: config.thermostat_activated = atoi(value);
					break;
				/* had control activated */
				case 11: config.had_control_activated = atoi(value);
					break;
			}
		}
	}

	fclose(config_file);
	return 1;
}

