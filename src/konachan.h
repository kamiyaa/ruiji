/* Given a http://konachan.com/ url,
 * parse the html to get the source image url
 */
char *konachan_get_image_url(char *html_content);

struct image_tag_db *konachan_get_image_tags(char *html_content);
