/*
 * Copyright (c) 2007 - May 2, 2014:
 * Prasun Ratn <prasun@ncsu.edu>, Todd Gamblin <tgamblin@cs.unc.edu>,
 * Xing Wu <xwu3.ncsu.edu>
 * Copyright (c) of subsequent modifications of this code:
 * Frederic Suter, CNRS / IN2P3 Computing Center <fsuter@cc.in2p3.fr>
 * Henri Casanova, ICS Dept., University of Hawai`i at Manoa <henric@hawaii.edu>

 * The license of the original code is unknown. Modifications are made
 * under the terms of the GNU LGPL license.
 */

#include "PtrHandler.h"

#include <cstdlib>
#include <cassert>
#include <vector>

using namespace std;
//#include "utils.h"


template <class T>
class HandleMapper {
private:
  /// Vector of bulitins for the handle.  You must init this with add_builtin().
  /// This must be initialized so that the 0th element is the NULL handle 
  /// (e.g. MPI_COMM_NULL for MPI_Comm handles).
  vector<T> builtins;

  T *buffer;              /// Buffer full of user-defined handles we're remembering.
  size_t current;             /// current index in user-defined handle buffer.
  const int bufsize;      /// Size of user-defined handle buffer


  /// Looks up an MPI handlein the custom type buffer.
  /// Returns -1 if it is not found.
  int lookup_handle(const T& handle) {
    for (int i = 0; i < bufsize; i++) {
      if (buffer[i] == handle) {
        // return (i - current + bufsize) % bufsize;
        return i;
      }
    }
    return -1;
  }

  /// Searches for a builtin handle in the list of builtins.  
  /// Returns -1 if not found.
  int lookup_builtin(const T& handle) {
    for (size_t i=0; i < builtins.size(); i++) {
      if (builtins[i] == handle) return (int)i;
    }
    return -1;
  }

  /// Gets a builtin for the index if it's there, otherwise returns null handle.
  /// This requires that builtins be inited so that bulitins[0] is the null handle.
  T get_builtin_or_null(int index) {
    if (index >= 0 && index < (int)builtins.size()) {
      return builtins[index];
    } else {
      return builtins[0];  // this must be the null handle!
    }
  }

public:
  /// Allocates user-defined handle buffer and inits current index.
  HandleMapper(int size) : current(0), bufsize(size) { 
    buffer = new T[bufsize];
  }


  /// Destructor only needs to free internally allocated buffer.
  ~HandleMapper() {
    delete [] buffer;
  }

  //Method to fill the buffer with default null handle
  void clear_handle_buffer() {
    for(int i=0; i<bufsize; i++) {
      buffer[i] = builtins[0];
    }
  }

  int get_current(){
    return (int)current;
  }

  int get_bufsize(){
    return bufsize;
  }

  /// Maps an MPI handle to a unique int.  If the handle is not
  /// found, this will return -1.
  int handle_to_index(const T& handle) {
    int builtin  = lookup_builtin(handle);
    if (builtin >= 0) {
      return builtin;
    }

    int index = lookup_handle(handle);

    // user-defined handles map to ints >= bufsize
    return (index >= 0) ? index + bufsize : index;
  }

  /// Returns the MPI handle that maps to a particular unique
  /// integer.  Returns the appropriate MPI null handle if 
  /// nothing maps to that integer.
  T index_to_handle(int index) {
    T builtin = get_builtin_or_null(index);
    if (builtin != builtins[0]) {
      return builtin;
    }
    // return buffer[(current + index) % bufsize];
    return buffer[index % bufsize];
  }

  /// Adds a mapping for the MPI handle to this HandleMapper.
  void add_handle(const T& handle) {
    buffer[current++] = handle;
    if((int)current >= bufsize) {
      current = 0;
    }
  }

  /// Removes a mapping for the MPI handle from this HandleMapper.
  void remove_handle(const T& handle) {
    int i = lookup_handle(handle);
    if (i >= 0) {
      // buffer[(current + i) % bufsize] = builtins[0];  // set to null
      buffer[i % bufsize] = builtins[0];
    }
  }


  // --- Methods below affect the list of builtins. --- //

  /// Adds a builtin to the back of the builtin vector.
  void add_builtin(const T& builtin) {
    builtins.push_back(builtin);
  }

