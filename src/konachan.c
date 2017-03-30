#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define KONACHAN_PNG_SOURCE_ID "<li><a class=\"original-file-unchanged\" href=\""
#define KONACHAN_JPG_SOURCE_ID "<li><a class=\"original-file-changed\" href=\""
#define HTTP "http:"

/* Given a https://konachan.com/ url,
 * parse the html to get the source image url
 */
char* konachan_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, KONACHAN_PNG_SOURCE_ID);
	char *jpg_index = strstr(html_content, KONACHAN_JPG_SOURCE_ID);
	char *img_src_url;

	/* check if png html pattern has been found */
	if (png_index) {
		/* move png_index pointer to the beginning of the source image
		 * url */
		png_index = &png_index[strlen(KONACHAN_PNG_SOURCE_ID)];
		unsigned int url_len = get_distance(png_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) *
					(url_len + strlen(HTTP) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTP);
		strncat(img_src_url, png_index, url_len);
	}
	/* otherwise, check if jpg html pattern has been found */
	else if (jpg_index) {
		/* move jpg_index pointer to the beginning of the source image
		 * url */
		jpg_index = &jpg_index[strlen(KONACHAN_JPG_SOURCE_ID)];
		unsigned int url_len = get_distance(jpg_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) *
					(url_len + strlen(HTTP) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTP);
		strncat(img_src_url, jpg_index, url_len);
	}
	else {
		printf("Error: konachan_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		img_src_url = "Error\0";
	}
	/* deallocate the memory used to download
	 * and store the webpage's content */
	free(html_content);
	/* return the image source url */
	return img_src_url;
}
