#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zerochan.h"
#include "parser.h"

/* Given a http://www.zerochan.net/ url,
 * parse the html to get the source image url
 */
char *zerochan_get_image_url(char *html_content)
{
	const char *source_uuid = "fullsizeUrl = '";
	const char source_end = '\'';

	const unsigned int len_source_uuid = strlen(source_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, source_uuid);

	/* If found, return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &(source_index[len_source_uuid]);
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHAR_SIZE * (url_len + 1));
		strncpy(img_src_url, source_index, url_len);
		img_src_url[url_len] = '\0';
	}
	else {
		fprintf(stderr,
			"zerochan_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}