  size_t num_builtins() {
    return builtins.size();
  }
};


ptr_handler_t record_ptr;


/// Push all known datatypes onto the datatypes list.  This needs to be
/// done dynamically and not via a static initializer, as some MPI implementations
/// #define the basic datatypes as the result of a function, e.g. MPI_Type_f2c().
static void init_types(HandleMapper<MPI_Datatype>& types) {
  if (types.num_builtins()) return;  // only init once.

  /* ---------------------------------------------------- */
  /* MPI-1 datatypes.                                     */
  /* ---------------------------------------------------- */
  types.add_builtin(MPI_DATATYPE_NULL);

  /* Elementary datatypes (C) */
  types.add_builtin(MPI_CHAR);					// 1
  types.add_builtin(MPI_SHORT);					// 2
  types.add_builtin(MPI_INT);					// 3
  types.add_builtin(MPI_LONG);					// 4
  types.add_builtin(MPI_UNSIGNED_CHAR);			// 5
  types.add_builtin(MPI_UNSIGNED_SHORT);		// 6
  types.add_builtin(MPI_UNSIGNED);				// 7
  types.add_builtin(MPI_UNSIGNED_LONG);			// 8
  types.add_builtin(MPI_FLOAT);					// 9
  types.add_builtin(MPI_DOUBLE);				// 10
  types.add_builtin(MPI_LONG_DOUBLE);			// 11
  types.add_builtin(MPI_BYTE);					// 12
  types.add_builtin(MPI_PACKED);				// 13

  /* Elementary datatypes (Fortran) */ 
  types.add_builtin(MPI_INTEGER);				// 14
  types.add_builtin(MPI_REAL);					// 15
  types.add_builtin(MPI_DOUBLE_PRECISION);		// 16
  types.add_builtin(MPI_COMPLEX);				// 17
  types.add_builtin(MPI_DOUBLE_COMPLEX);		// 18
  types.add_builtin(MPI_LOGICAL);				// 19
#ifdef MPI_CHARACTER	// Added by Henri Casanova
  types.add_builtin(MPI_CHARACTER);				// 20
#endif
  /* MPI_BYTE   is mentioned here but covered by C. */
  /* MPI_PACKED is mentioned here but covered by C. */

  /* Datatypes for reduction functions (C) */
  types.add_builtin(MPI_FLOAT_INT);				// 21
  types.add_builtin(MPI_DOUBLE_INT);			// 22
  types.add_builtin(MPI_LONG_INT);				// 23
  types.add_builtin(MPI_2INT);					// 24
  types.add_builtin(MPI_SHORT_INT);				// 25
  types.add_builtin(MPI_LONG_DOUBLE_INT);		// 26

  /* Datatypes for reduction functions (Fortran) */
#ifdef MPI_2REAL	// Added by Henri Casanova
  types.add_builtin(MPI_2REAL);					// 27
#endif
  types.add_builtin(MPI_2DOUBLE_PRECISION);		// 28
#ifdef MPI_2INTEGER	// Added by Henri Casanova
  types.add_builtin(MPI_2INTEGER);				// 29
#endif
  // ...
  /* Optional datatypes (Fortran) */
/*
#ifdef MPI_INTEGER1
  types.add_builtin(MPI_INTEGER1);
#endif
#ifdef MPI_INTEGER2
  types.add_builtin(MPI_INTEGER2);
#endif
#ifdef MPI_INTEGER4
  types.add_builtin(MPI_INTEGER4);
#endif
#ifdef MPI_INTEGER8
  types.add_builtin(MPI_INTEGER8);  /* not mentioned in the standard */
#endif
#ifdef MPI_INTEGER16
  types.add_builtin(MPI_INTEGER16);  /* not mentioned in the standard */
#endif

#ifdef MPI_REAL2
  types.add_builtin(MPI_REAL2);
#endif
#ifdef MPI_REAL4
  types.add_builtin(MPI_REAL4);
#endif
#ifdef MPI_REAL8
  types.add_builtin(MPI_REAL8);
#endif
#ifdef MPI_REAL16
  types.add_builtin(MPI_REAL16);  /* not mentioned in the standard. */
#endif

#ifdef MPI_COMPLEX8
  types.add_builtin(MPI_COMPLEX8);  /* not mentioned in the standard */
#endif
#ifdef MPI_COMPLEX16
  types.add_builtin(MPI_COMPLEX16);  /* not mentioned in the standard */
#endif
#ifdef MPI_COMPLEX32
  types.add_builtin(MPI_COMPLEX32);  /* not mentioned in the standard */
#endif
*/

  /* Optional datatypes (C) */
#ifdef MPI_LONG_LONG_INT
  types.add_builtin(MPI_LONG_LONG_INT);
#endif
#ifdef MPI_LONG_LONG
  types.add_builtin(MPI_LONG_LONG);  /* Not really in MPI-1, but allowed as a synonym 
                                              for MPI_LONG_LONG_INT in most implementations. */
#endif

  /* Special datatypes for constructing derived datatypes */
  types.add_builtin(MPI_UB);
  types.add_builtin(MPI_LB);

  /* Old versions of OpenMPI had #defines but not definitions
     for these.  Should work with newer versions, but if you
     must compile with oldv ersions, you can define
     SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES at build time.       */
#ifndef SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES
#ifdef MPI_2COMPLEX
  types.add_builtin(MPI_2COMPLEX);
#endif
#ifdef MPI_2DOUBLE_COMPLEX
  types.add_builtin(MPI_2DOUBLE_COMPLEX);
#endif
#endif /* SCALATRACE_OMIT_OMPI_UNDEFINED_TYPES */

  /* ---------------------------------------------------- */
  /* MPI-2 datatypes.                                     */
  /* ---------------------------------------------------- */
#if (MPI_VERSION >= 2)
  /* Named Predefined Datatypes */
  types.add_builtin(MPI_WCHAR);

  /* Optional C and C++ (no fortran) Named Predefined Datatypes */
#ifdef MPI_UNSIGNED_LONG_LONG
  types.add_builtin(MPI_UNSIGNED_LONG_LONG);
#endif
#ifdef MPI_SIGNED_CHAR
  types.add_builtin(MPI_SIGNED_CHAR);
#endif
#endif /* (MPI_VERSION >= 2) */
}


