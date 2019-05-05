#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "interface.h"
#include "parser.h"
#include "udload.h"
#include "util.h"

#define IQDB_URL "https://iqdb.org"
#define IQDB_UPLOAD_FIELD "file"
#define MAX_FILE_SIZE 8192000

/* struct for holding command line arguments */
struct ruiji_args {
	bool verbose;
	bool prompt;
	bool tags;
	unsigned short threshold;
	char *format;
};

/* set default command line options */
void set_default_opt(struct ruiji_args *args)
{
	args->verbose = true;
	args->prompt = true;
	args->tags = false;
	args->threshold = 0;
	args->format = NULL;
}


void ruiji_get_tags(enum domain_t domain_uid, char *api_content)
{
	puts("Tags:");
	struct image_tag_db *tag_db;
	if ((tag_db = get_image_tags(domain_uid, api_content))) {
		print_image_tags(tag_db);
		free_image_tags(tag_db);
	}
}

int ruiji_index_prompt()
{
	int index;
	do {
		printf("Which one to download? (-1 to exit): ");
	} while (scanf("%d", &index) != 1);
	return index;
}


void ruiji(struct ruiji_args *args, char *path)
{
	if (args->verbose)
		image_upload_toast(path, IQDB_URL);

	char *iqdb_html = upload_image(IQDB_URL, path, IQDB_UPLOAD_FIELD);
	if (iqdb_html == NULL) {
		fprintf(stderr, "Error: Failed to upload %s\n", path);
		ruiji_exit(2);
	}

	struct similar_image_list *image_list =
		create_image_list(iqdb_html, args->threshold);
	free(iqdb_html);

	if (image_list == NULL) {
		fprintf(stderr, "No results found! :(\n");
		ruiji_exit(1);
	}

	int index;
	if (args->prompt) {
		print_sim_results(image_list->head);
		index = ruiji_index_prompt();
	} else {
		index = 0;
	}

	if (index < 0 || index >= image_list->size) {
		fprintf(stderr, "Error: Invalid option selected\n");
		similar_image_list_free(image_list);
		ruiji_exit(1);
	}

	struct similar_image_llnode *list_ptr = image_list->head;
	for (int i = 0; i < index; i++)
		list_ptr = list_ptr->next;

	struct similar_image_result *dl_image = list_ptr->image;
	enum domain_t domain_uid = parse_domain(dl_image->post_link);

	char *api_link = generate_api_link(domain_uid, dl_image->post_link);
	if (api_link == NULL) {
		fprintf(stderr, "Error: Failed to generate api\n");
		similar_image_list_free(image_list);
		ruiji_exit(1);
	}

	char *api_content = get_html(api_link);
	free(api_link);
	if (api_content == NULL) {
		fprintf(stderr, "Error: Failed to get api data\n");
		similar_image_list_free(image_list);
		ruiji_exit(1);
	}

	char *dl_url = parse_download_url(domain_uid, api_content);
	if (dl_url == NULL) {
		fprintf(stderr, "Error: Failed to find image link\n");
		similar_image_list_free(image_list);
		free(api_content);
		ruiji_exit(1);
	}

	if (args->verbose)
		image_download_toast(dl_image->post_link);

	char *file_save_name;
	file_save_name = parse_file_name(dl_url);

	if (args->verbose)
		image_save_toast(file_save_name);

	if (access(file_save_name, F_OK) != -1) {
		fprintf(stderr, "Error: File already exists: %s\n", file_save_name);

		similar_image_list_free(image_list);
		free(dl_url);
		free(api_content);
		free(file_save_name);

		ruiji_exit(1);
	}

	FILE *img_fp;
	if ((img_fp = fopen(file_save_name, "wb")) == NULL) {
		perror(file_save_name);

		similar_image_list_free(image_list);
		free(api_content);
		free(file_save_name);

		ruiji_exit(1);
	}
	free(file_save_name);

	if (download_image(dl_url, img_fp) != 0) {
		fclose(img_fp);
		similar_image_list_free(image_list);
		free(dl_url);
		free(api_content);

		ruiji_exit(1);
	}
	fclose(img_fp);
	free(dl_url);

	/* print tags */
	if (args->tags)
		ruiji_get_tags(domain_uid, api_content);

	free(api_content);
	similar_image_list_free(image_list);
	ruiji_exit(0);
}

void ruiji_validate_file(const char *path)
{
	struct stat statbuf;
	if (stat(path, &statbuf) == -1) {
		perror(path);
		ruiji_exit(1);
	}
	if (statbuf.st_size > MAX_FILE_SIZE) {
		fprintf(stderr, "Error: Maximum file size exceeded (%dKB)\n",
			MAX_FILE_SIZE / 1000);
		ruiji_exit(1);
	}
}

int process_args(int argc, char **argv, struct ruiji_args *args)
{
	set_default_opt(args);
	int c;
	while ((c = getopt(argc, argv, "f:qt:yTV")) != EOF) {
		switch(c) {
		case 'f':
			args->format = optarg;
			break;
		case 'q':
			args->verbose = false;
			break;
		case 't':
			args->threshold = atoi(optarg);
			break;
		case 'y':
			args->prompt = false;
			break;
		case 'T':
			args->tags = true;
			break;
		case 'V':
			fprintf(stderr, "%s-%s\n", argv[0], VERSION);
			exit(0);
		default:
			return 1;
		}
	}
	return 0;
}

int main(int argc, char **argv)
{
	extern int optind;
	extern char *optarg;
	if (argc == 1) {
		print_help();
		return 1;
	}

	struct ruiji_args args;

	if (process_args(argc, argv, &args) || optind == argc) {
		print_help();
		return 1;
	}
	char *path = argv[optind];

	ruiji_validate_file(path);
	ruiji(&args, path);
}
