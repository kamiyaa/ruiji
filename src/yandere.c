#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define YANDERE_PNG_SOURCE_ID "<li><a class=\"original-file-unchanged\" id=\"png\" href=\""
#define YANDERE_JPG_SOURCE_ID "<li><a class=\"original-file-changed\" id=\"highres\" href=\""
#define YANDERE_TAG_ID "\"tags\":{"

/* Given a https://yande.re/ url,
 * parse the html to get the source image url
 */
char* yandere_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, YANDERE_PNG_SOURCE_ID);
	char *jpg_index = strstr(html_content, YANDERE_JPG_SOURCE_ID);

	/* check if png html pattern has been found */
	if (png_index) {
		/* move png_index pointer to the beginning of
		 * the source image url */
		png_index = &png_index[strlen(YANDERE_PNG_SOURCE_ID)];
		int url_len = get_distance(png_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, png_index, url_len);
	}
	/* otherwise, check if jpg html pattern has been found */
	else if (jpg_index) {
		/* move jpg_index pointer to the beginning of
		 * the source image url */
		jpg_index = &jpg_index[strlen(YANDERE_JPG_SOURCE_ID)];
		int url_len = get_distance(jpg_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the string over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, jpg_index, url_len);
	}
	/* otherwise, this html content did not contain any html pattern we
	 * recognize, so error */
	else
		printf("yandere_get_image_url(): Error: Failed to parse website\n");
	/* return the image source url */
	return img_src_url;
}

/*
struct image_tags* yandere_get_image_tags(char *html_content)
{
	char *tags_ptr = strstr(html_content, YANDERE_TAG_ID);

	tags_ptr = &tags_ptr[strlen(YANDERE_TAG_ID)];

	int slice_distance = get_distance(tags_ptr, '}');
	tags_ptr[slice_distance] = ',';
	tags_ptr[slice_distance+1] = '\0';

	struct image_tags *tags_db = malloc(sizeof(struct image_tags));

	struct ll_node *tag_array[6] = { tags_db->artist,
					tags_db->character,
					tags_db->circle,
					tags_db->copyright,
					tags_db->fault,
					tags_db->general };

	char *tag_category;
	int colon_distance = get_distance(tags_ptr, ':');
	int comma_distance;
	unsigned int tag_index;

	while (colon_distance > 0) {
		comma_distance = get_distance(tags_ptr, ',');
		tags_ptr[comma_distance] = '\0';

		tag_category = &(tags_ptr[colon_distance]);

		tag_index = 5;
		if (strstr(tag_category, "\"artist\""))
			tag_index = 0;
		else if (strstr(tag_category, "\"character\""))
			tag_index = 1;
		else if (strstr(tag_category, "\"circle\""))
			tag_index = 2;
		else if (strstr(tag_category, "\"copyright\""))
			tag_index = 3;
		else if (strstr(tag_category, "\"fault\""))
			tag_index = 4;

		tag_array[tag_index] = malloc(sizeof(struct llnode));
		tags_ptr[colon_distance-1] = '\0';
		tag_array[tag_index]->data = &(tags_ptr[1]);
		tag_array[tag_index] = tag_array[tag_index]->next;

		colon_distance = get_distance(tags_ptr, ':');
	}

	return tags_db;
}
*/
