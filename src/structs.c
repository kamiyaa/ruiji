#include <stdlib.h>

#include "structs.h"

struct image_tag_db *init_image_tag_db()
{
	struct image_tag_db *tag_db = malloc(sizeof(struct image_tag_db));
	if (tag_db == NULL) {
		return NULL;
	}

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

void similar_image_result_free(struct similar_image_result *image)
{
	if (image) {
		if (image->post_link)
			free(image->post_link);
		free(image);
	}
}

void similar_image_llnode_free(struct similar_image_llnode *head)
{
	while (head) {
		struct similar_image_llnode *tmp = head;
		head = head->next;
		similar_image_result_free(tmp->image);
		free(tmp);
	}
}

void similar_image_list_free(struct similar_image_list *image_list)
{
	if (image_list) {
		similar_image_llnode_free(image_list->head);
		free(image_list);
	}
}
