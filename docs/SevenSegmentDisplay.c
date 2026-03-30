
#include <stdio.h>
#include <stdlib.h>

#define NEGATIVE_SIGN 0x01
#define BLANK         0x00

// 7 segment display pattern for digits 0-9
unsigned char digits[] = {
    /* 0 */ 0x7e,
    /* 1 */ 0x30,
    /* 2 */ 0x6d,
    /* 3 */ 0x79,
    /* 4 */ 0x33,
    /* 5 */ 0x5b,
    /* 6 */ 0x5f, 
    /* 7 */ 0x70,
    /* 8 */ 0x7f,
    /* 9 */ 0x7b
};

int main(void) {

    unsigned char eeprom[2048];
    int address;
    int value;
    int i;
    int x;

    // Clear the entire eeprom array to BLANK
    for (address = 0; address <= 2047; address++){
        eeprom[address] = BLANK;
    }

    // Unsigned programming
    printf("Programming unsigned mode (0-255)...\n");

    for (value=0; value <= 255; value++){
        eeprom[value] = digits[value % 10];
        eeprom[value + 256] = digits[(value / 10) % 10];
        eeprom[value + 512] = digits[(value / 100) % 10];
        eeprom[value + 768] = 0;
    }
    
    // Signed mode programming
    printf("Programming signed mode (-128 to +127)...\n");

    for (value=-128; value<=127; value++){
        eeprom[(unsigned char)value+1024] = digits[abs(value) % 10];
        eeprom[(unsigned char)value+1024 + 256] = digits[(abs(value) / 10) % 10];
        eeprom[(unsigned char)value+1024 + 512] = digits[(abs(value) / 100) % 10];
        if (value < 0){
            eeprom[(unsigned char)value+1024 + 768] = NEGATIVE_SIGN;
        }
        else{
            eeprom[(unsigned char)value+1024 + 768] = 0;
        }
    }

    // Write contents of eeprom array to a .bin file
    FILE *fp = fopen("SevenSegmentDisplay.bin", "wb");
    if (fp == NULL){
        printf("ERROR: Could not create .bin file\n");
        return 1;
    }
    int written = fwrite(eeprom, 1, 2048, fp);
    fclose(fp);
    
      if (written != 2048) {
        printf("ERROR: Only wrote %4d bytes (expected 2048)\n", written);
        return 1;
    }

    printf("\nSuccess. Created SevenSegmentDisplay.bin (2048 bytes)\n");
    printf("Load this into the TL866 software, select AT28C16, and program.\n");


    // Verification testing unsigned
    printf("Testing unsigned 0 to 10:\n");

    for(i=0; i<=10; i++){
        printf(" %4d: sign= 0x%02x hundreds= 0x%02x tens= 0x%02x ones= 0x%02x\n",
            i,
            eeprom[i + 768],
            eeprom[i + 512],
            eeprom[i + 256],
            eeprom[i]);
    }

    // Verification testing signed
    printf("Testing signed -5 to 0:\n");

    for(i=-5; i<=0; i++){
        printf(" %4d: sign= 0x%02x hundreds= 0x%02x tens= 0x%02x ones= 0x%02x\n",
            i,
            eeprom[(unsigned char)i + 1024 + 768],
            eeprom[(unsigned char)i + 1024 + 512],
            eeprom[(unsigned char)i + 1024 + 256],
            eeprom[(unsigned char)i + 1024]);
    }

    return 0;
}

