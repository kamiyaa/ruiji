#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>

#include "parser.h"

#define VERSION "0.5"

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


/* struct for command line argument options */
static struct argp_option options[] = {
	{ "help",	'h',	0, 0,
		"Show help message" },
	{ "noprompt",	'y',	0, 0,
		"Skips user interactions and downloads the most similar image" },
	{ "quiet",	'q',	0, 0,
		"Suppress verbose output" },
	{ "threshold",	't',	"number", 0,
		"Only show images above certain similarity percentage" },
	{ "tags",	'T',	0, 0,
		"Outputs tags of downloaded image" },
	{ "verbose",	'v',	0, 0,
		"Produce verbose output" },
	{ "version",	'V',	0, 0,
		"Show version number" },
	{ 0 }
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
	case 'y':
		arguments->prompt = 0;
		break;
	case 'T':
		arguments->showtags = 1;
		break;
	case 'V':
		arguments->showversion = 1;
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


static struct argp ruiji_args = {
	options, parse_opt
};

int validate_upload_file(char *file_name)
{
	int valid = 0;

	/* check if selected image file exists */
	FILE *img_fd;
	/* rb for reading binary file */
	img_fd = fopen(file_name, "rb");
	if (!img_fd) {
		printf("Error: No such file: %s\n", file_name);
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


int main(int argc, char *argv[])
{
	/* return value */
	short exit_code = 0;

	/* create new arguments struct for ruiji and set them to default values */
	struct ruiji_cmd_args cmd_args;
	set_default_opt(&cmd_args);

	/* parse given command line arguments */
	argp_parse(&ruiji_args, argc, argv, 0, 0, &cmd_args);

	/* If one wants to see, version number, just print and exit */
	if (cmd_args.showversion) {
		printf("ruiji-%s\n", VERSION);
		return 0;
	}
	if (cmd_args.showhelp) {
		print_help();
		return 0;
	}

	exit_code = validate_upload_file(cmd_args.file);
	if (exit_code)
		return exit_code;

	/* notify user we are uploading their file */
	if (cmd_args.verbose)
		image_upload_toast(cmd_args.file, IQDB_URL);
	char *iqdb_html = upload_image(IQDB_URL, cmd_args.file, IQDB_UPLOAD_FIELD);

	if (!iqdb_html) {
		fprintf(stderr, "Error: Failed to upload file\n");
		ruiji_curl_cleanup();
		return 1;
	}

	printf("\n");

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_llnode *image_list =
		create_image_list(iqdb_html, cmd_args.threshold);

	/* free up allocated memory */
	free(iqdb_html);

	/* if any results were found, ask user which to download */
	if (image_list) {
		/* initialize image selection to 0 */
		short user_input = 0;
		unsigned int image_list_size = 1;
		if (cmd_args.prompt) {
			/* print out all results and its properties */
			image_list_size = print_sim_results(image_list);

			/* ask user which website they would like to download from */
			printf("Which one would you like to download? (-1 to exit): ");
			/* get short int input */
			short retval = scanf("%hd", &user_input);
		}

		if (user_input < 0 || user_input >= image_list_size) {
			fprintf(stderr, "Error: Invalid option selected\n");
			exit_code = 1;
		}

		else {

			struct similar_image_llnode *list_ptr = image_list;
			for (int i = 0; i < user_input; i++) {
				list_ptr = list_ptr->next;
			}
			/* select the selected image */
			struct similar_image *dl_image = list_ptr->image;

			/* get internal uuid of domain */
			int domain_uuid =
				get_internal_domain_value(dl_image->post_link);
			char *api_link = generate_api_link(domain_uuid,
						dl_image->post_link);
			/* get the html contents of the website */
			char *html_content = get_html(api_link);
			free(api_link);

			/* used to know where to slice string for getting
			 * file name. Default is NULL character */
			char stop_seq = '\0';
			/* parse for the source url of the image */
			char *dl_url = get_source_image_url(domain_uuid,
					html_content, &stop_seq);

			/* used to check if download was successful */
			short dl_state = -1;

			/* Check if we've successfully parsed the source image
			 * url */
			if (dl_url) {
				/* Notify the user we are downloading the image */
				if (cmd_args.verbose)
					image_download_toast(dl_image->post_link);

				/* get the name of the file from its server */
				char *file_save_name =
					get_server_file_name(dl_url, stop_seq);

				/* notify the user */
				if (cmd_args.verbose)
					image_save_toast(file_save_name);

				/* save the image as it's name on the server */
				dl_state =
					download_image(dl_url, file_save_name);

				/* free allocated memory */
				free(file_save_name);
				free(dl_url);

				/* print tags if told to */
				if (cmd_args.showtags) {
					struct image_tag_db *tags_db =
						get_image_tags(
						domain_uuid, html_content);
					printf("Tags:\n");
					print_image_tags(tags_db);
					/* free allocated memory */
					free_image_tags(tags_db);
				}
			}
			/* Report back to the user how the download went */
			if (dl_state) {
				fprintf(stderr, "Error: Download failed\n");
				exit_code = 1;
			}
			if (html_content)
				free(html_content);
		}
	}
	else {
		printf("No results found! :(\n");
		exit_code = 1;
	}

	/* free up allocated memory */
	free_similar_image_list(image_list);
	/* clean up curl */
	ruiji_curl_cleanup();

	return exit_code;
}
