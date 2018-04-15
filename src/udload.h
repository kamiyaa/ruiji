
/* Given a url and the name to save as, download the file from the website
 * and return a integer indicating if successful or not.
 * 0 = successful
 * !0 = something went wrong
 */
int download_image(char *dl_url, char *file_name);

/* Given the full link of a website,
 * fetch and return the html source of the website
 */
char *get_html(char *web_url);

size_t StoreData(char *contents, size_t size, size_t nmemb, void *user_struct);

/* Given the name of an existing file and a website to upload it to,
 * upload the file and return the html content of the website after
 */
char *upload_image(char *website, char *file_name, char *field_name);

void free_html_data(struct html_data *web_data);

/* Clean up curl */
void ruiji_curl_cleanup(void);
