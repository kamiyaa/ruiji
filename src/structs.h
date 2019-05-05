#ifndef STRUCTS_H
#define STRUCTS_H

struct html_data;
struct image_tag_db;
struct llnode;
struct similar_image;
struct similar_image_llnode;


/* struct for storing downloaded web content */
struct html_data {
	char *data;
	size_t size;
	size_t realsize;
};

/* struct for holding all image tags */
struct image_tag_db {
	/* artist, character, circle, copyright, meta, general tags respectively */
	struct llnode *tags[6];
	/* how many of those tags are there */
	unsigned int tag_size[6];
};

/* basic linked list node struct */
struct llnode {
	char *data;
	struct llnode *next;
};

/* struct representing a similar image to a parent image */
struct similar_image_result {
	char *post_link;
	unsigned short similarity;
	unsigned int dimensions[2];
};

struct similar_image_list {
	struct similar_image_llnode *head;
	unsigned int size;
};

/* linked list holding similar_image structs as data */
struct similar_image_llnode {
	struct similar_image_result *image;
	struct similar_image_llnode *next;
};

/* initialize an empty database of tags */
struct image_tag_db *init_image_tag_db();

void free_image_tags(struct image_tag_db *tags_db);
void free_linked_list(struct llnode *head);

void similar_image_result_free(struct similar_image_result *image);
void similar_image_llnode_free(struct similar_image_llnode *image_list);
void similar_image_list_free(struct similar_image_list *image_list);


#endif
