#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "ArrayList.c"
#define MAX_SYMBOL_TABLE_SIZE 100
#define cons 1
#define var 2
#define proc 3
#include"module_1.h"
#define MAX_STACK_HEIGHT 2000
#define MAX_CODE_LENGTH 500
#define MAX_LEXI_LEVELS 3
#define NEXT_INSTRUCTION 3

enum symbols
{
    nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
minussym = 5, multsym = 6,  slashsym = 7, oddsym = 8,
eqlsym = 9, neqsym = 10, lessym = 11, leqsym = 12,
gtrsym = 13, geqsym = 14, lparentsym = 15, rparentsym = 16,
commasym = 17, semicolonsym = 18, periodsym = 19, becomessym
= 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
varsym = 29, procsym = 30, writesym = 31, readsym = 32,
elsesym = 33,
};

typedef struct symbol {
  int kind;    // const = 1, var = 2, proc = 3
  char name[12]; // name up to 11 chars
  int val;    // number (ASCII value)
  int level;    // L level
  int addr;    // M address
} symbol;

typedef struct Token
{
    char word [30];
    int sym;
    int value;
}Token;

const char *errors[30] =
{
    "period expected.",
    //#1
    "identifier expected",
    //#2
    "equal sign expected",
    //#3
    "number expected",
    //#4
    "semi-colon expected",
	//#5
	"becomes symbol expected",
	//#6
	"end expected",
	//#7
	"do keyword after while loop expected",
	//#8
	"relational operator expected",
	//#9
	"parenthesis is not closed",
	//#10
	"some factor symbol is expected",
	//#11
	"Variable has not been previously declared"
};

FILE *lexemeTableFile;
FILE *emitWriter;
Token tokenList [1000];
Token token;
int tokenSpot = 0;
int tokenCount = 0;
int symbolSpot = 0;
int symbolretriever;
int symbolTableSize = 0;
int codeSpot = 0;
int code3 [3000];

//THE START OF MOD2 CODE
char buffer[256];
FILE *codeFile;
FILE *output;
FILE *lexemeTable;
FILE *lexemeList;
int structIndex = 0;
int getter;
int codeCount = 0;
Token *tokenArray;
ArrayList *codeArray;
int tokenArrayCount = 0;
symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
symbol symbolHolder;

struct Instruction
{
    int op;
    int l;
    int m;
};

int lexiLevel = 0;
int partition = 0;
int codeSize = 0;
int buffer1 = 0;
int sp = 0;
int bp = 1;
int pc = 0;
int stack [MAX_STACK_HEIGHT ];
int code [MAX_CODE_LENGTH ];
FILE *codeFile;
FILE *output;
char *opString;
struct Instruction ir;

int main()
{
    project1();
    program2();
    project3();
    return 0;
}
// THE START OF MOD3 CODE
void printOut()
{
    printf("\nOUTPUT :");
    printf("\n");
    printf("OP\t");
    printf("L\t");
    printf("M\n");
    int i =0;
    while(i < codeSpot)
    {
        printf("%d\t%d\t%d\n",code3[(i *3)], code3[(i *3) + 1], code3[(i *3) + 2]);
        fprintf(emitWriter, "%d %d %d \n", code3[(i *3)], code3[(i *3) + 1], code3[(i *3) + 2]);
        i++;
    }
}
void load()
{
    char string [20];
    char string2 [20];
    int number;
    lexemeTableFile = fopen("lexemeTable.txt", "r");
    fscanf(lexemeTableFile, "%s %s", string, string2);
    while(fscanf(lexemeTableFile, "%s %d", string, &number) != EOF)
    {
        if(number == 3)
            tokenList[tokenCount].value = atoi(string);
        else
            tokenList[tokenCount].value = 0;
        //printf("%s %d\n", string, tokenCount);
        strcpy(tokenList[tokenCount].word,string);
        tokenList[tokenCount].sym = number;
        tokenCount += 1;
    }
}

void program2()
{
    load();
    initEmit();
    getToken();
    block();
   // if (token.sym != periodsym)
      //  getError(0);
    emit(9, 0, 2);
    printOut();
    closeEmit();
}

