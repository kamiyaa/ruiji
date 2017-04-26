#include "udload.h"

/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *create_sim_image(char *url_begin, unsigned short similarity,
					unsigned int x, unsigned int y);

void free_image_tags(struct image_tag_db *tags_db);
void free_linked_list(struct ll_node *head);

/* Frees the allocated memory for a similar_image_db */
void free_similar_image_db(struct similar_image_db *sim_db);

/* get the character distance from the beginning of the string to find */
int get_distance(char *string, char find);

struct image_tag_db *get_image_tags(char *link);

/* Given the full link of a website,
 * parse the link to get the file name
 */
char *get_server_file_name(char *web_url, char stop);

/* Given a link, get the source image url from its html
 * if a stop sequence is needed for extracting the file
 * name later, assign it to stop_seq
 */
char *get_source_image_url(char *url, char *stop_seq);

struct image_tag_db *init_image_tag_db(void);

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_percent_similar(char *contents, unsigned short *similarity);

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_xy_img_dimensions(char* contents, unsigned int *x, unsigned int *y);

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in the struct similar_image_db *sim_db
 */
void populate_sim_db(struct similar_image_db *sim_db, char *html_content,
			unsigned short similar_threshold);

