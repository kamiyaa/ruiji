/* Given a http://gelbooru.com/ url,
 * parse the html to get the source image url
 */
char *gelbooru_get_image_url(char *web_content);

struct image_tag_db *gelbooru_get_image_tags(char *web_content);

unsigned int gelbooru_get_tag_type(char *tag_contents);
