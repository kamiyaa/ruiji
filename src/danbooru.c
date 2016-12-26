#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define DANBOORU_URL "http://danbooru.donmai.us"
#define DANBOORU_SOURCE_ID "Size: <a href=\""

char* danbooru_get_image_url(char *web_url)
{
	char *html_content = get_html(web_url);

	char *index = strstr(html_content, DANBOORU_SOURCE_ID);
	char *img_src_url;

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
		printf("Error: Failed to parse \"%s\"\n", web_url);
		return "ERROR";
	}
	return img_src_url;
}