void block()
{
    int varCount = 0;
    //constant symbol function
    if (token.sym ==  constsym)
    {
        do
        {
            getToken();
            if(token.sym != identsym)
                getError(1);
            strcpy(symbolHolder.name, token.word);
             getToken();
            if(token.sym != eqlsym)
                getError(2);
            getToken();
            if(token.sym != numbersym)
                getError(3);
            enter(cons, symbolHolder.name, 0, 0, token.value);
            getToken();
        }
        while (token.sym == commasym);

        if(token.sym != semicolonsym)
            getError(4);
        getToken();
    }
    //variable symbol function
    if(token.sym == varsym)
    {
        do
        {

           getToken();
           if(token.sym != identsym)
                getError(1);
            enter(var, token.word, 0, 4 + varCount, 0);
            varCount ++;
           getToken();
        }
        while(token.sym == commasym);
        if(token.sym != semicolonsym)
            getError(4);
        getToken();
    }
    if(token.sym == procsym)
    {
        do
        {
            getToken();
            if(token.sym != identsym)
                getError(1);
            enter(proc, token.word, 0, codeSpot, token.sym);
            getToken();
            if(token.sym != semicolonsym)
                getError(4);
            getToken();
            block();
            if(token.sym != semicolonsym)
                getError(4);
            getToken();
        }
        while(token.sym == procsym);
    }
    statement(varCount);
}

void getToken()
{
    token = tokenList[tokenSpot];
    //printf("This token being read is %s\n", token.word);
    tokenSpot ++;
}

void getError(int error)
{
    printf("%s", errors[error]);
    exit(error);
}

void statement(int varCount)
{
    int changeSpot = 0;
    int sym;
    symbol localSymbolHolder;
    int localVarCount = varCount;
    if(token.sym == identsym)
    {
        //printf("\n\nThe token word is : %s\n\n", token.word);
        //printf("The symbol word is : %s\n", symbol_table[1].name);
        if(isSymbol(token.word))
            localSymbolHolder = symbol_table[getSymbol(token.word)];
        else
            getError(11);
        getToken();
    if(token.sym != becomessym)
        getError(5);
    getToken();
    expression();
    enter(var,localSymbolHolder.name,0,0,getTop());
    emit(4, 0, localSymbolHolder.addr);
   // printf("\n\nThe TOS IS %d\n\n", getTop());
    }
    else if(token.sym == callsym)
    {
        getToken();
        if(token.sym != identsym)
            getError(2);
        getToken();
    }
    else if(token.sym == beginsym)
    {
        emit(6, 0, 4 + localVarCount);
        getToken();
        statement(localVarCount);
        while(token.sym == semicolonsym)
        {
            getToken();
            statement(localVarCount);
        }
        if(token.sym != endsym)
             getError(6);
        getToken();
    }
    else if(token.sym == ifsym)
    {
        getToken();
        changeSpot = condition();
        if(token.sym != thensym)
            getError(6);
        getToken();
        statement(localVarCount);
    }
    else if(token.sym == whilesym)
    {
        getToken();
        condition();
        if(token.sym != dosym)
            getError(7);
        getToken();
        statement(localVarCount);
    }
   // printf("\n\nThe change spot and codespot  is: %d, %d\n\n", changeSpot, codeSpot);
    if(changeSpot != 0)
        code3[(changeSpot * 3) + 2] =codeSpot;
}

int condition()
{   Token op;
    int changeSpot = 0;
    if(token.sym == oddsym)
    {
        getToken();
        expression();
        //odd command
        emit(2, 0, 6);
    }
    else
    {
        expression();
        if(!isRelation())
            getError(8);
        op = token;
        getToken();
        expression();
        changeSpot = writeRelation(op.sym);
    }
    return changeSpot;
}

void expression()
{
    if (token.sym == plussym)
    {
        getToken();
        term();
        //add
        emit(2, 0, 2);
    }
    else if (token.sym == minussym)
    {
        getToken();
        term();
        //sub
        emit(2, 0, 3);
    }
    else
        term();


    while(token.sym == plussym || token.sym == minussym)
    {

        if (token.sym == plussym)
        {
            getToken();
            term();
            //add
            emit(2, 0, 2);
        }
        else if (token.sym == minussym)
        {
            getToken();
            term();
            //sub
            emit(2, 0, 3);
        }
    }
}

