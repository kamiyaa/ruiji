#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "danbooru.h"
#include "parser.h"

char *danbooru_generate_api_url(char *url)
{
	/* constants used to find values */
	const char json_fformat[] = ".json";
	const unsigned int json_len = strlen(json_fformat);
	unsigned int url_len = strlen(url);

	char *api_url = malloc(CHAR_SIZE * (url_len + json_len + 1));

	strncpy(api_url, url, url_len);
	strncpy(&(api_url[url_len]), json_fformat, json_len + 1);

	return api_url;
}

char *danbooru_get_image_url(char *html_content)
{
	/* constants used to find values */
	const char danbooru_url[] = "https://danbooru.donmai.us";

	const char source_uuid[] = "Size: <a href=\"";
	const char source_end = '"';

	const unsigned int source_uuid_len = strlen(source_uuid);
	const unsigned int danbooru_url_len = strlen(danbooru_url);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, source_uuid);

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
		img_src_url = malloc(CHAR_SIZE *
					(url_len + danbooru_url_len + 1));

		strncpy(img_src_url, danbooru_url, danbooru_url_len);
		strncpy(&(img_src_url[danbooru_url_len]),
			source_index, url_len + 1);
	}
	else {
		fprintf(stderr,
			"danbooru_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

char *danbooru_get_image_url_json(char *json_content)
{

	/* constants used to find values */
	const char danbooru_url[] = "https://danbooru.donmai.us";
	const unsigned int danbooru_url_len = strlen(danbooru_url);

	const char source_uuid[] = "\"file_url\":\"";
	const unsigned int source_uuid_len = strlen(source_uuid);

	const char source_end = '"';

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	char *source_index = strstr(json_content, source_uuid);
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
		img_src_url = malloc(CHAR_SIZE *
					(url_len + danbooru_url_len + 1));
		strncpy(img_src_url, danbooru_url, danbooru_url_len);
		strncpy(&(img_src_url[danbooru_url_len]),
			source_index, url_len);
		img_src_url[url_len + danbooru_url_len] = '\0';
	}
	else {
		fprintf(stderr,
			"danbooru_get_image_url_json(): Error: Failed to parse website\n");
	}
	return img_src_url;
}

struct image_tag_db *danbooru_get_image_tags(char *html_content)
{
	/* constants for finding values */
	const char tags_uuid[] = "<section id=\"tag-list\">";
	const char tags_end[] = "</section>";
	const char tag_category_uuid[] = "category-";
	const char tag_name_uuid[] = "href=\"/posts?tags=";
	const char tag_name_end = '"';

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
		/* get the tag type of the tag */
		unsigned int tag_index = danbooru_get_tag_type(tag_contents[0]);

		/* move pointer to the start of the tag name */
		tag_contents = strstr(tag_contents, tag_name_uuid);
		tag_contents = &(tag_contents[name_offset]);

		/* get where the tag name ends is */
		int tag_name_len = get_distance(tag_contents, tag_name_end);

		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(CHAR_SIZE * (tag_name_len + 1));
		strncpy(tag_name, tag_contents, tag_name_len);
		tag_name[tag_name_len] = '\0';


		/* allocate memory for node */
		*(tag_ptrs[tag_index]) = malloc(LLNODE_SIZE);

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

	return tag_db;
}

struct image_tag_db *danbooru_get_image_tags_json(char *json_content)
{
	char artist_tag_uuid[] = "\"tag_string_artist\":\"";
	char character_tag_uuid[] = "\"tag_string_character\":\"";
	char copyright_tag_uuid[] = "\"tag_string_copyright\":\"";
	char general_tag_uuid[] = "\"tag_string_general\":\"";

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();

	tag_db->tags[0] = danbooru_parse_tags_json(artist_tag_uuid, json_content,
				&(tag_db->tag_size[0]));
	tag_db->tags[1] = danbooru_parse_tags_json(character_tag_uuid, json_content,
				&(tag_db->tag_size[1]));
	tag_db->tags[3] = danbooru_parse_tags_json(copyright_tag_uuid, json_content,
				&(tag_db->tag_size[3]));
	tag_db->tags[5] = danbooru_parse_tags_json(general_tag_uuid, json_content,
				&(tag_db->tag_size[5]));

	return tag_db;
}

struct llnode *danbooru_parse_tags_json(char *tag_pattern, char *json_content,
	unsigned int *size)
{
	int tag_name_len = -1;
	char *ptr_end = NULL;

	int tag_len = strlen(tag_pattern);

	/* look for the pattern in json_content */
	char *json_ptr = strstr(json_content, tag_pattern);
	if (json_ptr) {
		/* if we found the pattern,
		 * set json_ptr to the end of the pattern */
		json_ptr = &(json_ptr[tag_len]);

		/* get the end of this json property, set
		 * ptr_end to it and set it to a null terminator */
		int tag_end = get_distance(json_ptr, ',');
		ptr_end = &(json_ptr[tag_end]);
		*ptr_end = '\0';

		/* set the character before it to be a space */
		json_ptr[tag_end - 1] = ' ';
		/* get the next space offset and set tag_name_len to it */
		tag_name_len = get_distance(json_ptr, ' ');
	}

	struct llnode *tags = NULL;
	struct llnode **tags_ptr = &(tags);

	while (tag_name_len > 0) {
		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(CHAR_SIZE * (tag_name_len + 1));
		/* copy tag name to tag_name */
		strncpy(tag_name, json_ptr, tag_name_len);
		tag_name[tag_name_len] = '\0';


		/* allocate memory for a llnode */
		*tags_ptr = malloc(LLNODE_SIZE);
		/* set next to NULL and data to tag_name */
		(*tags_ptr)->next = NULL;
		(*tags_ptr)->data = tag_name;
		/* move tags_ptr to next */
		tags_ptr = &((*tags_ptr)->next);


		/* move json_ptr to next tag starting position */
		json_ptr = &(json_ptr[tag_name_len + 1]);
		/* get the next ' ' offset */
		tag_name_len = get_distance(json_ptr, ' ');

		(*size)++;
	}
	/* put comma back, un null terminating the string */
	if (ptr_end) {
		*ptr_end = ',';
	}
	return tags;
}

unsigned int danbooru_get_tag_type(char category)
{
	unsigned int tag_index;
	/* figure out what tag type this is */
	switch (category) {
	/* artist */
	case '1':
		tag_index = 0;
		break;
	/* copyright */
	case '3':
		tag_index = 3;
		break;
	/* character */
	case '4':
		tag_index = 1;
		break;
	/* general */
	case '0':
	default:
		tag_index = 5;
		break;
	}
	return tag_index;
}
