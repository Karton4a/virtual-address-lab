#pragma once
#define ADDR(segment,page,offset) (offset | (page << 12) | (segment << 22))
#define MOV_0(reg1,reg2)  0x1A, (reg2 | reg1 << 4)
#define MOV_1(reg,addr) 0x1B,reg, ADDR_TO_ARRAY(addr)
#define MOV_2(addr,reg) 0x1B,(0x10 | reg),ADDR_TO_ARRAY(addr)
#define MOV_3(reg,lit) 0x1C,reg,lit
#define SUB_0(reg1,reg2) 0x0A, (reg2 | reg1 << 4)
#define SUB_1(reg,addr) 0x0B,reg,ADDR_TO_ARRAY(addr)
#define JL_0(offset) 0x92,offset
#define CMP_0(reg1,reg2) 0x80,(reg2 | reg1 << 4)
#define JL_0(addr) 0x93, ADDR_TO_ARRAY(addr)
#define ADDR_TO_ARRAY(addr) ((addr & 0xFF000000) >> 24),((addr & 0x00FF0000) >> 16),((addr & 0x0000FF00) >> 8),(addr & 0xFF)

void run_all_tests();