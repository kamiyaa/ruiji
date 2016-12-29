#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "domains.h"
#include "parser.h"
#include "udload.h"

int main(int argv, char *argc[])
{
	/* Check if we have at least an additional argument,
	 * which is the file name */
	if (argv < 2) {
		fprintf(stderr, "Error: No given argument\n");
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
	char *html_data = upload_image(argc[1], IQDB_URL);
	printf("Upload successful\n\n");

	/* Initialize a struct to hold all the images similar
	 * to the uploaded image */
	struct similar_image_db sim_db;
	populate_sim_db(&sim_db, html_data);

	/* If any results were found, ask user which to download */
	if (sim_db.size > 0) {
		/* Print out all results and its properties */
		print_sim_results(&sim_db);

		/* Ask user which website they would like to download from */
		printf("Which one would you like to download?: ");
		int user_input;
		scanf("%d", &user_input);

		/* Select the selected image */
		struct similar_image *dl_image = sim_db.img_db[user_input];

		int dl_state = -1;
		char *dl_url;
		char *dl_location;

		if (strstr(dl_image->link, DANBOORU_DOMAIN)) {
			dl_url = danbooru_get_image_url(dl_image->link);
			dl_location = get_server_file_name(dl_url, ' ');
			printf("Saving image as %s from %s...\n", dl_location, dl_url);
			dl_state = download_image(dl_url, dl_location);
		}
		else if (strstr(dl_image->link, SANKAKU_COMPLEX_DOMAIN)) {
			dl_url = sankaku_complex_get_image_url(dl_image->link);
			dl_location = get_server_file_name(dl_url, '?');
			printf("Saving image as %s from %s...\n", dl_location, dl_url);
			dl_state = download_image(dl_url, dl_location);
		}
		else if(strstr(dl_image->link, YANDERE_DOMAIN)) {
			dl_url = yandere_get_image_url(dl_image->link);
			dl_location = get_server_file_name(dl_url, ' ');
			printf("Saving image as %s from %s...\n", dl_location, dl_url);
			dl_state = download_image(dl_url, dl_location);
		}
		else if(strstr(dl_image->link, KONACHAN_DOMAIN)) {
			dl_url = konachan_get_image_url(dl_image->link);
			dl_location = get_server_file_name(dl_url, ' ');
			printf("Saving image as %s from %s...\n", dl_location, dl_url);
			dl_state = download_image(dl_url, dl_location);
		}
		else if(strstr(dl_image->link, ESHUUSHUU_DOMAIN)) {
			dl_url = eshuushuu_get_image_url(dl_image->link);
			dl_location = get_server_file_name(dl_url, ' ');
			printf("Saving image as %s from %s...\n", dl_location, dl_url);
			dl_state = download_image(dl_url, dl_location);
		}

		if (dl_state == 0)
			printf("Done!\n");
		else
			printf("Error: Download failed\n");
		/* Free allocated memory */
		if (dl_location)
			free(dl_location);
		if (dl_url)
			free(dl_url);
	}
	else
		printf("No similar results! :(\n");

	/* Free allocated memory */
	if (html_data)
		free(html_data);

	return 0;
}
