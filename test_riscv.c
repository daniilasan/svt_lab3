#include "riscv_emu.h"

void test_add() {
    CPU* cpu = cpu_init();
    uint32_t prog[] = {
        0x00500133, // add x2, x0, x5
        0x00100293, // addi x5, x0, 5
        0x00500133, // add x2, x0, x5
        0x00100073  // ebreak
    };
    memcpy(cpu->memory, prog, sizeof(prog));
    run_emulator(cpu);
    assert(cpu->regs[2] == 5);
    free(cpu);
}

void test_lw_sw() {
    CPU* cpu = cpu_init();
    uint32_t prog[] = {
        0x00A00293, // addi x5, x0, 10
        0x00502023, // sw x5, 0(x0)
        0x00002303, // lw x6, 0(x0)
        0x00100073  // ebreak
    };
    memcpy(cpu->memory, prog, sizeof(prog));
    run_emulator(cpu);
    assert(cpu->regs[6] == 10);
    free(cpu);
}

void test_beq() {
    CPU* cpu = cpu_init();
    uint32_t prog[] = {
        0x00500293, // addi x5, x0, 5
        0x00500313, // addi x6, x0, 5
        0x00628463, // beq x5, x6, 8
        0x00100293, // addi x5, x0, 1 (пропускаем)
        0x00A00293, // addi x5, x0, 10
        0x00100073  // ebreak
    };
    memcpy(cpu->memory, prog, sizeof(prog));
    run_emulator(cpu);
    assert(cpu->regs[5] == 10);
    free(cpu);
}

void test_jal() {
    CPU* cpu = cpu_init();
    uint32_t prog[] = {
        0x008000EF, // jal x1, 8
        0x00100293, // addi x5, x0, 1
        0x00200293, // addi x5, x0, 2
        0x00008067, // jalr x0, x1, 0
        0x00300293, // addi x5, x0, 3
        0x00100073  // ebreak
    };
    memcpy(cpu->memory, prog, sizeof(prog));
    run_emulator(cpu);
    assert(cpu->regs[5] == 3);
    free(cpu);
}

void test_fibonacci() {
    CPU* cpu = cpu_init();
    uint32_t prog[] = {
        0x00000293, 0x00100313, 0x00A00393, 0x02000363,
        0x006283B3, 0x00600293, 0x00800313, 0xFFF38393,
        0xFE9FF06F, 0x00100073
    };
    memcpy(cpu->memory, prog, sizeof(prog));
    run_emulator(cpu);
    assert(cpu->regs[8] == 55);
    free(cpu);
}

int main() {
    test_add();
    test_lw_sw();
    test_beq();
    test_jal();
    test_fibonacci();
    printf("All tests passed!\n");
    return 0;
}