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
char *yandere_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* initialize generic source image index */
	char *source_index = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, YANDERE_PNG_SOURCE_ID);
	char *jpg_index = strstr(html_content, YANDERE_JPG_SOURCE_ID);


	/* find source image link */
	if (png_index)
		source_index = &png_index[strlen(YANDERE_PNG_SOURCE_ID)];
	else if (jpg_index)
		source_index = &jpg_index[strlen(YANDERE_JPG_SOURCE_ID)];

	/* check if any html pattern was detected */
	if (source_index) {
		/* get the length of the source image url */
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	/* otherwise, this html content did not contain any html pattern we
	 * recognize, so error */
	else {
		printf("yandere_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *yandere_get_image_tags(char *html_content)
{
	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(html_content, YANDERE_TAG_ID);
	tag_contents = &(tag_contents[strlen(YANDERE_TAG_ID)]);

	/* get how long the string of tags are */
	int slice_distance = get_distance(tag_contents, '}');
	/* isolate the tags with the rest of html */
	tag_contents[slice_distance] = ',';
	tag_contents[slice_distance+1] = '\0';

	struct image_tag_db *tag_db = init_image_tag_db();
	struct ll_node *tag_ptrs[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	/* get the next colon */
	int colon_distance = get_distance(tag_contents, ':');
	int comma_distance;
	/* store tag name category and size of tag name */
	unsigned int tag_index;
	unsigned int tag_name_size;

	while (colon_distance > 0) {
		/* get where the next comma is */
		comma_distance = get_distance(tag_contents, ',');

		/* change comma to escape char, essentially slicing the string
		at comma */
		tag_contents[comma_distance] = '\0';

		/* figure out which tag category this tag belongs to */
		if (strstr(tag_contents, "artist"))
			tag_index = 0;
		else if (strstr(tag_contents, "character"))
			tag_index = 1;
		else if (strstr(tag_contents, "circle"))
			tag_index = 2;
		else if (strstr(tag_contents, "copyright"))
			tag_index = 3;
		else if (strstr(tag_contents, "fault"))
			tag_index = 4;
		else
			tag_index = 5;

		/* put back comma to reunite the string */
		tag_contents[comma_distance] = ',';

		/* get length of tag name */
		tag_name_size = get_distance(&(tag_contents[1]), '"');

		/* create the linked list node to store the information */
		if (!(tag_db->tags[tag_index])) {
			tag_db->tags[tag_index] = malloc(sizeof(struct ll_node));
			tag_db->tags[tag_index]->next = NULL;
			tag_db->tags[tag_index]->data = malloc(sizeof(char) * (tag_name_size + 1));
			tag_db->tags[tag_index]->data[0] = '\0';
			strncat(tag_db->tags[tag_index]->data, &(tag_contents[1]), tag_name_size);

			tag_ptrs[tag_index] = tag_db->tags[tag_index];
		}
		else {
			tag_ptrs[tag_index]->next = malloc(sizeof(struct ll_node));
			tag_ptrs[tag_index]->next->data = malloc(sizeof(char) *
							(tag_name_size + 1));
			tag_ptrs[tag_index]->next->data[0] = '\0';
			strncat(tag_ptrs[tag_index]->next->data,
				&(tag_contents[1]), tag_name_size);

			tag_ptrs[tag_index] = tag_ptrs[tag_index]->next;
			tag_ptrs[tag_index]->next = NULL;
		}

		/* increment the amount of tags in this category we currently
		found */
		(tag_db->tag_size[tag_index])++;

		/* search for next colon */
		tag_contents = &(tag_contents[comma_distance+1]);
		colon_distance = get_distance(tag_contents, ':');
	}

	return tag_db;
}
