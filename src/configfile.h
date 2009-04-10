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
#ifndef __CONFIGFILE_H__
#define __CONFIGFILE_H__

/** @file config.h
 * Config file routines
 */

#define CONFIG_DEFAULT_HAD_ACTIVATED 0
#define CONFIG_DEFAULT_HAD_IP "192.168.0.29"
#define CONFIG_DEFAULT_HAD_PORT 4123
#define CONFIG_DEFAULT_GRAPH_ACTIVATED 1
#define CONFIG_DEFAULT_GRAPH_DATABASE "home_automation"
#define CONFIG_DEFAULT_GRAPH_DATABASE_WS2000 "wetterstation"
#define CONFIG_DEFAULT_GRAPH_HOST "bjoern-b.de"
#define CONFIG_DEFAULT_GRAPH_PORT 3306
#define CONFIG_DEFAULT_GRAPH_USER "weather_read"
#define CONFIG_DEFAULT_GRAPH_PASSWORD "QPiEVtJ/6tLxQ"
#define CONFIG_DEFAULT_THERMOSTAT_ACTIVATED 0
#define CONFIG_DEFAULT_CONTROL_ACTIVATED 0

struct _config
{
	int had_activated;
	char had_ip[16]; 
	int had_port;
	char had_password[128];
	int graph_activated;
	char graph_database[100];
	char graph_database_ws2000[100];
	char graph_host[100];
	int graph_port;
	char graph_user[100];
	char graph_password[100];
	int thermostat_activated;
	int had_control_activated;
	int graph_oe_out;
	int graph_oe_wohn;
	int graph_bo_out;
	int graph_bo_wohn;
	int graph_oe_vor;
	int graph_oe_rueck;
	int graph_bo_hk_soll;
	int graph_bo_hk_ist;
	int graph_bo_hk_ventil;
	int graph_bo_hk_spannung;
	int graph_oe_hk_soll;
	int graph_oe_hk_ist;
	int graph_oe_hk_ventil;
	int graph_oe_hk_spannung;
	int graph_bo_door;
	int graph_bo_window;
	int graph_oe_dachboden;
	int last_graph_width;
	int last_graph_height;
	char last_graph_filename[4096];
}config;

/**
 * @param *conf config file to load
 * @return 0 on failure, 1 on success
 */
extern int loadConfig(char *conf);
extern int saveConfig(char *conf);

/* Default values */

#endif

