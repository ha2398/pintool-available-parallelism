#include "list.h"
#include <pin.H>
#include <stdio.h>
#include <stdlib.h>

FILE *output;
list *addr_list;
unsigned long max_prl;

void fini(int code, void *v)
{
	fprintf(output, "%lu", max_prl);
	fclose(output);
}

bool addr_eq(const void *addr1, const void *addr2)
{
	return *(unsigned long*) addr1 == *(unsigned long*)addr2;
}

void check_addr(void *ip, void *addr)
{
	unsigned long list_length = list_size(addr_list);
	unsigned long *new_addr = (unsigned long*)malloc(sizeof(*new_addr));
	*new_addr = (unsigned long)addr;

	if (list_contains(addr_list, new_addr, addr_eq)) {
		if (list_length > max_prl)
			max_prl = list_length;

		delete_list(addr_list);
	} else {
		list_add(addr_list, new_addr);
	}
}

void Instruction(INS ins, void *v)
{
	unsigned i;
	unsigned int mem_op = INS_MemoryOperandCount(ins);

	for (i = 0; i < mem_op; i++) {
		if (!INS_IsMemoryRead(ins) && !INS_IsMemoryWrite(ins))
			return;

		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE,
			(AFUNPTR)check_addr,
			IARG_INST_PTR,
			IARG_MEMORYOP_EA, i, IARG_END);
	}
}

int main(int argc, char **argv)
{
	/* Initializations */
	output = fopen("trace.out", "w");
	addr_list = new_list();
	max_prl = 0;

	/* Instrumentation */
	PIN_InitSymbols();
	PIN_Init(argc, argv);

	INS_AddInstrumentFunction(Instruction, 0);

	PIN_AddFiniFunction(fini, 0);
	PIN_StartProgram();

	return 0;
}
