#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

#define COLOR_DEFAULT	"\x1B[0m"
#define COLOR_RED	"\x1B[31m"
#define COLOR_GREEN	"\x1B[32m"
#define COLOR_YELLOW	"\x1B[33m"
#define COLOR_BLUE	"\x1B[34m"
#define COLOR_MAGENTA	"\x1B[35m"
#define COLOR_CYAN	"\x1B[36m"
#define COLOR_WHITE	"\x1B[37m"

#ifdef COLOR
/* Given a similar_image, print out all its information */
void print_image_info(struct similar_image *img)
{
	printf("source: %s%s%s\n", COLOR_BLUE, img->link, COLOR_DEFAULT);
	if (img->similarity >= 90)
		printf(	"similarity: %s%u%%%s\n",
			COLOR_GREEN, img->similarity, COLOR_DEFAULT);
	else if (img->similarity >= 70)
		printf(	"similarity: %s%u%%%s\n",
			COLOR_YELLOW, img->similarity, COLOR_DEFAULT);
	else
		printf(	"similarity: %s%u%%%s\n",
			COLOR_RED, img->similarity, COLOR_DEFAULT);
	printf("dimensions: %ux%u\n\n",
		img->dimensions[0], img->dimensions[1]);
}


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
#endif


#ifndef COLOR
/* Given a similar_image, print out all its information */
void print_image_info(struct similar_image *img)
{
	printf("source: %s\nsimilarity: %u%%\ndimensions: %ux%u\n\n",
		img->link, img->similarity,
		img->dimensions[0], img->dimensions[1]);
}

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
#endif

void print_help(void)
{
	puts("Reverse image searching program using iqdb.org\n");
	puts("Usage: ruiji -f [file] [options]\n");
	puts("  -f --file \t\t Takes in the given file to upload");
	puts("  -h --help \t\t Show this help message and quit");
	puts("  -q --quiet \t\t Suppress verbose output");
	puts("  -t <0-100> \t\t Only process and show images above given");
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
