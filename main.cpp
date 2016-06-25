#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int8_t   i8;
typedef int16_t  i16;
typedef int32_t  i32;

u8* mem;        // Main memory
u8* wm;         // Additional memory that we use to store wordmark bits
u8* inst;       // Instruction pointer
i32 b_register; // B address register

i32 get_till_next_wm(char *str)
{
    i32 len    = 0;
    u8* cur_wm = wm + (inst - mem) + 1;
    while (*cur_wm == 0)
    {
        cur_wm++;
        len++;
        if (len >= 10)
        {
            printf("ERROR: Invalid instruction encountered.\n");
            return -1;
        }
    }

    if (str) { memcpy(str, inst + 1, len); }
    return len;
}

void inst_read_card(FILE* file)
{
    u8 card[81] = {0};
    fread(card, 80, 1, file);
    memcpy(mem + 1, card, 80);
}

void inst_set_word_mark(bool dyadic)
{
    char c1[4] = {0};
    memcpy(c1, inst+1, 3);
    i32 loc1 = atoi(c1);
    wm[loc1] = 1;

    if (dyadic)
    {
        char c2[4] = {0};
        memcpy(c2, inst+4, 3);
        i32 loc2 = atoi(c2);
        wm[loc2] = 1;
    }

    inst += dyadic ? 7 : 4;
}

void inst_clear_storage()
{
    char str[5] = {0};
    i32 len = get_till_next_wm(str);
    i32 loc;

    if (len == 3)
    {
        loc = atoi(str);
    }
    else if (len == 0)
    {
        loc = b_register;
    }

    i32 base = (loc / 100) * 100;
    i32 size = loc - base + 1;
    memset(mem + base, ' ', size);
    memset(wm  + base, 0, size);
    b_register = base - 1;

    inst += len + 1;
}

void inst_move()
{
    char str[7] = {0};
    i32 len = get_till_next_wm(str);

    if (len == 0)
    {
        // TODO: Implement this variant of the move instruction
    }
    else if (len == 6)
    {
        // Source
        char str_src[4] = {0};
        memcpy(str_src, str, 3);
        i32 src = atoi(str_src);

        // Destination
        char str_dst[4] = {0};
        memcpy(str_dst, str+3, 3);
        i32 dst = atoi(str_dst);

        while (wm[src] == 0 &&
               wm[dst] == 0)
        {
            mem[dst] = mem[src];
            src--;
            dst--;
        }
        mem[dst] = mem[src];
    }

    inst += len + 1;
}

void inst_print_line()
{
    char str[133] = {0};
    memcpy(str, mem + 201, 132);
    printf("%s\n", str);
    inst++;
}

void inst_eject_page()
{
    i32 len = get_till_next_wm(0);
    // Eject <len> number of pages from the printer.
    // The 1401 didn't have a monitor to display output.
    inst += len + 1;
}

bool inst_halt()
{
    char str[4] = {0};
    i32 len = get_till_next_wm(str);
    printf(".%s    (Halted. Press '<Enter>' to emulate button push, or 'q<Enter>' to quit.\n", str);
    inst = mem + atoi(str);


    if (getchar() == 'q') { return true; }
    return false;
}

int main()
{
    // Allocate
    mem = (u8*)malloc(16000);
    wm  = (u8*)malloc(16000);

    // Read file
    FILE* file = fopen("/home/apoorvaj/sw/1401/hello.mc", "r");
    if (!file)
    {
        printf("ERROR: Input file couldn't be opened.\n");
        return 1;
    }

    // Emulate the start button
    inst_read_card(file); // Read a card
    memset(wm, 0, 81);    // Zero out  word marks at 0-81
    *(wm + 1) = 1;        // Set the first word mark bit
    inst = mem + 1;       // Set the instruction pointer to the beginning

    inst_set_word_mark(true); // First instruction is always a dyadic set word mark

    // Program execution
    while (1)
    {
        switch (*inst)
        {
            case ',':
            {
                i32 len = get_till_next_wm(0);
                if      (len == 6) { inst_set_word_mark(true); }
                else if (len == 3) { inst_set_word_mark(true); }
                else { printf("ERROR: Invalid instruction length.\n"); }
            } break;

            case '/': { inst_clear_storage();       } break;
            case 'M': { inst_move();                } break;
            case '2': { inst_print_line();          } break;
            case 'F': { inst_eject_page();          } break;
            case '.': { if (inst_halt()) goto Halt; } break;

            default:
            {
                printf("Instruction '%c' not implemented.\n", *inst);
            } break;
        }
    }

Halt:

    // Visualize word marks
    printf("\n--------------------------------\nVisualize word marks: \n");
    mem[81] = 0;
    printf("%s", mem+1);

    for (i32 i = 1; i < 81; i++) { printf("%c", wm[i] ? '^' : ' '); }
    printf("\n");

    free(mem);
    free(wm);
    return 0;
}