void term()
{
    factor();
    while(token.sym == multsym || token.sym == slashsym)
    {
       if (token.sym == multsym)
        {
            getToken();
            factor();
            //mult
            emit(2, 0, 4);
        }
        else if (token.sym == slashsym)
        {
            getToken();
            factor();
            //div
            emit(2, 0, 5);
        }
    }
}

void factor ()
{
    if (token.sym == identsym)
    {
        emit(1,0,getSymbolValue(token.word));
        getToken();
    }
    else if(token.sym == numbersym)
    {
        emit(1, 0, atoi(token.word));
        getToken();
    }
    else if(token.sym == '(')
    {
        getToken();
        expression();
        if(token.sym != ')')
            getError(9);
        getToken();
    }
    else
        getError(10);

}

int writeRelation(int opSym)
{
    //write the correlated emit code for the op symbol
    switch(opSym)
    {
        case 4:
            emit(2, 0, 2);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 5:
            emit(2, 0, 3);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 6:
            emit(2, 0, 4);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 7:
            emit(2, 0, 5);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 8:
            emit(2, 0, 6);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 9:
            emit(2, 0, 8);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 10:
            emit(2, 0, 9);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 11:
            emit(2, 0, 10);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 12:
            emit(2, 0, 11);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 13:
            emit(2, 0, 12);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        case 14:
            emit(2, 0, 13);
            emit(8, 0, 0);
            return codeSpot - 1;
            break;
        default:
            getError(12);
            return 12;
    }
}

void initEmit()
{
    //setup the file pointer for emit to write to
    emitWriter = fopen("mcode.txt", "w");
}

void emit(int op, int l, int m)
{
    code3[codeSpot * 3] = op;
    code3[(codeSpot * 3) + 1] = l;
    code3[(codeSpot * 3) + 2] = m;
    codeSpot ++;
    //printf("%d %d %d \n", op, l, m);
}

void closeEmit()
{
    fclose(emitWriter);
    /*int i = 0;
    while (i < symbolSpot)
    {
        printf("%s,", symbol_table[i].name);
        printf("%d\n", symbol_table[i].val);
        i ++;
    }
    */
}

void enter(int kind, char *name, int l, int m, int val)
{
    symbol tempSymbolHolder;
    tempSymbolHolder.kind = kind;
    strcpy(tempSymbolHolder.name, name);
    tempSymbolHolder.level = l;
    tempSymbolHolder.addr = m;
    tempSymbolHolder.val = val;
    //printf("The name that is being inputed is : %s", tempSymbolHolder.name);
    if(isInTable(tempSymbolHolder.name) == 0)
    {
        symbol_table[symbolSpot] = tempSymbolHolder;
        symbolSpot ++;
        symbolTableSize ++;
        //printf("The name that is being inputed is : %s\n", tempSymbolHolder.name);
        //printf("The table size is being increased\n");
    }
    //update value if already in the table
    else
    {
        symbol_table[getSymbol(name)].val = val;
       // printf(" It is working for symbol %s and The value of the symbol is : %d\n", symbol_table[getSymbol(name)].name, symbol_table[getSymbol(name)].val);
    }
}

int isInTable(char *name)
{
    int i =0;
   // printf("\nThe counter is at : %d and the table size is at : %d\n", i, symbolTableSize);
    while(i <= symbolTableSize)
    {
       // printf("The name of the parameter is : %s\n", name);
        //printf("The name of the symbol name is %s\n", symbol_table[i].name);
        if(strcmp(name, symbol_table[i].name) == 0 )
        {
            return 1;
        }
        i ++;
    }
    return 0;
}

int getSymbol(char *name)
{
    int i =0;
    while(i < symbolTableSize)
    {
        if(strcmp(name, symbol_table[i].name) == 0 )
           return i;
        i ++;
      //  printf("The symbol %s has a position of %d\n", symbol_table[0].name, symbol_table[0].addr);
       // printf("The symbol %s has a position of %d\n", symbol_table[1].name, symbol_table[1].addr);
    }
    return 0;
}




int getSymbolValue(char *name)
{
    int i =0;
   // printf("the token name the value that is being fetched is : %s\n", name);
    while(i < symbolTableSize)
    {
        if(strcmp(name, symbol_table[i].name) == 0 )
            return symbol_table[i].val;
        i ++;
    }
    getError(11);
    return 0;
}

