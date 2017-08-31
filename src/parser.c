#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "domains.h"

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * parse all the results and store them in the struct similar_image_db *image_list
 */
struct similar_image_llnode *
create_image_list(char *html_content, unsigned short similar_threshold)
{

	const char iqdb_result_uuid[] =
		"match</th></tr><tr><td class='image'><a href=\"";
	/* Get length of the string we are looking for and search for it */
	const unsigned int iqdb_result_len = strlen(iqdb_result_uuid);

	const int node_size = sizeof(struct similar_image_llnode);


	struct similar_image_llnode *image_list = NULL;

	/* Check if matching string is found */
	char *url_begin = strstr(html_content, iqdb_result_uuid);
	if (url_begin == NULL) {
		fprintf(stderr, "Error: Failed to populate similar image database: No results found!\n");
		return image_list;
	}

	struct similar_image_llnode **list_ptr = &(image_list);

	/* Go through the data and get every valid link to a similar image */
	while (url_begin) {
		/* Get where the url of weblink begins */
		url_begin = &(url_begin[iqdb_result_len]);

		char *walker = url_begin;

		/* Go through the string, looking for '"'. Once found,
		 * slice the string */
		int url_len = get_distance(url_begin, '"');

		unsigned short similarity = 0;
		unsigned int x = 0, y = 0;

		if (url_len > 0) {
			walker = parse_xy_img_dimensions(walker, &x, &y);
			walker = parse_percent_similar(walker, &similarity);

			url_begin[url_len] = '\0';
		}

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

			url_begin[url_len] = '"';
		}

		/* set the starting point of the string
		 * to the next valid weblink */
		url_begin = strstr(url_begin, iqdb_result_uuid);
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
		const char *prefix_add = "https:";
		const unsigned int prefix_len = strlen(prefix_add);

		len_url += prefix_len;

		image->post_link = malloc(CHAR_SIZE * len_url);
		strcpy(image->post_link, prefix_add);
		strcat(image->post_link, web_url);
	}
	else {
		image->post_link = malloc(CHAR_SIZE * len_url);
		strcpy(image->post_link, web_url);
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
	/* danbooru domain */
	case DANBOORU_UUID:
		tags_db = danbooru_get_image_tags_json(html_content);
		break;
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UUID:
		tags_db = sankakucomplex_get_image_tags(html_content);
		break;
	/* gelbooru domain */
	case GELBOORU_UUID:
		tags_db = gelbooru_get_image_tags(html_content);
		break;
	/* if the link given is a yandere domain or konachan domain */
	case KONACHAN_UUID:
	case YANDERE_UUID:
		tags_db = yandere_get_image_tags(html_content);
		break;
	/* all others */
	case ESHUUSHUU_UUID:
	case MANGADRAWING_UUID:
	case ZEROCHAN_UUID:
	default:
		tags_db = init_image_tag_db();
		break;
	}

	return tags_db;
}

unsigned int get_internal_domain_value(char *link) {

	/* danbooru domain */
	if (strstr(link, DANBOORU_DOMAIN))
		return DANBOORU_UUID;
	/* eshuushuu domain */
	if (strstr(link, ESHUUSHUU_DOMAIN))
		return ESHUUSHUU_UUID;
	/* gelbooru domain */
	if (strstr(link, GELBOORU_DOMAIN))
		return GELBOORU_UUID;
	/* konachan domain */
	if (strstr(link, KONACHAN_DOMAIN))
		return KONACHAN_UUID;
	/* mangadrawing domain */
	if (strstr(link, MANGADRAWING_DOMAIN))
		return MANGADRAWING_UUID;
	/* sankakucomplex domain */
	if (strstr(link, SANKAKUCOMPLEX_DOMAIN))
		return SANKAKUCOMPLEX_UUID;
	/* if the link given is a yandere domain */
	if (strstr(link, YANDERE_DOMAIN))
		return YANDERE_UUID;
	return 0;
}

char *generate_api_link(int domain_uuid, char *post_link)
{
	char *api_url;
	switch (domain_uuid) {
	/* danbooru domain */
	case DANBOORU_UUID:
		api_url = danbooru_generate_api_url(post_link);
		break;
	/* if the link given is a yandere domain */
	case YANDERE_UUID:
	/* konachan domain */
	case KONACHAN_UUID:
	/* eshuushuu domain */
	case ESHUUSHUU_UUID:
	/* gelbooru domain */
	case GELBOORU_UUID:
	/* mangadrawing domain */
	case MANGADRAWING_UUID:
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UUID:
	/* zerochan domain */
	case ZEROCHAN_UUID:
	default:
		api_url = strdup(post_link);
		break;
	}
	return api_url;
}

/* Given a website url, a unique html pattern to look for and */
char *get_source_image_url(int domain_uuid, char *html_content, char *stop_seq)
{
	char *dl_url;
	switch (domain_uuid) {
	/* danbooru domain */
	case DANBOORU_UUID:
		dl_url = danbooru_get_image_url_json(html_content);
		break;
	/* eshuushuu domain */
	case ESHUUSHUU_UUID:
		dl_url = eshuushuu_get_image_url(html_content);
		break;
	/* gelbooru domain */
	case GELBOORU_UUID:
		dl_url = gelbooru_get_image_url(html_content);
		break;
	/* konachan domain */
	case KONACHAN_UUID:
		dl_url = konachan_get_image_url(html_content);
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
	/* yandere domain */
	case YANDERE_UUID:
		dl_url = yandere_get_image_url(html_content);
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
	char *name_start = &(web_url[index+1]);

	int filename_len = strlen(name_start);
	/* If a stop sequence is given, terminate the
	 * string copy at stop sequence */
	if (stop)
		filename_len = get_distance(name_start, stop);

	/* Allocate enough memory for the file name */
	char *file_name = malloc(CHAR_SIZE * (filename_len + 1));

	/* copy the file name to file_name and null terminate the string */
	strncpy(file_name, name_start, filename_len);
	file_name[filename_len] = '\0';

	return file_name;
}

/* Given the html contents of http://iqdb.org after an image has been uploaded,
 * get the x, y dimensions of the image and return a pointer pointing to the
 * html contents where the parsing stopped.
 */
char *parse_percent_similar(char* contents, unsigned short *similarity)
{
	/* initialize pointer to hold where walker leaves off */
	char *next_weblink = NULL;

	const char *patterns[] = { "<td>" };
	const int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = contents;

	int pattern_index = 0;
	/* move walker to the beginning of image x,y dimensions */
	while (walker && pattern_index < num_patterns) {
		walker = strstr(walker, patterns[pattern_index]);
		pattern_index++;
	}
	if (walker) {
		/* Get the similarity percentage of image and set it to similarity */
		sscanf(walker, "<td>%hu%%", similarity);

		next_weblink = strstr(walker, "</div>");
	}

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
	char *next_weblink = NULL;

	const char *patterns[] = {
		"class=\"service-icon\">",
		"<td>"
	};
	const int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = contents;

	int pattern_index = 0;
	/* move walker to the beginning of image x,y dimensions */
	while (walker && pattern_index < num_patterns) {
		walker = strstr(walker, patterns[pattern_index]);
		pattern_index++;
	}
	if (walker) {
		/* point next_weblink to the rest of the sliced string
		 */
		next_weblink = strstr(walker, "</td>");
		/* Set x,y to the image dimensions */
		sscanf(walker, "<td>%u×%u ", x, y);
	}

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
		if (image->post_link)
			free(image->post_link);
		free(image);
	}
}

void free_linked_list(struct llnode *head)
{
	struct llnode *ptr = head;
	struct llnode *prev;
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

