#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "domains.h"
#include "interface.h"
#include "udload.h"

#define IQDB_UPLOAD_FIELD "file"
#define DANBOORU_SOURCE_ID "Size: <a href=\""
#define DANBOORU_URL "http://danbooru.donmai.us"

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
	if (sim_db.size) {
		/* Print out all results and its properties */
		print_sim_results(&sim_db);

		/* Ask user which website they would like to download from */
		printf("Which one would you like to download?: ");
		int user_input;
		scanf("%d", &user_input);

		if (user_input < 0 || user_input >= sim_db.size) {
			printf("Error: Invalid option selected\n");
			return 1;
		}

		/* Select the selected image */
		struct similar_image *dl_image = sim_db.img_db[user_input];

		int dl_state = -1;
		char stop_seq = '\0';
		char *dl_url = NULL;

		if (strstr(dl_image->link, DANBOORU_DOMAIN)) {
			dl_url = danbooru_get_image_url(dl_image->link);
		}
		else if (strstr(dl_image->link, SANKAKU_COMPLEX_DOMAIN)) {
			dl_url = sankaku_complex_get_image_url(dl_image->link);
			stop_seq = '?';
		}
		else if (strstr(dl_image->link, YANDERE_DOMAIN)) {
			dl_url = yandere_get_image_url(dl_image->link);
		}
		else if (strstr(dl_image->link, KONACHAN_DOMAIN)) {
			stop_seq = ' ';
		}
		else if (strstr(dl_image->link, ESHUUSHUU_DOMAIN)) {
			dl_url = eshuushuu_get_image_url(dl_image->link);
		}
		else if (strstr(dl_image->link, GELBOORU_DOMAIN)) {
			dl_url = gelbooru_get_image_url(dl_image->link);
		}
	/*	else if (strstr(dl_image->link, ZEROCHAN_DOMAIN)) {
			dl_url = zerochan_get_image_url(dl_image->link);
			stop_seq = ' ';
		} */

		if (dl_url) {
			/* Get the name of the file */
			char *file_save_name;
			file_save_name = get_server_file_name(dl_url,
							stop_seq);
			/* Notify the user */
			image_save_toast(file_save_name, dl_url);
			/* Save the image as it's name on the server */
			dl_state = download_image(dl_url, file_save_name);
			/* Free allocated memory */
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

	/* Free up allocated memory */
	free_similar_image_db(&sim_db);
	/* clean up curl */
        curl_global_cleanup();

	return 0;
}

/*	dl_url = get_image_url(dl_image->link, DANBOORU_SOURCE_ID, DANBOORU_URL, '"'); */
