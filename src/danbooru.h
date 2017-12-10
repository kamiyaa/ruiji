/* given a danbooru url, parse it to generate a url
 * to a json version of the webpage
 */
char *danbooru_generate_api_url(char *url);

/* Given the json of a danbooru post webpage, parse the json
 * to get the source image url
 */
char *danbooru_get_image_url_json(char *web_content);


/* Given the json of a danbooru post webpage, parse the json
 * to get the tags of the image
 */
struct image_tag_db *danbooru_get_image_tags_json(char *web_content);

struct llnode *danbooru_parse_tags_json(char *tag_pattern, char *web_content,
	unsigned int *size);
