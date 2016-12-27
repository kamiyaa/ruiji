#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define DANBOORU_URL "http://danbooru.donmai.us"
#define DANBOORU_SOURCE_ID "Size: <a href=\""

/* Given a http://danbooru.donmai.us url,
 * parse the html to get the source image url
 */
char* danbooru_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *index = strstr(html_content, DANBOORU_SOURCE_ID);
	char *img_src_url;

	/* If found, add the danbooru url to it and return it */
	if (index) {
		char *walker = &index[strlen(DANBOORU_SOURCE_ID)];
		char *beginning = walker;
		while (*walker != '"')
			walker = &walker[1];
		walker[0] = '\0';
		unsigned int url_len = strlen(beginning) + strlen(DANBOORU_URL) + 1;
		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		strcat(img_src_url, DANBOORU_URL);
		strcat(img_src_url, beginning);
	}
	else {
		printf("Error: danbooru_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		return "ERROR";
	}
	return img_src_url;
}
