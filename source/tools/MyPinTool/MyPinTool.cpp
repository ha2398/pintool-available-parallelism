#include <pin.H>
#include <stdio.h>
#include <stdlib.h>

FILE *output;
unsigned long long icount = 0;
unsigned long long mem_ref = 0;
unsigned long long reads = 0;
unsigned long long writes = 0;

void fini(int code, void *v)
{
	fprintf(output, "Instructions: %llu\n", icount);
	fprintf(output, "Memory references: %llu\n", mem_ref);
	fprintf(output, "Reads: %llu\n", reads);
	fprintf(output, "Writes: %llu\n", writes);
	fprintf(output, "#eof\n");
	fclose(output);
}

void count()
{
	icount++;
}

void print_read(void *ip, void *addr)
{
	mem_ref++;
	reads++;
	fprintf(output, "READ:\t%p:ADDRESS:\t%p\n", ip, addr);
}

void print_write(void *ip, void *addr)
{
	mem_ref++;
	writes++;
	fprintf(output, "WRITE:\t%p:ADDRESS:\t%p\n", ip, addr);
}

void Instruction(INS ins, void *v)
{
	unsigned int i;
	unsigned int memOperands = INS_MemoryOperandCount(ins);

	INS_InsertPredicatedCall(ins, IPOINT_BEFORE, (AFUNPTR)count,
		IARG_END);

	for (i = 0; i < memOperands; i++) {
		if (INS_MemoryOperandIsRead(ins, i)) {
			INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR)print_read,
				IARG_INST_PTR, IARG_MEMORYOP_EA, i, IARG_END);
		}

		if (INS_MemoryOperandIsWritten(ins, i)) {
			INS_InsertPredicatedCall(
				ins, IPOINT_BEFORE, (AFUNPTR)print_write,
				IARG_INST_PTR, IARG_MEMORYOP_EA, i, IARG_END);
		}
	}
}

int main(int argc, char **argv)
{
	output = fopen("trace.out", "w");
	PIN_Init(argc, argv);
	INS_AddInstrumentFunction(Instruction, 0);
	PIN_AddFiniFunction(fini, 0);
	PIN_StartProgram();
	return 0;
}
