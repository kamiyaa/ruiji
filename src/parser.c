#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "parser.h"

#define IQDB_RESULT_ID	"match</th></tr><tr><td class='image'><a href=\""

size_t StoreData(char *contents, size_t size, size_t nmemb, struct html_data *userp)
{
	size_t realsize = size * nmemb;

	struct html_data *mem = userp;

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

/* Replace the first instance of find with replace,
 * mutating the string.
 */
void replace_first_with(char *string, char find, char replace)
{
	/* Use a pointer to go through the string */
	char *walker = string;
	/* Go through the string and find the char replace */
	while (*walker != find)
		walker = &walker[1];

	/* Once we find the char we are looking for,
	 * replace it with replace
	 */
	walker[0] = replace;
}


/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *
create_sim_image(char *url_begin, unsigned int similarity, unsigned int image_x, unsigned int image_y)
{
	/* Create a new similar image struct to store information */
	struct similar_image *image = malloc(sizeof(struct similar_image));

	/* Set it's values to the given parameters */
	image->similarity = similarity;
	image->dimensions[0] = image_x;
	image->dimensions[1] = image_y;

	unsigned int size_alloc = strlen(url_begin) + 1;
	/* Format url to be complete with protocol, if none is provided */
	if (!strstr(url_begin, "http")) {
		char *prefix_add = "https:\0";

		size_alloc += strlen(prefix_add);
		image->link = malloc(sizeof(char) * size_alloc);
		strcpy(image->link, prefix_add);
		strcat(image->link, url_begin);

	}
	else {
		image->link = malloc(sizeof(char) * size_alloc);
		strcpy(image->link, url_begin);
	}
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
	char *tmp = &(next_weblink[0]);

	/* Set the pointer to point to the rest of the sliced string */
	next_weblink = &(next_weblink[1]);
	tmp[0] = '\0';

	/* Get the similarity percentage of image and set it to similarity */
	sscanf(walker, "<td>%u%%", similarity);

	/* Return a pointer to the rest of the sliced string */
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

	/* Null terminate the string at the next space in the string, and
	 * point next_weblink to the rest of the sliced string
	 */
	char *next_weblink = strstr(walker, " ");
	char *tmp = &(next_weblink[0]);
	next_weblink = &(next_weblink[1]);
	tmp[0] = '\0';

	/* Set x,y to the image dimensions */
	sscanf(walker, "<td>%u×%u ", x, y);

	/* Return a pointer to the rest of the sliced string */
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
	if (!index) {
		printf("Error: Failed to populate similar image database: No results found!\n");
		return;
	}

	/* Get length of the string we are looking for and search for it */
	unsigned int iqdb_result_len = strlen(IQDB_RESULT_ID);

	/* Go through the data and get every valid link to a similar image */
	while (index != NULL) {
		/* Get where the url of the website begins */
		char *url_begin = &index[iqdb_result_len];
		/* Set a arbitrary pointer to go through the string */
		char *walker = url_begin;

		/* Go through the string, looking for '"'. Once found,
		 * slice the string */
		while (*walker != '"')
			walker = &(walker[1]);
		char *tmp = &(walker[0]);
		/* Move on to the rest of the string */
		walker = &(walker[1]);
		tmp[0] = '\0';

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


struct similar_image *get_most_similar_image(struct similar_image_db *sim_db)
{
	struct similar_image *most_similar = sim_db->img_db[0];
	for (int i = 1; i < sim_db->size; i++) {
		if (sim_db->img_db[i]->similarity > most_similar->similarity)
			most_similar = sim_db->img_db[i];
	}
	return most_similar;
};


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
		if (res != CURLE_OK)
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

		/* cleanup */
		curl_easy_cleanup(curl_handle);
	}
	return web_data.data;
}


/* Given the full link of a website,
 * parse the link to get the file name
 */
char *get_server_file_name(char *web_url, char stop) {
	/* Go through and get the last section of a url */
	char *final_slash;
	for (int i = strlen(web_url); i > 0; i--) {
		if (web_url[i] == '/') {
			final_slash = &(web_url[i+1]);
			break;
		}
	}

	unsigned int size_cpy = strlen(final_slash);
	/* If a stop sequence is given, terminate the
	 * string at stop sequence */
	if (stop) {
		size_cpy--;
		while (final_slash[size_cpy] != stop)
			size_cpy--;
	}

	char *file_name = malloc(sizeof(char) * (size_cpy + 1));
	file_name[0] = '\0';
	strncat(file_name, final_slash, size_cpy);

	return file_name;
}


/* Given a website url, a unique html pattern to look for and */
char *get_image_url(char *web_url, char *trademark, char* prefix, char suffix)
{
	/* Fetch the html source code of the website */
	char *html_content = get_html(web_url);

	/* Find the source image link */
	char *index = strstr(html_content, trademark);
	char *img_src_url;

	/* If found, add the danbooru url to it and return it */
	if (index) {
		index = &index[strlen(trademark)];
		replace_first_with(index, suffix, '\0');
		unsigned int url_len = strlen(index) + 1;
		if (prefix)
			url_len += strlen(prefix);

		img_src_url = malloc(sizeof(char) * url_len);
		img_src_url[0] = '\0';
		if (prefix)
			strcat(img_src_url, prefix);
		strcat(img_src_url, index);
	}
	else {
		printf("Error: get_image_url():\n\tFailed to parse \"%s\"\n", web_url);
		return "ERROR";
	}
	free(html_content);
	return img_src_url;
}


/* Frees the allocated memory for a similar_image_db */
void free_similar_image_db(struct similar_image_db *sim_db)
{
	for (int i = 0; i < sim_db->size; i++) {
		free(sim_db->img_db[i]->link);
		free(sim_db->img_db[i]);
	}
}
