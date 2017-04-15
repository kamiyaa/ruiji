#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define ZEROCHAN_SOURCE_ID "fullsizeUrl = '"

/* Given a http://www.zerochan.net/ url,
 * parse the html to get the source image url
 */
char* zerochan_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, ZEROCHAN_SOURCE_ID);

	/* If found, return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(ZEROCHAN_SOURCE_ID)];
		int url_len = get_distance(source_index, '\'');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	else
		printf("zerochan_get_image_url(): Error: Failed to parse website\n");

	/* return the image source url */
	return img_src_url;
}
