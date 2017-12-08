#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "yandere.h"
#include "parser.h"

/* Given a https://yande.re/ url,
 * parse the html to get the source image url
 */
char *yandere_get_image_url(char *html_content)
{
	/* constants used to find values */
	const char *png_source_uuid = "<li><a class=\"original-file-unchanged\"";
	const char *jpg_source_uuid = "<li><a class=\"original-file-changed\"";
	const char *url_uuid = "href=\"";

	const unsigned int len_png = strlen(png_source_uuid);
	const unsigned int len_jpg = strlen(jpg_source_uuid);
	const unsigned int len_url_uuid = strlen(url_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* initialize generic source image index */
	char *source_index = NULL;

	/* get png html pattern index and jpg html pattern index */
	char *png_index = strstr(html_content, png_source_uuid);
	char *jpg_index = strstr(html_content, jpg_source_uuid);


	/* find source image link */
	if (png_index) {
		source_index = &(png_index[len_png]);
		source_index = strstr(source_index, url_uuid);
	}
	else if (jpg_index) {
		source_index = &(jpg_index[len_jpg]);
		source_index = strstr(source_index, url_uuid);
	}

	/* check if any html pattern was detected */
	if (source_index) {
		source_index = &(source_index[len_url_uuid]);
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
		fprintf(stderr,
			"yandere_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *yandere_get_image_tags(char *html_content)
{
	/* constants for finding values */
	const char *tags_uuid = "\"tags\":{";
	const char tags_end = '}';
	const char tag_category_uuid = ':';
	const char tag_name_uuid = ',';
	const char tag_name_end = '"';

	/* offsets from actual value */
	const unsigned int initial_offset = strlen(tags_uuid);

	/* get the next colon */
	int next_tag_distance = 0;

	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(html_content, tags_uuid);
	if (tag_contents) {
		/* move pointer to start of tag */
		tag_contents = &(tag_contents[initial_offset]);

		/* get the end of tags section and slice string at the end */
		int tag_contents_end = get_distance(tag_contents, tags_end);

		/* replace end of tags with tag_name_uuid for easier parsing */
		tag_contents[tag_contents_end] = tag_name_uuid;

		/* slice string after it */
		tag_contents[tag_contents_end + 1] = '\0';
		next_tag_distance = get_distance(tag_contents, tag_name_uuid);
	}

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();
	struct llnode **tag_ptrs[6] = {
		&(tag_db->tags[0]),
		&(tag_db->tags[1]),
		&(tag_db->tags[2]),
		&(tag_db->tags[3]),
		&(tag_db->tags[4]),
		&(tag_db->tags[5])
	};

	while (next_tag_distance > 0) {
		/* get the end of the category name */
		unsigned int category_end_distance =
			get_distance(tag_contents, tag_name_uuid);
		unsigned int category_start_distance =
			get_distance(tag_contents, tag_category_uuid);

		/* temporarily slice the string at the category_end position */
		tag_contents[category_end_distance] = '\0';
		/* get the tag type of the tag */
		unsigned int tag_type = yandere_get_tag_type(
				&(tag_contents[category_start_distance]));
		/* restore sliced string */
		tag_contents[category_end_distance] = tag_name_uuid;

		/* move pointer to beginning of tag name */
		tag_contents = &(tag_contents[1]);
		/* get length of tag name */
		unsigned int tag_name_len = get_distance(tag_contents, tag_name_end);

		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(sizeof(char) * (tag_name_len + 1));
		strncpy(tag_name, tag_contents, tag_name_len);
		tag_name[tag_name_len] = '\0';


		/* allocate memory for node */
		*(tag_ptrs[tag_type]) = malloc(sizeof(struct llnode));
		/* set next to NULL and data to tag_name */
		(*(tag_ptrs[tag_type]))->next = NULL;
		(*(tag_ptrs[tag_type]))->data = tag_name;

		/* set tag_ptrs to next node */
		tag_ptrs[tag_type] = &((*(tag_ptrs[tag_type]))->next);


		/* increment the amount of tags in this category we currently
		 * found */
		(tag_db->tag_size[tag_type])++;

		/* move on to next tag */
		tag_contents = &(tag_contents[category_end_distance]);
		/* search for next tag */
		next_tag_distance = get_distance(tag_contents, tag_name_uuid);
	}

	return tag_db;
}

unsigned int yandere_get_tag_type(char *tag_contents)
{
	unsigned int tag_type;
	/* figure out which tag category this tag belongs to */
	if (strstr(tag_contents, "artist"))
		tag_type = 0;
	else if (strstr(tag_contents, "character"))
		tag_type = 1;
	else if (strstr(tag_contents, "circle"))
		tag_type = 2;
	else if (strstr(tag_contents, "copyright"))
		tag_type = 3;
	else if (strstr(tag_contents, "fault"))
		tag_type = 4;
	else
		tag_type = 5;

	return tag_type;
}
