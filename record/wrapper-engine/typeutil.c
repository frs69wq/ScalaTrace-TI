/*
 * Copyright (c) 2002-May 2, 2014: Bronis de Supinski (bronis@llnl.gov)
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

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


