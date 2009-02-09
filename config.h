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
#ifndef __CONFIG_H__
#define __CONFIG_H__

/** @file config.h
 * Config file routines
 */

struct _config
{
	int had_activated;
	char had_ip[16]; 
	int had_port;
	int graph_activated;
	char graph_database[100];
	char graph_database_ws2000[100];
	char graph_host[100];
	int graph_port;
	char graph_user[100];
	char graph_password[100];
	int thermostat_activated;
	int had_control_activated;
}config;

/**
 * @param *conf config file to load
 * @return 0 on failure, 1 on success
 */
extern int loadConfig(char *conf);
extern int saveConfig(char *conf);

/* Default values */

#endif

