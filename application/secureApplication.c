#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "transferMsg.h"
#include "armDefines.h"
#include "arm7_support.h"
#include "rxApi.h"

#define ENTER_SECURE_USING_MONITOR() asm("mov r2, #1\n" "SMC #0\n")

#define ENTER_NON_SECURE_USING_MONITOR() asm("mov r2, #2\n" "SMC #0\n")

#define TZPC_BASE         ((volatile unsigned char *) 0x10001000) 

#define TZPC_DECPROT0_SET ((volatile unsigned char *) (TZPC_BASE + 0x804)) 

#define TZPC_DECPROT0_CLR ((volatile unsigned char *) (TZPC_BASE + 0x808))

#define DISABLE_BRIDGE_COMMUNICATION() *TZPC_DECPROT0_CLR = 1

#define ENABLE_BRIDGE_COMMUNICATION() *TZPC_DECPROT0_SET = 1;

register r2 asm("r2");
register r5 asm("r5");

enum
{
    NONE,
    TO_SECURE,
    TO_UNSECURE
};

// Enter non-secure mode by setting SCR.NS (bit 0)
void enterNonSecure() 
{
    unsigned int scr;
    RD_SCR(scr);
    WR_SCR(scr | 1);
}

// Define a struct that stores each flit and the time that it arrive in the router
typedef struct{

    int sender; // @0

    int messageSize; // @4

}messageHeader;

 

// ############# Callbacks #############


//
// SMC handler - 
//
static void smc_handler(void *ud)
{
    asm(
    ".global SMC_Handler_Main\n"
    "TST lr, #4\n"
    "ITE EQ\n"
    "B SMC_Handler_Main\n"
    );
}

// Supervisor mode entry;
static void SVC_entry()
{
    printf("test\n");
}

//
// IRQ handler - called when any sort peripheral signals completion
//
static void irq_handler(void *ud)
{
    char buffer[8]; // buffer to retrieve header information
    int i; // loop counter
    messageHeader *header; //message header

    printf("Received interrupt. Starting handle\n\n############\n\n");
    DISABLE_INTERRUPTS();
    ackInterruptHandler();

    //verifies header, sender and hash
    // FUTURE

    printf("Entering in secure world\n");
    
    ENTER_SECURE_USING_MONITOR();

    ENABLE_BRIDGE_COMMUNICATION();
    
    printf("Entered in secure world\n");

    for(i = 0; i< 8; i++)
    {
        buffer[i] = *(RG_READ_HEADER_DATA);
    }

    memcpy(header,buffer,8);
    
    receiveMessage(header->messageSize);
    
    
    DISABLE_BRIDGE_COMMUNICATION();
    
    setSVCHandler();
    ENTER_NON_SECURE_USING_MONITOR();
    
    printf("Entered in non secure world\nExiting irq exception\n\n############\n\n");
   
    asm(
    "MRS     R0, CPSR\n\t"
    "BIC     R0, R0, #0xC0\n\t" 
    "MSR     CPSR, R0\n"
    "LDMFD   SP!, {R8, R12}\n"//    ; Load R8, SPSR
    "BIC     R12, R12, #0x80 \n"//   ; Set IRQ flag to disable it
    "MSR     SPSR_cxsf, R12\n"//    ; Set SPSR(Saved Program Status Register)
    "CPS     #0x13\n" // Set execution Level
    "LDR PC, =main\n"
    );
}


