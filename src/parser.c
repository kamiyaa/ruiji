#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "domains.h"

#define IQDB_RESULT_ID	"match</th></tr><tr><td class='image'><a href=\""


/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in the struct similar_image_db *image_list
 */
struct similar_image_llnode *
create_image_list(char *html_content, unsigned short similar_threshold)
{
	const int node_size = sizeof(struct similar_image_llnode);
	/* Get length of the string we are looking for and search for it */
	const unsigned int iqdb_result_len = strlen(IQDB_RESULT_ID);

	struct similar_image_llnode *image_list = NULL;

	/* Check if matching string is found */
	char *index = strstr(html_content, IQDB_RESULT_ID);
	if (!index) {
		fprintf(stderr, "Error: Failed to populate similar image database: No results found!\n");
		return image_list;
	}

	struct similar_image_llnode **list_ptr = &(image_list);

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
			/* allocate memory for node */
			*list_ptr = malloc(node_size);

			(*list_ptr)->image = image;
			(*list_ptr)->next = NULL;
			list_ptr = &((*list_ptr)->next);
		}

		/* set the starting point of the string
		 * to the next valid weblink */
		index = strstr(walker, IQDB_RESULT_ID);
	}
	return image_list;
}

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

struct image_tag_db *init_image_tag_db(void)
{
	/* initialize a image tag database to store all the tags */
	struct image_tag_db *tag_db = malloc(sizeof(struct image_tag_db));
	/* set all values to 0 and NULL */
	for (int i = 0; i < 6; i++) {
		tag_db->tags[i] = NULL;
		tag_db->tag_size[i] = 0;
	}
	return tag_db;
}

/* get how far away a char is from the beginning of the string */
int get_distance(char *string, char find)
{
	int distance = 0;
	/* keep on incrementing until we've found the char */
	while (string[distance] != '\0' && string[distance] != find)
		distance++;
	if (string[distance] == '\0')
		distance = -1;
	/* return the distance */
	return distance;
}

struct image_tag_db *get_image_tags(int domain_uuid, char *html_content)
{
	struct image_tag_db *tags_db;

	switch (domain_uuid) {
	/* if the link given is a yandere domain or konachan domain */
	case KONACHAN_UUID:
	case YANDERE_UUID:
		tags_db = yandere_get_image_tags(html_content);
		break;
	/* danbooru domain */
	case DANBOORU_UUID:
		tags_db = danbooru_get_image_tags(html_content);
		break;
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UUID:
		tags_db = sankakucomplex_get_image_tags(html_content);
		break;
	case GELBOORU_UUID:
		tags_db = gelbooru_get_image_tags(html_content);
		break;
	default:
		tags_db = init_image_tag_db();
		break;
	}

	return tags_db;
}

unsigned int get_internal_domain_value(char *link) {

	/* if the link given is a yandere domain */
	if (strstr(link, YANDERE_DOMAIN))
		return YANDERE_UUID;
	/* danbooru domain */
	if (strstr(link, DANBOORU_DOMAIN))
		return DANBOORU_UUID;
	/* konachan domain */
	if (strstr(link, KONACHAN_DOMAIN))
		return KONACHAN_UUID;
	/* eshuushuu domain */
	if (strstr(link, ESHUUSHUU_DOMAIN))
		return ESHUUSHUU_UUID;
	/* gelbooru domain */
	if (strstr(link, GELBOORU_DOMAIN))
		return GELBOORU_UUID;
	/* mangadrawing domain */
	if (strstr(link, MANGADRAWING_DOMAIN))
		return MANGADRAWING_UUID;
	/* sankakucomplex domain */
	if (strstr(link, SANKAKUCOMPLEX_DOMAIN))
		return SANKAKUCOMPLEX_UUID;
	return 0;
}

/* Given a website url, a unique html pattern to look for and */
char *get_source_image_url(int domain_uuid, char *html_content, char *stop_seq)
{
	char *dl_url;
	switch (domain_uuid) {
	/* if the link given is a yandere domain */
	case YANDERE_UUID:
		dl_url = yandere_get_image_url(html_content);
		break;
	/* danbooru domain */
	case DANBOORU_UUID:
		dl_url = danbooru_get_image_url(html_content);
		break;
	/* konachan domain */
	case KONACHAN_UUID:
		dl_url = konachan_get_image_url(html_content);
		break;
	/* eshuushuu domain */
	case ESHUUSHUU_UUID:
		dl_url = eshuushuu_get_image_url(html_content);
		break;
	/* gelbooru domain */
	case GELBOORU_UUID:
		dl_url = gelbooru_get_image_url(html_content);
		break;
	/* mangadrawing domain */
	case MANGADRAWING_UUID:
		dl_url = mangadrawing_get_image_url(html_content);
		break;
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UUID:
		dl_url = sankakucomplex_get_image_url(html_content);
		/* change the sequence to stop parsing at
		 * to '?' for sankakucomplex */
		*stop_seq = '?';
		break;
	default:
		dl_url = NULL;
		break;
	}
	return dl_url;
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



/* Frees the allocated memory for a linked list of similar_image_llnode */
void free_similar_image_list(struct similar_image_llnode *image_list)
{
	while (image_list) {
		struct similar_image_llnode *tmp = image_list;
		image_list = image_list->next;
		free_similar_image(tmp->image);
		free(tmp);
	}
}

void free_similar_image(struct similar_image *image)
{
	if (image) {
		free(image->link);
		free(image);
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

