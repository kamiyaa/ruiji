#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "gelbooru.h"
#include "helpers.h"

/* Given a http://gelbooru.com/ url,
 * parse the html to get the source image url
 */
char *gelbooru_get_image_url(char *web_content)
{
	const char *source_uuid = ">Resize image</a></li><li><a href=\"";
	const unsigned int source_uuid_len = strlen(source_uuid);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(web_content, source_uuid);

	/* If found, add the danbooru url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[source_uuid_len];
		/* get the length of the source image url */
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) * (url_len + 1));
		strncpy(img_src_url, source_index, url_len);
		img_src_url[url_len] = '\0';
	}
	else {
		fprintf(stderr,
			"gelbooru_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *gelbooru_get_image_tags(char *web_content)
{
	const char *tags_uuid = "<h3>Tags</h3>";
	const char *tags_end = "<br />";
	const char *tag_category_uuid = "class=\"tag-type-";
	const char tag_category_end = '"';
	const char *tag_name_uuid = "page=post&amp;s=list&amp;tags=";
	const char tag_name_end = tag_category_end;

	/* offsets from actual value */
	const unsigned int initial_offset = strlen(tags_uuid);
	const unsigned int category_offset = strlen(tag_category_uuid);
	const unsigned int name_offset = strlen(tag_name_uuid);

	/* pointer pointing to the end of tag string */
	char *end_ptr = NULL;

	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(web_content, tags_uuid);
	if (tag_contents) {
		tag_contents = &(tag_contents[initial_offset]);
		end_ptr = strstr(tag_contents, tags_end);
		if (end_ptr)
			end_ptr[0] = '\0';
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

		/* get length of tag name */
		int tag_name_len = get_distance(tag_contents, tag_name_end);

		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(sizeof(char) * (tag_name_len + 1));
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

		tag_contents = &(tag_contents[tag_name_len]);
		tag_contents = strstr(tag_contents, tag_category_uuid);
	}
	/* unslice the string */
	if (end_ptr)
		end_ptr[0] = tags_end[0];

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
