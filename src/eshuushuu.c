#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "eshuushuu.h"
#include "parser.h"

/* Given a http://e-shuushuu.net url,
 * parse the html to get the source image url
 */
char *eshuushuu_get_image_url(char *html_content)
{
	/* constants used to find values */
	const char eshuushuu_url[] = "http://e-shuushuu.net";
	const char source_uuid[] = "<a class=\"thumb_image\" href=\"";
	const char source_end = '"';

	const unsigned int len_source_uuid = strlen(source_uuid);
	const unsigned int len_eshuushuu_url = strlen(eshuushuu_url);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, source_uuid);

	/* If found, add the website url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[len_source_uuid];
		/* get the length of the source image url */
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHAR_SIZE *
					(url_len + len_eshuushuu_url + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, eshuushuu_url);
		strncat(img_src_url, source_index, url_len);
	}
	else {
		fprintf(stderr,
			"eshuushuu_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}
