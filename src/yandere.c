#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define YANDERE_JPEG "<li><a class=\"original-file-changed\" id=\"highres\" href=\""
#define YANDERE_PNG "<li><a class=\"original-file-unchanged\" id=\"png\" href=\""

/* Get size of char to prevent excessive function calling */
unsigned int char_size = sizeof(char);

char* yandere_get_image_url(char *web_url)
{
	char *html_content = get_html(web_url, "Download PNG");

	char *index = strstr(html_content, YANDERE_PNG);
	char *img_src_url;

	if (index) {
		char *walker = index + char_size * strlen(YANDERE_PNG);
		img_src_url = walker;
		while (*walker != '"')
			walker = &walker[1];
		walker[0] = '\0';
	}
	else if (strstr(html_content, YANDERE_JPEG)) {
		index = strstr(html_content, YANDERE_JPEG);
	}
	else {
		printf("Error: Failed to parse %s\n", web_url);
		return "ERROR";
	}
	return imge_src_url;
}
