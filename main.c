#include "binary_processor.h"
#include "segment_loader.h"
#include "test.h"
int main()
{
	FILE* file = fopen("myfile.dat", "rb");
	load_segment_data("segments.csv");

	run_all_tests();
	//process_binary(file, stdout);

	free_segments();
	fclose(file);
	return 0;
}