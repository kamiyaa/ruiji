/* Given a https://chan.sankakucomplex.com url,
 * parse the html to get the source image url
 */
char *sankakucomplex_get_image_url(char *html_content);

struct image_tag_db *sankakucomplex_get_image_tags(char *html_content);

unsigned int sankakucomplex_get_tag_type(char *tag_contents);
