#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "danbooru.h"
#include "parser.h"

#define DANBOORU_URL "https://danbooru.donmai.us"
#define DANBOORU_SOURCE_ID "Size: <a href=\""
#define DANBOORU_TAG_ID "<section id=\"tag-list\">"

/* Given a https://danbooru.donmai.us url,
 * parse the html to get the source image url
 */
char *danbooru_get_image_url(char *html_content)
{
	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, DANBOORU_SOURCE_ID);

	/* If source image link is found,
	 * add the danbooru url to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(DANBOORU_SOURCE_ID)];
		/* get the length of the source image url */
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHARSIZE *
					(url_len + strlen(DANBOORU_URL) + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, DANBOORU_URL);
		strncat(img_src_url, source_index, url_len);
	}
	else {
		fprintf(stderr,
			"danbooru_get_image_url(): Error: Failed to parse website\n");
	}

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *danbooru_get_image_tags(char *html_content)
{
	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(html_content, DANBOORU_TAG_ID);
	tag_contents = &(tag_contents[strlen(DANBOORU_TAG_ID)]);

	char *slice_pos = strstr(tag_contents, "</section>");
	slice_pos[0] = '\0';

	/* initialize a tags database to store tags */
	struct image_tag_db *tag_db = init_image_tag_db();
	struct ll_node *tag_ptrs[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	/* store tag name category and size of tag name */
	unsigned int tag_index;
	unsigned int tag_name_size;
	int caret_distance;

	/* set pointer to beginning of first tag */
	tag_contents = strstr(tag_contents, "category-");
	int category_offset = strlen("category-");
	int tag_name_offset = strlen("href=\"/posts?tags=");
	while (tag_contents) {
		/* move to beginning of tag type */
		tag_contents = &(tag_contents[category_offset]);
		/* get the tag type of the tag */
		tag_index = danbooru_get_tag_type(tag_contents[0]);

		/* move pointer to the start of the tag name */
		tag_contents = strstr(tag_contents, "class=\"search-tag\"");
		tag_contents = strstr(tag_contents, "href=\"/posts?tags=");
		tag_contents = &(tag_contents[tag_name_offset]);

		/* get where the tag name ends is */
		tag_name_size = get_distance(tag_contents, '"');
		tag_contents[tag_name_size] = '\0';

		/* create the linked list node to store the information */
		if (!(tag_db->tags[tag_index])) {
			/* malloc memory for node */
			tag_db->tags[tag_index] = malloc(sizeof(struct ll_node));
			/* malloc memory for char array in node */
			tag_db->tags[tag_index]->data = malloc(sizeof(char) * (tag_name_size + 1));
			/* set first element to \0 */
			tag_db->tags[tag_index]->data[0] = '\0';
			/* concatentate tag name to char array */
			strncat(tag_db->tags[tag_index]->data,
					tag_contents, tag_name_size);

			/* set tag_ptrs to it */
			tag_ptrs[tag_index] = tag_db->tags[tag_index];
		}
		else {
			/* malloc memory for node */
			tag_ptrs[tag_index]->next = malloc(sizeof(struct ll_node));
			/* malloc memory for char array in node */
			tag_ptrs[tag_index]->next->data = malloc(sizeof(char) * (tag_name_size + 1));
			/* set first element to \0 */
			tag_ptrs[tag_index]->next->data[0] = '\0';
			strncat(tag_ptrs[tag_index]->next->data,
				tag_contents, tag_name_size);

			/* set tag_ptrs to its next value */
			tag_ptrs[tag_index] = tag_ptrs[tag_index]->next;
		}
		/* change value back to " */
		tag_contents[tag_name_size] = '"';

		/* increment the amount of tags in this category we currently
		 * found */
		(tag_db->tag_size[tag_index])++;

		tag_contents = &(tag_contents[tag_name_size]);
		tag_contents = strstr(tag_contents, "category-");
	}

	return tag_db;
}

unsigned int danbooru_get_tag_type(char category) {
	unsigned int tag_index;
	/* figure out what tag type this is */
	switch (category) {
	/* general */
	case '0':
		tag_index = 5;
		break;
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
	default:
		tag_index = 5;
		break;
	}
	return tag_index;
}