int getTop()
{
    return code3[((codeSpot - 1) * 3) + 2];
}

int isSymbol(char *name)
{
    int i =0;
    while(i < symbolTableSize)
    {
        if(strcmp(name, symbol_table[i].name) == 0)
           return 1;
        i ++;
    }
    return 0;
}

int isRelation()
{
     switch(token.sym)
    {
       case 4:
            return 1;

        case 5:
            return 1;

        case 6:
            return 1;

        case 7:
            return 1;

        case 8:
            return 1;

        case 9:
            return 1;

        case 10:
            return 1;

        case 11:
            return 1;

        case 12:
            return 1;

        case 13:
            return 1;

        case 14:
            return 1;

        default:
            getError(12);
            return 0;
    }
}

//THE START OF MOD 1 CODE
void project3()
{
    output = fopen("output.txt", "w");
    initArray();
    //printf("It initialized the arrays\n");
    load3();
    //testFunction();
    format2();
    while(1==1)
    {
        fetch();
        execute();
    }

}


void load3()
{
    stack[1] = 0;
    stack[2] = 0;
    stack[3] = 0;
    getCode();
    format();
    fclose(codeFile);
}

void format ()
{
   // printf("The codesize is : %d\n", codeSpot);
    fprintf(output,"Line\t");
    fprintf(output,"OP\t");
    fprintf(output,"L\t");
    fprintf(output,"M\n");
    int i =0;
    while(i < codeSpot)
    {
        fprintf(output, "%d\t%d\t%d\t%d\n",i,  code3[(i *3)], code3[(i *3) + 1], code3[(i *3) + 2]);
        i++;
    }
   // printf("sucessfully formatted\n");
}

void getCode()
{
    int i = 0;
    codeFile = fopen("mcode.txt", "r");
    while(fscanf(codeFile, "%d", &buffer1)!=EOF)
    {
        code[i]=buffer1;
        codeSize++;
        //testing purposes
        //printf("%d", code[i]);
        i++;
        if(i % 3 == 0)
            printf("\n");
    }
}

void fetch ()
{
    getInstruction();
    pc ++;
    //printf("After it is %d \n",pc);
}

void format2()
{
    fprintf(output, "\t\t\t\tpc\t\tbp\tsp\tstack\n");
    fprintf(output, "Initial Values\t\t\t0\t\t1\t0\n");

}
void getInstruction()
{
    //For testing purposes
    //printf("during the fetch pc is %d \n",pc);
    //print out the instruction number, opcode string, l, and m.
    ir.op = code[pc * NEXT_INSTRUCTION];
    ir.l = code[pc * NEXT_INSTRUCTION + 1];
    ir.m =  code[pc * NEXT_INSTRUCTION + 2];
    opString = getOpString(ir.op);
    fprintf(output, "%d\t%s\t%d\t%d", pc, opString, ir.l, ir.m );
}

char *getOpString(int op)
{
    switch (op)
        {
        //lit
        case 1:
            return "lit";
            break;
        // opr will require another sub-function to decide
        // which operation to run
        case 2:
            return "opr";
            break;
        //lod
        case 3:
            return "lod";
            break;
        //sto
        case 4:
            return "sto";
            break;
        //cal
        case 5:
            return "cal";
            break;
        //inc
        case 6:
            return "inc";
            break;
        //jmp
        case 7:
            return "jmp";
            break;
        //jpc
        case 8:
            return "jpc";
            break;
        //sio
        //this will require another sub function to decide
        //which i/o to run
        case 9:
            return "sio";
            break;

        default:
            fprintf(output, "OP code input was invalid. ");
            sio3();
        }

}

int isDone ()
{
    if(code[pc * NEXT_INSTRUCTION] == 9 && code[pc * NEXT_INSTRUCTION + 2] == 3)
        return 1;
    return 0;
}

