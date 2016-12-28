#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "parser.h"

#define HTTPS "https:"
#define SANKAKU_COMPLEX_SOURCE_ID "<li>Original: <a href=\""

/* Given a https://chan.sankakucomplex.com url,
 * parse the html to get the source image url
 */
char* sankaku_complex_get_image_url(char *web_url)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *index = strstr(html_content, SANKAKU_COMPLEX_SOURCE_ID);
	char *img_src_url;

	/* If found, add http extension to it and return it */
	if (index) {
		index = &index[strlen(SANKAKU_COMPLEX_SOURCE_ID)];
		char *walker = index;
		while (*walker != '"')
			walker = &walker[1];
		walker[0] = '\0';

		unsigned int url_len = strlen(index) + strlen(HTTPS) + 1;
		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		strcat(img_src_url, HTTPS);
		strcat(img_src_url, index);
	}
	else {
		printf("Error: sankaku_complex_get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		return "ERROR";
	}
	return img_src_url;
}
