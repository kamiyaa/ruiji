#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define GELBOORU_SOURCE_ID ">Resize image</a></li><li><a href=\""

/* Given a http://gelbooru.com/ url,
 * parse the html to get the source image url
 */
char* gelbooru_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *source_index = strstr(html_content, GELBOORU_SOURCE_ID);
	char *img_src_url;

	/* If found, add the danbooru url to it and return it */
	if (source_index) {
		source_index = &source_index[strlen(GELBOORU_SOURCE_ID)];
		unsigned int url_len = get_distance(source_index, '"');

		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	else {
		printf("Error: gelbooru_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	free(html_content);
	return img_src_url;
}
