#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include "interface.h"
#include "parser.h"
#include "udload.h"

#define IQDB_URL "https://iqdb.org"
#define IQDB_UPLOAD_FIELD "file"
#define MAX_FILE_SIZE 8192000

/* struct for holding command line arguments */
struct ruiji_args {
	bool verbose;
	bool prompt;
	bool tags;
	unsigned short threshold;
};

/* set default command line options */
void set_default_opt(struct ruiji_args *args)
{
	args->verbose = true;
	args->prompt = true;
	args->tags = false;
	args->threshold = 0;
}


void ruiji_get_tags(enum domain_t domain_uid, char *api_content)
{
	printf("Tags:\n");

	struct image_tag_db *tag_db;
	if ((tag_db = get_image_tags(domain_uid, api_content))) {
		print_image_tags(tag_db);
		free_image_tags(tag_db);
	}
}

int ruiji_get_image(struct ruiji_args *args, struct similar_image_llnode *image_list, int index)
{
	struct similar_image_llnode *list_ptr = image_list;
	for (int i = 0; i < index; i++)
		list_ptr = list_ptr->next;

	struct similar_image *dl_image = list_ptr->image;

	enum domain_t domain_uid = get_domain_uid(dl_image->post_link);

	char *api_link = generate_api_link(domain_uid, dl_image->post_link);
	if (api_link == NULL) {
		fprintf(stderr, "Error: Failed to generate api\n");
		return 1;
	}

	/* get the html contents of the website */
	char *api_content = get_html(api_link);
	free(api_link);

	if (api_content == NULL) {
		fprintf(stderr, "Error: Failed to generate api\n");
		return 1;
	}

	/* parse for the source url of the image */
	char *dl_url = get_image_source_url(domain_uid, api_content);

	/* Check if we've successfully parsed the source image
	 * url */
	if (dl_url == NULL) {
		fprintf(stderr, "Error: Failed to find image link\n");
		return 1;
	}

	if (args->verbose)
		image_download_toast(dl_image->post_link);

	char *file_save_name = get_server_file_name(dl_url);

	if (args->verbose)
		image_save_toast(file_save_name);

	int dl_status;
	/* save the image as it's name on the server */
	if ((dl_status = download_image(dl_url, file_save_name)) != 0) {
		fprintf(stderr, "Error: Download failed\n");
		return dl_status;
	}

	/* print tags */
	if (args->tags)
		ruiji_get_tags(domain_uid, api_content);

	free(file_save_name);
	free(dl_url);
	free(api_content);

	/* Report back to the user how the download went */
	return 0;
}

int ruiji(struct ruiji_args *args, char *file_name)
{
	/* notify user we are uploading argument file */
	if (args->verbose)
		image_upload_toast(file_name, IQDB_URL);

	char *iqdb_html = upload_image(IQDB_URL, file_name, IQDB_UPLOAD_FIELD);

	if (iqdb_html == NULL) {
		fprintf(stderr, "Error: Failed to upload file\n");
		return 1;
	}

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_llnode *image_list =
		create_image_list(iqdb_html, args->threshold);
	/* free up allocated memory */
	free(iqdb_html);

	if (image_list == NULL) {
		fprintf(stderr, "No results found! :(\n");
		return 1;
	}

	/* initialize image selection to 0 */
	int index = 0;
	int image_list_size = 1;
	if (args->prompt) {
		/* print out all results and its properties */
		image_list_size = print_sim_results(image_list);
		do {
			printf("Which one to download? (-1 to exit): ");
		} while (scanf("%d", &index) != 1);
	}

	if (index < 0 || index >= image_list_size) {
		fprintf(stderr, "Error: Invalid option selected\n");

		/* free up allocated memory */
		free_similar_image_list(image_list);
		return 1;
	}
	int exit_code = ruiji_get_image(args, image_list, index);

	/* free up allocated memory */
	free_similar_image_list(image_list);

	return exit_code;
}

int ruiji_validate_file(char *file_name)
{
	/* check if selected image file exists */
	FILE *img_fd;
	/* rb for reading binary file */
	img_fd = fopen(file_name, "rb");
	if (img_fd == NULL) {
		perror(file_name);
		return 1;
	}

	int valid = 0;
	/* get the size of the image file */
	fseek(img_fd, 0L, SEEK_END);
	long image_size = ftell(img_fd);
	/* Check if it exceeds the max file size limit */
	if (image_size > MAX_FILE_SIZE) {
		fprintf(stderr, "Error: Maximum file size exceeded (%dKB)\n",
			MAX_FILE_SIZE / 1000);
		valid = 1;
	}
	fclose(img_fd);
	return valid;
}

void ruiji_exit(int exit_status)
{
	ruiji_curl_cleanup();
	exit(exit_status);
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
	set_default_opt(&args);

	int c, exit_status = 0;
	while ((c = getopt(argc, argv, "qt:yTV")) != EOF) {
		switch(c) {
		case 'q':
			args.verbose = false;
			break;
		case 't':
			args.threshold = atoi(optarg);
			break;
		case 'y':
			args.prompt = false;
			break;
		case 'T':
			args.tags = true;
			break;
		case 'V':
			fprintf(stderr, "%s-%s\n", argv[0], VERSION);
			return 0;
		default:
			exit_status = 1;
			break;
		}
	}

	if (exit_status || optind == argc) {
		print_help();
		return 1;
	}
	if (ruiji_validate_file(argv[optind]) == 0) {
		exit_status = ruiji(&args, argv[optind]);
	}

	ruiji_exit(exit_status);
}