/// Push all known ops onto the ops list.  This is done similarly
/// to the init_datatypes() routine above.
static void init_ops(HandleMapper<MPI_Op>& ops) {
  if (ops.num_builtins()) return;  // only init once.

  /* ---------------------------------------------------- */
  /* MPI-1 Ops.                                           */
  /* ---------------------------------------------------- */
  ops.add_builtin(MPI_OP_NULL);

  ops.add_builtin(MPI_MAX);
  ops.add_builtin(MPI_MIN);
  ops.add_builtin(MPI_SUM);
  ops.add_builtin(MPI_PROD);
  ops.add_builtin(MPI_MAXLOC);
  ops.add_builtin(MPI_MINLOC);
  ops.add_builtin(MPI_BAND);
  ops.add_builtin(MPI_BOR);
  ops.add_builtin(MPI_BXOR);
  ops.add_builtin(MPI_LAND);
  ops.add_builtin(MPI_LOR);
  ops.add_builtin(MPI_LXOR);

  /* ---------------------------------------------------- */
  /* MPI-1 Ops.                                           */
  /* ---------------------------------------------------- */
#if (MPI_VERSION >= 2)
  ops.add_builtin(MPI_REPLACE);
#endif // (MPI_VERSION >= 2)
}


/// Put the builtin, known comms into the comms list.
static void init_comms(HandleMapper<MPI_Comm>& comms) {
  if (comms.num_builtins()) return;   // only init once 

  comms.add_builtin(MPI_COMM_NULL);
  comms.add_builtin(MPI_COMM_SELF);
  comms.add_builtin(MPI_COMM_WORLD);
}


/// No special groups right now (aside from null group).
static void init_groups(HandleMapper<MPI_Group>& groups) {
  if (groups.num_builtins()) return;   // only init once 
  groups.add_builtin(MPI_GROUP_NULL);
}

/* i/o tracing */
static void init_file_handles(HandleMapper<MPI_File>& files) {
  if(files.num_builtins()) return;
  files.add_builtin(0);
}


