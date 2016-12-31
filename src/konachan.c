#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define KONACHAN_PNG_SOURCE_ID "<li><a class=\"original-file-unchanged\" href=\""
#define KONACHAN_JPG_SOURCE_ID "<li><a class=\"original-file-changed\" href=\""
#define HTTP "http:"

/* Given a https://yande.re/ url,
 * parse the html to get the source image url
 */
char* konachan_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	char *index;
	char *img_src_url;

	/* If found, return it */
	if (strstr(html_content, KONACHAN_PNG_SOURCE_ID)) {
		index = strstr(html_content, KONACHAN_PNG_SOURCE_ID);
		index = &index[strlen(KONACHAN_PNG_SOURCE_ID)];
		replace_first_with(index, '"', '\0');

		unsigned int url_len = strlen(index) + strlen(HTTP) + 1;
		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTP);
		strcat(img_src_url, index);
	}
	else if (strstr(html_content, KONACHAN_JPG_SOURCE_ID)) {
		index = strstr(html_content, KONACHAN_JPG_SOURCE_ID);
		index = &index[strlen(KONACHAN_JPG_SOURCE_ID)];
		replace_first_with(index, '"', '\0');

		unsigned int url_len = strlen(index) + strlen(HTTP) + 1;
		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTP);
		strcat(img_src_url, index);
	}
	else {
		printf("Error: konachan_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	free(html_content);
	return img_src_url;
}