void SMC_Handler_Main( unsigned int *svc_args )
{
    
    int smc_number = r2;
    printf("NS - Entered in Super Monitor Call Handler!!! Operation is %d\n", smc_number);
    switch (smc_number)
    {
    case TO_SECURE:
        printf("Writing 0x0 to ns bit\n");
        asm(
            "MRC P15, 0, R1, C1, C1, 0 \n" // Read SCR.
            "ORR R1, R1, #(1 << 0)\n" // Set SCR.NS (bit 0).
            "BIC R1, R1, #(1 << 7)\n" // Clear SCR.SCD (bit 7).
            "MCR P15, 0, R1, C1, C1, 0\n" // Write SCR.
            // Initialize registers to save values.
            "MOV R0, #0\n"
            "MCR P15, 0, R0, C1, C0, 0\n" // SCTLR(NS).
            "LDR R1, =0x0\n"
            "MCR P15, 0, R1, C12, C0, 0\n" // VBAR(NS).

            "MSR SPSR_cxsf, #0x13\n" 
            //"LDR PC, =SVC_entry\n" // SVC_entry points to the first 
            //"MOVS PC, LR\n" // in the ARM instruction set
            //"ERET\n"
        );
        

        //WR_SCR(0);
        break;
    case TO_UNSECURE:
        printf("Writing 1 to ns bit\n");
        WR_SCR(1);
    
        break;
    default:
        asm(
            "MSR SPSR_cxsf, #0x13\n"
            "LDR PC, =run\n" // SVC_entry points to the first 
        );
        break;
    }
}

/// ############# Config methods #############

// Add a section entry to the translation table mapping 1MB
// starting at VA to physical address PA
void addSection(unsigned int VA, unsigned int PA) {

    unsigned int index  = (VA >> 20);
    unsigned int entry  = (PA & 0xfff00000)             |  // Section base address
                          0x3 << 10                     |  // AP = read/write
                          0x2;                             // Section entry, PXN=0

    // Set the entry in the table to be a section
    translationTable[index] = entry;

}

// Setup translation table entries for first 2MB of memory and UART addresses
void setupTranslationTable() {
    
    addSection(0x00000000, 0x00000000);
    addSection(0x00100000, 0x00100000);
    addSection(0xfffffff0,0x001f0000);
    addSection(0xfffffff8,0x001f0008);
    addSection(0x200000,0x001ff000);
    
    addSection(0x300000,0x001fff00);
    addSection(TZPC_BASE,TZPC_BASE);
    addSection(RG_ACK_INTERRUPT,RG_ACK_INTERRUPT);
    addSection(0x01000000,0x01000000);
    addSection(0xea0020c2, 0x001fff00);
    
}

// Enable the TLB
void enableTLB() {

    // Write the translation table base register
    WR_TTBR0((unsigned int)translationTable);

    // Set DACR to 1 to enable client permissions for domain 0
    WR_DACR(1);

    // Set SCTLR.M (bit 0) to enable TLB
    unsigned int sctlr;
    RD_SCTLR(sctlr);
    WR_SCTLR(sctlr | 1);

}


void setSVCHandler()
{
    // reset vector
    unsigned int *reset = (void *) 0x8;
    // branch instruction encoding
    unsigned int instr = 0xea000000;
    unsigned int start = (unsigned int)(void*)&smc_handler;

    start = ((start-8)>>2) & 0x00ffffff;
    // write svc handle vector
    *reset = (instr | start);
}


static int
fib(int i)
{
    return (i > 1) ? fib(i - 1) + fib(i - 2) : i;
}

void fibPrint()
{    
    // enter in __ mode
    asm("CPS #0x12\n");
    int i, j;
    for (j = 0; j < 1; j++) {

            for (i = 0; i < 32; i++) {
                printf("fib(%d) = %d\n", i, fib(i));
            }

        }
}

void boot()
{
    printf("Hello World from secure-processor!!!\n");
    
    // Boot for secure world
    CPU_INIT();
    setupTranslationTable();
    enableTLB();
    setSVCHandler();
    

    // Boot for unsecure world
    //CPU_INIT();
    //setSVCHandler();
    
    enterNonSecure();   
    REGISTER_ISR(irq, irq_handler, (void *)NULL);
    ENABLE_INTERRUPTS(); 
}

void run()
{
    fibPrint();
    printf("Finished fibonatti\n");

    //Test code to verify memory protection
    r2 = TO_SECURE;
    asm("SMC #0\n");
    printf("First data in secure space %c\n\n",*SECURE_MEMORY_REGION);
}

static int startup_flag = 0;
int main() 
{
    if(startup_flag == 0)
    {
        startup_flag = 1;
        boot();
    }
    
    run();

    return 0;
}
