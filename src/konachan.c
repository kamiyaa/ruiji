#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "konachan.h"
#include "parser.h"

#define KONACHAN_PNG_SOURCE_ID "<li><a class=\"original-file-unchanged\" href=\""
#define KONACHAN_JPG_SOURCE_ID "<li><a class=\"original-file-changed\" href=\""
#define HTTP "http:"

/* Given a https://konachan.com/ url,
 * parse the html to get the source image url
 */
char *konachan_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* initialize generic source image index */
	char *source_index = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, KONACHAN_PNG_SOURCE_ID);
	char *jpg_index = strstr(html_content, KONACHAN_JPG_SOURCE_ID);


	/* find source image link */
	if (png_index)
		source_index = &png_index[strlen(KONACHAN_PNG_SOURCE_ID)];
	else if (jpg_index)
		source_index = &jpg_index[strlen(KONACHAN_JPG_SOURCE_ID)];

	/* check if any html pattern was detected */
	if (source_index) {
		/* get the length of the source image url */
		int url_len = get_distance(png_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHARSIZE *
					(url_len + strlen(HTTP) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTP);
		strncat(img_src_url, source_index, url_len);
	}
	else {
		printf("konachan_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}
