#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "SymbolTable.h"

void HashTableInit()
{
    for(size_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        gSymbolTable[i] = NULL;
    }
}

int HashTableInsert(char* name, Type type)
{
    unsigned int hashVal;

    if(HashTableFind(name) != NULL)
    {
        return 0;
    }

    Symbol symbol = (Symbol)malloc(sizeof(FieldList_));
    strcpy(symbol->name, name);
    symbol->next = NULL;
    symbol->type = type;

    hashVal = HashPjw(name);
    if(gSymbolTable[hashVal] != NULL)
    {
        symbol->next = gSymbolTable[hashVal];
        gSymbolTable[hashVal] = symbol;
    }
    else
    {
        gSymbolTable[hashVal] = symbol;
    }
    
    return 1;
}

Symbol HashTableFind(const char* name)
{
    unsigned int hashVal = HashPjw(name);
    Symbol symbol = gSymbolTable[hashVal];
    while (symbol != NULL)
    {
        if(strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
        Symbol childSymbol = HashTableFindMemberSymbol(name, symbol);
        if(childSymbol != NULL) 
        {
            return childSymbol;
        }
        symbol = symbol->next;
    }
    return NULL;
}

Symbol HashTableFindMemberSymbol(const char* name, Symbol symbol)
{
    if(strcmp(symbol->name, name) == 0)
    {
        return symbol;
    }
    if(symbol->type->kind == BASIC || symbol->type->kind == ARRAY) return NULL;
    if(symbol->type->kind == STRUCTURE)
    {

    }
    else if(symbol->type->kind == FUNCTION)
    {

    }
    else
    {
        
    }
    
}

unsigned int HashPjw(const char* name)
{
    unsigned int val = 0, x;
    size_t len = strlen(name);
    for(size_t i = 0; i < len; i++)
    {
        val = (val << 2) + name[i];
        x = val;
        if(x & ~0x3fff) val = (val ^ (x >> 12)) & 0x3fff;
    }
    return val;
}