#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "sankakucomplex.h"
#include "parser.h"

#define HTTPS "https:"
#define SANKAKUCOMPLEX_SOURCE_ID "<li>Original: <a href=\""
#define SANKAKUCOMPLEX_TAG_ID " "

/* Given a https://chan.sankakucomplex.com url,
 * parse the html to get the source image url
 */
char *sankakucomplex_get_image_url(char *html_content)
{
	static int https_len = sizeof(HTTPS);

	/* initialize the image source url to be returned later */
	char *img_src_url = NULL;

	/* Find the source image link */
	char *source_index = strstr(html_content, SANKAKUCOMPLEX_SOURCE_ID);

	/* If source image link is found,
	 * add http extension to it and return it */
	if (source_index) {
		/* move source_index pointer to the beginning of
		 * the source image url */
		source_index = &source_index[strlen(SANKAKUCOMPLEX_SOURCE_ID)];
		int url_len = get_distance(source_index, '"');

		/* allocate enough memory to hold the image source url,
		 * then copy the url over to img_src_url and return it */
		img_src_url = malloc(CHARSIZE *
					(url_len + https_len + 1));
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTPS);
		strncat(img_src_url, source_index, url_len);
	}
	else
		fprintf(stderr,
			"sankaku_complex_get_image_url(): Error: Failed to parse website\n");

	/* return the image source url */
	return img_src_url;
}

struct image_tag_db *sankakucomplex_get_image_tags(char *html_content)
{
	/* set tag_ptr to the beginning in which the tags begin */
	char *tag_contents = strstr(html_content, SANKAKUCOMPLEX_TAG_ID);
	tag_contents = &(tag_contents[strlen(SANKAKUCOMPLEX_TAG_ID)]);

	struct image_tag_db *tag_db = init_image_tag_db();
	struct ll_node *tag_ptrs[6] = { NULL, NULL, NULL, NULL, NULL, NULL };

	return tag_db;
}