// Struct to hold one each of the datatype handlers we need.
struct ptr_handler {
  HandleMapper<MPI_Datatype> types;
  HandleMapper<MPI_Comm>     comms;
  HandleMapper<MPI_Group>    groups;
  HandleMapper<MPI_Op>       ops;
  HandleMapper<MPI_File>     files;

  ptr_handler(int bufsize = PTR_HANDLER_SZ) 
  : types(bufsize), comms(bufsize), groups(bufsize), ops(bufsize), files(bufsize) {

    // init the lookup tables for builtins
    init_types(types);
    types.clear_handle_buffer();
    init_comms(comms);
    comms.clear_handle_buffer();
    init_groups(groups);
    groups.clear_handle_buffer();
    init_ops(ops);
    ops.clear_handle_buffer();    
    init_file_handles(files);
    files.clear_handle_buffer();
  } 
};


ptr_handler_t init_ptr(int size) {
  return new ptr_handler(size);
}


void finalize_ptr(ptr_handler_t ptr) {
  assert(ptr);
  delete ptr;
}


// --- Below are C bindings for internal generic calls --- //

int type_to_index(ptr_handler_t ptr, MPI_Datatype type) {
  return ptr->types.handle_to_index(type);
}
int comm_to_index(ptr_handler_t ptr, MPI_Comm comm) {
  return ptr->comms.handle_to_index(comm);
}

int comm_current(ptr_handler_t ptr){
  return ptr->comms.get_current();
}
int comm_bufsize(ptr_handler_t ptr){
  return ptr->comms.get_bufsize();
}
/*
int comm_abs_index(ptr_handler_t ptr, int index){
	if(index >= ptr->comms.get_bufsize() )
		return (index - ptr->comms.get_bufsize() + ptr->comms.get_current()) % ptr->comms.get_bufsize() + ptr->comms.get_bufsize();
	else
		return index;
}
 */
int group_to_index(ptr_handler_t ptr, MPI_Group group) {
  return ptr->groups.handle_to_index(group);
}
int op_to_index(ptr_handler_t ptr, MPI_Op op) {
  return ptr->ops.handle_to_index(op);
}

MPI_Datatype index_to_type(ptr_handler_t ptr, int index) {
  return ptr->types.index_to_handle(index);
}
MPI_Comm index_to_comm(ptr_handler_t ptr, int index) {
  return ptr->comms.index_to_handle(index);
}
MPI_Group index_to_group(ptr_handler_t ptr, int index) {
  return ptr->groups.index_to_handle(index);
}
MPI_Op index_to_op (ptr_handler_t ptr, int index) {
  return ptr->ops.index_to_handle(index);
}

void add_type_entry(ptr_handler_t ptr, MPI_Datatype type) {
  ptr->types.add_handle(type);
}
void add_comm_entry(ptr_handler_t ptr, MPI_Comm comm) {
  ptr->comms.add_handle(comm);
}

void add_group_entry(ptr_handler_t ptr, MPI_Group group) {
  ptr->groups.add_handle(group);
}
void add_op_entry(ptr_handler_t ptr, MPI_Op op) {
  ptr->ops.add_handle(op);
}

void remove_type_entry(ptr_handler_t ptr, MPI_Datatype type) {
  ptr->types.remove_handle(type);
}
void remove_comm_entry(ptr_handler_t ptr, MPI_Comm comm) {
  ptr->comms.remove_handle(comm);
}
void remove_group_entry(ptr_handler_t ptr, MPI_Group group) {
  ptr->groups.remove_handle(group);
}
void remove_op_entry(ptr_handler_t ptr, MPI_Op op) {
  ptr->ops.remove_handle(op);
}


/* i/o tracing*/
int file_handle_to_index(ptr_handler_t ptr, MPI_File file) {
  return ptr->files.handle_to_index(file);
}

MPI_File index_to_file_handle(ptr_handler_t ptr, int index) {
  return ptr->files.index_to_handle(index);
}

void add_file_handle_entry(ptr_handler_t ptr, MPI_File file) {
  ptr->files.add_handle(file);
}

void remove_file_handle_entry(ptr_handler_t ptr, MPI_File file) {
  ptr->files.remove_handle(file);
}

/* vim: set tw=80 sw=2 expandtab: */
