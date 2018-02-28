#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "udload.h"
#include "helpers.h"

struct similar_image_llnode *create_image_list(char *web_content,
	unsigned short similar_threshold)
{

	const char iqdb_result_uid[] =
		"match</th></tr><tr><td class='image'><a href=\"";
	/* get length of the string we are looking for and search for it */
	const unsigned int iqdb_result_len = strlen(iqdb_result_uid);
	/* get size of node for memory allocations later */
	const int node_size = sizeof(struct similar_image_llnode);

	/* check if matching string is found */
	char *url_begin = strstr(web_content, iqdb_result_uid);
	if (url_begin == NULL) {
		fprintf(stderr, "Error: Failed to populate similar image database: No results found!\n");
		return NULL;
	}

	/* initialize an empty linked list  */
	struct similar_image_llnode *image_list = NULL;
	/* initialize a double pointer for going through image_list */
	struct similar_image_llnode **list_ptr = &(image_list);

	/* Go through the data and get every valid link to a similar image */
	while (url_begin) {
		/* move url_begin to where the url starts */
		url_begin = &(url_begin[iqdb_result_len]);

		char *walker = url_begin;

		/* get how far url_begin is from the first " */
		int url_len = get_distance(url_begin, '"');

		unsigned short similarity = 0;
		unsigned int x = 0, y = 0;

		if (url_len > 0) {
			/* parse dimensions of image */
			walker = parse_xy_img_dimensions(walker, &x, &y);
			/* parse similarity percentage of image */
			walker = parse_percent_similar(walker, &similarity);

			/* slice string where first " is */
			url_begin[url_len] = '\0';
		}

		/* If the image passes given threshold, add it to list of
		 * similar images */
		if (similarity >= similar_threshold) {
			/* Create a new similar_image struct to
			 * hold all the image's information */
			struct similar_image *image =
				create_sim_image(url_begin, similarity, x, y);

			/* allocate memory for node */
			*list_ptr = malloc(node_size);

			(*list_ptr)->image = image;
			(*list_ptr)->next = NULL;

			/* set list_ptr to point to the next node */
			list_ptr = &((*list_ptr)->next);
		}

		/* unslice string */
		if (url_len > 0)
			url_begin[url_len] = '"';

		/* set the starting point of the string
		 * to the next valid weblink */
		url_begin = strstr(url_begin, iqdb_result_uid);
	}
	return image_list;
}

/* Given the necessary information of a similar image, create a similar image
 * struct with the given values and return it.
 */
struct similar_image *create_sim_image(char *web_url,
	unsigned short similarity, unsigned int xpx, unsigned int ypx)
{
	/* Create a new similar image struct to store information */
	struct similar_image *image = malloc(sizeof(struct similar_image));

	/* Set it's values to the given parameters */
	image->similarity = similarity;
	image->dimensions[0] = xpx;
	image->dimensions[1] = ypx;

	unsigned int len_url = strlen(web_url) + 1;
	/* Format url to be complete with protocol, if none is provided */
	if (!strstr(web_url, "http")) {
		const char *prefix_add = "https:";
		const unsigned int prefix_len = strlen(prefix_add);

		len_url += prefix_len;

		image->post_link = malloc(sizeof(char) * len_url);
		strcpy(image->post_link, prefix_add);
		strcat(image->post_link, web_url);
	}
	else {
		image->post_link = malloc(sizeof(char) * len_url);
		strcpy(image->post_link, web_url);
	}
	return image;
}

char *generate_api_link(enum domain_t id, char *post_link)
{
	char *api_url;
	switch (id) {
	/* danbooru domain */
	case danbooru:
		api_url = danbooru_generate_api_url(post_link);
		break;
	case animegallery:
	case animepictures:
	case eshuushuu:
	case gelbooru:
	case konachan:
	case mangadrawing:
	case sankakucomplex:
	case yandere:
	case zerochan:
	default:
		api_url = malloc(sizeof(sizeof(char)) * (strlen(post_link) + 1));
		strcpy(api_url, post_link);
		break;
	}
	return api_url;
}

