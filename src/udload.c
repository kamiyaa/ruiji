#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "parser.h"

#define IQDB_LOOKFOR "match</th></tr><tr><td class='image'><a href=\""

char *upload_image(char *file_name, char* website)
{
	/* Open selected file to upload and check if it exists */
	FILE *img_fd;
	img_fd = fopen(file_name, "rb");

	struct html_data web_data;
	/* will be grown as needed by the realloc above */
	web_data.data = malloc(1);
	/* no data at this point */
	web_data.size = 0;

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode res;
	/* Record upload speed and time */
	/* double upload_speed, total_time; */

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;

	curl_global_init(CURL_GLOBAL_ALL);

	if (curl_handle) {
		/* enable verbose for easier tracing
		 * curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L); */

		/* set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, website);

		/* Fill in the file upload field */
		curl_formadd(&formpost, &lastptr,
			CURLFORM_COPYNAME, "file",
			CURLFORM_FILE, file_name,
			CURLFORM_END);

		/* Fill out the upload form */
		curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);

		/* set where to read from */
		curl_easy_setopt(curl_handle, CURLOPT_READDATA, img_fd);

		/* Set the function to call when data is received */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, StoreData);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &web_data);

		res = curl_easy_perform(curl_handle);

		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

		}

		/*else { */
			/* now extract transfer info */
/*			curl_easy_getinfo(curl_handle, CURLINFO_SPEED_UPLOAD, &upload_speed);
			curl_easy_getinfo(curl_handle, CURLINFO_TOTAL_TIME, &total_time);

			printf("Speed: %.3f bytes/sec in %.3f seconds\n",
				upload_speed, total_time);
		}*/

		/* cleanup */
		curl_easy_cleanup(curl_handle);

		/* then cleanup the formpost chain */
		curl_formfree(formpost);
	}
	fclose(img_fd);

	return web_data.data;
}

int download_image(char* dl_url, char* file_name)
{
	FILE *img_fp;
	/* If NULL file_name was given, save it as the
	 * name on the server side 
	 */
	if (file_name == NULL) {
		file_name = dl_url;
		do {
			file_name = strstr(file_name, "/");
			file_name = &file_name[1];
		} while (strstr(file_name, "/"));
	}

	printf("Saving image as %s...\n", file_name);
	img_fp = fopen(file_name, "wb");

	if (!img_fp) {
		printf("Error: No write permissions");
		return 1;
	}

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode res;

	if (curl_handle) {
		/* set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, dl_url);

		/* Set the function to call when data is received */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, NULL);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, img_fp);

		res = curl_easy_perform(curl_handle);

		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
			curl_easy_strerror(res));

		}
	}
	fclose(img_fp);
	return 0;
}


