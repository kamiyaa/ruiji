#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define YANDERE_PNG_SOURCE_ID "<li><a class=\"original-file-unchanged\" id=\"png\" href=\""
#define YANDERE_JPG_SOURCE_ID "<li><a class=\"original-file-changed\" id=\"highres\" href=\""

/* Given a https://yande.re/ url,
 * parse the html to get the source image url
 */
char* yandere_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, YANDERE_PNG_SOURCE_ID);
	char *jpg_index = strstr(html_content, YANDERE_JPG_SOURCE_ID);
	char *img_src_url;

	/* check if png html pattern has been found */
	if (png_index) {
		/* move png_index pointer to the beginning of the source image
		 * url */
		png_index = &png_index[strlen(YANDERE_PNG_SOURCE_ID)];
		unsigned int url_len = get_distance(png_index, '"');

		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, png_index, url_len);
	}
	/* otherwise, check if jpg html pattern has been found */
	else if (jpg_index) {
		jpg_index = &jpg_index[strlen(YANDERE_JPG_SOURCE_ID)];
		unsigned int url_len = get_distance(jpg_index, '"');

		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, jpg_index, url_len);
	}
	/* otherwise, this html content did not contain any html pattern we
	 * recognize, so error */
	else {
		printf("Error: yandere_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	free(html_content);
	return img_src_url;
}
