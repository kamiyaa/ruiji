#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domains.h"
#include "udload.h"

#define IQDB_RESULT_ID	"match</th></tr><tr><td class='image'><a href=\""


struct similar_image *create_sim_image(char *url_begin, unsigned short similarity,
					unsigned int x, unsigned int y);
int get_distance(char *string, char find);
struct image_tag_db *get_image_tags(char *link);
char *get_server_file_name(char *web_url, char stop);
char *get_source_image_url(char *url, char *stop_seq);
void free_similar_image_db(struct similar_image_db *sim_db);
char *parse_percent_similar(char *contents, unsigned short *similarity);
char *parse_xy_img_dimensions(char* contents, unsigned int *x, unsigned int *y);
void populate_sim_db(struct similar_image_db *sim_db, char *html_content,
			unsigned short similar_threshold);


/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *
create_sim_image(char *web_url, unsigned short similarity,
		unsigned int x_pixels, unsigned int y_pixels)
{
	/* Create a new similar image struct to store information */
	struct similar_image *image = malloc(sizeof(struct similar_image));

	/* Set it's values to the given parameters */
	image->similarity = similarity;
	image->dimensions[0] = x_pixels;
	image->dimensions[1] = y_pixels;

	unsigned int len_url = strlen(web_url) + 1;
	/* Format url to be complete with protocol, if none is provided */
	if (!strstr(web_url, "http")) {
		char *prefix_add = "https:\0";

		len_url += strlen(prefix_add);
		image->link = malloc(sizeof(char) * len_url);
		strcpy(image->link, prefix_add);
		strcat(image->link, web_url);
	}
	else {
		image->link = malloc(sizeof(char) * len_url);
		strcpy(image->link, web_url);
	}
	return image;
}

/* get how far away a char is from the beginning of the string */
int get_distance(char *string, char find)
{
	int distance = 0;
	unsigned int len_string = strlen(string);
	/* keep on incrementing until we've found the char */
	while (distance < len_string && string[distance] != find)
		distance++;
	if (distance == len_string)
		distance = -1;
	/* return the distance */
	return distance;
}

/* Given a website url, a unique html pattern to look for and */
char *get_source_image_url(char *link, char *stop_seq)
{
	char *dl_url = NULL;
	/* Fetch the html source code of the website */
	char *html_content = get_html(link);
	/* if the link given is a yandere domain */
	if (strstr(link, YANDERE_DOMAIN)) {
		dl_url = yandere_get_image_url(html_content);
	}
	/* danbooru domain */
	else if (strstr(link, DANBOORU_DOMAIN)) {
		dl_url = danbooru_get_image_url(html_content);
	}
	/* konachan domain */
	else if (strstr(link, KONACHAN_DOMAIN)) {
		dl_url = konachan_get_image_url(html_content);
	}
	/* eshuushuu domain */
	else if (strstr(link, ESHUUSHUU_DOMAIN)) {
		dl_url = eshuushuu_get_image_url(html_content);
	}
	/* gelbooru domain */
	else if (strstr(link, GELBOORU_DOMAIN)) {
		dl_url = gelbooru_get_image_url(html_content);
	}
	/* mangadrawing domain */
	else if (strstr(link, MANGADRAWING_DOMAIN)) {
		dl_url = mangadrawing_get_image_url(html_content);
	}
	/* sankakucomplex domain */
	else if (strstr(link, SANKAKU_COMPLEX_DOMAIN)) {
		dl_url = sankaku_complex_get_image_url(html_content);
		/* change the sequence to stop parsing at
		 * to '?' for sankakucomplex */
		*stop_seq = '?';
	}

	/* deallocate the memory used to download
	 * and store the webpage's content */
	free(html_content);
	/* return the url */
	return dl_url;
}

struct image_tag_db *get_image_tags(char *link)
{
	struct image_tag_db *tags_db = NULL;
	/* Fetch the html source code of the website */
	char *html_content = get_html(link);
	/* if the link given is a yandere domain */
	if (strstr(link, YANDERE_DOMAIN)) {
		tags_db = yandere_get_image_tags(html_content);
	}
	free(html_content);

	return tags_db;
}

