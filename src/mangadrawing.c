#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mangadrawing.h"
#include "parser.h"
#include "udload.h"

/* Given a https://mangadrawing.net/ url,
 * parse the html to get the source image url
 */
char *mangadrawing_get_image_url(char *web_content)
{
	const char *source_uuid = "NAME=\"download\" ACTION=\"";
	const char source_end = '"';

	const unsigned int len_source_uuid = strlen(source_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *source_index = strstr(web_content, source_uuid);

	/* check if png html pattern has been found */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[len_source_uuid];
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);

		/* Refetch the html source code of the source website*/
		char *web_content_redirected = get_html(img_src_url);
		char *intermediate_source_id = "<img src=\"";
		source_index = strstr(web_content_redirected, intermediate_source_id);
		source_index = &source_index[strlen(intermediate_source_id)];
		free(img_src_url);

		url_len = get_distance(source_index, '"');
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	/* otherwise, this html content did not contain any html pattern we
	 * recognize, so error */
	else {
		fprintf(stderr,
			"managadrawing_get_image_url(): Error: Failed to parse website\n");
	}

	/* deallocate the memory used to download
	 * and store the webpage's content */
	free(web_content);
	/* return the image source url */
	return img_src_url;
}
