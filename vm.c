/*
Assignment:
vm.c - Implement a P-machine virtual machine

Authors: <Xavier Soto and Gregory Berzinski >
Language: C (only)

To Compile:
    gcc -O2 -Wall -std=c11 -o vm vm.c

To Execute (on Eustis):
    ./vm input.txt

Notes:
- Implements the PM/0 virtual machine described in the homework instructions.
- No dynamic memory allocation or pointer arithmetic.
- Does not implement any VM instruction using a separate function.
- Runs on Eustis.

Class: COP 3402 - Systems Software - Fall 2025
Instructor : Dr. Jie Lin
Due Date: Friday, September 12th, 2025
*/

#include <stdio.h>
#include <stdlib.h>

//Define fixed size
#define MAX_PAS 500

// Process Address Space 
int pas[MAX_PAS];

// Global operation Names
char *operationNames[] = {
    "Invalid Operation",  // 0 (invalid)
    "LIT",  // 1
    "OPR",  // 2
    "LOD",  // 3
    "STO",  // 4
    "CAL",  // 5
    "INC",  // 6
    "JMP",  // 7
    "JPC",  // 8
    "SYS"   // 9
};


// Registers
int PC, BP, SP;

//global variable to help in the printing proccess(He mentioned using this in his email)
int STACK_TOP; // highest stack address to print (fixed after init)

struct {
    int OP, L, M;
} IR;

// Helper base function to follow static links
int base(int BP, int L) {
    int arb = BP;
    while (L > 0) {
        arb = pas[arb]; // follow static link
        L--;
    }
    return arb;
}

// Helper: print trace of stack with AR separator
void printStack() 
{
    //walks from highest stack address to lowest address
    for (int i = STACK_TOP; i >= SP; i--) 
    {
        // checks that AR instruction has run and puts | before AR and that its not the first cycle
        if (i == BP && i != SP && BP != STACK_TOP) printf("| ");   
        printf("%2d ", pas[i]);  // pad to width 3 when printing information from pas
    }
    printf("\n");
}

