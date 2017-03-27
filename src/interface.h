#include "parser.h"

/* Given a similar_image, print out all its information */
void print_similar_image_info(struct similar_image *img);

/* Gives user verbose output on image download */
void image_download_toast(char *website_url);

/* Gives user verbose output on image save */
void image_save_toast(char *file_name);

/* Gives user verbose output on image upload */
void image_upload_toast(char *file_name, char *website_url);

/* Prints help menu for user */
void print_help(void);

/* Given a similar_image_db, print out all its contents */
void print_sim_results(struct similar_image_db *sim_db);

