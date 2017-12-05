#ifndef STRUCTS_H

struct html_data;
struct image_tag_db;
struct llnode;
struct similar_image;
struct similar_image_llnode;


/* struct for storing downloaded web content */
struct html_data {
	char *data;
	size_t size;
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
struct similar_image {
	char *post_link;
	unsigned short similarity;
	unsigned int dimensions[2];
};

/* linked list holding similar_image structs as data */
struct similar_image_llnode {
	struct similar_image *image;
	struct similar_image_llnode *next;
};

#endif
