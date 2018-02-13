#include <stdlib.h>

#include "structs.h"

struct image_tag_db *init_image_tag_db()
{
	/* initialize a image tag database to store all the tags */
	struct image_tag_db *tag_db = malloc(sizeof(struct image_tag_db));
	/* set all values to 0 and NULL */
	for (int i = 0; i < 6; i++) {
		tag_db->tags[i] = NULL;
		tag_db->tag_size[i] = 0;
	}
	return tag_db;
}



void free_image_tags(struct image_tag_db *tags_db)
{
	for (int i = 0; i < 6; i++)
		free_linked_list(tags_db->tags[i]);
	free(tags_db);
}

void free_linked_list(struct llnode *head)
{
	struct llnode *ptr = head;
	struct llnode *prev;
	while (ptr) {
		prev = ptr;
		ptr = ptr->next;
		free(prev->data);
		free(prev);
	}
}

void free_similar_image(struct similar_image *image)
{
	if (image) {
		if (image->post_link)
			free(image->post_link);
		free(image);
	}
}

void free_similar_image_list(struct similar_image_llnode *image_list)
{
	while (image_list) {
		struct similar_image_llnode *tmp = image_list;
		image_list = image_list->next;
		free_similar_image(tmp->image);
		free(tmp);
	}
}
