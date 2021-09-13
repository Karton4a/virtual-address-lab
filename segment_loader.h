#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct segment_data
{
	struct page_list* pages;
};

struct segment_list
{
	int len;
	struct segment_data data[0];
};

struct page_data
{
	bool is_loaded;
	uint8_t frame_number;
};

struct page_list
{
	int len;
	struct page_data data[0];
};

void load_segment_data(const char* path);
void free_segments();
struct segment_list* get_loaded_segments();
