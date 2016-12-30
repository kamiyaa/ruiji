#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "parser.h"

#define IQDB_LOOKFOR "match</th></tr><tr><td class='image'><a href=\""

/* Given the name of an existing file and a website to upload it to,
 * upload the file and return the html content of the website after
 */
char *upload_image(char *website, char *file_name, char *field_name)
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

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;

	curl_global_init(CURL_GLOBAL_ALL);

	if (curl_handle) {
		/* set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, website);

		/* Fill in the file upload field */
		curl_formadd(&formpost, &lastptr,
			CURLFORM_COPYNAME, field_name,
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

		/* then cleanup the formpost chain */
		curl_formfree(formpost);

		/* cleanup */
		curl_easy_cleanup(curl_handle);

		/* Check for errors */
		if (res != CURLE_OK)
			printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
	}
	fclose(img_fd);

	return web_data.data;
}


/* Given a url and the name to save as, download the file from the website
 * and return a integer indicating if successful or not.
 * 0 = successful
 * !0 = something went wrong
 */
int download_image(char *web_url, char *file_name)
{
	FILE *img_fp;
	img_fp = fopen(file_name, "wb");

	/* Check if we have write permissions */
	if (!img_fp) {
		printf("Error: No write permissions");
		return 1;
	}

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode res;

	if (curl_handle) {
		/* ask libcurl to show us the verbose output */
		/* curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L); */

		/* set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, web_url);

		/* Set the user agent to chrome */
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "chrome/55.0.2883.75");

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, img_fp);

		res = curl_easy_perform(curl_handle);

		/* cleanup */
		curl_easy_cleanup(curl_handle);
	}
	fclose(img_fp);

	/* Check for errors */
	if (res != CURLE_OK) {
		printf("curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		return 1;
	}
	return 0;
}


