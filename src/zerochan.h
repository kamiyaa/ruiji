/* Given a http://www.zerochan.net/ url,
 * parse the html to get the source image url
 */
char *zerochan_get_image_url(char *html_content);

/* Given the html of a zerochan post webpage, parse the html
 * to get the tags of the image
 */
struct image_tag_db *zerochan_get_image_tags_html(char *html_content);
