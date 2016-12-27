#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.h"
#include "udload.h"
#include "danbooru.h"
#include "sankakucomplex.h"
#include "domains.h"


int main(int argv, char *argc[])
{
	/* Check if we have at least an additional argument,
	 * which is the file name */
	if (argv < 2) {
		fprintf(stderr, "Error: No given argument\n");
		return 1;
	}

	/* Open selected file to upload and check if it exists */
	FILE *img_fd;
	img_fd = fopen(argc[1], "rb");
	if (!img_fd) {
		fprintf(stderr, "Error: No such file\n");
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

	/* Print out all results and its properties */
	for (int i = 0; i < sim_db.size; i++) {
		struct similar_image *img_a = sim_db.img_db[i];
		printf("[%d]\n", i);
		printf("source: %s\n", img_a->link);
		printf("similarity: %u%%\n", img_a->similarity);
		printf("dimensions: %ux%u\n\n", img_a->dimensions[0], img_a->dimensions[1]);
	}

	/* Ask user which website they would like to download from */
	printf("Which one would you like to download?: ");
	int user_input;
	scanf("%d", &user_input);

	/* Select the selected image */
	struct similar_image *dl_image = sim_db.img_db[user_input];

	int return_val = -1;
	char *dl_url;
	char *dl_location;
	if (strstr(dl_image->link, DANBOORU_DOMAIN)) {
		dl_url = danbooru_get_image_url(dl_image->link);
		dl_location = get_server_file_name(dl_url, ' ');
		return_val = download_image(dl_url, dl_location);
	}
	else if (strstr(dl_image->link, SANKAKU_COMPLEX_DOMAIN)) {
		dl_url = sankaku_complex_get_image_url(dl_image->link);
		dl_location = get_server_file_name(dl_url, '?');
		return_val = download_image(dl_url, dl_location);
	}

	if (return_val == 0)
		printf("Done!\n");
	else {
		printf("Error: Download failed\n");
		return 1;
	}

	free(dl_url);
	free(html_data);

	return 0;
}
