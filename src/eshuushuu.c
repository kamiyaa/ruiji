#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "eshuushuu.h"
#include "parser.h"

/* Given a http://e-shuushuu.net url,
 * parse the html to get the source image url
 */
char *eshuushuu_get_image_url(char *web_content)
{
	/* constants used to find values */
	const char *eshuushuu_url = "http://e-shuushuu.net";
	const char *source_uuid = "<a class=\"thumb_image\" href=\"";
	const char source_end = '"';

	const unsigned int len_source_uuid = strlen(source_uuid);
	const unsigned int len_eshuushuu_url = strlen(eshuushuu_url);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(web_content, source_uuid);

	/* If found, add the website url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &(source_index[len_source_uuid]);
		/* get the length of the source image url */
		int url_len = get_distance(source_index, source_end);

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(sizeof(char) *
					(len_eshuushuu_url + url_len + 1));

		strncpy(img_src_url, eshuushuu_url, len_eshuushuu_url);
		strncpy(&(img_src_url[len_eshuushuu_url]), source_index, url_len);
		img_src_url[url_len + len_eshuushuu_url] = '\0';
	}
	else {
		fprintf(stderr,
			"eshuushuu_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *eshuushuu_get_image_tags(char *web_content)
{

	char *artist_tag_uuid = "Artist:";
	char *character_tag_uuid = "Characters:";
	char *copyright_tag_uuid = "Source:";
	char *general_tag_uuid = "Tags:";

	/* offset web_content first for easier parsing */
	web_content = strstr(web_content, "<dt>Dimensions:</dt>");

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();

	/* populate artist tags */
	tag_db->tags[0] = eshuushuu_parse_tags_html(
				artist_tag_uuid,
				web_content,
				&(tag_db->tag_size[0])
				);

	/* populate character tags */
	tag_db->tags[1] = eshuushuu_parse_tags_html(
				character_tag_uuid,
				web_content,
				&(tag_db->tag_size[1])
				);

	/* populate copyright tags */
	tag_db->tags[3] = eshuushuu_parse_tags_html(
				copyright_tag_uuid,
				web_content,
				&(tag_db->tag_size[3])
				);

	/* populate general tags */
	tag_db->tags[5] = eshuushuu_parse_tags_html(
				general_tag_uuid,
				web_content,
				&(tag_db->tag_size[5])
				);

	return tag_db;
}

struct llnode *eshuushuu_parse_tags_html(char *tag_pattern, char *web_content,
	unsigned int *size)
{
	const char *tags_uuid = "<span class='tag'>\"<a href=\"";
	const char *tags_end = "</dd>";
	const int tag_uuid_len = strlen(tags_uuid);

	char *html_ptr = strstr(web_content, tag_pattern);
	char *end_ptr = strstr(html_ptr, tags_end);

	/* if we found an ending point to the string, terminate it there */
	if (end_ptr) {
		end_ptr[0] = '\0';

		/* move html_ptr to next tag starting position */
		html_ptr = strstr(html_ptr, tags_uuid);
	}

	struct llnode *tags = NULL;
	struct llnode **tags_ptr = &(tags);

	while (html_ptr) {
		/* move html_ptr to end of tags_uuid pattern */
		html_ptr = &(html_ptr[tag_uuid_len]);
		/* get where next '>' is and move html_ptr 1 char past it */
		int tag_start_distance = get_distance(html_ptr, '>');
		html_ptr = &(html_ptr[tag_start_distance+1]);

		/* get the next '<' offset and set tag_name_len to it */
		int tag_name_len = get_distance(html_ptr, '<');

		/* allocate enough memory for the tag name +
		 * null terminator */
		char *tag_name = malloc(sizeof(char) * (tag_name_len + 1));
		/* copy tag name to tag_name */
		strncpy(tag_name, html_ptr, tag_name_len);
		tag_name[tag_name_len] = '\0';

		/* allocate memory for a llnode */
		*tags_ptr = malloc(sizeof(struct llnode));
		/* set next to NULL and data to tag_name */
		(*tags_ptr)->next = NULL;
		(*tags_ptr)->data = tag_name;
		/* move tags_ptr to next */
		tags_ptr = &((*tags_ptr)->next);

		/* move html_ptr to next tag starting position */
		html_ptr = strstr(html_ptr, tags_uuid);

		(*size)++;
	}

	/* put value back, unterminating the string */
	if (end_ptr)
		*end_ptr = tags_end[0];

	return tags;
}
