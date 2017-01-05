/* struct for storing html content */
struct html_data {
	char *data;
	size_t size;
};

/* struct representing a similar image to a parent image */
struct similar_image {
	char *link;
	unsigned int similarity;
	unsigned int dimensions[2];
};

/* struct representing a database of similar images
 * all to the same parent image.
 */
struct similar_image_db {
	struct similar_image *img_db[16];
	unsigned int size;
};

