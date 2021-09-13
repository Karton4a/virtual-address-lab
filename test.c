#include <stdint.h>
#include "test.h"
#include "binary_processor.h"

void run_test(struct test_data* strct);

struct test_data 
{
	const char* expected;
	uint8_t data[20];
	int count;
};

static struct test_data tests[] = {
	{.expected = "MOV R0, R5",.data = {MOV_0(0,5)},.count = 2 },
	{.expected = "MOV R2, 0x1C",.data = {MOV_3(2,0x1C)}, .count = 3  },
	{.expected = "SUB R4, R5",.data = {SUB_0(4,5)}, .count = 2 },
	{.expected = "JL [0x0A]",.data = {JL_0(0x0A)}, .count = 2 },
	{.expected = "CMP R6, R8",.data = {CMP_0(6,8)}, .count = 2 },
	{.expected = "MOV R4, [0x000011B8]",.data = {MOV_1(4,ADDR(0,2,440))}, .count = 6 },
	{.expected = "MOV [0x00000078],R6",.data = {MOV_2(ADDR(0,0,120),6)}, .count = 6 },
	{.expected = "SUB R6, [0x00000078]",.data = {SUB_1(6,ADDR(0,0,120))}, .count = 6 },
	{.expected = "error : page 1 not loaded",.data = {SUB_1(6,ADDR(0,1,120))}, .count = 6 },
	{.expected = "error : missing segment 51",.data = {SUB_1(6,ADDR(51,0,120))}, .count = 6 },
	{.expected = "JL [0x00000078]",.data = {JL_0(ADDR(0,0,120))}, .count = 5 },
	
};

void run_all_tests()
{
	for (size_t i = 0; i < sizeof(tests)/sizeof(struct test_data); i++)
	{
		if (i == 9)
		{
			int a = 0;
		}
		run_test(tests + i);
	}
	remove("test.temp");
}

void run_test(struct test_data* strct)
{
	printf("Expected %s got: \n", strct->expected);
	FILE* stream = fopen("test.temp", "wb");
	fwrite(strct->data, sizeof(uint8_t), strct->count, stream);
	fclose(stream);
	stream = fopen("test.temp", "rb");
	process_binary(stream, stdout);
	fclose(stream);
}