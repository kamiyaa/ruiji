#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "config.h"

#define COLOR_DEFAULT	"\x1B[0m"

void image_download_toast(char *website_url)
{

#ifdef COLOR
	printf("Downloading from %s%s%s...\n",
		url_color, website_url, COLOR_DEFAULT);
#endif

#ifndef COLOR
	printf("Downloading from %s...\n", website_url);
#endif

}

/* A verbose message to the user when saving an image */
void image_save_toast(char *file_name)
{

#ifdef COLOR
	printf("Saving image as %s%s%s...\n",
		file_color, file_name, COLOR_DEFAULT);
#endif

#ifndef COLOR
	printf("Saving image as %s...\n", file_name);
#endif

}

void image_upload_toast(char *file_name, char *website_url)
{

#ifdef COLOR
	printf("Uploading %s%s%s to %s%s%s...\n",
		file_color, file_name, COLOR_DEFAULT,
		url_color, website_url, COLOR_DEFAULT);
#endif

#ifndef COLOR
	printf("Uploading %s to %s...\n", file_name, website_url);
#endif

}

/* Given a similar_image, print out all its information */
void print_image_info(struct similar_image *img)
{

#ifdef COLOR
	char *sim_color;
	if (img->similarity >= 90)
		sim_color = good_match_color;
	else if (img->similarity >= 70)
		sim_color = med_match_color;
	else
		sim_color = bad_match_color;

	printf("source: %s%s%s\n", url_color, img->post_link, COLOR_DEFAULT);
	printf("similarity: %s%u%%%s\n",
		sim_color, img->similarity, COLOR_DEFAULT);
	printf("dimensions: %ux%u\n\n",
		img->dimensions[0], img->dimensions[1]);
#endif

#ifndef COLOR
	printf("source: %s\nsimilarity: %u%%\ndimensions: %ux%u\n\n",
		img->post_link, img->similarity,
		img->dimensions[0], img->dimensions[1]);
#endif

}

void print_image_tags(struct image_tag_db *tag_db)
{
	const char *tag_names[] = {
		"artist",
		"character",
		"circle",
		"copyright",
		"meta",
		"general"
	};

#ifdef COLOR
	const char *color_scheme[] = {
		artist_tag_color,
		character_tag_color,
		circle_tag_color,
		copyright_tag_color,
		meta_tag_color,
		general_tag_color
	};

	struct llnode *ptr;
	for (unsigned int i = 0; i < 6; i++) {
		ptr = tag_db->tags[i];
		printf("%s%s (%d)\t: ", COLOR_DEFAULT, tag_names[i], tag_db->tag_size[i]);
		while (ptr) {
			printf("%s%s %s", color_scheme[i], ptr->data, COLOR_DEFAULT);
			ptr = ptr->next;
		}
		putchar('\n');
	}
#endif

#ifndef COLOR
	struct llnode *ptr;
	for (unsigned int i = 0; i < 6; i++) {
		ptr = tag_db->tags[i];
		printf("%s (%d)\t: ", tag_names[i], tag_db->tag_size[i]);
		while (ptr) {
			printf("%s ", ptr->data);
			ptr = ptr->next;
		}
		putchar('\n');
	}
#endif

}

void print_help()
{
	fputs("Reverse image searching program using iqdb.org\n", stderr);
	fputs("Usage: ruiji [options] file\n", stderr);
	fputs("  -h --help \t\t Show this help message and quit\n", stderr);
	fputs("  -q --quiet \t\t Suppress verbose output\n", stderr);
	fputs("  -t --threshold <0-100> Only process and show images above given\n", stderr);
	fputs("             \t\t similarity percentage\n", stderr);
	fputs("  -T --tags \t\t Shows the tags associated with the image downloaded\n", stderr);
	fputs("  -v --version \t\t Show version number and quit\n", stderr);
	fputs("  -y --noprompt \t Skips user interactions and downloads\n", stderr);
	fputs("                \t the most similar image\n", stderr);
	fputs("\nreport bugs to https://github.com/Kamiyaa/ruiji\n", stderr);
}


/* Given a similar_image_db, print out all its contents */
unsigned int print_sim_results(struct similar_image_llnode *image_list)
{
	unsigned int size = 0;
	while (image_list) {
		printf("[%d]\n", size);
		print_image_info(image_list->image);
		image_list = image_list->next;
		size++;
	}
	return size;
}
