#include "interface.h"
#include "udload.h"
#include "domains.h"

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in a linked list.
 */
struct similar_image_llnode *create_image_list(char *html_content,
	unsigned short similar_threshold);


/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *create_sim_image(char *web_url,
	unsigned short similarity, unsigned int xpx, unsigned int ypx);


/* given a known domain type and its link, generate an api
 * link that is much easier to parse later
 */
char *generate_api_link(enum domain_t id, char *post_link);


/* get how far a given char is from the beginning of the string
 * -1 if char was not found. Will keep going until null terminator or found char.
 */
int get_distance(char *string, char find);


/* Given a link, get the source image url from its html
 * if a stop sequence is needed for extracting the file
 * name later, assign it to stop_seq
 */
char *get_image_source_url(enum domain_t id, char *html_content, char *stop_seq);


/* given a known domain type and its downloaded content,
 * parse for the image's tags
 */
struct image_tag_db *get_image_tags(enum domain_t id, char *html_content);


/* get unique id for known domain names */
enum domain_t get_domain_uid(char *link);


/* Given the full link of a website,
 * parse the link to get the file name
 */
char *get_server_file_name(char *web_url, char stop);


/* initialize an empty database of tags */
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


/* Frees the allocated memory for a image_tag_db struct */
void free_image_tags(struct image_tag_db *tags_db);
/* Frees the allocated memory for a linked list of llnode */
void free_linked_list(struct llnode *head);
/* Frees the allocated memory for a similar_image struct */
void free_similar_image(struct similar_image *image);
/* Frees the allocated memory for a linked list of similar_image_llnode */
void free_similar_image_list(struct similar_image_llnode *image_list);

