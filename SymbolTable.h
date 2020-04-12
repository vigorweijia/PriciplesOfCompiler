#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#define HASH_TABLE_SIZE 0x3fff
#define NAME_LENTH 128

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct FieldList_* Symbol;

typedef struct Type_
{
    enum {UNKNOWN, BASIC, ARRAY, STRUCTURE, FUNCTION, K_ERROR} kind;
    union{
        enum {B_UNKNOWN, B_INT, B_FLOAT} basic;
        struct {Type elem; int size;} array;
        FieldList structure;
        struct {Type rtnType; FieldList params; int cnt;} function;
    };
}Type_;

Type gError;

typedef struct FieldList_
{
    char* name;
    Type type;
    FieldList next;
}FieldList_;

typedef enum{O_UNKNOWN,O_ExtDecList,O_StructSpecifier,O_FunDec,O_CompSt}Origin;

Symbol gSymbolTable[HASH_TABLE_SIZE];

void HashTableInit();
int HashTableInsert(char* name, Type type);
Symbol HashTableFind(const char* name);
Symbol HashTableFindMemberSymbol(const char* name, Symbol symbol);
Symbol HashTableFindStructureMember(const char* name, Type type);

unsigned int HashPjw(const char* name);

#endif
