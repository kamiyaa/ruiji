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

	/* initialize an empty linked list  */
	struct similar_image_llnode *image_list = NULL;
	/* initialize a double pointer for going through image_list */
	struct similar_image_llnode **list_ptr = &(image_list);

	char *url_begin = web_content;
	/* Go through the data and get every valid link to a similar image */
	while ((url_begin = strstr(url_begin, iqdb_result_uid))) {
		/* move url_begin to where the url starts */
		url_begin = &(url_begin[sizeof(iqdb_result_uid) - 1]);

		char *ptr = url_begin;

		/* get how far url_begin is from the first " */
		int url_len = get_distance(url_begin, '"');

		unsigned short similarity = 0;
		unsigned int x = 0, y = 0;

		if (url_len > 0) {
			/* parse dimensions of image */
			ptr = parse_xy_img_dimensions(ptr, &x, &y);
			/* parse similarity percentage of image */
			ptr = parse_percent_similar(ptr, &similarity);

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
			*list_ptr = malloc(sizeof(struct similar_image_llnode));
			/* out of memory */
			if (*list_ptr == NULL) {
				fprintf(stderr, "Error: Out of memory\n");
				break;
			}

			(*list_ptr)->image = image;
			(*list_ptr)->next = NULL;

			/* set list_ptr to point to the next node */
			list_ptr = &((*list_ptr)->next);
		}

		/* unslice string */
		if (url_len > 0)
			url_begin[url_len] = '"';
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

	if (image == NULL) {
		fprintf(stderr, "Error: Out of memory\n");
		return NULL;
	}

	/* Set it's values to the given parameters */
	image->similarity = similarity;
	image->dimensions[0] = xpx;
	image->dimensions[1] = ypx;

	unsigned int len_url = strlen(web_url);
	/* Format url to be complete with protocol, if none is provided */
	if (len_url >= 4 && strncmp(web_url, "http", 4) != 0) {
		const char prefix_add[] = "https:";

		len_url += sizeof(prefix_add);

		image->post_link = malloc(sizeof(char) * (len_url + 1));
		strcpy(image->post_link, prefix_add);
		strcat(image->post_link, web_url);
	}
	else {
		image->post_link = malloc(sizeof(char) * (len_url + 1));
		strcpy(image->post_link, web_url);
	}
	return image;
}

char *generate_api_link(enum domain_t id, char *post_link)
{
	char *api_url;
	switch (id) {
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
	unsigned int index = strlen(web_url) - 1;
	unsigned int filename_len = 0;
	/* interate through the string backwards until we find a '/' */
	while (index > 0 && web_url[index] != '/') {
		filename_len++;
		index--;
	}

	/* get memory address we stopped at */
	char *name_start = &(web_url[index+1]);

	/* Allocate enough memory for the file name */
	char *file_name = malloc(sizeof(char) * (filename_len + 1));

	if (file_name == NULL) {
		fprintf(stderr, "Error: Out of memory\n");
		return NULL;
	}
	strncpy(file_name, name_start, filename_len);
	file_name[filename_len] = '\0';

	return file_name;
}

char *parse_percent_similar(char* web_content, unsigned short *similarity)
{
	const char *patterns[] = { "<td>" };
	const unsigned int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *ptr = web_content;

	unsigned int pattern_index = 0;
	/* move ptr to the beginning of image x,y dimensions */
	while (ptr && pattern_index < num_patterns) {
		ptr = strstr(ptr, patterns[pattern_index]);
		pattern_index++;
	}

	/* initialize pointer to hold where ptr leaves off */
	char *next_weblink = NULL;
	if (ptr) {
		/* Get the similarity percentage of image and set it to similarity */
		sscanf(ptr, "<td>%hu%%", similarity);

		next_weblink = strstr(ptr, "</div>");
	}

	/* Return a pointer to the rest of the sliced string */
	return next_weblink;
}

char *parse_xy_img_dimensions(char* web_content, unsigned int *x, unsigned int *y)
{

	const char *patterns[] = {
		"class=\"service-icon\">",
		"<td>"
	};
	const int num_patterns = sizeof(patterns) / sizeof(char *);

	/* Set an arbitrary pointer to point to the first element of contents */
	char *ptr = web_content;

	int pattern_index = 0;
	/* move ptr to the beginning of image x,y dimensions */
	while (ptr && pattern_index < num_patterns) {
		ptr = strstr(ptr, patterns[pattern_index]);
		pattern_index++;
	}
	/* initialize pointer to hold where ptr leaves off */
	char *next_weblink = NULL;
	if (ptr) {
		/* point next_weblink to the rest of the sliced string */
		next_weblink = strstr(ptr, "</td>");
		/* Set x,y to the image dimensions */
		sscanf(ptr, "<td>%u×%u ", x, y);
	}

	/* Return a pointer to the rest of the sliced string */
	return next_weblink;
}
