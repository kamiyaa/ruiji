#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define DANBOORU_URL "https://danbooru.donmai.us"
#define DANBOORU_SOURCE_ID "Size: <a href=\""

/* Given a https://danbooru.donmai.us url,
 * parse the html to get the source image url
 */
char* danbooru_get_image_url(char *html_content)
{
	/* Find the source image link */
	char *source_index = strstr(html_content, DANBOORU_SOURCE_ID);
	/* initialize the image source url to be returned later */
	char *img_src_url = "\0";

	/* If source image link is found,
	 * add the danbooru url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(DANBOORU_SOURCE_ID)];
		unsigned int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) *
					(url_len + strlen(DANBOORU_URL) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, DANBOORU_URL);
		strncat(img_src_url, source_index, url_len);
	}
	else
		printf("danbooru_get_image_url(): Error: Failed to parse website\n");

	/* return the image source url */
	return img_src_url;
}
