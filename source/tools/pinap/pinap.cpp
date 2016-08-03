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
unsigned long instr_count;
unsigned long max_instr_count;
struct htable addresses;

bool inst_enable = false; /* enables instrumentation */

static size_t addr_hash(const void *elem, void *unused)
{
	size_t h = *(uint64_t *)elem / 2;
	h |= -1UL << NUM_BITS;
	return h;
}

static void fini(int code, void *v)
{
	fprintf(output, "Max: %lu", max_instr_count);
	fclose(output);
}

static bool cmp(const void *candidate, void *ptr)
{
	return *(address *)candidate == *(address *)ptr;
}

static void do_count()
{
	if (!inst_enable)
		return;

	instr_count++;
}

static void check_addr(void *ip, void *addr)
{
	if (!inst_enable)
		return;

	address *new_addr = (address*)malloc(sizeof(*new_addr));
	*new_addr = (address)addr;

	size_t new_addr_hash = addr_hash(new_addr, NULL);

	if (htable_get(&addresses, new_addr_hash, cmp, new_addr)) {
		if (instr_count > max_instr_count) {
			max_instr_count = instr_count;
			instr_count = 0;
		}

		htable_clear(&addresses);
	}

	htable_add(&addresses, new_addr_hash, new_addr);
}

static void Instruction(INS ins, void *v)
{
	INS_InsertPredicatedCall(
		ins, IPOINT_BEFORE,
		(AFUNPTR)do_count,
		IARG_END);

	// Filters out non memory reference instructions.
	if (!INS_IsMemoryRead(ins) && !INS_IsMemoryWrite(ins))
		return;

	// Filters out references to stack.
	if (INS_IsStackRead(ins) || INS_IsStackWrite(ins))
		return;

	// Filters out instructions out of main executable.
	IMG img = IMG_FindByAddress(INS_Address(ins));
	if (!IMG_Valid(img) || !IMG_IsMainExecutable(img))
		return;

	unsigned i;
	unsigned int mem_op = INS_MemoryOperandCount(ins);

	for (i = 0; i < mem_op; i++) {
		INS_InsertPredicatedCall(
			ins, IPOINT_BEFORE,
			(AFUNPTR)check_addr,
			IARG_INST_PTR,
			IARG_MEMORYOP_EA, i, IARG_END);
	}
}

static void pin_enable()
{
	inst_enable = true;
}

static void pin_disable()
{
	inst_enable = false;
}

static void Routine(RTN rtn, void *v)
{
	RTN_Open(rtn);

	if (strcmp(&(RTN_Name(rtn)[0]), "pin_go") == 0)
		RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)pin_enable,
			IARG_END);

	if (strcmp(&(RTN_Name(rtn)[0]), "pin_stop") == 0)
		RTN_InsertCall(rtn, IPOINT_AFTER, (AFUNPTR)pin_disable,
			IARG_END);

	RTN_Close(rtn);
}

int main(int argc, char **argv)
{
	/* Initializations */
	output = fopen("trace.out", "w");
	instr_count = 0;
	max_instr_count = 0;
	htable_init(&addresses, addr_hash, NULL);
	PIN_InitSymbols();
	PIN_Init(argc, argv);

	/* Instrumentation */
	RTN_AddInstrumentFunction(Routine, 0);
	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(fini, 0);
	PIN_StartProgram();

	return 0;
}
