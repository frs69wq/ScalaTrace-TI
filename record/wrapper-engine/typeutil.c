
#include "wrapper.h"
#include "y.tab.h"

const char *NameForType(const type_t *type) {
    /* TODO: This covers all the types listed in parser.y, but it's not exhaustive. */
    switch (type->type) {
    case T_CHAR:   
        return "char";
    case T_DOUBLE: 
        return "double";
    case T_INT:    
        return "int";
    case T_STRING: 
        return "char*";
    case T_VOID: 
        return "void";
    }
    return type->name;
}


