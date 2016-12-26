struct html_data {
	char *data;
	size_t size;
};


struct similar_image {
	char *link;
	unsigned int similarity;
	char *dl_link;
	unsigned int dimensions[2];
	unsigned int size;
};


struct similar_image_db {
	struct similar_image *img_db[20];
	unsigned int size;
};


struct similar_image *create_sim_image(char *url_begin, unsigned int similarity, unsigned int x, unsigned int y);

size_t StoreData(void *contents, size_t size, size_t nmemb, struct html_data *userp);

char *parse_similarity_percentage(char *contents, unsigned int *similarity);
char *parse_xy_img_dimensions(char* contents, unsigned int *x, unsigned int *y);

void parse_sim_info(struct similar_image_db *sim_db, char *html_content);

char *get_html(char *web_url);
