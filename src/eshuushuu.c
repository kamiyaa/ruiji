#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define ESHUUSHUU_URL "http://e-shuushuu.net"
#define ESHUUSHUU_SOURCE_ID "<a class=\"thumb_image\" href=\""

/* Given a http://e-shuushuu.net url,
 * parse the html to get the source image url
 */
char* eshuushuu_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *source_index = strstr(html_content, ESHUUSHUU_SOURCE_ID);
	char *img_src_url;

	/* If found, add the website url to it and return it */
	if (source_index) {
		source_index = &source_index[strlen(ESHUUSHUU_SOURCE_ID)];
		unsigned int url_len = get_distance(source_index, '"');

		img_src_url = malloc(sizeof(char) *
					(url_len + strlen(ESHUUSHUU_URL) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, ESHUUSHUU_URL);
		strncat(img_src_url, source_index, url_len);
	}
	else {
		printf("Error: eshuushuu_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	free(html_content);
	return img_src_url;
}
