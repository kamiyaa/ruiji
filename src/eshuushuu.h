/* Given a http://e-shuushuu.net/ url,
 * parse the html to get the source image url
 */
char *eshuushuu_get_image_url(char *html_content);

struct image_tag_db *eshuushuu_get_image_tags(char *html_content);

struct llnode *eshuushuu_parse_tags_html(char *tag_pattern, char *html_content,
	unsigned int *size);
