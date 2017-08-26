#ifndef STRUCTS_H

struct html_data;
struct image_tag_db;
struct llnode;
struct similar_image;
struct similar_image_db;


/* struct for storing html content */
struct html_data {
	char *data;
	size_t size;
};

/* struct for holding all image tags */
struct image_tag_db {
	/* artist, character, circle, copyright, fault, general tags respectively */
	struct ll_node *tags[6];
	unsigned int tag_size[6];
};

/* basic linked list node struct */
struct ll_node {
	char *data;
	struct ll_node *next;
};

/* struct representing a similar image to a parent image */
struct similar_image {
	char *link;
	unsigned short similarity;
	unsigned int dimensions[2];
};

/* struct representing a database of similar images
 * all to the same parent image.
 */
struct similar_image_db {
	struct similar_image **images;
	unsigned short size;
};

#endif
