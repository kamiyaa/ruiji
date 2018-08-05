#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include "structs.h"
#include "udload.h"

#define USER_AGENT "chrome/65"

/* Given a url and the name to save as, download the file from the website
 * and return a integer indicating if successful or not.
 * 0 = successful
 * !0 = something went wrong
 */
int download_image(char *web_url, char *file_name)
{
	int result = 0;
	FILE *img_fp;
	/* Check if we have write permissions */
	if ((img_fp = fopen(file_name, "wb")) == NULL) {
		perror(file_name);
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
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, img_fp);

		res = curl_easy_perform(curl_handle);

		/* cleanup */
		curl_easy_cleanup(curl_handle);
		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr,
				"curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			fprintf(stderr,	"url: %s\n", web_url);
			result = 1;
		}
	}
	fclose(img_fp);

	return result;
}

/* clean up curl */
void ruiji_curl_cleanup()
{
	/* clean up curl */
	curl_global_cleanup();
}

/* Given the full link of a website, fetch and return the
 * html source of the website
 */
char *get_html(char *web_url)
{
	struct html_data web_data;
	/* will be grown as needed by the realloc above */
	web_data.data = malloc(sizeof(char) * 16);
	/* no data at this point */
	web_data.size = 0;
	web_data.realsize = 16;

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode result;

	if (curl_handle) {

		/* Set cURL to follow redirects */
		curl_easy_setopt(curl_handle, CURLOPT_FOLLOWLOCATION, 1L);
		/* Set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, web_url);

		/* Set the user agent to chrome */
		curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, USER_AGENT);

		/* Set the function to call when data is received */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, StoreData);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &web_data);

		/* Perform the network protocol */
		result = curl_easy_perform(curl_handle);

		/* Check for errors */
		if (result != CURLE_OK) {
			fprintf(stderr,
				"curl_easy_perform() failed: %s\n",
				curl_easy_strerror(result));
			fprintf(stderr,	"url: %s\n", web_url);
		}

		/* cleanup */
		curl_easy_cleanup(curl_handle);
	}

	return web_data.data;
}

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct)
{
	struct html_data *htmlst = (struct html_data *)user_struct;

	size_t realsize = size * nmemb;

	/* if we run out of space, we will double our space to reduce future
	 * reallocs
	 */
	if (htmlst->realsize - htmlst->size <= realsize) {
		htmlst->realsize = (htmlst->realsize + realsize) * 2;
		void *new_mem_ptr = realloc(htmlst->data, htmlst->realsize);
		if (new_mem_ptr == NULL) {
			/* out of memory! */
			fprintf(stderr, "not enough memory (realloc returned NULL)\n");
			return 0;
		}
		htmlst->data = new_mem_ptr;
	}

	memcpy(&(htmlst->data[htmlst->size]), contents, realsize);
	htmlst->size += realsize;
	/* null terminate string */
	htmlst->data[htmlst->size] = '\0';
	return realsize;
}

/* Given the name of an existing file and a website to upload it to,
 * upload the file and return the html content of the website after
 */
char *upload_image(char *web_url, char *file_name, char *field_name)
{
	struct html_data web_data;
	/* will be grown as needed by the realloc above */
	web_data.data = malloc(sizeof(char) * 16);
	/* no data at this point */
	web_data.size = 0;
	web_data.realsize = 16;

	/* Initialize curl */
	CURL *curl_handle = curl_easy_init();
	CURLcode res;

	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;

	if (curl_handle) {
		/* set the working website to this domain */
		curl_easy_setopt(curl_handle, CURLOPT_URL, web_url);

		/* Fill in the file upload field */
		curl_formadd(&formpost, &lastptr,
			CURLFORM_PTRNAME, field_name,
			CURLFORM_FILE, file_name,
			CURLFORM_END);

		/* Fill out the upload form */
		curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, formpost);

		/* Set the function to call when data is received */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, StoreData);

		/* Set the data to pass when the function is called */
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &web_data);

		res = curl_easy_perform(curl_handle);

		/* Check for errors */
		if (res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
			fprintf(stderr,	"url: %s\n", web_url);
		}

		/* then cleanup the formpost chain */
		curl_formfree(formpost);

		/* cleanup */
		curl_easy_cleanup(curl_handle);
	}

	return web_data.data;
}

void free_html_data(struct html_data *web_data)
{
	if (web_data->data)
		free(web_data->data);
	free(web_data);
}
