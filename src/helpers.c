#include "helpers.h"

int get_distance(char *string, char find)
{
	int distance = 0;
	/* keep on incrementing until we've found the char */
	while (string[distance] != '\0' && string[distance] != find)
		distance++;
	if (string[distance] == '\0')
		distance = -1;
	/* return the distance */
	return distance;
}
