#include "segment_loader.h"
#include <stdlib.h>
#include <string.h>
struct page_list* load_page_data(const char* path);

static struct segment_list* segments = NULL;

void load_segment_data(const char* path)
{
    char buffer[255];
    int lines = line_count(path);
    free_segments();
    segments = malloc(sizeof(struct segment_list) + lines * sizeof(struct segment_data));
    segments->len = lines;
	FILE* file = fopen(path, "rt");
    int i = 0;
    while (fgets(buffer, 255, file))
    {
        char* token = strtok(buffer, ";");
        segments->data[i].pages = load_page_data(token);
        i++;
    }
    fclose(file);
}
struct page_list* load_page_data(const char* path)
{
    char buffer[255];
    int lines = line_count(path);
    struct page_list* list = malloc(sizeof(struct page_list) + lines * sizeof(struct page_data));
    list->len = lines;
    FILE* file = fopen(path, "rt");
    int i = -1;
    while (fgets(buffer, 255, file))
    {
        i++;
        char* token = strtok(buffer, ";");
        long value = strtol(token, NULL, 10);
        list->data[i].is_loaded = value;
        if (value == 0)
        {
            continue;
        }
        token = strtok(NULL, ";");
        value = strtol(token, NULL, 2);
        list->data[i].frame_number = value;
    }
    return list;
}

void free_segments()
{
    if (segments == NULL) return;
    for (size_t i = 0; i < segments->len; i++)
    {
        free(segments->data[i].pages);
    }
    free(segments);
    segments = NULL;
}
struct segment_list* get_loaded_segments()
{
    return segments;
}
int line_count(const char* path)
{
    FILE* file = fopen(path, "rt");
    char c = 0;
    int count = 0;
    while ((c = fgetc(file)) != -1)
    {
        if (c == '\n')
        {
            count++;
        }
    }
    fclose(file);
    return count;
}