/* Given the full link of a website, parse the link to get the file name */
char *get_server_file_name(char *web_url, char stop)
{
	/* Go through and get the last section of a url */
	int index = strlen(web_url) - 1;
	/* interate through the string backwards until we find a '/' */
	while (web_url[index] != '/')
		index--;
	/* get memory address we stopped at */
	char *slash_ptr = &(web_url[index+1]);

	int filename_len = strlen(slash_ptr);
	/* If a stop sequence is given, terminate the
	 * string copy at stop sequence */
	if (stop)
		filename_len = get_distance(slash_ptr, stop);

	/* Allocate enough memory for the file name */
	char *file_name = malloc(sizeof(char) * (filename_len + 1));
	/* NULL terminate the file name and concatenate the file name to it */
	file_name[0] = '\0';
	strncat(file_name, slash_ptr, filename_len);

	return file_name;
}

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_percent_similar(char* contents, unsigned short *similarity)
{
	/* initialize pointer to hold where walker leaves off */
	char *next_weblink;

	/* Get size of char to prevent excessive function calling */
	char *walker = contents;
	/* Parse for the similarity percentage of the image and slice it */
	walker = strstr(walker, "<td>");

	/* Get the similarity percentage of image and set it to similarity */
	sscanf(walker, "<td>%hu%%", similarity);

	next_weblink = strstr(walker, "</div>");
	/* Return a pointer to the rest of the sliced string */
	return next_weblink;
}

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_xy_img_dimensions(char* contents, unsigned int *x, unsigned int *y)
{
	/* initialize pointer to hold where walker leaves off */
	char *next_weblink;

	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = contents;

	/* Find the dimensions of the image and point x,y to them */
	walker = strstr(walker, "class=\"service-icon\"");
	walker = strstr(walker, "<td>");

	/* Null terminate the string at the next space in the string, and
	 * point next_weblink to the rest of the sliced string
	 */
	next_weblink = strstr(walker, "</td>");

	/* Set x,y to the image dimensions */
	sscanf(walker, "<td>%u×%u ", x, y);

	/* Return a pointer to the rest of the sliced string */
	return next_weblink;
}

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in the struct similar_image_db *sim_db
 */
void
populate_sim_db(struct similar_image_db *sim_db,
		char *html_content, unsigned short similar_threshold)
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
	unsigned short iqdb_result_len = strlen(IQDB_RESULT_ID);

	/* Go through the data and get every valid link to a similar image */
	while (index != NULL) {
		/* Get where the url of the website begins */
		char *url_begin = &index[iqdb_result_len];

		/* Go through the string, looking for '"'. Once found,
		 * slice the string */
		int url_len = get_distance(url_begin, '"');
		/* Set a arbitrary pointer to go through the string */
		char *walker = &(url_begin[url_len+1]);
		url_begin[url_len] = '\0';

		/* Get the image x,y dimensions as well as the similarity
		 * of the image */
		unsigned short similarity = 0;
		unsigned int x = 0, y = 0;
		walker = parse_xy_img_dimensions(walker, &x, &y);
		walker = parse_percent_similar(walker, &similarity);

		/* If the image passes given threshold, add it to
		 * the collection of similar images */
		if (similarity >= similar_threshold) {
			/* Create a new similar_image struct to
			 * hold all the image's information */
			struct similar_image *image =
				create_sim_image(url_begin, similarity, x, y);

			/* Add it to our database of similar images */
			sim_db->img_db[sim_db->size] = image;
			sim_db->size++;
		}

		/* set the starting point of the string
		 * to the next valid weblink */
		index = strstr(walker, IQDB_RESULT_ID);
	}
}

/* Frees the allocated memory for a similar_image_db */
void free_similar_image_db(struct similar_image_db *sim_db)
{
	for (int i = 0; i < sim_db->size; i++) {
		free(sim_db->img_db[i]->link);
		free(sim_db->img_db[i]);
	}
}


void free_linked_list(struct ll_node *head)
{
	struct ll_node *ptr = head;
	struct ll_node *prev;
	while (ptr) {
		prev = ptr;
		ptr = ptr->next;
		free(prev->data);
		free(prev);
	}
}

void free_image_tags(struct image_tag_db *tags_db)
{
	for (int i = 0; i < 6; i++)
		free_linked_list(tags_db->tags[i]);
	free(tags_db);
}

