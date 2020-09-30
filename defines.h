#define COL_BEG "\033["
#define COL_END "m"
#define COL_RES COL_BEG "0" COL_END

#define COL_NONE "0"

#define FG_BLACK "30"
#define FG_RED "31"
#define FG_GREEN "32"
#define FG_YELLOW "33"
#define FG_BLUE "34"
#define FG_MAGENTA "35"
#define FG_CYAN "36"
#define FG_WHITE "37"
#define FGB_BLACK "90"
#define FGB_RED "91"
#define FGB_GREEN "92"
#define FGB_YELLOW "93"
#define FGB_BLUE "94"
#define FGB_MAGENTA "95"
#define FGB_CYAN "96"
#define FGB_WHITE "97"

#define BG_BLACK "40"
#define BG_RED "41"
#define BG_GREEN "42"
#define BG_YELLOW "43"
#define BG_BLUE "44"
#define BG_MAGENTA "45"
#define BG_CYAN "46"
#define BG_WHITE "47"
#define BGB_BLACK "100"
#define BGB_RED "101"
#define BGB_GREEN "102"
#define BGB_YELLOW "103"
#define BGB_BLUE "104"
#define BGB_MAGENTA "105"
#define BGB_CYAN "106"
#define BGB_WHITE "107"

#define COL(COL_BF) COL_BEG COL_BF COL_END


#define HISTORY_PATH "utils/history.agsh"
#define CONF_PATH "conf/"


#define AGSH_TOK_BUFSIZE 64
#define AGSH_TOK_DELIM " \t\r\n\a"

#define AGSH_PINFO_BUF 256