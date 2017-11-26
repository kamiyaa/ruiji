#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "domains.h"

struct similar_image_llnode *create_image_list(char *html_content,
	unsigned short similar_threshold)
{

	const char iqdb_result_uid[] =
		"match</th></tr><tr><td class='image'><a href=\"";
	/* get length of the string we are looking for and search for it */
	const unsigned int iqdb_result_len = strlen(iqdb_result_uid);
	/* get size of node for memory allocations later */
	const int node_size = sizeof(struct similar_image_llnode);

	/* check if matching string is found */
	char *url_begin = strstr(html_content, iqdb_result_uid);
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

		if (url_len > 0) {
			/* unslice string */
			url_begin[url_len] = '"';
		}

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

char *generate_api_link(int domain_uid, char *post_link)
{
	char *api_url;
	switch (domain_uid) {
	/* danbooru domain */
	case DANBOORU_UID:
		api_url = danbooru_generate_api_url(post_link);
		break;
	/* eshuushuu domain */
	case ESHUUSHUU_UID:
	/* gelbooru domain */
	case GELBOORU_UID:
	/* konachan domain */
	case KONACHAN_UID:
	/* mangadrawing domain */
	case MANGADRAWING_UID:
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UID:
	/* yandere domain */
	case YANDERE_UID:
	/* zerochan domain */
	case ZEROCHAN_UID:
	default:
		api_url = malloc(sizeof(CHAR_SIZE) * (strlen(post_link) + 1));
		strcpy(api_url, post_link);
		break;
	}
	return api_url;
}

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

/* Given a website url, a unique html pattern to look for and */
char *get_image_source_url(int domain_uid, char *html_content, char *stop_seq)
{
	char *dl_url;
	switch (domain_uid) {
	/* danbooru domain */
	case DANBOORU_UID:
		dl_url = danbooru_get_image_url_json(html_content);
		break;
	/* eshuushuu domain */
	case ESHUUSHUU_UID:
		dl_url = eshuushuu_get_image_url(html_content);
		break;
	/* gelbooru domain */
	case GELBOORU_UID:
		dl_url = gelbooru_get_image_url(html_content);
		break;
	/* konachan domain */
	case KONACHAN_UID:
		dl_url = konachan_get_image_url(html_content);
		break;
	/* mangadrawing domain */
	case MANGADRAWING_UID:
		dl_url = mangadrawing_get_image_url(html_content);
		break;
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UID:
		dl_url = sankakucomplex_get_image_url(html_content);
		/* change the sequence to stop parsing at
		 * to '?' for sankakucomplex */
		*stop_seq = '?';
		break;
	/* yandere domain */
	case YANDERE_UID:
		dl_url = yandere_get_image_url(html_content);
		break;
	case ZEROCHAN_UID:
		dl_url = zerochan_get_image_url(html_content);
		break;
	default:
		dl_url = NULL;
		break;
	}
	return dl_url;
}

struct image_tag_db *get_image_tags(int domain_uid, char *html_content)
{
	struct image_tag_db *tags_db;

	switch (domain_uid) {
	/* danbooru domain */
	case DANBOORU_UID:
		tags_db = danbooru_get_image_tags_json(html_content);
		break;
	/* all others */
	case ESHUUSHUU_UID:
		tags_db = eshuushuu_get_image_tags(html_content);
		break;
	/* gelbooru domain */
	case GELBOORU_UID:
		tags_db = gelbooru_get_image_tags(html_content);
		break;
	/* sankakucomplex domain */
	case SANKAKUCOMPLEX_UID:
		tags_db = sankakucomplex_get_image_tags(html_content);
		break;
	/* if the link given is a yandere domain or konachan domain */
	case KONACHAN_UID:
	case YANDERE_UID:
		tags_db = yandere_get_image_tags(html_content);
		break;
	case MANGADRAWING_UID:
	case ZEROCHAN_UID:
	default:
		tags_db = init_image_tag_db();
		break;
	}

	return tags_db;
}

unsigned int get_domain_uid(char *link)
{
	/* danbooru domain */
	if (strstr(link, DANBOORU_DOMAIN))
		return DANBOORU_UID;
	/* eshuushuu domain */
	if (strstr(link, ESHUUSHUU_DOMAIN))
		return ESHUUSHUU_UID;
	/* gelbooru domain */
	if (strstr(link, GELBOORU_DOMAIN))
		return GELBOORU_UID;
	/* konachan domain */
	if (strstr(link, KONACHAN_DOMAIN))
		return KONACHAN_UID;
	/* mangadrawing domain */
	if (strstr(link, MANGADRAWING_DOMAIN))
		return MANGADRAWING_UID;
	/* sankakucomplex domain */
	if (strstr(link, SANKAKUCOMPLEX_DOMAIN))
		return SANKAKUCOMPLEX_UID;
	/* if the link given is a yandere domain */
	if (strstr(link, YANDERE_DOMAIN))
		return YANDERE_UID;
	if (strstr(link, ZEROCHAN_DOMAIN))
		return ZEROCHAN_UID;
	return 0;
}

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


void free_image_tags(struct image_tag_db *tags_db)
{
	for (int i = 0; i < 6; i++)
		free_linked_list(tags_db->tags[i]);
	free(tags_db);
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

void free_similar_image(struct similar_image *image)
{
	if (image) {
		if (image->post_link)
			free(image->post_link);
		free(image);
	}
}

void free_similar_image_list(struct similar_image_llnode *image_list)
{
	while (image_list) {
		struct similar_image_llnode *tmp = image_list;
		image_list = image_list->next;
		free_similar_image(tmp->image);
		free(tmp);
	}
}
