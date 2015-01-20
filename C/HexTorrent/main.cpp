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
#include <stdarg.h>
#include <string>
#include <vector>
#include <locale>

#include "libtorrent/entry.hpp"
#include "libtorrent/bencode.hpp"
#include "libtorrent/session.hpp"
#include "hexchat-plugin.h"

static hexchat_plugin *ph;

static char* PNAME = "HexTorrent";
static char* PDESC = "Simple hexchat Torrent client - by GHOSTnew";
static char* PVERSION = "1.0.18";

using namespace libtorrent;
using namespace std;
static session ses;
static error_code ec;

void putsInfo(const char* msg, ...) {
	char content[512];
	va_list	args;
	va_start(args, msg);
	vsnprintf(content, 512, msg, args);
	hexchat_printf(ph, "\002[HexTorrent]\002 %s", content);
	va_end(args);
}

char* strUpper(const char* str) {
	locale loc;
	string mystring(str);
	for (unsigned int i = 0; i<mystring.length(); i++) {
		mystring[i] = toupper(mystring[i], loc);
	}
	return const_cast<char*>(mystring.c_str());
}

void putsError(const char* msg, ...) {
	char content[512];
	va_list	args;
	va_start(args, msg);
	vsnprintf(content, 512, msg, args);
	hexchat_printf(ph, "\002[HexTorrent]\002 \0034Error: %s", content);
	va_end(args);
}

static int torrent_command (char *word[], char *word_eol[], void *userdata){
	if (strcmp(strUpper(word[2]), "DOWNLOAD") == 0) {
		add_torrent_params tparam;
		tparam.save_path = "/tmp/"; //TODO change to hexchat download dir
		if (string(word[3]).find(".torrent") != string::npos) {
			// torrent file
			tparam.ti = new torrent_info(word[3], ec);
		} else {
			// magnet link
			tparam.url = string(word[3]);
		}
		if (ec) {
			putsError(ec.message().c_str());
		} else {
			ses.add_torrent(tparam, ec);
			if (ec) {
				putsError(ec.message().c_str());
			} else {
				putsInfo("Torrent added");
			}
		}
	} else if (strcmp(strUpper(word[2]), "PAUSE") == 0) {
		vector<torrent_handle> tl = ses.get_torrents();
		tl[atoi(word[3])].auto_managed(false);
		tl[atoi(word[3])].pause();
		putsInfo("Torrent : %s paused", tl[atoi(word[3])].name().c_str());
	} else if (strcmp(strUpper(word[2]), "RESUME") == 0) {
		vector<torrent_handle> tl = ses.get_torrents();
		tl[atoi(word[3])].auto_managed(true);
		tl[atoi(word[3])].resume();
		putsInfo("Torrent : %s resumed", tl[atoi(word[3])].name().c_str());
	} else if (strcmp(strUpper(word[2]), "LIST") == 0) {
		vector<torrent_handle> tl = ses.get_torrents();
		putsInfo("TORRENT LIST:");
		string dinfo("");
		for (unsigned int i = 0; i < tl.size(); i++) {
			if (tl[i].status().paused) {
				dinfo = " (\002\0037Paused\003\002)";
			} else if (tl[i].status().is_finished) {
				dinfo = " (\002\0033Finished\003\002)";
			} else {
				dinfo = "";
			}

			hexchat_printf(ph, " - %d - %s%s [%.1f%%]", i, tl[i].name().c_str(), dinfo.c_str(), tl[i].status().progress_ppm/10000.f);

		}
		putsInfo("END OF TORRENT LIST");
	} else {
		putsError("Usage: Torrent [download|stop|pause|resume|list] [magnet|.torrent]");
	}
	return HEXCHAT_EAT_ALL;
}

extern "C" {
	int hexchat_plugin_init (hexchat_plugin *plugin_handle, char* *plugin_name, char* *plugin_desc, char* *plugin_version, char* arg) {
		ph = plugin_handle;

		*plugin_name = PNAME;
		*plugin_desc = PDESC;
		*plugin_version = PVERSION;

		hexchat_hook_command (ph, "Torrent", HEXCHAT_PRI_NORM, torrent_command, "Usage: Torrent [download|stop|pause|resume|list] [magnet|.torrent]", 0);

		ses.listen_on(std::make_pair(6881, 6889), ec);
		if (ec) {
			hexchat_printf(ph, "HexchatTorrent failed to open listen socket: %s", ec.message().c_str() );
			return -1;
		}
		putsInfo("v%s loaded", PVERSION);
		return 1;
	}

	int hexchat_plugin_deinit (hexchat_plugin *ph) {
		ses.abort();
		putsInfo("v%s unloaded.", PVERSION);

		return 1;
	}
}
