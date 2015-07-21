#ifndef MODULE_1_H_INCLUDED
#define MODULE_1_H_INCLUDED

/*add prototypes here*/
void load();
void getCode();//changes pc
void fetch ();
void getInstruction();
int isDone ();
void execute();
void initArray();
int getBase(int level, int base);//tested
void testFunction();//testing purposes
void lit (int pushValue);
void lod (int levels, int offset);
void sto (int levels, int offset);
void cal (int levels, int pro_Location);//changes pc
void inc (int space);
void jmp (int jumpSpot);//changes pc
void jpc (int jumpSpot);//changes pc
void sio1();
void sio2();
void sio3();
void ret ();//changes pc
void neg ();
void add ();
void sub ();
void mul ();
void divid();
void odd();
void mod ();
void eql ();
void neq ();
void lss ();
void leq ();
void gtr ();
void geq ();
void opr();
void sio();
char *getOpString();
void format2();
void stackPrint();
void format ();
void errorCheck();
void  cleanArrayList();
void load1();
int isSymbol2(int x);
void printCleanInput();
int isEnd();
void skipComment();
void printTest();
void project1();
void printOutMcode();
void setFlags();
void load3();
void project3();
int isLetter(char letter);
void putSymbolToken(char *string, int symbol);
void putReserveToken(char *string, int symbol);
void putNumToken(char *string);
void putIdentifierToken(char *string);
int symbolSwitch(char *string);
int isNumber(char *string);
int wordSwitch(char *string);
void findToken();
void printLexemeTable();
int isNextGts();
int isNextEqual();
void load();
void program2();
void block();
void statement(int varCount);
int condition();
void printOutLexemeTable();
void printOutLexemeList();
void done();
void printOutStack();
int isRelation();
int writeRelation(int opSym);
void getToken();
void getError();
void expression();
void term();
void printLexemeOutput();
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
#endif /* MODULE_1_H_INCLUDED*/
