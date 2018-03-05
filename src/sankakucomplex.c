#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sankakucomplex.h"
#include "helpers.h"

/* Given a https://chan.sankakucomplex.com url,
 * parse the html to get the source image url
 */
char *sankakucomplex_get_image_url(char *web_content)
{
	/* constants for finding values */
	const char *https = "https:";
	const char *source_uuid = "<li>Original: <a href=\"";
	const char source_end = '?';

	const unsigned int len_https = strlen(https);
	const unsigned int len_source = strlen(source_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(web_content, source_uuid);

	/* If source image link is found,
	 * add http extension to it and return it */
	if (source_index == NULL) {
		fprintf(stderr,
			"sankaku_complex_get_image_url(): Error: Failed to parse website\n");
		return NULL;
	}

	/* move source_index pointer to the beginning of
	 * the source image url */
	source_index = &source_index[len_source];
	int len_url = get_distance(source_index, source_end);

	/* allocate enough memory to hold the image source url,
	 * then copy the url over to img_src_url and return it */
	img_src_url = malloc(sizeof(char) *
				(len_https + len_url + 1));
	if (img_src_url == NULL) {
		fprintf(stderr,
			"sankaku_complex_get_image_url(): Error: Out of memory\n");
		return NULL;
	}

	strncpy(img_src_url, https, len_https);
	strncpy(&(img_src_url[len_https]), source_index, len_url);
	img_src_url[len_https + len_url] = '\0';

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *sankakucomplex_get_image_tags(char *web_content)
{
	/* constants for finding values */
	const char *tags_uuid = "<h5>Tags</h5>";
	const char *tags_end = "</ul>";
	const char *tag_category_uuid = "<li class=tag-type-";
	const char tag_category_end = '>';
	const char *tag_name_uuid = "href=\"/?tags=";
	const char tag_name_end = '"';

	/* offsets from actual value */
	const unsigned int initial_offset = strlen(tags_uuid);
	const unsigned int category_offset = strlen(tag_category_uuid);
	const unsigned int name_offset = strlen(tag_name_uuid);

	struct image_tag_db *tag_db = init_image_tag_db();
	/* initialize a tags database to store tags */
	struct llnode **tag_ptrs[6] = {
		&(tag_db->tags[0]),
		&(tag_db->tags[1]),
		&(tag_db->tags[2]),
		&(tag_db->tags[3]),
		&(tag_db->tags[4]),
		&(tag_db->tags[5])
	};

	/* set tag_contents to the beginning in which the tags begin */
	char *tag_contents = strstr(web_content, tags_uuid);
	/* set pointer to beginning of first tag */
	if (tag_contents == NULL)
		return tag_db;

	/* pointer pointing to the end of tag string */
	char *end_ptr;

	/* slice string at where all tags section ends */
	if ((end_ptr = strstr(tag_contents, tags_end)))
		*end_ptr = '\0';

	tag_contents = &(tag_contents[initial_offset]);

	while ((tag_contents = strstr(tag_contents, tag_category_uuid))) {

		tag_contents = &(tag_contents[category_offset]);

		/* get how far we are from the end of current tag */
		unsigned int category_end_distance = get_distance(tag_contents,
					tag_category_end);
		/* temporarily slice the string at the category_end position */
		tag_contents[category_end_distance] = '\0';
		/* get the tag type of the tag */
		unsigned tag_index = sankakucomplex_get_tag_type(tag_contents);
		/* restore sliced string */
		tag_contents[category_end_distance] = tag_category_end;

		/* move pointer to start of tag name */
		tag_contents = strstr(tag_contents, tag_name_uuid);
		tag_contents = &(tag_contents[name_offset]);
		/* get length of tag name */
		int tag_name_len = get_distance(tag_contents, tag_name_end);

		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(sizeof(char) * (tag_name_len + 1));
		if (tag_name == NULL)
			break;

		strncpy(tag_name, tag_contents, tag_name_len);
		tag_name[tag_name_len] = '\0';


		/* allocate memory for node */
		*(tag_ptrs[tag_index]) = malloc(sizeof(struct llnode));

		(*(tag_ptrs[tag_index]))->next = NULL;
		(*(tag_ptrs[tag_index]))->data = tag_name;

		/* set tag_ptrs to its next value */
		tag_ptrs[tag_index] = &((*(tag_ptrs[tag_index]))->next);
		/* increment the amount of tags in this category we currently
		 * found */
		(tag_db->tag_size[tag_index])++;

		/* move on to next tag */
		tag_contents = &(tag_contents[tag_name_len]);
	}
	/* unslice the string */
	if (end_ptr)
		*end_ptr = tags_end[0];

	return tag_db;
}

unsigned int sankakucomplex_get_tag_type(char *tag_contents)
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