int main(int argc, char *argv[]) {


    //Handle the Command Line
    if (argc != 2) {
        printf("Error: expected 1 argument (input file)\n");
        return 1;
    }

    //opens the file based on whats passed into the file when running bash.
    FILE *in = fopen(argv[1], "r"); 
    if (!in) {
        printf("Error: cannot open input file\n");
        return 1;
    }

    //Initialize the pas[] values to 0
    for (int i = 0; i < MAX_PAS; i++){
        pas[i] = 0;
    }

    // Load code into PAS from address 499 downward
    int op, l, m;
    int addr = MAX_PAS - 1;
    while (fscanf(in, "%d %d %d", &op, &l, &m) == 3) {
        pas[addr--] = op;
        pas[addr--] = l;
        pas[addr--] = m;
    }

    fclose(in);

    //Initialze Registers
    PC = MAX_PAS - 1;  // first OP is at 499
    SP = addr + 1;     // first free cell below code
    BP = SP - 1;
    STACK_TOP = SP - 1; // stack initially empty; establish top boundary for printing

    //Print first line for format
    printf("        L        M    PC   BP   SP   stack\n");

    // Print initial state with formatting
    printf("Initial values :      %d  %d  %d\n", PC, BP, SP);

    //Fetch-Execute Loop 
    int halt = 0; //halt variable to check when to stop loop
    while (!halt) {
        // Fetch
        IR.OP = pas[PC];
        IR.L  = pas[PC - 1];
        IR.M  = pas[PC - 2];
        PC = PC - 3;

        // Execute for Operations of PM (1-9)
        //Considering Static Link Distance = l
        //Modifier = M
        switch (IR.OP) {

            // LIT (case 01)
            case 1: 
                /*
                Operation description:
                LIT (1) = Literal Push
                sp <- sp - 1
                pas[sp] <- n
                */
                SP = SP - 1;
                pas[SP] = IR.M; // use IR.M

                break;

            //OPR (case 02)
            case 2: 
                //Check for the arithmetic and Relational Operations
                switch (IR.M) {

                    //RTN (case M = 00)
                    case 0: 
                        //Return from subroutine and restore caller's AR
                        SP = BP + 1;
                        BP = pas[SP - 2];
                        PC = pas[SP - 3];
                        break;
                    
                    //ADD (case M = 01)
                    case 1: 
                        //Addition
                        pas[SP + 1] = pas[SP + 1] + pas[SP];
                        SP = SP + 1;
                        break;
                    
                    //SUB (case M = 02)
                    case 2:
                        //Substraction
                        pas[SP + 1] = pas[SP + 1] - pas[SP]; // fixed '=' and operation
                        SP = SP + 1;
                        break;
                    
                    //MUL (case M = 03)
                    case 3:
                        //Multiplication
                        pas[SP + 1] = pas[SP + 1] * pas[SP];
                        SP = SP + 1;
                        break;
                    
                    //DIV (case M = 04)
                    case 4:
                        //Integer Division
                        pas[SP + 1] = pas[SP + 1] / pas[SP];
                        SP = SP + 1;
                        break;

                    //EQL (case M = 05)
                    case 5:
                        //Equality comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] == pas[SP]);
                        SP = SP + 1;
                        break;
                    
                    //NEQ (case M = 06)
                    case 6:
                        //Inequality comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] != pas[SP]);
                        SP = SP + 1;
                        break;
                    
                    //LSS (case M = 07)
                    case 7:
                        //Less than comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] < pas[SP]);
                        SP = SP + 1;
                        break;

                    //LEQ (case M = 08)
                    case 8:
                        //Less-or-equal comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] <= pas[SP]);
                        SP = SP + 1;
                        break;

                    //GTR (case M = 09)
                    case 9:
                        //Greater than comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] > pas[SP]);
                        SP = SP + 1;
                        break;
                    //GEQ (case M = 10)
                    case 10:
                        //Greater-or-equal comparison (result 0/1)
                        pas[SP + 1] = (pas[SP + 1] >= pas[SP]);
                        SP = SP + 1;
                        break;
                    
                    //handling out of bounds case
                    default:
                        printf("Invalid M input\n");
                        break;
                    
                }
                break;
            
            //LOD (case 03)
            case 3: 
                /*
                Operation description:
                Load Value to top of stack from offset a
                in the AR n static levels down
                sp <- sp - 1
                pas[sp] <- pas[base(bp,n) -a]
                */
                SP = SP - 1;
                pas[SP] = pas[base(BP, IR.L) - IR.M]; // here we need to use IR not m 
                
                break;
            //STO (Case 04)
            case 4: 
            
                /*
                Operation Description:
                Store top of stack into offset
                o in the AR n static leves down
                pas[base(bp,n) -o] <- pas[sp]
                sp <- sp + 1
                */
               
                pas[base(BP, IR.L) - IR.M] = pas[SP]; // use IR.L / IR.M instead of l and m
                SP = SP + 1;

                break;

            //CAL (Case 05)
            case 5: 
                
                /*
                Operation description:
                Call procedure at code address a; 
                create activation record.
                pas[sp-1] <- base(bp,n)
                pas[sp-2] <- bp
                pas[sp-3] <- pc
                bp <- sp - 1
                */
                pas[SP - 1] = base(BP, IR.L);  // static link
                pas[SP - 2] = BP;              // dynamic link
                pas[SP - 3] = PC;              // return address
                BP = SP - 1;
                PC = (MAX_PAS - 1) - IR.M;     // map IR.M to address instead of maping PC to m

                break;
            
            //INC (Case 06)
            case 6: 

                /*
                Operation Description:
                Allocate n locals on the stack
                sp <- sp - n
                */
                SP = SP - IR.M; // use IR.M
                break;

            //JMP (Case 07)
            case 7: 
                /*
                Operation Description:
                Unconditional jump to address a
                pc <- a
                */
                PC = (MAX_PAS - 1) - IR.M; // Mapping to the first OP slot
                break;

            //JPC (Case 08)
            case 8: 
                /*
                Operation Description:
                Conditional Jump: if value at 
                top of stack is 0, jump to a;
                pop the stack
                if pas [sp] = 0 then pc <- a
                sp <- sp + 1
                */

                if (pas[SP] == 0){
                    PC = (MAX_PAS - 1) - IR.M; // Mapping to the first OP slot
                }

                SP = SP + 1;
                break;
            
            //SYS (Case 09)
            case 9: 

                //if M is 1 
                if (IR.M == 1) 
                {
                    /*
                    Output integer value at top of stack; then pop.
                    print(pas[sp])
                    sp <- sp + 1
                    */
                    printf("Output result is : %d\n", pas[SP]); //print output
                    SP = SP + 1;

                } 
                else if (IR.M == 2) { //if M is 2
                    /*
                    Read an integer from stdin and push it
                    sp <- sp - 1
                    pas[sp] <- readInt()
                    */
                   //implementation for reading integer input
                    printf("Please Enter an Integer : ");      
                    fflush(stdout); //better new line
                    SP = SP - 1;
                    if (scanf("%d", &pas[SP]) != 1) {  // check return value to avoid warning
                        printf("Error: invalid input\n");
                        halt = 1;
                    }

                } 
                else if (IR.M == 3) { //If M is 3

                    //Halt the program
                    halt = 1; //Indicator to halt the program
                } 
                else //Prints a diagnostic message showing the invalid M value i used it for debugging and left it
                {
                    printf("Invalid SYS M: %d\n", IR.M);
                }
                break;
            default:
                printf("Error: invalid opcode %d\n", IR.OP);
                halt = 1;
        }

        // Print trace after executing instruction
        const char *mn = operationNames[IR.OP];     //stores the operation name
        if (IR.OP == 2) //if the OP code is 2
        {
            switch (IR.M)// picking the OP based on M
            { 
                case 0:  mn = "RTN"; break;
                case 1:  mn = "ADD"; break;
                case 2:  mn = "SUB"; break;
                case 3:  mn = "MUL"; break;
                case 4:  mn = "DIV"; break;
                case 5:  mn = "EQL"; break;
                case 6:  mn = "NEQ"; break;
                case 7:  mn = "LSS"; break;
                case 8:  mn = "LEQ"; break;
                case 9:  mn = "GTR"; break;
                case 10: mn = "GEQ"; break;
                default: mn = "OPR"; break;
            }
        }

        //Print each operation with formatting for L, M, PC, BP & SP
        printf("%-7s %3d %9d %5d %5d %5d  ", mn, IR.L, IR.M, PC, BP, SP);

        //Call Function to print the stack status
        printStack();
    }

    return 0;
}