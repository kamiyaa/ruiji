#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "interface.h"

#define VERSION "0.1"

#define IQDB_URL "https://iqdb.org"
#define IQDB_UPLOAD_FIELD "file"
#define DANBOORU_SOURCE_ID "Size: <a href=\""

/* struct for holding command line arguments */
struct arg_options {
	short verbose;
	short prompt;
	short showhelp;
	short showversion;
};

/* set default command line options */
void set_default_opt(struct arg_options *arg_opt)
{
	arg_opt->verbose = 1;
	arg_opt->prompt = 1;
	arg_opt->showhelp = 0;
	arg_opt->showversion = 0;
}

/* parse command line arguments */
void parse_arguments(int argv, char **argc, struct arg_options *arguments)
{
	int c;
	while ((c = getopt(argv, argc, "hvy")) != -1) {
		switch(c) {
		case 'h':
			arguments->showhelp = 1;
			break;
		case 'q':
			arguments->verbose = 0;
			break;
		case 'v':
			arguments->showversion = 1;
			break;
		case 'y':
			arguments->prompt = 0;
			break;
		}
	}
}


int main(int argv, char *argc[])
{
	/* Check if we have at least an additional argument,
	 * which is the file name */
	if (argv < 2) {
		printf("Error: No given argument\n");
		return 1;
	}

	/* create a struct for command line arguments,
	 * set it to default options, then parse for any
	 * user options */
	struct arg_options arg_opt;
	set_default_opt(&arg_opt);
	parse_arguments(argv, argc, &arg_opt);


	if (arg_opt.showhelp) {
		print_help();
		return 0;
	}
	if (arg_opt.showversion) {
		printf("ruiji-%s\n", VERSION);
		return 0;
	}

	int fi = argv - 1;
	/* check if selected image file exists */
	FILE *img_fd;
	img_fd = fopen(argc[fi], "rb");
	if (!img_fd) {
		printf("Error: No such file\n");
		return 1;
	}
	fclose(img_fd);

	/* Get the html output after uploading the image */
	char *html_data = upload_image(IQDB_URL, argc[fi], IQDB_UPLOAD_FIELD);
	printf("Upload successful\n\n");

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_db sim_db;
	populate_sim_db(&sim_db, html_data);
	free(html_data);

	int user_input = 0;
	/* if any results were found, ask user which to download */
	if (sim_db.size) {
		if (arg_opt.prompt) {
			/* print out all results and its properties */
			print_sim_results(&sim_db);

			/* ask user which website they would like to download from */
			printf("Which one would you like to download?: ");
			scanf("%d", &user_input);

			if (user_input < 0 || user_input >= sim_db.size) {
				printf("Error: Invalid option selected\n");
				return 1;
			}
		}

		/* select the selected image */
		struct similar_image *dl_image = sim_db.img_db[user_input];

		/* used to check if download was successful */
		int dl_state = -1;
		/* used to know where to slice string for getting
		 * file name */
		char stop_seq = '\0';
		/* get source image url */
		char *dl_url = get_image_url(dl_image->link, &stop_seq);

		if (dl_url) {
			/* get the name of the file */
			char *file_save_name;
			file_save_name = get_server_file_name(dl_url,
							stop_seq);
			/* notify the user */
			image_save_toast(file_save_name, dl_url);
			/* save the image as it's name on the server */
			dl_state = download_image(dl_url, file_save_name);
			/* free allocated memory */
			free(file_save_name);
			free(dl_url);
		}

		if (!dl_state)
			printf("Done!\n");
		else
			printf("Error: Download failed\n");
	}
	else
		printf("No similar results! :(\n");

	/* free up allocated memory */
	free_similar_image_db(&sim_db);
	/* clean up curl */
	ruiji_curl_cleanup();

	return 0;
}

/*	dl_url = get_image_url(dl_image->link, DANBOORU_SOURCE_ID, DANBOORU_URL, '"');
*/
