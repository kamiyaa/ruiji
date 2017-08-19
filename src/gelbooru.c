#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gelbooru.h"
#include "parser.h"

#define GELBOORU_SOURCE_ID ">Resize image</a></li><li><a href=\""

/* Given a http://gelbooru.com/ url,
 * parse the html to get the source image url
 */
char *gelbooru_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, GELBOORU_SOURCE_ID);

	/* If found, add the danbooru url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(GELBOORU_SOURCE_ID)];
		/* get the length of the source image url */
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHAR_SIZE * (url_len + 1));
		img_src_url[0] = '\0';
		strncat(img_src_url, source_index, url_len);
	}
	else {
		printf("gelbooru_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *gelbooru_get_image_tags(char *html_content)
{
	const char tags_uuid[] = "<h3>Tags</h3>";
	const char tags_end[] = "<br />";
	const char tag_category_uuid[] = "class=\"tag-type-";
	const char tag_category_end = '"';
	const char tag_name_uuid[] = "page=post&amp;s=list&amp;tags=";
	const char tag_name_end = tag_category_end;

	/* offsets from actual value */
	const unsigned int initial_offset = strlen(tags_uuid);
	const unsigned int category_offset = strlen(tag_category_uuid);
	const unsigned int name_offset = strlen(tag_name_uuid);

	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(html_content, tags_uuid);
	if (tag_contents) {
		tag_contents = &(tag_contents[initial_offset]);
		char *tag_contents_end = strstr(tag_contents, tags_end);
		tag_contents_end[0] = '\0';
	}

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();
	struct ll_node *tag_ptrs[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	/* set pointer to beginning of first tag */
	tag_contents = strstr(tag_contents, tag_category_uuid);
	while (tag_contents) {
		/* move to beginning of tag type */
		tag_contents = &(tag_contents[category_offset]);

		/* get how far we are from the end of current tag */
		unsigned int category_end_distance = get_distance(tag_contents,
					tag_category_end);
		/* temporarily slice the string at the category_end position */
		tag_contents[category_end_distance] = '\0';
		/* get the tag type of the tag */
		unsigned tag_index = gelbooru_get_tag_type(tag_contents);
		/* restore sliced string */
		tag_contents[category_end_distance] = tag_category_end;

		/* move pointer to the start of the tag name */
		tag_contents = strstr(tag_contents, tag_name_uuid);
		tag_contents = &(tag_contents[name_offset]);

		/* get where the tag name ends is */
		int len_tag_name = get_distance(tag_contents, tag_name_end);

		/* create the linked list node to store the information */
		if (!(tag_db->tags[tag_index])) {
			/* malloc memory for node */
			tag_db->tags[tag_index] = malloc(sizeof(struct ll_node));
			/* malloc memory for char array in node */
			tag_db->tags[tag_index]->data = malloc(sizeof(char) * (len_tag_name + 1));
			/* set first element to \0 */
			tag_db->tags[tag_index]->data[0] = '\0';
			/* concatentate tag name to char array */
			strncat(tag_db->tags[tag_index]->data,
					tag_contents, len_tag_name);

			/* set tag_ptrs to it */
			tag_ptrs[tag_index] = tag_db->tags[tag_index];
		}
		else {
			/* malloc memory for node */
			tag_ptrs[tag_index]->next = malloc(sizeof(struct ll_node));
			/* malloc memory for char array in node */
			tag_ptrs[tag_index]->next->data = malloc(sizeof(char) * (len_tag_name + 1));
			/* set first element to \0 */
			tag_ptrs[tag_index]->next->data[0] = '\0';
			strncat(tag_ptrs[tag_index]->next->data,
				tag_contents, len_tag_name);

			/* set tag_ptrs to its next value */
			tag_ptrs[tag_index] = tag_ptrs[tag_index]->next;
		}
		tag_ptrs[tag_index]->next = NULL;
		/* increment the amount of tags in this category we currently
		 * found */
		(tag_db->tag_size[tag_index])++;

		tag_contents = &(tag_contents[len_tag_name]);
		tag_contents = strstr(tag_contents, tag_category_uuid);
	}

	return tag_db;
}

unsigned int gelbooru_get_tag_type(char *tag_contents)
{
	unsigned int tag_index;
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

	return tag_index;
}
