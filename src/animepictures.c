#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "animepictures.h"
#include "helpers.h"

char *animepictures_get_image_url(char *web_content)
{

	/* constants used to find values */
	const char *animepictures_url = "https://anime-pictures.net";
	const unsigned int animepictures_url_len = strlen(animepictures_url);

	const char *source_uuid = "class=\"download_icon\"";
	const unsigned int source_uuid_len = strlen(source_uuid);

	const char source_end = '"';

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	char *source_index = strstr(web_content, source_uuid);
	source_index = strstr(source_index, "href=\"");

	/* If source image link is found,
	 * add the danbooru url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &(source_index[source_uuid_len]);
		/* get the length of the source image url */
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) *
					(url_len + animepictures_url_len + 1));
		strncpy(img_src_url, animepictures_url, animepictures_url_len);
		strncpy(&(img_src_url[animepictures_url_len]),
			source_index, url_len);
		img_src_url[url_len + animepictures_url_len] = '\0';
	}
	else {
		fprintf(stderr,
			"animepictures_get_image_url(): Error: Failed to parse website\n");
	}
	return img_src_url;
}

struct image_tag_db *animepictures_get_image_tags(char *web_content)
{
	char *artist_tag_uuid = "author</span>";
	char *character_tag_uuid = "character</span>";
	char *copyright_tag_uuid = "copyright</span>";
	char *general_tag_uuid = "reference</span>";
	char *meta_tag_uuid = "object</span>";

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();

	// TODO

	/* populate artist tags */

	/* populate character tags */

	/* populate copyright tags */

	/* populate meta tags */

	/* populate general tags */

	return tag_db;
}

struct llnode *animepictures_parse_tags(char *tag_pattern, char *web_content,
	unsigned int *size)
{
	// TODO

	return NULL;
}
