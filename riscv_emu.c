#include "riscv_emu.h"

CPU* cpu_init(void) {
    CPU* cpu = (CPU*)malloc(sizeof(CPU));
    memset(cpu, 0, sizeof(CPU));
    cpu->is_running = true;
    return cpu;
}

uint32_t mem_read_u32(CPU* cpu, uint32_t addr) {
    return (uint32_t)cpu->memory[addr] |
           ((uint32_t)cpu->memory[addr+1] << 8) |
           ((uint32_t)cpu->memory[addr+2] << 16) |
           ((uint32_t)cpu->memory[addr+3] << 24);
}

void mem_write_u32(CPU* cpu, uint32_t addr, uint32_t value) {
    cpu->memory[addr] = value & 0xFF;
    cpu->memory[addr+1] = (value >> 8) & 0xFF;
    cpu->memory[addr+2] = (value >> 16) & 0xFF;
    cpu->memory[addr+3] = (value >> 24) & 0xFF;
}

void handle_add_sub(CPU* cpu, uint32_t instr) {
    uint8_t rd = (instr >> 7) & 0x1F;
    uint8_t rs1 = (instr >> 15) & 0x1F;
    uint8_t rs2 = (instr >> 20) & 0x1F;
    uint8_t funct3 = (instr >> 12) & 0x7;
    uint8_t funct7 = (instr >> 25) & 0x7F;
    if (funct3 == 0x00) {
        if (funct7 == 0x00 && rd != 0)
            cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
        else if (funct7 == 0x20 && rd != 0)
            cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
    }
}

void handle_addi(CPU* cpu, uint32_t instr) {
    uint8_t rd = (instr >> 7) & 0x1F;
    uint8_t rs1 = (instr >> 15) & 0x1F;
    int32_t imm = ((int32_t)instr) >> 20;
    if (rd != 0)
        cpu->regs[rd] = cpu->regs[rs1] + imm;
}

void handle_lw(CPU* cpu, uint32_t instr) {
    uint8_t rd = (instr >> 7) & 0x1F;
    uint8_t rs1 = (instr >> 15) & 0x1F;
    int32_t imm = ((int32_t)instr) >> 20;
    if (rd != 0)
        cpu->regs[rd] = mem_read_u32(cpu, cpu->regs[rs1] + imm);
}

void handle_sw(CPU* cpu, uint32_t instr) {
    uint8_t rs1 = (instr >> 15) & 0x1F;
    uint8_t rs2 = (instr >> 20) & 0x1F;
    int32_t imm = ((int32_t)instr) >> 20;
    mem_write_u32(cpu, cpu->regs[rs1] + imm, cpu->regs[rs2]);
}

void handle_beq(CPU* cpu, uint32_t instr, uint32_t old_pc) {
    uint8_t rs1 = (instr >> 15) & 0x1F;
    uint8_t rs2 = (instr >> 20) & 0x1F;
    int32_t imm = ((instr >> 31) & 0x1) ? 0xFFFFF000 : 0;
    imm |= ((instr >> 7) & 0x1E) << 3;
    imm |= ((instr >> 25) & 0x3F) << 5;
    imm |= ((instr >> 8) & 0xF) << 1;
    if (cpu->regs[rs1] == cpu->regs[rs2])
        cpu->pc = old_pc + imm;
}

void handle_jal(CPU* cpu, uint32_t instr, uint32_t old_pc) {
    uint8_t rd = (instr >> 7) & 0x1F;
    int32_t imm = ((instr >> 31) & 0x1) ? 0xFFF00000 : 0;
    imm |= ((instr >> 12) & 0xFF) << 12;
    imm |= ((instr >> 20) & 0x1) << 11;
    imm |= ((instr >> 21) & 0x3FF) << 1;
    if (rd != 0)
        cpu->regs[rd] = cpu->pc;
    cpu->pc = old_pc + imm;
}

void handle_ebreak(CPU* cpu) {
    cpu->is_running = false;
}

void execute(CPU* cpu, uint32_t instr, uint32_t old_pc) {
    uint8_t opcode = instr & 0x7F;
    switch (opcode) {
        case 0x33: handle_add_sub(cpu, instr); break;
        case 0x13: handle_addi(cpu, instr); break;
        case 0x03: handle_lw(cpu, instr); break;
        case 0x23: handle_sw(cpu, instr); break;
        case 0x63: handle_beq(cpu, instr, old_pc); break;
        case 0x6F: handle_jal(cpu, instr, old_pc); break;
        case 0x73:
            if (instr == 0x00100073)
                handle_ebreak(cpu);
            break;
        default: cpu->is_running = false; break;
    }
}

void run_emulator(CPU* cpu) {
    while (cpu->is_running) {
        uint32_t instr = mem_read_u32(cpu, cpu->pc);
        uint32_t old_pc = cpu->pc;
        cpu->pc += 4;
        execute(cpu, instr, old_pc);
    }
}