/* Given a website url, a unique html pattern to look for and */
char *get_image_source_url(enum domain_t id, char *web_content)
{
	char *dl_url;
	switch (id) {
	case danbooru:
		dl_url = danbooru_get_image_url_json(web_content);
		break;
	case yandere:
		dl_url = yandere_get_image_url(web_content);
		break;
	case gelbooru:
		dl_url = gelbooru_get_image_url(web_content);
		break;
	case konachan:
		dl_url = konachan_get_image_url(web_content);
		break;
	case sankakucomplex:
		dl_url = sankakucomplex_get_image_url(web_content);
		break;
	case eshuushuu:
		dl_url = eshuushuu_get_image_url(web_content);
		break;
	case zerochan:
		dl_url = zerochan_get_image_url(web_content);
		break;
	case animegallery:
		dl_url = animegallery_get_image_url(web_content);
		break;
	case animepictures:
		dl_url = animepictures_get_image_url(web_content);
		break;
	case mangadrawing:
		dl_url = mangadrawing_get_image_url(web_content);
		break;
	default:
		dl_url = NULL;
		break;
	}
	return dl_url;
}

struct image_tag_db *get_image_tags(enum domain_t id, char *web_content)
{
	struct image_tag_db *tag_db;
	switch (id) {
	case danbooru:
		tag_db = danbooru_get_image_tags_json(web_content);
		break;
	/* konachan and yandere layouts are exactly the same */
	case konachan:
	case yandere:
		tag_db = yandere_get_image_tags(web_content);
		break;
	case gelbooru:
		tag_db = gelbooru_get_image_tags(web_content);
		break;
	case sankakucomplex:
		tag_db = sankakucomplex_get_image_tags(web_content);
		break;
	case eshuushuu:
		tag_db = eshuushuu_get_image_tags(web_content);
		break;
	case zerochan:
		tag_db = zerochan_get_image_tags_html(web_content);
		break;
	case animegallery:
	case animepictures:
	case mangadrawing:
	default:
		tag_db = init_image_tag_db();
		break;
	}
	return tag_db;
}

enum domain_t get_domain_uid(char *link)
{
	enum domain_t id = 0;

	if (strstr(link, DANBOORU_DOMAIN))
		id = danbooru;
	else if (strstr(link, YANDERE_DOMAIN))
		id = yandere;
	else if (strstr(link, GELBOORU_DOMAIN))
		id = gelbooru;
	else if (strstr(link, KONACHAN_DOMAIN))
		id = konachan;
	else if (strstr(link, SANKAKUCOMPLEX_DOMAIN))
		id = sankakucomplex;
	else if (strstr(link, ESHUUSHUU_DOMAIN))
		id = eshuushuu;
	else if (strstr(link, ZEROCHAN_DOMAIN))
		id = zerochan;
	else if (strstr(link, ANIMEGALLERY_DOMAIN))
		id = animegallery;
	else if (strstr(link, ANIMEPICTURES_DOMAIN))
		id = animepictures;
	else if (strstr(link, MANGADRAWING_DOMAIN))
		id = mangadrawing;

	return id;
}

char *get_server_file_name(char *web_url)
{
	/* Go through and get the last section of a url */
	int index = strlen(web_url) - 1;
	/* interate through the string backwards until we find a '/' */
	while (web_url[index] != '/')
		index--;

	/* get memory address we stopped at */
	char *name_start = &(web_url[index+1]);

	int filename_len = strlen(name_start);

	/* Allocate enough memory for the file name */
	char *file_name = malloc(sizeof(char) * (filename_len + 1));

	/* copy the file name to file_name and null terminate the string */
	strncpy(file_name, name_start, filename_len);
	file_name[filename_len] = '\0';

	return file_name;
}

char *parse_percent_similar(char* web_content, unsigned short *similarity)
{
	/* initialize pointer to hold where walker leaves off */
	char *next_weblink = NULL;

	const char *patterns[] = { "<td>" };
	const unsigned int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = web_content;

	unsigned int pattern_index = 0;
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

char *parse_xy_img_dimensions(char* web_content, unsigned int *x, unsigned int *y)
{
	/* initialize pointer to hold where walker leaves off */
	char *next_weblink = NULL;

	const char *patterns[] = {
		"class=\"service-icon\">",
		"<td>"
	};
	const int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *walker = web_content;

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