void execute()
{
    switch ( ir.op)
        {
        //lit
        case 1:
            lit(ir.m);
            break;
        // opr will require another sub-function to decide
        // which operation to run
        case 2:
            opr();
            break;
        //lod
        case 3:
            lod(ir.l, ir.m);
            break;
        //sto
        case 4:
            sto(ir.l, ir.m);
            break;
        //cal
        case 5:
            cal(ir.l, ir.m);
            break;
        //inc
        case 6:
            inc(ir.m);
            break;
        //jmp
        case 7:
            jmp(ir.m);
            break;
        //jpc
        case 8:
            jpc(ir.m);
            break;
        //sio
        //this will require another sub function to decide
        //which i/o to run
        case 9:
            sio();
            break;

        default:
            fprintf(output, "OP code input was invalid. ");
            sio3();
        }
        //print pc, bp, sp
        //use loop to print stack
        fprintf(output, "\t%d\t\t%d\t%d\t", pc, bp, sp);
        stackPrint();
}
//initializes the stack and code arrays.
void initArray()
{
    for(int i = 0; i < MAX_STACK_HEIGHT; i ++)
        stack[0] = 0;
    for(int i = 0; i < MAX_CODE_LENGTH; i ++)
        code[0] = 0;

}
//iterates through the dynamic link values to return the desired AR.
int getBase(int level, int base)
{
    for(int i = level; i > 0; i --)
    {
        base = stack[base + 2];
    }
    return base;
}

//this is simply a push function
void lit (int pushValue)
{
    sp ++;
    stack[sp] = pushValue;
}

void testFunction()
{
    printf(" the very last number should be 3? = %d", code[50]);
}

//lod function will do 3 things
//get the base value L levels down
//grab the number at offset M
//push that number
void lod (int levels, int offset)
{
    int base = getBase(levels, bp);
    int value = stack[base + offset];
    lit(value);
}

//this function is simply a store function
void sto (int levels, int offset)
{
    int base = getBase(levels, bp);
    stack [base + offset] = stack[sp];
    sp --;
}

void cal (int levels, int pro_Location)
{
    stack[sp + 1] = 0;
    stack[sp + 2] = getBase(levels, bp);
    stack[sp + 3] = bp;
    stack[sp + 4] = pc;
    bp = sp + 1;
    pc = pro_Location;
}

void inc (int space)
{
    sp += space;
    lexiLevel ++;
}

void jmp (int jumpSpot)
{
    pc = jumpSpot;
}

void jpc (int jumpSpot)
{
    if (stack[sp] == 0)
        pc = jumpSpot;
    sp --;
}

//this is the first standard i/o function which will pop and print
void sio1()
{
    printf("%d" ,stack[sp]);
    sp --;
}

//the second i/o function will take in input and push it to the top of the stack
void sio2()
{
    int input;
    scanf("%d" ,&input);
    lit(input);
}

//Third standard i/o halts the system
void sio3()
{
    fprintf(output, "\t%d\t\t%d\t%d\t", pc, bp, sp);
    stackPrint();
    exit(0);
}

void stackPrint()
{
    if(lexiLevel == 1)
    {
        for(int i = 1; i <= sp; i ++)
            fprintf(output, "%d ", stack[i]);
        fprintf(output, "\n");

    }
    else if(lexiLevel == 2)
    {
        for(int i = 1; i <= sp; i ++)
        {
            if(i == bp)
                fprintf(output, "| ");
            fprintf(output, "%d ", stack[i]);

        }
        fprintf(output, "\n");
    }
    else
    {
       int base = getBase(1, bp);
       for(int i = 1; i <= sp; i ++)
        {
            if(i == bp || base)
                fprintf(output, "| ");
            fprintf(output, "%d ", stack[i]);

        }
        fprintf(output, "\n");

    }


}

void ret ()
{
    sp = bp - 1;
    pc = stack[sp + 4];
    bp = stack[sp + 3];
    lexiLevel --;
}

void neg ()
{
    stack[sp] = -stack[sp];
}

void add ()
{
    sp --;
    stack[sp] = stack[sp] + stack[sp + 1];
}

void sub ()
{
    sp --;
    stack[sp] = stack[sp] - stack[sp + 1];
}

void mul ()
{
    sp --;
    stack[sp] = stack[sp] - stack[sp + 1];
}

void divid()
{
    sp --;
    stack[sp] = stack[sp] / stack[sp + 1];
}

void mod ()
{
    sp --;
    stack[sp] = stack[sp] % stack[sp + 1];
}

void eql ()
{
    sp --;
    stack[sp] = stack[sp] == stack[sp + 1];
}

void neq ()
{
    sp --;
    stack[sp] = stack[sp] != stack[sp + 1];
}

