// Representation of all VM values.

// This is the core of the VM, used heavily from module 1 on.

#ifndef VALUE_INCLUDED
#define VALUE_INCLUDED

#include <stdint.h>
#include <stdbool.h>

#include "iformat.h"
#include "gcmeta.h"

//// Each value is identified by one of the following tags ////

typedef enum
{
  Nil = 0, // vScheme value not found in uScheme
           // (it's the initial value of an otherwise
           // undefined variable)

  Boolean, // vScheme S-expressions
  Number,
  String,
  Emptylist,
  ConsCell, // represented as Block (for now)

  VMFunction, // first-order function (sequence of instructions)
  VMClosure,  // higher-order function

  ///// stretch goals for more interesting languages

  Block, // block of values of fixed size: record, etc
  Seq,   // variable-size sequence in the style of Hanson/Icon/CLU
  Table, // hash table

  ///// potential support for writing primitives/libraries in C

  CFunction,
  LightUserdata, // pointer to data on the C heap
} VTag;

extern const char *tagnames[13]; // map VTag to string

////////////////  representation of values

typedef double Number_T; // representation of a VM number
struct Value;
extern bool eqvalue(Value v1, Value v2); // vscheme = primitive

typedef struct Value
{
  VTag tag;
  union
  { // payloads for everything except Nil and EmptyList
    bool b;
    Number_T n;
    struct VMString *s;
    struct VMBlock *block; // also ConsCell
    struct VMFunction *f;
    struct VMClosure *hof;
    struct VSeq_T *seq;
    struct VTable_T_ *table;
    struct CFunction *cf;
    void *p; // light userdata == external memory
  };
  bool operator==(const Value &other) const
  {
    return eqvalue(*this, other);
  }
} Value; // Note!  Not a pointer type!

////////////////////////////////////////////////////////////////

//////   creation/introduction for values

extern struct Value nilValue;       // initialized statically
extern struct Value emptylistValue; // initialized statically

// remaining creators are embedding functions

static inline Value mkNumberValue(Number_T n);
static inline Value mkStringValue(struct VMString *s);
static inline Value mkBooleanValue(bool b);
static inline Value mkVMFunctionValue(struct VMFunction *f);
static inline Value mkClosureValue(struct VMClosure *cl);
static inline Value mkBlockValue(struct VMBlock *bl);
static inline Value mkConsValue(struct VMBlock *bl);
static inline Value mkTableValue(struct VTable_T_ *table);

////// observation/elimination for values

// projection functions (to be called via macros below)

typedef struct VMState *VMStateP; // for stack trace if projection fails

static inline struct VMBlock *asBlock_(VMStateP, Value, const char *file, int line);
static inline struct VMBlock *asCons_(VMStateP, Value, const char *file, int line);
static inline const char *asCString_(VMStateP, Value, const char *file, int line);
static inline Number_T asNumber_(VMStateP, Value, const char *file, int line);
static inline struct VMClosure *asClosure_(VMStateP, Value, const char *file, int line);
static inline struct VMFunction *asVMFunction_(VMStateP, Value, const char *file, int line);
static inline struct VMString *asVMString_(VMStateP, Value, const char *file, int line);
static inline bool asBool_(VMStateP, Value, const char *file, int line);

#define AS_BLOCK(VM, V) asBlock_((VM), (V), __FILE__, __LINE__)
#define AS_CONS_CELL(VM, V) asCons_((VM), (V), __FILE__, __LINE__)
#define AS_CSTRING(VM, V) asCString_((VM), (V), __FILE__, __LINE__)
#define AS_NUMBER(VM, V) asNumber_((VM), (V), __FILE__, __LINE__)
#define AS_CLOSURE(VM, V) asClosure_((VM), (V), __FILE__, __LINE__)
#define AS_VMFUNCTION(VM, V) asVMFunction_((VM), (V), __FILE__, __LINE__)
#define AS_VMSTRING(VM, V) asVMString_((VM), (V), __FILE__, __LINE__)
#define AS_BOOLEAN(VM, V) asBool_((VM), (V), __FILE__, __LINE__)
// additional observers for values

extern uint32_t hashvalue(Value v);
extern bool identical(Value v1, Value v2); // for hashing
extern bool eqtests(Value v1, Value v2);   // for check-expect

