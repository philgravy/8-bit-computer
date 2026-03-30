/*   
    9  8  |  7  |  6  5  4  3  |  2  1  0
    flag  |byte | instruction  | step
    
    Flag combos: 00, 01, 10, 11 (4 Flag states)
    Byte: 0, 1 (Selects upper 8 bits or lower 8 bits. Each EEPROM can only handle 8 bits)
    Instruction: 16 possible instructions
    Step: Within each instruction, there are 8 steps
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#define HLT 0b1000000000000000  // Halt clock
#define MI  0b0100000000000000  // Memory address register in
#define RI  0b0010000000000000  // RAM data in
#define RO  0b0001000000000000  // RAM data out
#define IO  0b0000100000000000  // Instruction register out
#define II  0b0000010000000000  // Instruction register in
#define AI  0b0000001000000000  // A register in
#define AO  0b0000000100000000  // A register out
#define EO  0b0000000010000000  // ALU out
#define SU  0b0000000001000000  // ALU subtract
#define BI  0b0000000000100000  // B register in
#define OI  0b0000000000010000  // Output register in
#define CE  0b0000000000001000  // Program counter enable
#define CO  0b0000000000000100  // Program counter out
#define J   0b0000000000000010  // Jump (program counter in)
#define FI  0b0000000000000001  // Flags in

#define FLAGS_Z0C0 0
#define FLAGS_Z0C1 1
#define FLAGS_Z1C0 2
#define FLAGS_Z1C1 3

#define JC  0b0111
#define JZ  0b1000

// Define each of the 16 instructions. Each of which are made up of 8 steps
uint16_t MICROCODE_TEMPLATE[16][8] = {
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 0000 - NOP
  { MI|CO,  RO|II|CE,  IO|MI,  RO|AI,  0,           0, 0, 0 },   // 0001 - LDA
  { MI|CO,  RO|II|CE,  IO|MI,  RO|BI,  EO|AI|FI,    0, 0, 0 },   // 0010 - ADD
  { MI|CO,  RO|II|CE,  IO|MI,  RO|BI,  EO|AI|SU|FI, 0, 0, 0 },   // 0011 - SUB
  { MI|CO,  RO|II|CE,  IO|MI,  AO|RI,  0,           0, 0, 0 },   // 0100 - STA
  { MI|CO,  RO|II|CE,  IO|AI,  0,      0,           0, 0, 0 },   // 0101 - LDI
  { MI|CO,  RO|II|CE,  IO|J,   0,      0,           0, 0, 0 },   // 0110 - JMP
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 0111 - JC
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1000 - JZ
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1001
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1010
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1011
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1100
  { MI|CO,  RO|II|CE,  0,      0,      0,           0, 0, 0 },   // 1101
  { MI|CO,  RO|II|CE,  AO|OI,  0,      0,           0, 0, 0 },   // 1110 - OUT
  { MI|CO,  RO|II|CE,  HLT,    0,      0,           0, 0, 0 },   // 1111 - HLT
};

uint16_t microcode[4][16][8];

// Four Flag states, copying the microcode template for each into each flag state

void initMicroCode(){
    // ZF = 0, CF = 0
    memcpy(microcode[FLAGS_Z0C0], MICROCODE_TEMPLATE, sizeof(MICROCODE_TEMPLATE));

    // ZF = 0, CF = 1
    memcpy(microcode[FLAGS_Z0C1], MICROCODE_TEMPLATE, sizeof(MICROCODE_TEMPLATE));
    microcode[FLAGS_Z0C1][JC][2] = IO|J;

    // ZF = 1, CF = 0
    memcpy(microcode[FLAGS_Z1C0], MICROCODE_TEMPLATE, sizeof(MICROCODE_TEMPLATE));
    microcode[FLAGS_Z1C0][JZ][2] = IO|J;

    // ZF = 1, CF = 1
    memcpy(microcode[FLAGS_Z1C1], MICROCODE_TEMPLATE, sizeof(MICROCODE_TEMPLATE));
    microcode[FLAGS_Z1C1][JC][2] = IO|J;
    microcode[FLAGS_Z1C1][JZ][2] = IO|J;
}

int main(void){
    initMicroCode();
    unsigned char eeprom_high[512];
    unsigned char eeprom_low[512];
    int written;
    int high_index = 0;
    int low_index = 0;
    for (int address=0; address<1024; address++){
        int flags       = (address & 0b1100000000) >> 8;  // 1s at bits 9-8
        int byte_sel    = (address & 0b0010000000) >> 7;  // 1s at bit 7
        int instruction = (address & 0b0001111000) >> 3;  // 1s at bits 6-3
        int step        = (address & 0b0000000111);       // 1s at bits 2-0
        if(byte_sel){
            eeprom_low[low_index++] = microcode[flags][instruction][step];
        }
        else{
            eeprom_high[high_index++] = microcode[flags][instruction][step] >> 8;
        }
    }
    // Write contents of eeprom array to a .bin file
    FILE *fp1 = fopen("microcode_high.bin", "wb");
    if (fp1 == NULL){
        printf("ERROR: Could not create .bin file\n");
        return 1;
    }
    written = fwrite(eeprom_high, 1, 512, fp1);
    fclose(fp1);
    
      if (written != 512) {
        printf("ERROR: Only wrote %4d bytes (expected 512)\n", written);
        return 1;
    }

    printf("\nSuccess. Created microcode_high.bin (512 bytes)\n");

    FILE *fp2 = fopen("microcode_low.bin", "wb");
    if (fp2 == NULL){
        printf("ERROR: Could not create .bin file\n");
        return 1;
    }
    written = fwrite(eeprom_low, 1, 512, fp2);
    fclose(fp2);
    
      if (written != 512) {
        printf("ERROR: Only wrote %4d bytes (expected 512)\n", written);
        return 1;
    }

    printf("\nSuccess. Created microcode_low.bin (512 bytes)\n");
    printf("Load this into the TL866 software, select AT28C16, and program.\n");

    return 0;
}

