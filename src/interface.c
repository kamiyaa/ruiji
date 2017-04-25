#include <stdio.h>
#include <stdlib.h>

#include "structs.h"

#define COLOR_DEFAULT	"\x1B[0m"
#define COLOR_RED	"\x1B[31m"
#define COLOR_GREEN	"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE	"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN	"\x1B[36m"
#define COLOR_WHITE	"\x1B[37m"


void image_download_toast(char *website_url);
void image_save_toast(char *file_name);
void image_upload_toast(char *file_name, char *website_url);
void print_help(void);
void print_image_info(struct similar_image *img);
void print_image_tags(struct image_tag_db *tags_db);
void print_sim_results(struct similar_image_db *sim_db);


#ifdef COLOR

void image_download_toast(char *website_url)
{
	printf("Downloading from %s%s%s...\n",
		COLOR_BLUE, website_url, COLOR_DEFAULT);
}

/* A verbose message to the user when saving an image */
void image_save_toast(char *file_name)
{
	printf("Saving image as %s%s%s...\n",
		COLOR_YELLOW, file_name, COLOR_DEFAULT);
}

void image_upload_toast(char *file_name, char *website_url)
{
	printf("Uploading %s%s%s to %s%s%s...\n",
		COLOR_YELLOW, file_name, COLOR_DEFAULT,
		COLOR_BLUE, website_url, COLOR_DEFAULT);
}

/* Given a similar_image, print out all its information */
void print_image_info(struct similar_image *img)
{
	char *sim_color;
	if (img->similarity >= 90)
		sim_color = COLOR_GREEN;
	else if (img->similarity >= 70)
		sim_color = COLOR_YELLOW;
	else
		sim_color = COLOR_RED;

	printf("source: %s%s%s\n", COLOR_BLUE, img->link, COLOR_DEFAULT);
	printf("similarity: %s%u%%%s\n",
		sim_color, img->similarity, COLOR_DEFAULT);
	printf("dimensions: %ux%u\n\n",
		img->dimensions[0], img->dimensions[1]);
}

void print_image_tags(struct image_tag_db *tags_db)
{
	char *tag_names[] = {
		"artist",
		"character",
		"circle",
		"copyright",
		"fault",
		"general"
	};
	char *color_scheme[] = {
		COLOR_YELLOW,
		COLOR_GREEN,
		COLOR_CYAN,
		COLOR_MAGENTA,
		COLOR_RED,
		COLOR_WHITE
	};

	struct ll_node *ptr;
	for (int i = 0; i < 6; i++) {
		ptr = tags_db->tags[i];
		printf("%s%s: ", COLOR_DEFAULT, tag_names[i]);
		while (ptr) {
			printf("%s%s %s", color_scheme[i], ptr->data, COLOR_DEFAULT);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

#endif


#ifndef COLOR

void image_download_toast(char *website_url)
{
	printf("Downloading from %s...\n", website_url);
}

void image_save_toast(char *file_name)
{
	printf("Saving image as %s...\n", file_name);
}

void image_upload_toast(char *file_name, char *website_url)
{
	printf("Uploading %s to %s...\n", file_name, website_url);
}

/* Given a similar_image, print out all its information */
void print_image_info(struct similar_image *img)
{
	printf("source: %s\nsimilarity: %u%%\ndimensions: %ux%u\n\n",
		img->link, img->similarity,
		img->dimensions[0], img->dimensions[1]);
}

void print_image_tags(struct image_tag_db *tags_db)
{
	char *tag_names[] = {
		"artist",
		"character",
		"circle",
		"copyright",
		"fault",
		"general"
	};

	struct ll_node *ptr;
	for (int i = 0; i < 6; i++) {
		ptr = tags_db->tags[i];
		printf("%s: ", tag_names[i]);
		while (ptr) {
			printf("%s ", ptr->data);
			ptr = ptr->next;
		}
		printf("\n");
	}
}

#endif

void print_help(void)
{
	puts("Reverse image searching program using iqdb.org\n");
	puts("Usage: ruiji [options] [file]\n");
	puts("  -h --help \t\t Show this help message and quit");
	puts("  -q --quiet \t\t Suppress verbose output");
	puts("  -t --threshold <0-100> Only process and show images above given");
	puts("             \t\t similarity percentage");
	puts("  -v --verbose \t\t Show verbose output");
	puts("  -V --version \t\t Show version number and quit");
	puts("  -y --noprompt \t Skips user interactions and downloads");
	puts("                \t the most similar image");
	puts("\nreport bugs to https://github.com/Kamiyaa/ruiji");
}


/* Given a similar_image_db, print out all its contents */
void print_sim_results(struct similar_image_db *sim_db)
{
	for (int i = 0; i < sim_db->size; i++) {
		printf("[%d]\n", i);
		print_image_info(sim_db->img_db[i]);
	}
}



