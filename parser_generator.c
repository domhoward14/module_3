#include <stdio.h>
#include <stdlib.h>
#include<string.h>

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

enum operations
{
    ret = 0, neg = 1, add = 2, sub = 3, mul = 4,
divi = 5, odd = 6,  mod = 7, eql = 8,
neq = 9, lss = 10, leq = 11, gtr = 12,
geq = 13,
};


#define MAX_SYMBOL_TABLE_SIZE 100
#define cons 1
#define var 2
#define proc 3

typedef struct symbol {
  int kind;    // const = 1, var = 2, proc = 3
  char name[12]; // name up to 11 chars
  int val;    // number (ASCII value)
  int level;    // L level
  int addr;    // M address
} symbol;

symbol symbol_table[MAX_SYMBOL_TABLE_SIZE];
symbol symbolHolder;



void load();
void program();
void block();
void statement(int varCount);
int condition();
int isRelation();
int writeRelation(int opSym);
void getToken();
void getError();
void expression();
void term();
void factor();
void emit(int op, int l, int m);
void enter(int kind, char *name, int l, int m, int val);
void closeEmit();
void initEmit();
int isInTable(char *name);
int getSymbol(char *name);
int getSymbolValue(char *name);
int getTop();
int isSymbol(char *name);
void printOut();





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
    "period expected",
    //#4
    "semi-colon expected",
	//#5
	"becomes symbol expected",
	//#6
	"then after if statement expected",
	//#7
	"do keyword after while loop expected",
	//#8
	"relational operator expected",
	//#9
	"parenthesis is not closed",
	//#10
	"some factor symbol is expected",
	//#11
	"Variable has not been previously declared",
	//12
	"Invalid relational opcode entered."
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
int code [3000];




int main()
{
    load();
    initEmit();
    program();
    closeEmit();
    printOut();
}

void printOut()
{
    printf("OP\t");
    printf("L\t");
    printf("M\n");
    int i =0;
    while(i < 12)
    {
        printf("%d\t%d\t%d\n",code[(i *3)], code[(i *3) + 1], code[(i *3) + 2]);
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

void program()
{
    getToken();
    block();
    if (token.sym != periodsym)
        getError(0);
    emit(9, 0, 2);

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
    printf("This token being read is %s\n", token.word);
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
        printf("\n\nThe token word is : %s\n\n", token.word);
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
    emit(4, 0, localSymbolHolder.addr);
    enter(var,localSymbolHolder.name,0,0,getTop());
    printf("\n\nThe TOS IS %d\n\n", getTop());
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
    printf("\n\nThe change spot and codespot  is: %d, %d\n\n", changeSpot, codeSpot);
    if(changeSpot != 0)
        code[(changeSpot * 3) + 2] =codeSpot;
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
    code[codeSpot * 3] = op;
    code[(codeSpot * 3) + 1] = l;
    code[(codeSpot * 3) + 2] = m;
    codeSpot ++;
    fprintf(emitWriter, "%d %d %d \n", op, l, m);
    printf("%d %d %d \n", op, l, m);
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
    printf("The name that is being inputed is : %s", tempSymbolHolder.name);
    if(isInTable(tempSymbolHolder.name) == 0)
    {
        symbol_table[symbolSpot] = tempSymbolHolder;
        symbolSpot ++;
        symbolTableSize ++;
        printf("The name that is being inputed is : %s\n", tempSymbolHolder.name);
        printf("The table size is being increased\n");
    }
    //update value if already in the table
    else
    {
        symbol_table[getSymbol(name)].val = val;
        printf(" It is working for symbol %s and The value of the symbol is : %d\n", symbol_table[getSymbol(name)].name, symbol_table[getSymbol(name)].val);
    }
}

int isInTable(char *name)
{
    int i =0;
    printf("\nThe counter is at : %d and the table size is at : %d\n", i, symbolTableSize);
    while(i <= symbolTableSize)
    {
        printf("The name of the parameter is : %s\n", name);
        printf("The name of the symbol name is %s\n", symbol_table[i].name);
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
    printf("the token name the value that is being fetched is : %s\n", name);
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
    return code[((codeSpot - 1) * 3) + 2];
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
