#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <argp.h>

#include "interface.h"

#define VERSION "0.2"

#define IQDB_URL "https://iqdb.org"
#define IQDB_UPLOAD_FIELD "file"
#define DANBOORU_SOURCE_ID "Size: <a href=\""
#define MAX_FILE_SIZE 8192000

/* Program documentation. */
const char *argp_program_bug_address =
	"report bugs to https://github.com/Kamiyaa/ruiji";
static char ruiji_doc[] =
	"\nReverse image searching program using iqdb.org";


/* struct for holding command line arguments */
struct ruiji_arg_opts {
	char *args[2];
	short silent;
	short verbose;
	short prompt;
	short showhelp;
	short showversion;
	unsigned short threshold;
	char *file;
};


/* struct for command line argument options */
static struct argp_option options[] = {
	{ "file",	'f',	"FILE", 0,
		"Takes in the given file to upload" },
	{ "help",	'h',	0, 0,
		"Show help message" },
	{ "noprompt",	'y',	0, 0,
		"Skips user interactions and downloads the most similar image" },
	{ "quiet",	'q',	0, 0,
		"Suppress verbose output" },
	{ "threshold",	't',	"number", 0,
		"Only show images above certain similarity percentage" },
	{ "verbose",	'v',	0, 0,
		"Produce verbose output" },
	{ "version",	'V',	0, 0,
		"Show version number" },
	{ 0 }
};


/* set default command line options */
void set_default_opt(struct ruiji_arg_opts *arg_opt)
{
	arg_opt->verbose = 1;
	arg_opt->silent = 0;
	arg_opt->prompt = 1;
	arg_opt->showhelp = 0;
	arg_opt->showversion = 0;
	arg_opt->threshold = 0;
	arg_opt->file = "file.png";
}


/* Parse and process command line arguments */
error_t parse_opt(int key, char *arg, struct argp_state *state)
{
	struct ruiji_arg_opts *arguments = state->input;

	switch (key) {
	case 'f':
		arguments->file = arg;
		break;
	case 'h':
		arguments->showhelp = 1;
		break;
	case 'q':
		arguments->silent = 1;
		break;
	case 't':
		arguments->threshold = atoi(arg);
		break;
	case 'v':
		arguments->verbose = 1;
		break;
	case 'y':
		arguments->prompt = 0;
		break;
	case 'V':
		arguments->showversion = 1;
		break;
//	case ARGP_KEY_ARG:
//		arguments->args[state->arg_num] = arg;
//		break;
	case ARGP_KEY_END:
		/* Not enough arguments */
		if (state->arg_num >= 2)
			argp_usage (state);
		break;

	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}


static struct argp ruiji_args = {
	options, parse_opt,
	0, ruiji_doc
};



int main(int argc, char *argv[])
{
	int exit_code = 0;
	/* create new arguments struct for ruiji and set them to default values */
	struct ruiji_arg_opts arg_opts;
	set_default_opt(&arg_opts);

	/* parse given command line arguments */
	argp_parse(&ruiji_args, argc, argv, 0, 0, &arg_opts);

	/* If one wants to see, version number, just print and exit */
	if (arg_opts.showversion) {
		printf("ruiji-%s\n", VERSION);
		return exit_code;
	}
	if (arg_opts.showhelp) {
		print_help();
		return exit_code;
	}

	/* check if selected image file exists */
	FILE *img_fd;
	char *file_name = arg_opts.file;
	img_fd = fopen(file_name, "rb");
	if (!img_fd) {
		printf("Error: No such file: %s\n", file_name);
		return 1;
	}
	/* get the size of the image file */
	fseek(img_fd, 0L, SEEK_END);
	int image_size = ftell(img_fd);
	/* Check if it exceeds the max file size limit */
	if (image_size > MAX_FILE_SIZE) {
		printf("Error: File size limit exceeded (%dKB)\n", MAX_FILE_SIZE / 1000);
		fclose(img_fd);
		return 1;
	}
	/* Close the file handle */
	fclose(img_fd);

	/* Get the html output after uploading the image */
	char *html_data = upload_image(IQDB_URL, file_name, IQDB_UPLOAD_FIELD);
	printf("Upload successful\n\n");

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_db sim_db;
	populate_sim_db(&sim_db, html_data, arg_opts.threshold);
	free(html_data);

	/* if any results were found, ask user which to download */
	if (sim_db.size) {
		short user_input = 0;
		if (arg_opts.prompt) {
			/* print out all results and its properties */
			print_sim_results(&sim_db);

			/* ask user which website they would like to download from */
			printf("Which one would you like to download? (-1 to exit): ");
			/* get short int input */
			scanf("%hd", &user_input);
		}

		if (user_input < 0 || user_input >= sim_db.size) {
			printf("Error: Invalid option selected\n");
			exit_code = 1;
		}

		else {
			/* select the selected image */
			struct similar_image *dl_image = sim_db.img_db[user_input];

			/* used to check if download was successful */
			short dl_state = -1;
			/* used to know where to slice string for getting
			 * file name. Default is NULL character */
			char stop_seq = '\0';
			/* get source image url */
			char *dl_url = get_image_url(dl_image->link, &stop_seq);

			/* Check if we've successfully parsed the source image
			 * url */
			if (dl_url) {
				/* Notify the user we are downloading the image */
				image_download_toast(dl_image->link);

				/* get the name of the file from its server */
				char *file_save_name =
					get_server_file_name(dl_url, stop_seq);

				/* notify the user */
				image_save_toast(file_save_name);

				/* save the image as it's name on the server */
				dl_state =
					download_image(dl_url, file_save_name);
				/* free allocated memory */
				free(file_save_name);
				free(dl_url);
			}
			/* Report back to the user how the download went */
			if (!dl_state)
				printf("Done!\n");
			else
				printf("Error: Download failed\n");
		}
	}
	else {
		printf("No results! :(\n");
		exit_code = 1;
	}

	/* free up allocated memory */
	free_similar_image_db(&sim_db);
	/* clean up curl */
	ruiji_curl_cleanup();

	return exit_code;
}