void lss ()
{
    sp --;
    stack[sp] = stack[sp] < stack[sp + 1];
}

void leq ()
{
    sp --;
    stack[sp] = stack[sp] <= stack[sp + 1];
}

void gtr ()
{
    sp --;
    stack[sp] = stack[sp] > stack[sp + 1];
}

void geq ()
{
    sp --;
    stack[sp] = stack[sp] >= stack[sp + 1];
}

void odd()
{
    stack[sp] = stack[sp] % 2;
}

void opr()
{
    switch ( ir.m)
    {
        //rtn
        case 0:
            ret();
            break;
        //neg
        case 1:
            neg();
            break;
       //add
        case 2:
            add();
            break;
       //sub
        case 3:
            sub();
            break;
        //mul
        case 4:
            mul();
            break;
        //div
        case 5:
            divid();
            break;
        //odd
        case 6:
            odd();
            break;
        //mod
        case 7:
            mod();
            break;
        //eql
        case 8:
            eql();
            break;
        //neq
        case 9:
            neq();
            break;
        //lss
        case 10:
            lss();
            break;
        //leq
        case 11:
            leq();
            break;
        //gtr
        case 12:
            gtr();
            break;
        //geq
        case 13:
            geq();
            break;

        default:
            fprintf(output, "OP code input was invalid. ");
            sio3();
    }
}

void sio()
{
    switch ( ir.m)
    {
        //output to the screen
        case 0:
            sio1();
            break;

        //take input
        case 1:
            sio2();
            break;

        //halt
        case 2:
            sio3();
            break;

        default:
            fprintf(output, "OP code input was invalid. ");
            sio3();
    }

}



//THE START OF MOD 2 CODE
void structPut(char *word, int num)
{
    if(!tokenArray)
    {
        printf("Token Array not properly initialized");
    }
    strcpy(tokenArray[structIndex].word, word);
    tokenArray[structIndex].sym = num;
}

void initArrays()
{
    tokenArray = malloc(sizeof(Token)*10000);
    codeArray = createArrayList(10);
}


void project1()
{
    codeFile = fopen("input.txt", "r");
    output = fopen("cleaninput.txt", "w");
    initArrays();
    load1();
    printCleanInput();
    cleanArrayList();
    findToken();
    //errorCheck();
    printLexemeTable();
    printLexemeList();
    printTest();
}

//function responsible for retrieving data from the input file and then loading it to the code array for
//code output and further token processing.
void load1()
{
    codeCount = 0;
    char symbolBuffer[2];
    int i = 0;
    int x, prev = 0, codeIndex, endSwitch = 0, commentSwitch = 0;
    if(!codeFile)
    {
        printf("Error in opening the file.");
        exit(0);
    }
    while (isEnd() != 1)
    {
        x = fgetc(codeFile);
        //printf("%d", codeCount);
        //printf("this iteration of x is %c \n", x);
        char tempString [256];

        if(isSymbol2(x))
        {
            if(x == '*' && prev == '/')
            {
                skipComment();
                prev = 33;
                continue;
            }

            if(!isSymbol2(prev))
            {
                codeCount += 2;
               //printf("\nthe string is %c%c%c\n", buffer[0],buffer[1],buffer[2]);
               put(codeArray, buffer);
               //printf("\n%s\n", buffer);
            }

            symbolBuffer[0] = x;
            symbolBuffer[1] = '\0';
            put(codeArray, symbolBuffer);
            //printf("\n%s\n", symbolBuffer);
            i = 0;
        }
        else
        {
            codeCount ++;
            buffer[i] = x;
            buffer[i+1] = '\0';
            i++;
        }
        prev = x;
        //printf("%d\n", codeCount);
    }
    put(codeArray, "end");
    put(codeArray, ".");
    //printArrayList(codeArray);
    fclose(codeFile);

}

int isSymbol2(int x)
{
    if((x>=0 && x<=47) || (x>=58 && x<=64) || (x>= 91 && x<=96) || (x>=123 && x<=126))
    {
        return 1;
    }
    return 0;
}

void printCleanInput()
{
    int i = 0;
    char* tempString;
   //printf("the count code is %d", codeCount);
    while(i < codeArray ->size)
    {
        int temp;
        tempString = get(codeArray, i);
        temp = tempString[0];
       if((tempString = get(codeArray, i)) == "\n")
            fprintf(output, "\n");
        if(temp != 47)
            fprintf(output, "%s", tempString);
        i ++;
    }
    fclose(output);
}

