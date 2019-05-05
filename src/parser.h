#include "domains.h"

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in a linked list.
 */
struct similar_image_list *create_image_list(char *web_content,
	unsigned short similar_threshold);


/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image_result *new_similar_image_result(char *web_url,
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
 */
char *parse_download_url(enum domain_t id, char *html_content);


/* given a known domain type and its downloaded content,
 * parse for the image's tags
 */
struct image_tag_db *get_image_tags(enum domain_t id, char *web_content);


/* get unique id for known domain names */
enum domain_t parse_domain(char *link);


/* Given the full link of a website,
 * parse the link to get the file name
 */
char *parse_file_name(char *web_url);


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
