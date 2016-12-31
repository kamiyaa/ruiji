#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domains.h"
#include "parser.h"
#include "udload.h"

#define IQDB_UPLOAD_FIELD "file"

int main(int argv, char *argc[])
{
	/* Check if we have at least an additional argument,
	 * which is the file name */
	if (argv < 2) {
		printf("Error: No given argument\n");
		return 1;
	}

	/* check if selected image file exists */
	FILE *img_fd;
	img_fd = fopen(argc[1], "rb");
	if (!img_fd) {
		printf("Error: No such file\n");
		return 1;
	}
	fclose(img_fd);

	/* Get the html output after uploading the image */
	printf("Uploading %s to %s...\n", argc[1], IQDB_URL);
	char *html_data = upload_image(IQDB_URL, argc[1], IQDB_UPLOAD_FIELD);
	printf("Upload successful\n\n");

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_db sim_db;
	populate_sim_db(&sim_db, html_data);
	free(html_data);

	/* If any results were found, ask user which to download */
	if (sim_db.size > 0) {
		/* Print out all results and its properties */
		print_sim_results(&sim_db);

		/* Ask user which website they would like to download from */
		printf("Which one would you like to download?: ");
		int user_input;
		scanf("%d", &user_input);

		if (user_input >= sim_db.size) {
			printf("Error: Invalid option selected\n");
			return 1;
		}

		/* Select the selected image */
		struct similar_image *dl_image = sim_db.img_db[user_input];

		int dl_state = -1;
		char stop_seq;
		char *dl_url, *save_location;

		if (strstr(dl_image->link, DANBOORU_DOMAIN)) {
			dl_url = danbooru_get_image_url(dl_image->link);
			stop_seq = ' ';
		}
		else if (strstr(dl_image->link, SANKAKU_COMPLEX_DOMAIN)) {
			dl_url = sankaku_complex_get_image_url(dl_image->link);
			stop_seq = '?';
		}
		else if (strstr(dl_image->link, YANDERE_DOMAIN)) {
			dl_url = yandere_get_image_url(dl_image->link);
			stop_seq = ' ';
		}
		else if (strstr(dl_image->link, KONACHAN_DOMAIN)) {
			dl_url = konachan_get_image_url(dl_image->link);
			stop_seq = ' ';
		}
		else if (strstr(dl_image->link, ESHUUSHUU_DOMAIN)) {
			dl_url = eshuushuu_get_image_url(dl_image->link);
			stop_seq = ' ';
		}
	/*	else if (strstr(dl_image->link, ZEROCHAN_DOMAIN)) {
			dl_url = zerochan_get_image_url(dl_image->link);
			stop_seq = ' ';
		} */
		else if (strstr(dl_image->link, GELBOORU_DOMAIN)) {
			dl_url = gelbooru_get_image_url(dl_image->link);
			stop_seq = ' ';
		}

		if (stop_seq) {
			save_location = get_server_file_name(dl_url, stop_seq);
			printf("Saving image as %s from %s...\n", save_location, dl_url);
			dl_state = download_image(dl_url, save_location);
			/* Free allocated memory */
			free(save_location);
			free(dl_url);
		}

		if (dl_state == 0)
			printf("Done!\n");
		else
			printf("Error: Download failed\n");
	}
	else
		printf("No similar results! :(\n");

	free_similar_image_db(&sim_db);

	return 0;
}
