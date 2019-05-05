#include "structs.h"

void image_download_toast(char *website_url);

void image_save_toast(char *file_name);

void image_upload_toast(char *file_name, char *website_url);

void print_help(void);

void print_similar_image_result(struct similar_image_result *img);

void print_image_tags(struct image_tag_db *tags_db);

void print_sim_results(struct similar_image_llnode *image_list);

