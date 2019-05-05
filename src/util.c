#include <stdlib.h>
#include <curl/curl.h>

#include "util.h"

int get_distance(char *str, char find)
{
	int distance = 0;
	char *p = str;
	while (*p && *p != find) {
		distance++;
		p++;
	}
	if (*p == '\0')
		return -1;
	return distance;
}

void ruiji_exit(int exit_status)
{
	curl_global_cleanup();
	exit(exit_status);
}
