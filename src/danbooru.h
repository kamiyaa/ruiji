/* Given a http://danbooru.donmai.us url, parse the html
 * to get the source image url
 */
char *danbooru_get_image_url(char *html_content);

struct image_tag_db *danbooru_get_image_tags(char *html_content);