int isEnd ()
{
    //printf("code cont is %d", codeCount);
    //printf("\n1-> %d. 2-> %d, 3-> %d\n", buffer[0], buffer[1], buffer[2]);
    if (buffer[0] == 'e' && buffer[1] == 'n' && buffer[2] == 'd')
        return 1;
    return 0;
}

void skipComment()
{
        int previousPointer = 0, currentPointer = fgetc(codeFile);
        while (previousPointer != '*' && currentPointer != '/')
        {
            previousPointer = currentPointer;
            currentPointer = fgetc(codeFile);
        }
}
void printTest()
{
    int i = 0;
   // while(i < tokenArrayCount)
   // {
    //    printf("%s\t", tokenArray[i].word);
    //    printf("%d\n", tokenArray[i].sym);
   //     i ++;
    //}

   //printf("The code account is %d", codeCount);
   //printArrayList(codeArray);



   // printf("%d\n", tokenArrayCount);
}

void cleanArrayList()
{
    char* tempString;
    int temp;
    for(int i = 0; i < codeArray ->size; i ++)
    {
        tempString = get(codeArray, i);
        temp = tempString[0];
        if(temp == 0 || temp == 10 || temp == 47 || temp == 9 || temp == 11 || temp == 32)
        {
            removeElement(codeArray,i);
            i --;
        }
    }
}

void findToken()
{
    structIndex = 0;
    getter = 0;
    char *string;
    char bos;
    while(getter < codeArray ->size)
    {
            string = get(codeArray, getter);
            bos = string[0];
            if(isLetter(bos))
            {
                if(wordSwitch(string))
                {
                    getter ++;
                    continue;
                }
                else
                {
                    putIdentifierToken(string);
                    getter ++;
                    continue;
                }
            }
            else
            {
                if(isNumber(string))
                {
                    getter ++;
                    continue;
                }
                else
                    symbolSwitch(string);
                    getter ++;
            }
    }
    int i = 0;
    printf("CLEAN INPUT :\n");
    while(i < tokenArrayCount)
    {
        printf("%s\t", tokenArray[i].word);
        printf("%d\n", tokenArray[i].sym);
        i ++;
    }
}
int isLetter(char letter)
{
    if(letter >= 65 && letter <= 90)
        return 1;
    else if(letter >= 97 && letter <= 122)
        return 1;
    else
        return 0;
}

