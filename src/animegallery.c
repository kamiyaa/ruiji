#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "animegallery.h"
#include "helpers.h"

/* Given a https://yande.re/ url,
 * parse the html to get the source image url
 */
char *animegallery_get_image_url(char *web_content)
{
	/* constants used to find values */
	const char *animegallery_url = "http://www.theanimegallery.com/";
	const char *source_uuid = "<a class=\"main\" href=\"";
	const char source_end = '"';

	const unsigned int len_source_uuid = strlen(source_uuid);
	const unsigned int len_animegallery_url = strlen(animegallery_url);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(web_content, source_uuid);

	/* If found, add the website url to it and return it */
	if (source_index == NULL) {
		fprintf(stderr,
			"animegallery_get_image_url(): Error: Failed to parse website\n");
		return NULL;
	}
	/* move source_index pointer to the beginning of
	 * the source image url */
	source_index = &(source_index[len_source_uuid]);
	/* get the length of the source image url */
	int url_len = get_distance(source_index, source_end);

	/* allocate enough memory to hold the image source url,
	 * then copy the url over to img_src_url and return it */
	img_src_url = malloc(sizeof(char) *
			(len_animegallery_url + url_len + 1));
	if (img_src_url == NULL) {
		fprintf(stderr,
			"animegallery_get_image_url(): Error: Out of memory\n");
		return NULL;
	}

	strncpy(img_src_url, animegallery_url, len_animegallery_url);
	strncpy(&(img_src_url[len_animegallery_url]), source_index, url_len);
	img_src_url[url_len + len_animegallery_url] = '\0';

	/* return the image source url */
	return img_src_url;
}
