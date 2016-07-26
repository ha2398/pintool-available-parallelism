#include <pin.H>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// External native C modules.
extern "C" {
	#include "htable.h"
	void __assert_fail() {}
}

#define NUM_BITS 7
typedef unsigned long address;

FILE *output;
unsigned long max_prl;
struct htable addresses;

static size_t addr_hash(const void *elem, void *unused)
{
	size_t h = *(uint64_t *)elem / 2;
	h |= -1UL << NUM_BITS;
	return h;
}

static void fini(int code, void *v)
{
	fprintf(output, "%lu", max_prl);
	fclose(output);
}

static bool cmp(const void *candidate, void *ptr)
{
	return *(address *)candidate == *(address *)ptr;
}

static void check_addr(void *ip, void *addr)
{
	unsigned long num_addr = (unsigned long)addresses.elems;
	address *new_addr = (address*)malloc(sizeof(*new_addr));
	*new_addr = (address)addr;

	size_t new_addr_hash = addr_hash(new_addr, NULL);

	if (htable_get(&addresses, new_addr_hash, cmp, new_addr)) {
		if (num_addr > max_prl)
			max_prl = num_addr;

		htable_clear(&addresses);
	}
	
	htable_add(&addresses, new_addr_hash, new_addr);
}

static void Instruction(INS ins, void *v)
{
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
		return;

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
	max_prl = 0;
	htable_init(&addresses, addr_hash, NULL);

	/* Instrumentation */
	PIN_Init(argc, argv);

	INS_AddInstrumentFunction(Instruction, 0);

	PIN_AddFiniFunction(fini, 0);
	PIN_StartProgram();

	return 0;
}
