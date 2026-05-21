#ifndef RISCV_EMU_H
#define RISCV_EMU_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#define MEMORY_SIZE (1024 * 1024)
#define REG_COUNT 32

typedef struct {
    uint32_t regs[REG_COUNT];
    uint32_t pc;
    uint8_t memory[MEMORY_SIZE];
    bool is_running;
} CPU;

CPU* cpu_init(void);
uint32_t mem_read_u32(CPU* cpu, uint32_t addr);
void mem_write_u32(CPU* cpu, uint32_t addr, uint32_t value);
void handle_add_sub(CPU* cpu, uint32_t instr);
void handle_addi(CPU* cpu, uint32_t instr);
void handle_lw(CPU* cpu, uint32_t instr);
void handle_sw(CPU* cpu, uint32_t instr);
void handle_beq(CPU* cpu, uint32_t instr, uint32_t old_pc);
void handle_jal(CPU* cpu, uint32_t instr, uint32_t old_pc);
void handle_ebreak(CPU* cpu);
void execute(CPU* cpu, uint32_t instr, uint32_t old_pc);
void run_emulator(CPU* cpu);

#endif