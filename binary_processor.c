#include "binary_processor.h"
#include "stdint.h"
#include "segment_loader.h"
enum addres_convert_error
{
	OK = 1,
	MISSING_PAGE = 0,
	MISSING_SEGMENT = -1,
	PAGE_NOT_LOADED = -2
};
//1, 2, 3, 8, 9, 22, 23, 26, 27
/*
* MOV <reg1>,
 <reg2> 1A /reg1 /reg2

 MOV <reg>,
 <addr> 1B 0 /reg /addr

 MOV <addr>,
 <reg> 1B 1 /reg /addr

 SUB <reg1>,
 <reg2> 0A /reg1 /reg2

 SUB <reg>,
 <addr> 0B 0 /reg /addr

 JL <shift> 92 /shift

 CMP <reg1>,
 <reg2> 80 /reg1 /reg2

 MOV <reg>,
 <lit8> 1C 0 /reg /lit8
*/

void process_mov_0(FILE* input_stream, FILE* output_stream);
void process_mov_1(FILE* input_stream, FILE* output_stream);
void process_sub_0(FILE* input_stream, FILE* output_stream);
void process_sub_1(FILE* input_stream, FILE* output_stream);
void process_jl_0(FILE* input_stream, FILE* output_stream);
void process_jl_1(FILE* input_stream, FILE* output_stream);
void process_cmp_0(FILE* input_stream, FILE* output_stream);
void process_mov_2(FILE* input_stream, FILE* output_stream);
uint32_t array_to_32(uint8_t* arr);

enum addres_convert_error convert_addr(uint32_t base, uint32_t* result,struct segment_list* segments);
void print_convert_error(enum addres_convert_error error, uint32_t value, FILE* stream);

extern void process_binary(FILE* input_stream, FILE* output_stream)
{
	uint8_t command = 0;
	while (fread(&command, sizeof(command), 1, input_stream))
	{
		//fread(&command, sizeof(command), 1, input_stream);
		fprintf(output_stream, "%02x ", command);
		switch (command)
		{
		case 0x1A:
			process_mov_0(input_stream, output_stream);
			break;
		case 0x1B:
			process_mov_1(input_stream, output_stream);
			break;
		case 0x0A:
			process_sub_0(input_stream, output_stream);
			break;
		case 0x0B:
			process_sub_1(input_stream, output_stream);
			break;
		case 0x92:
			process_jl_0(input_stream, output_stream);
			break;
		case 0x80:
			process_cmp_0(input_stream, output_stream);
			break;
		case 0x1C:
			process_mov_2(input_stream, output_stream);
			break;
		case 0x93:
			process_jl_1(input_stream, output_stream);
			break;
		default:
			fprintf(output_stream, " unknown command start\n");
			return;
		}
		fprintf(output_stream,"\n");
	}
}


void process_mov_0(FILE* input_stream, FILE* output_stream)
{
	uint8_t registers = 0;
	fread(&registers, sizeof(registers), 1, input_stream);
	uint8_t reg_1 = registers >> 4;
	uint8_t reg_2 = registers & 0x0F;
	fprintf(output_stream, "%02x:\n",registers);
	fprintf(output_stream, "MOV R%d, R%d",reg_1,reg_2);
}

void process_mov_1(FILE* input_stream, FILE* output_stream)
{
	uint8_t operands[5];
	fread(operands, sizeof(uint8_t), 5, input_stream);
	for (int i = 0; i < 5; i++)
	{
		fprintf(output_stream, "%02x ", operands[i]);
	}
	fprintf(output_stream, ":\n");
	uint8_t flag = operands[0] >> 4;
	uint8_t reg = operands[0] & 0x0F;
	uint32_t addr = array_to_32(operands + 1);
	uint32_t new_addr = 0;
	enum addres_convert_error err = convert_addr(addr, &new_addr, get_loaded_segments());
	if (err == OK)
	{
		if (flag == 0)
		{
			fprintf(output_stream, "MOV R%d, [%#010x]", reg, new_addr);
		}
		else if (flag == 1)
		{
			fprintf(output_stream, "MOV [%#010x], R%d", new_addr, reg);
		}
	}
	else
	{
		print_convert_error(err, new_addr, output_stream);
	}
}

void process_sub_0(FILE* input_stream, FILE* output_stream)
{
	uint8_t registers = 0;
	fread(&registers, sizeof(registers), 1, input_stream);
	uint8_t reg_1 = registers >> 4;
	uint8_t reg_2 = registers & 0x0F;
	fprintf(output_stream, "%02x:\n", registers);
	fprintf(output_stream, "SUB R%d, R%d", reg_1, reg_2);
}