int wordSwitch(char *string)
{
    char bos = string[0];
    switch(bos)
    {
        case 'c':
            if(strcmp (string, "const") == 0)
            {
                putReserveToken("const", 28);
                break;
            }
            if(strcmp (string, "call") == 0)
            {
                putReserveToken("call", 27);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'v':
            if(strcmp (string, "var") == 0)
            {
                putReserveToken("var", 29);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'p':
            if(strcmp (string, "procedure") == 0)
            {
                putReserveToken("procedure", 30);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'b':
            if(strcmp (string, "begin") == 0)
            {
                putReserveToken("begin", 21);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'e':
            if(strcmp (string, "end") == 0)
            {
                putReserveToken("end", 22);
                break;
            }

            if(strcmp (string, "else") == 0)
            {
                putReserveToken("else", 33);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'i':
            if(strcmp (string, "if") == 0)
            {
                putReserveToken("if", 23);
                break;
            }
            putIdentifierToken(string);
            break;

        case 't':
            if(strcmp (string, "then") == 0)
            {
                putReserveToken("then", 24);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'w':
            if(strcmp (string, "while") == 0)
            {
                putReserveToken("while", 25);
                break;
            }

            if(strcmp (string, "write") == 0)
            {
                putReserveToken("write", 31);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'd':
            if(strcmp (string, "do") == 0)
            {
                putReserveToken("do", 26);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'r':
            if(strcmp (string, "read") == 0)
            {
                putReserveToken("read", 32);
                break;
            }
            putIdentifierToken(string);
            break;

        case 'o':
            if(strcmp (string, "odd") == 0)
            {
                putReserveToken("odd", 8);
                break;
            }
            putIdentifierToken(string);
            break;

        default :
            putIdentifierToken(string);

    }
}

int isNumber(char *string)
{
    int number = string[0];

    if(number >= 48 && number <= 57)
    {
        putNumToken(string);
        return 1;
    }
    return 0;

}

int symbolSwitch(char *string)
{
    char symbol = string[0];
    switch(symbol)
    {
        case '+':
            putSymbolToken("+", 4);
            break;
        case '-':
            putSymbolToken("-", 5);
            break;
        case '*':
            putSymbolToken("*", 6);
            break;
        case '/':
            putSymbolToken("/", 7);
            break;
        case '=':
            putSymbolToken("=", 9);
            break;
        case '(':
            putSymbolToken("(", 15);
            break;
        case ')':
            putSymbolToken(")", 16);
            break;
        case ',':
            putSymbolToken(",", 17);
            break;
        case '.':
            putSymbolToken(".", 19);
            break;
        case '<':
            if(isNextEqual())
            {
                putSymbolToken("<=", 12);
                getter ++;
            }
            else if(isNextGts())
            {
                putSymbolToken("<>", 10);
                getter ++;
            }
            else
                putSymbolToken("<", 11);
            break;
        case '>':
            if(isNextEqual())
            {
                putSymbolToken(">=", 13);
                getter ++;
            }
            else
                putSymbolToken(">", 13);
            break;
        case ';':
            putSymbolToken(";", 18);
            break;
        case ':':
            putSymbolToken(":=", 20);
            getter ++;
            break;
        default:
            printf("Invalid symbol.");
    }
}


void putIdentifierToken(char *string)
{
    strcpy(tokenArray[structIndex].word, string);
    tokenArray[structIndex].sym = 2;
    structIndex ++;
    tokenArrayCount ++;
  //  printf("The count for the tokenArray is %d\n", tokenArrayCount);
}

void putNumToken(char *string)
{
    strcpy(tokenArray[structIndex].word, string);
    tokenArray[structIndex].sym = 3;
    structIndex ++;
    tokenArrayCount ++;
 //   printf("The count for the tokenArray is %d\n", tokenArrayCount);
}

void putReserveToken(char *string, int symbol)
{
    strcpy(tokenArray[structIndex].word, string);
    tokenArray[structIndex].sym = symbol;
    structIndex ++;
    tokenArrayCount ++;
   // printf("The count for the tokenArray is %d\n", tokenArrayCount);
}

void putSymbolToken(char *string, int symbol)
{
    strcpy(tokenArray[structIndex].word, string);
    tokenArray[structIndex].sym = symbol;
    structIndex ++;
    tokenArrayCount ++;
  //  printf("The count for the tokenArray is %d\n", tokenArrayCount);
}


void printLexemeTable()
{
    lexemeTable = fopen("lexemetable.txt", "w");
    fprintf(lexemeTable, "lexeme\t");
    fprintf(lexemeTable,"tokentype\n");
    int i = 0;
    while(i < tokenArrayCount)
    {
        fprintf(lexemeTable, "%s\t", tokenArray[i].word);
        fprintf(lexemeTable, "%d\n", tokenArray[i].sym);
        i ++;
    }
    fclose(lexemeTable);
}

void printLexemeList()
{
    int i = 0;
    lexemeList = fopen("lexemelist.txt", "w");
    printf("LEXEME LIST :\n ");
    while(i < tokenArrayCount)
    {

        fprintf(lexemeList, "%d ", tokenArray[i].sym);
        fprintf(output, "%d ", tokenArray[i].sym);
        printf("%d ", tokenArray[i].sym);
        if(tokenArray[i].sym == 3 || tokenArray[i].sym == 2)
        {
            fprintf(lexemeList, "%s ", tokenArray[i].word);
            fprintf(output, "%s ", tokenArray[i].word);
            printf("%s ", tokenArray[i].word);
        }
        i ++;
    }
    fclose(lexemeList);
}


int isNextEqual()
{
    char *string = get(codeArray, structIndex + 1);
    if(string[0] == '=')
        return 1;
    return 0;
}

int isNextGts()
{
    char *string = get(codeArray, structIndex + 1);
    if(string[0] == '>')
        return 1;
    return 0;
}

void errorCheck()
{
    int i = 0;
    int limit = tokenArrayCount;
    char *word;
    char letter;

}
void printLexemeOutput()

