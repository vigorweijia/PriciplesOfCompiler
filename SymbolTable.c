#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "SymbolTable.h"

void HashTableInit()
{
    for(size_t i = 0; i < HASH_TABLE_SIZE; i++)
    {
        gSymbolTable[i] = NULL;
    }

    gError = (Type)malloc(sizeof(Type_));
    gError->kind = K_ERROR;
}

int HashTableInsert(char* name, Type type)
{
    unsigned int hashVal;

    if(HashTableFind(name) != NULL)
    {
        return 0;
    }

    //printf("%s\n",name);

    FieldList symbol = (FieldList)malloc(sizeof(FieldList_));
    symbol->name = name;
    symbol->next = NULL;
    symbol->type = type;

    //printf("%s\n",name);
    

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

    //printf("%s\n",name);
    
    return 1;
}

Symbol HashTableFind(const char* name)
{
    if(name == NULL) return NULL;
    //printf("try to find: %s\n",name);
    unsigned int hashVal = HashPjw(name);
    Symbol symbol = gSymbolTable[hashVal];
    while (symbol != NULL)
    {
        if(strcmp(symbol->name, name) == 0)
        {
            return symbol;
        }
        if(symbol->type->kind == STRUCTURE || symbol->type->kind == FUNCTION)
        {
            Symbol childSymbol = HashTableFindMemberSymbol(name, symbol);
            if(childSymbol != NULL) 
            {
                return childSymbol;
            }
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
    Symbol field;
    if(symbol->type->kind == BASIC || symbol->type->kind == ARRAY) return NULL;
    else if(symbol->type->kind == STRUCTURE)
    {
        field = symbol->type->structure;
        while(field != NULL)
        {
            Symbol temp = HashTableFindMemberSymbol(name, field);
            if(temp != NULL) return temp;
            field = field->next;
        }
    }
    else if(symbol->type->kind == FUNCTION)
    {
        field = symbol->type->function.params;
        while(field != NULL)
        {
            Symbol temp = HashTableFindMemberSymbol(name, field);
            if(temp != NULL) return temp;
            field = field->next;
        }
    }
    else
    {
        assert(0);
    }
    return NULL;
}

Symbol HashTableFindStructureMember(const char* name, Type type)
{
    assert(type->kind == STRUCTURE);

    Symbol field = type->structure;

    while (field != NULL)
    {
        if(strcmp(field->name,name) == 0)
        {
            return field;
        }
        field = field->next;
    }
    return field;
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