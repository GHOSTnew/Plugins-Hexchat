/*
 * Copyright (c) 2014 GHOSTnew
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "hexchat-plugin.h"

#define PNAME "AntiPV"
#define PDESC "simple anti PV plugin"
#define PVERSION "1.0"

static hexchat_plugin *ph;      /* plugin handle */
static int enable = 0;

void strUpper(char * str) {
	int i = 0;
	for (i = 0; i < sizeof(str); i++) {
		str[i] = (char)toupper((int)str[i]);
	}
}

static int private_message_handler (char *word[], void *userdata) {
	if (enable) {
		hexchat_commandf (ph, "PRIVMSG %s :[AntiPV] Sorry, but I'm in AntiPV mode", word[1]);
	}
	return HEXCHAT_EAT_NONE;
}

static int antipv_command (char *word[], char *word_eol[], void *userdata)
{
	if (sizeof(word) > 1) {
		char arg[10];
		snprintf(arg, 10, "%s", word[2]);
		strUpper(arg);
		if (strcmp(arg, "ON") == 0 || strcmp(arg, "TRUE") == 0) {
			enable = 1;
			hexchat_print (ph, "[AntiPV] enabled\n");
		} else {
			enable = 0;
			hexchat_print (ph, "[AntiPV] disabled\n");
		}
	} else {
		hexchat_print (ph, "[AntiPV] error usage: ANTIPV [ON|OFF]\n");
	}
	return HEXCHAT_EAT_ALL;
}

void hexchat_plugin_get_info (char **name, char **desc, char **version, void **reserved) {
	*name = PNAME;
	*desc = PDESC;
	*version = PVERSION;
}

int hexchat_plugin_init (hexchat_plugin *plugin_handle, char **plugin_name, char **plugin_desc, char **plugin_version, char *arg) {
	ph = plugin_handle;

	*plugin_name = PNAME;
	*plugin_desc = PDESC;
	*plugin_version = PVERSION;

	hexchat_hook_command (ph, "ANTIPV", HEXCHAT_PRI_NORM, antipv_command, "Usage: ANTIPV [ON|OFF]", 0);
	hexchat_hook_print (ph, "Private Message", HEXCHAT_PRI_NORM, private_message_handler, 0);
	hexchat_print (ph, "AntiPV loaded\n");
	return 1;
}
