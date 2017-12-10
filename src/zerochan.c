#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "zerochan.h"
#include "parser.h"

/* Given a http://www.zerochan.net/ url,
 * parse the html to get the source image url
 */
char *zerochan_get_image_url(char *web_content)
{
	const char *source_uuid = "fullsizeUrl = '";
	const char source_end = '\'';

	const unsigned int len_source_uuid = strlen(source_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(web_content, source_uuid);

	/* If found, return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &(source_index[len_source_uuid]);
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
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

struct image_tag_db *zerochan_get_image_tags_html(char *web_content)
{
	const char *tags_uuid = "alt=\"Tags: ";
	const char tag_name_uuid = ',';
	const char tags_end = '"';
	const unsigned int initial_offset = strlen(tags_uuid);

	/* length of tag name */
	int tag_name_len = 0;

	/* pointer pointing to the end of tag string */
	char *end_ptr = NULL;

	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(web_content, tags_uuid);
	if (tag_contents) {
		/* move pointer to start of tag */
		tag_contents = &(tag_contents[initial_offset]);

		/* get the end of tags section and slice string at the end */
		int tag_contents_end = get_distance(tag_contents, tags_end);


		/* slice string */
		end_ptr = &(tag_contents[tag_contents_end]);
		end_ptr[0] = '\0';

		tag_name_len = get_distance(tag_contents, tag_name_uuid);
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

	/* Zerochan has no association with tag types,
	 * therefore, all tags will be of general type
	 */
	int tag_type = 5;

	while (tag_name_len > 0) {

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
		tag_contents = &(tag_contents[tag_name_len + 1]);

		/* search for next tag */
		tag_name_len = get_distance(tag_contents, tag_name_uuid);
	}
	/* unslice the string */
	if (end_ptr)
		*end_ptr = tags_end;

	return tag_db;
}
