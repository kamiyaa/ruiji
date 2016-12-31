#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define GELBOORU_SOURCE_ID ">Resize image</a></li><li><a"

/* Given a http://gelbooru.com/ url,
 * parse the html to get the source image url
 */
char* gelbooru_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *index = strstr(html_content, GELBOORU_SOURCE_ID);
	char *img_src_url;

	/* If found, add the danbooru url to it and return it */
	if (index) {
		index = &index[strlen(GELBOORU_SOURCE_ID)];
		replace_first_with(index, '"', '\0');

		unsigned int url_len = strlen(index) + 1;
		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		strcat(img_src_url, index);
	}
	else {
		printf("Error: gelbooru_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	free(html_content);
	return img_src_url;
}
