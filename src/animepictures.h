
char *animepictures_get_image_url(char *web_content);

struct image_tag_db *animepictures_get_image_tags(char *web_content);

struct llnode *animepictures_parse_tags(char *tag_pattern, char *web_content,
	unsigned int *size);