void process_sub_1(FILE* input_stream, FILE* output_stream)
{
	uint8_t operands[5];
	fread(operands, sizeof(uint8_t), 5, input_stream);
	for (int i = 0; i < 5; i++)
	{
		fprintf(output_stream, "%02x ", operands[i]);
	}
	fprintf(output_stream, ":\n");
	uint8_t reg = operands[0] & 0x0F;
	uint32_t addr = array_to_32(operands + 1);
	uint32_t new_addr = 0;
	enum addres_convert_error err = convert_addr(addr, &new_addr, get_loaded_segments());
	if (err == OK)
	{
		fprintf(output_stream, "SUB R%d, [%#010x]", reg, new_addr);
	}
	else
	{
		print_convert_error(err, new_addr, output_stream);
	}
}

void process_jl_0(FILE* input_stream, FILE* output_stream)
{
	uint8_t shift = 0;
	fread(&shift, sizeof(shift), 1, input_stream);
	fprintf(output_stream, "%02x:\n", shift);
	fprintf(output_stream, "JL [%#02x]", shift);
}

void process_jl_1(FILE* input_stream, FILE* output_stream)
{
	uint8_t operands[4];
	fread(operands, sizeof(uint8_t), 4, input_stream);
	for (int i = 0; i < 4; i++)
	{
		fprintf(output_stream, "%02x ", operands[i]);
	}
	fprintf(output_stream, ":\n");
	uint32_t addr = array_to_32(operands);
	uint32_t new_addr = 0;
	enum addres_convert_error err = convert_addr(addr, &new_addr, get_loaded_segments());
	if (err == OK)
	{
		fprintf(output_stream, "JL [%#010x]", new_addr);
	}
	else
	{
		print_convert_error(err, new_addr, output_stream);
	}
}

void process_cmp_0(FILE* input_stream, FILE* output_stream)
{
	uint8_t registers = 0;
	fread(&registers, sizeof(registers), 1, input_stream);

	uint8_t reg_1 = registers >> 4;
	uint8_t reg_2 = registers & 0x0F;
	fprintf(output_stream, "%02x:\n", registers);
	fprintf(output_stream, "CMP R%d, R%d", reg_1, reg_2);
}

void process_mov_2(FILE* input_stream, FILE* output_stream)
{
	uint8_t operands[2];
	fread(operands, sizeof(uint8_t), 2, input_stream);
	for (int i = 0; i < 2; i++)
	{
		fprintf(output_stream, "%02x ", operands[i]);
	}
	fprintf(output_stream, ":\n");
	uint8_t reg = operands[0] & 0x0F;
	uint8_t lit = operands[1];
	fprintf(output_stream, "MOV R%d, %d", reg, lit);
}

uint32_t array_to_32(uint8_t* arr)
{
	uint32_t value = 0;
	for (size_t i = 0; i < 4; i++)
	{
		value |= arr[i] << 8 * (3-i);
	}
	return value;
}

enum addres_convert_error convert_addr(uint32_t base, uint32_t* result, struct segment_list* segments)
{
	uint16_t segment_id = (base & (0b1111111111 << 22)) >> 22;
	if (segment_id >= segments->len)
	{
		*result = segment_id;
		return MISSING_SEGMENT;
	}
	struct page_list* pages = segments->data[segment_id].pages;
	uint16_t page_id = (base & (0b1111111111 << 12)) >> 12;
	if (page_id >= pages->len)
	{
		*result = page_id;
		return MISSING_PAGE;
	}
	struct page_data page = pages->data[page_id];
	if (!page.is_loaded)
	{
		*result = page_id;
		return PAGE_NOT_LOADED;
	}
	uint32_t offset = base & 0b111111111111;
	*result = offset | ((uint32_t)page.frame_number << 12);
	return OK;
}

void print_convert_error(enum addres_convert_error error, uint32_t value, FILE* stream)
{
	if (error == MISSING_PAGE)
	{
		fprintf(stream, "error : missing page %d",value);
	}
	else if (error == MISSING_SEGMENT)
	{
		fprintf(stream, "error : missing segment %d", value);
	}
	else if(error == PAGE_NOT_LOADED)
	{
		fprintf(stream, "error : page %d not loaded", value);
	}
}
