#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "danbooru.h"
#include "util.h"

char *danbooru_generate_api_url(char *url)
{
	/* constants used to find values */
	const char *json_fformat = ".json";
	const unsigned int json_len = strlen(json_fformat);
	unsigned int url_len = strlen(url);

	char *api_url = malloc(sizeof(char) * (url_len + json_len + 1));
	if (api_url == NULL) {
		fprintf(stderr,
			"danbooru_generate_api_url(): Error: Out of memory\n");
		return NULL;
	}

	strncpy(api_url, url, url_len);
	strncpy(&(api_url[url_len]), json_fformat, json_len + 1);

	return api_url;
}

char *danbooru_get_image_url_json(char *web_content)
{
	/* constants used to find values */
	const char *danbooru_url = "https://danbooru.donmai.us";
	const unsigned int danbooru_url_len = strlen(danbooru_url);

	const char *source_uuid = "\"file_url\":\"";
	const unsigned int source_uuid_len = strlen(source_uuid);

	const char source_end = '"';

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	char *source_index = strstr(web_content, source_uuid);
	/* If source image link is found,
	 * add the danbooru url to it and return it */
	if (source_index == NULL) {
		fprintf(stderr,
			"danbooru_get_image_url_json(): Error: Failed to parse website\n");
		return NULL;
	}

	/* move source_index pointer to the beginning of
	 * the source image url */
	source_index = &(source_index[source_uuid_len]);
	/* get the length of the source image url */
	int url_len = get_distance(source_index, source_end);

	/* allocate enough memory to hold the image source url,
	 * then copy the url over to img_src_url and return it */
	img_src_url = malloc(sizeof(char) *
				(url_len + danbooru_url_len + 1));
	if (img_src_url == NULL) {
		fprintf(stderr,
			"danbooru_get_image_url_json(): Error: Out of memory\n");
		return NULL;
	}

	int length = 0;
	img_src_url[length] = '\0';
	if (strstr(source_index, ".donmai.us") == NULL) {
		strncat(img_src_url, danbooru_url, danbooru_url_len);
		length += danbooru_url_len;
	}
	strncat(img_src_url, source_index, url_len);
	length += url_len;

	return img_src_url;
}

struct image_tag_db *danbooru_get_image_tags_json(char *web_content)
{
	char *artist_tag_uuid = "\"tag_string_artist\":\"";
	char *character_tag_uuid = "\"tag_string_character\":\"";
	char *copyright_tag_uuid = "\"tag_string_copyright\":\"";
	char *general_tag_uuid = "\"tag_string_general\":\"";
	char *meta_tag_uuid = "\"tag_string_meta\":\"";

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();
	if (tag_db == NULL) {
		perror("malloc");
		exit(1);
	}

	/* populate artist tags */
	tag_db->tags[0] = danbooru_parse_tags_json(
		artist_tag_uuid,
		web_content,
		&(tag_db->tag_size[0])
		);

	/* populate character tags */
	tag_db->tags[1] = danbooru_parse_tags_json(
		character_tag_uuid,
		web_content,
		&(tag_db->tag_size[1])
		);

	/* populate copyright tags */
	tag_db->tags[3] = danbooru_parse_tags_json(
		copyright_tag_uuid,
		web_content,
		&(tag_db->tag_size[3])
		);

	/* populate meta tags */
	tag_db->tags[4] = danbooru_parse_tags_json(
		meta_tag_uuid,
		web_content,
		&(tag_db->tag_size[4])
		);

	/* populate general tags */
	tag_db->tags[5] = danbooru_parse_tags_json(
		general_tag_uuid,
		web_content,
		&(tag_db->tag_size[5])
		);

	return tag_db;
}

struct llnode *danbooru_parse_tags_json(char *tag_pattern, char *web_content,
	unsigned int *size)
{
	const char tags_end = ',';

	/* look for the pattern in web_content */
	char *json_ptr = strstr(web_content, tag_pattern);
	if (json_ptr == NULL)
		return NULL;

	/* if we found the pattern, set json_ptr to the end
	 * of the pattern */
	json_ptr = &(json_ptr[strlen(tag_pattern)]);

	/* get the end of this json property, set
	 * end_ptr to it and set it to a null terminator */
	int tag_end = get_distance(json_ptr, tags_end);

	char *end_ptr;
	if ((end_ptr = &(json_ptr[tag_end])))
		*end_ptr = '\0';

	/* set the character before it to be a space */
	json_ptr[tag_end - 1] = ' ';

	struct llnode *tags = NULL;
	struct llnode **tags_ptr = &(tags);

	int tag_name_len;
	/* get the next space offset and set tag_name_len to it */
	while ((tag_name_len = get_distance(json_ptr, ' ')) > 0) {
		/* allocate enough memory for the tag name + null terminator */
		char *tag_name = malloc(sizeof(char) * (tag_name_len + 1));
		if (tag_name == NULL)
			break;

		/* copy tag name to tag_name */
		strncpy(tag_name, json_ptr, tag_name_len);
		tag_name[tag_name_len] = '\0';


		/* allocate memory for a llnode */
		*tags_ptr = malloc(sizeof(struct llnode));
		if (tags_ptr == NULL)
			break;

		/* set next to NULL and data to tag_name */
		(*tags_ptr)->next = NULL;
		(*tags_ptr)->data = tag_name;
		/* move tags_ptr to next */
		tags_ptr = &((*tags_ptr)->next);
		(*size)++;

		/* move json_ptr to next tag starting position */
		json_ptr = &(json_ptr[tag_name_len + 1]);
	}
	/* put comma back, unterminating the string */
	if (end_ptr)
		*end_ptr = tags_end;

	return tags;
}
