#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "parser.h"
#include "domains.h"

#define IQDB_RESULT_ID "match</th></tr><tr><td class='image'><a href=\""

/* Get size of char to prevent excessive function calling */
unsigned int char_size = sizeof(char);


size_t StoreData(void *contents, size_t size, size_t nmemb, struct html_data *userp)
{
	size_t realsize = size * nmemb;

	struct html_data *mem = (struct html_data *)userp;

	mem->data = realloc(mem->data, mem->size + realsize + 1);

	if (mem->data == NULL) {
		/* out of memory! */
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	memcpy(&(mem->data[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->data[mem->size] = 0;

	return realsize;
}


/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *
create_sim_image(char *url_begin, unsigned int similarity, unsigned int x, unsigned int y)
{
	struct similar_image *image = malloc(sizeof(struct similar_image));

	image->similarity = similarity;
	image->dimensions[0] = x;
	image->dimensions[1] = y;

	unsigned int url_len = strlen(url_begin);
	unsigned int additional_space;

	/* Format url to be complete with protocol, if none is provided */
	if (!strstr(url_begin, "http")) {
		char *prefix_add = "https:";

		if (strstr(url_begin, DANBOORU_DOMAIN))
			prefix_add = "http:";

		additional_space = strlen(prefix_add);
		image->link = malloc(sizeof(char) * (url_len + additional_space));
		image->link[0] = '\0';
		strcat(image->link, prefix_add);
		strcat(image->link, url_begin);
	}
	else
		image->link = url_begin;

	return image;
}


/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_percent_similar(char* contents, unsigned int *similarity)
{
	/* Get size of char to prevent excessive function calling */
	char *walker = contents;
	/* Parse for the similarity percentage of the image and slice it */
	walker = strstr(walker, "<td>");
	char *next_weblink = strstr(walker, " similarity");
	next_weblink[0] = '\0';

	/* Get the similarity percentage of image and set it to similarity */
	sscanf(walker, "<td>%u%%", similarity);

	/* Set the pointer to point to the rest of the sliced string
	 * and return it */
	next_weblink += char_size;

	return next_weblink;
}


/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_xy_img_dimensions(char* contents, unsigned int *x, unsigned int *y)
{
	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = contents;

	/* Find the dimensions of the image and point x,y to them */
	walker = strstr(walker, "class=\"service-icon\"");
	walker = strstr(walker, "<td>");

	/* Point to the next space in the string and slice it there */
	char *next_weblink = strstr(walker, " ");
	next_weblink[0] = '\0';

	/* Set x,y to the image dimensions */
	sscanf(walker, "<td>%u×%u ", x, y);

	/* Set the pointer to point to the rest of the sliced string
	 * and return it */
	next_weblink += char_size;

	return next_weblink;
}


/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in the struct similar_image_db *sim_db
 */
void populate_sim_db(struct similar_image_db *sim_db, char *html_content)
{
	/* Initialize the number of similar images in database to 0 */
	sim_db->size = 0;

	/* Check if matching string is found */
	char *index = strstr(html_content, IQDB_RESULT_ID);
	if (!index)
		return;

	/* Get length of the string we are looking for and search for it */
	unsigned int lookfor_len = strlen(IQDB_RESULT_ID);

	/* Go through the data and get every valid link to a similar image */
	while (index != NULL) {
		/* Get where the url of the website begins */
		char *url_begin = &index[lookfor_len];
		/* Set a arbitrary pointer to go through the string */
		char *walker = url_begin;

		/* Go through the string, looking for '"'. Once found,
		 * slice the string */
		while (*walker != '"')
			walker = &walker[1];
		walker[0] = '\0';

		/* Move on to the rest of the string */
		walker += char_size;


		unsigned int similarity = 0, x = 0, y = 0;
		/* Get the image x,y dimensions */
		walker = parse_xy_img_dimensions(walker, &x, &y);
		walker = parse_percent_similar(walker, &similarity);

		/* Create a new similar_image struct to hold all the image's information */
		struct similar_image *image = create_sim_image(url_begin, similarity, x, y);

		/* Add it to our database of similar images */
		sim_db->img_db[sim_db->size] = image;
		sim_db->size++;

		/* set the starting point of the string to the next valid weblink */
		index = strstr(walker, IQDB_RESULT_ID);
	}
}


/* Given the full link of a website,
 * fetch and return the html source of the website
 */
char *get_html(char *web_url)
{
	struct html_data web_data;
	/* will be grown as needed by the realloc above */
	web_data.data = malloc(1);
	/* no data at this point */
	web_data.size = 0;

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode res;

	if (curl_handle) {
		/* Set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, web_url);

		/* Set the user agent to chrome */
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "chrome/55.0.2883.75");

		/* Set the function to call when data is received */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, StoreData);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &web_data);

		res = curl_easy_perform(curl_handle);

		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

		}
		/* cleanup */
		curl_easy_cleanup(curl_handle);
	}
	return web_data.data;
}

/* Given the full link of a website,
 * parse the link to get the file name
 */
char *get_server_file_name(char *web_url, char stop) {
	/* If NULL stop was given, save it as the
	 * name on the server side
	 */
	char *final_slash = web_url;
	do {
		final_slash = strstr(final_slash, "/");
		final_slash = &final_slash[1];
	} while (strstr(final_slash, "/"));

	char *file_name = malloc(char_size * strlen(final_slash));
	file_name[0] = '\0';
	strcat(file_name, final_slash);

	if (stop != ' ') {
		char *walker = file_name;
		while (*walker != stop)
			walker = &walker[1];
		walker[0] = '\0';
	}
	return file_name;
}