////////////////////////////////////////////////////////////////
//
// representations

// when you're read to do cons,car,cdr (from module 1 onward):

struct VMBlock
{
  GCMETA(VMBlock)
  int nslots;
  struct Value slots[1];
};

// when you implement first-order functions (module 7 and onward):

struct VMFunction
{
  GCMETA(VMFunction)
  // Note: as yet, lacks GC support
  int arity; // number of args expected
  int nregs; // one more than the number of highest register read or written
  int size;  // number of instructions
  Instruction instructions[1];
};

// when you implement higher-order functions (module 10 and onward):

struct VMClosure
{
  GCMETA(VMClosure)
  struct VMFunction *f;
  int nslots;
  struct Value captured[1];
};

// stretch goals:

typedef int (*CFunction_Code)(VMStateP state, void *env, int actuals);
// result == number of things returned

struct CFunction
{ // a closure
  GCMETA(CFunction)
  void *env; // environment
  CFunction_Code code;
  int arity;        // number of arguments expected
  int nregs;        // one more than the number of highest register read or written
  const char *name; // for diagnostics
};

////////////////////////////////////////////////////////////////
// no user-serviceable parts past this point

#include <stdnoreturn.h>

#include "vmstring.h"

extern void typeerror(VMStateP state, const char *expected, Value got,
                      const char *file, int line);

static inline struct VMBlock *asBlock_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != Block)
    typeerror(vm, "a block", v, file, line);
  return GCVALIDATE(v.block);
}
static inline struct VMBlock *asCons_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != ConsCell)
    typeerror(vm, "a cons cell", v, file, line);
  return GCVALIDATE(v.block);
}
static inline const char *asCString_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != String)
    typeerror(vm, "a string", v, file, line);

  return GCVALIDATE(v.s)->bytes;
}
static inline Number_T asNumber_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != Number)
    typeerror(vm, "a number", v, file, line);
  return v.n;
}
static inline struct VMClosure *asClosure_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != VMClosure)
    typeerror(vm, "a closure", v, file, line);
  return GCVALIDATE(v.hof);
}
static inline struct VMFunction *asVMFunction_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != VMFunction)
    typeerror(vm, "a VM function", v, file, line);
  return GCVALIDATE(v.f);
}
static inline struct VMString *asVMString_(VMStateP vm, Value v, const char *file, int line)
{
  if (v.tag != String)
    typeerror(vm, "a string", v, file, line);
  return GCVALIDATE(v.s);
}

static inline bool asBool_(VMStateP vm, Value v, const char *file, int line)
{
  switch (v.tag)
  {
  case Boolean:
  {
    return v.b;
    break;
  }
  case Number:
  {
    return (v.n != 0);
    break;
  }
  case String:
  {
    return (v.s->length > 0);
    break;
  }
  case Emptylist:
  {
    return false;
    break;
  }
  case ConsCell:
  {
    return true;
    break;
  }
  default:
    typeerror(vm, "a boolean interpretable value", v, file, line);
  }
  return false;
}

////////////////////////////////////////////////////////////////

static inline Value mkNumberValue(Number_T n)
{
  Value val;
  val.tag = Number;
  val.n = n;
  return val;
}

static inline Value mkStringValue(struct VMString *s)
{
  Value val;
  val.tag = String;
  val.s = s;
  return val;
}

static inline Value mkBooleanValue(bool b)
{
  Value val;
  val.tag = Boolean;
  val.b = b;
  return val;
}

static inline Value mkVMFunctionValue(struct VMFunction *f)
{
  Value val;
  val.tag = VMFunction;
  val.f = f;
  return val;
}

static inline Value mkClosureValue(struct VMClosure *cl)
{
  Value val;
  val.tag = VMClosure;
  val.hof = cl;
  return val;
}

static inline Value mkBlockValue(struct VMBlock *bl)
{
  Value val;
  val.tag = Block;
  val.block = bl;
  return val;
}

static inline Value mkConsValue(struct VMBlock *bl)
{
  Value val;
  val.tag = ConsCell;
  val.block = bl;
  return val;
}

static inline Value mkTableValue(struct VTable_T_ *t)
{
  Value val;
  val.tag = Table;
  val.table = t;
  return val;
}

#endif
