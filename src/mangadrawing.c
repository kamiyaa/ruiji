#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define MANGADRAWING_SOURCE_ID "NAME=\"download\" ACTION=\""

/* Given a https://mangadrawing.net/ url,
 * parse the html to get the source image url
 */
char* mangadrawing_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *source_index = strstr(html_content, MANGADRAWING_SOURCE_ID);

	/* check if png html pattern has been found */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(MANGADRAWING_SOURCE_ID)];
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
		free(html_content);

		/* Refetch the html source code of the source website*/
		html_content = get_html(img_src_url);
		char *intermediate_source_id = "<img src=\"";
		source_index = strstr(html_content, intermediate_source_id);
		source_index = &source_index[strlen(intermediate_source_id)];
		free(img_src_url);

		url_len = get_distance(source_index, '"');
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	/* otherwise, this html content did not contain any html pattern we
	 * recognize, so error */
	else
		printf("yandere_get_image_url(): Error: Failed to parse website\n");

	/* deallocate the memory used to download
	 * and store the webpage's content */
	free(html_content);
	/* return the image source url */
	return img_src_url;
}
