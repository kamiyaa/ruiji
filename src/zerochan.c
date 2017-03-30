#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define ZEROCHAN_SOURCE_ID "fullsizeUrl = '"

/* Given a http://www.zerochan.net/ url,
 * parse the html to get the source image url
 */
char* zerochan_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *source_index = strstr(html_content, ZEROCHAN_SOURCE_ID);
	char *img_src_url;

	/* If found, return it */
	if (source_index) {
		source_index = &source_index[strlen(ZEROCHAN_SOURCE_ID)];
		unsigned int url_len = get_distance(source_index, '\'');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	else {
		printf("Error: zerochan_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	/* deallocate the memory used to download
	 * and store the webpage's content */
	free(html_content);
	/* return the image source url */
	return img_src_url;
}
