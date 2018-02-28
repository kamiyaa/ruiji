#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>

#include "interface.h"
#include "parser.h"
#include "udload.h"

#define VERSION "0.7.0"

#define IQDB_URL "https://iqdb.org"
#define IQDB_UPLOAD_FIELD "file"
#define MAX_FILE_SIZE 8192000

/* struct for holding command line arguments */
struct ruiji_cmd_args {
	char *args[2];
	short silent;
	short verbose;
	short prompt;
	short showhelp;
	short showtags;
	short showversion;
	unsigned short threshold;
	char *file;
};

void set_default_opt(struct ruiji_cmd_args *arg_opt);
error_t parse_opt(int key, char *arg, struct argp_state *state);
int validate_upload_file(char *file_name);
short initialize(struct similar_image_llnode *image_list);


/* create new arguments struct for ruiji and set them to default values */
static struct ruiji_cmd_args cmd_args;

/* struct for command line argument options */
static struct argp_option options[] = {
	{ "help",	'h',	0, 0,
		"Show help message", 0 },
	{ "noprompt",	'y',	0, 0,
		"Skips user interactions and downloads the most similar image", 0 },
	{ "quiet",	'q',	0, 0,
		"Suppress verbose output", 0 },
	{ "threshold",	't',	"number", 0,
		"Only show images above certain similarity percentage", 0 },
	{ "tags",	'T',	0, 0,
		"Outputs tags of downloaded image", 0 },
	{ "version",	'v',	0, 0,
		"Show version number", 0 },
	{ 0 }
};

static struct argp ruiji_args = {
	options, parse_opt,
	NULL, NULL, NULL, NULL, NULL
};


/* set default command line options */
void set_default_opt(struct ruiji_cmd_args *arg_opt)
{
	arg_opt->verbose = 1;
	arg_opt->prompt = 1;
	arg_opt->showhelp = 0;
	arg_opt->showtags = 0;
	arg_opt->showversion = 0;
	arg_opt->threshold = 0;
}

/* Parse and process command line arguments */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct ruiji_cmd_args *arguments = state->input;

	switch (key) {
	case 'h':
		arguments->showhelp = 1;
		break;
	case 'q':
		arguments->verbose = 0;
		break;
	case 't':
		arguments->threshold = atoi(arg);
		break;
	case 'v':
		arguments->showversion = 1;
		break;
	case 'y':
		arguments->prompt = 0;
		break;
	case 'T':
		arguments->showtags = 1;
		break;
	case ARGP_KEY_NO_ARGS:
		arguments->file = arg;
		break;
	case ARGP_KEY_ARG:
		arguments->file = arg;
		break;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

int validate_upload_file(char *file_name)
{
	int valid = 0;

	/* check if selected image file exists */
	FILE *img_fd;
	/* rb for reading binary file */
	img_fd = fopen(file_name, "rb");
	if (img_fd == NULL) {
		perror(file_name);
		valid = 1;
	}
	else {
		/* get the size of the image file */
		fseek(img_fd, 0L, SEEK_END);
		int image_size = ftell(img_fd);
		/* Check if it exceeds the max file size limit */
		if (image_size > MAX_FILE_SIZE) {
			printf("Error: Maximum file size exceeded (%dKB)\n",
				MAX_FILE_SIZE / 1000);
			valid = 1;
		}
		/* Close the file handle */
		fclose(img_fd);
	}
	return valid;
}

void ruiji_get_tags(enum domain_t domain_uid, char *api_content)
{
	struct image_tag_db *tag_db =
		get_image_tags(domain_uid, api_content);
	printf("Tags:\n");
	print_image_tags(tag_db);
	free_image_tags(tag_db);
}

int ruiji_get_image(struct similar_image_llnode *image_list, int index)
{
	struct similar_image_llnode *list_ptr = image_list;
	for (int i = 0; i < index; i++)
		list_ptr = list_ptr->next;

	struct similar_image *dl_image = list_ptr->image;

	/* get internal uid of domain */
	enum domain_t domain_uid = get_domain_uid(dl_image->post_link);
	/* get api link for post_link */
	char *api_link = generate_api_link(domain_uid, dl_image->post_link);

	/* get the html contents of the website */
	char *api_content = (api_link == NULL) ?
				NULL : get_html(api_link);
	free(api_link);

	/* parse for the source url of the image */
	char *dl_url = (api_content == NULL) ?
			NULL : get_image_source_url(domain_uid, api_content);

	/* used to check if download was successful */
	int dl_status = 1;

	/* Check if we've successfully parsed the source image
	 * url */
	if (dl_url) {
		/* get the name of the file from its server */
		char *file_save_name = get_server_file_name(dl_url);

		/* Notify the user we are downloading the image */
		if (cmd_args.verbose)
			image_download_toast(dl_image->post_link);
		/* notify the user */
		if (cmd_args.verbose)
			image_save_toast(file_save_name);

		/* save the image as it's name on the server */
		dl_status = download_image(dl_url, file_save_name);

		/* print tags if told to */
		if (cmd_args.showtags)
			ruiji_get_tags(domain_uid, api_content);

		/* free allocated memory */
		free(file_save_name);
		free(dl_url);

	}
	if (api_content)
		free(api_content);
	/* Report back to the user how the download went */
	return dl_status;
}

int main(int argc, char **argv)
{
	int exit_status;
	set_default_opt(&cmd_args);

	/* parse given command line arguments */
	argp_parse(&ruiji_args, argc, argv, 0, 0, &cmd_args);

	/* If one wants to see, version number, just print and exit */
	if (cmd_args.showversion) {
		fprintf(stderr, "%s-%s\n", argv[0], VERSION);
		return 0;
	}
	if (cmd_args.showhelp) {
		print_help();
		return 0;
	}

	/* return value */
	exit_status = validate_upload_file(cmd_args.file);
	if (exit_status)
		return exit_status;

	/* notify user we are uploading argument file */
	if (cmd_args.verbose)
		image_upload_toast(cmd_args.file, IQDB_URL);

	char *iqdb_html = upload_image(IQDB_URL, cmd_args.file, IQDB_UPLOAD_FIELD);

	if (iqdb_html == NULL) {
		fprintf(stderr, "Error: Failed to upload file\n");
		ruiji_curl_cleanup();
		return 1;
	}

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_llnode *image_list =
		create_image_list(iqdb_html, cmd_args.threshold);
	/* free up allocated memory */
	free(iqdb_html);

	if (image_list) {
		/* initialize image selection to 0 */
		int index = 0;
		int image_list_size = 1;
		if (cmd_args.prompt) {
			/* print out all results and its properties */
			image_list_size = print_sim_results(image_list);

			do {
				/* ask user which website they would like to download from */
				printf("Which one to download? (-1 to exit): ");
			} while (scanf("%d", &index) != 1);
		}

		if (index < 0 || index >= image_list_size) {
			fprintf(stderr, "Error: Invalid option selected\n");
			exit_status = 1;
		}
		else if ((exit_status = ruiji_get_image(image_list, index)) != 0) {
			fprintf(stderr, "Error: Download failed\n");
		}

		/* free up allocated memory */
		free_similar_image_list(image_list);
	}
	else {
		fprintf(stderr, "No results found! :(\n");
		exit_status = 1;
	}

	/* clean up curl */
	ruiji_curl_cleanup();

	return exit_status;
}
