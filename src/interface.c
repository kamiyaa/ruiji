#include <stdio.h>
#include <stdlib.h>

#include "interface.h"

#define COLOR_BLACK     "\x1B[0m"
#define COLOR_RED	"\x1B[31m"
#define COLOR_GREEN     "\x1B[32m"
#define COLOR_YELLOW    "\x1B[33m"
#define COLOR_BLUE	"\x1B[34m"
#define COLOR_MAGENTA   "\x1B[35m"
#define COLOR_CYAN	"\x1B[36m"
#define COLOR_WHITE     "\x1B[37m"

#ifndef COLOR
/* Given a similar_image_db, print out all its contents */
void print_sim_results(struct similar_image_db *sim_db)
{
	for (int i = 0; i < sim_db->size; i++) {
		printf("[%d]\nsource: %s\nsimilarity: %u%%\ndimensions: %ux%u\n\n",
			i, sim_db->img_db[i]->link, sim_db->img_db[i]->similarity,
			sim_db->img_db[i]->dimensions[0], sim_db->img_db[i]->dimensions[1]);
	}
}

void image_save_toast(char *file_name, char *website_url)
{
	printf("Saving image as %s from %s...\n",
		file_name, website_url);
}
#endif

#ifdef COLOR
/* Given a similar_image_db, print out all its contents */
void print_sim_results(struct similar_image_db *sim_db)
{
	for (int i = 0; i < sim_db->size; i++) {
		printf("[%d]\nsource: %s%s%s\n", i, COLOR_BLUE, sim_db->img_db[i]->link, COLOR_BLACK);
		if (sim_db->img_db[i]->similarity >= 90)
			printf(	"similarity: %s%u%%%s\n", COLOR_GREEN, sim_db->img_db[i]->similarity, COLOR_BLACK);
		else if (sim_db->img_db[i]->similarity >= 70)
			printf(	"similarity: %s%u%%%s\n", COLOR_YELLOW, sim_db->img_db[i]->similarity, COLOR_BLACK);
		else
			printf(	"similarity: %s%u%%%s\n", COLOR_RED, sim_db->img_db[i]->similarity, COLOR_BLACK);
		printf("dimensions: %ux%u\n\n",
			sim_db->img_db[i]->dimensions[0], sim_db->img_db[i]->dimensions[1]);
	}
}

void image_save_toast(char *file_name, char *website_url)
{
	printf("Saving image as %s%s%s from %s%s%s...\n",
		COLOR_YELLOW, file_name, COLOR_BLACK,
		COLOR_BLUE, website_url, COLOR_BLACK);
}
#endif
