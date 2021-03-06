// State of a VM, and functions to allocate, deallocate, add a literal

// This one's the essential part of module 1.
// You'll define the key representation, `struct VMState`,
// and you'll use it in your `vmrun` function.

#ifndef VMSTATE_INCLUDED
#define VMSTATE_INCLUDED

#include <stdint.h>
#include <unordered_map>
#include "value.h"
#include "vtable.h"
#include "vmstack.h"

#define NUM_REGISTERS (1 << 12)
#define NUM_LITERALS (1 << 10)
#define HINT_NUM_GLOBALS 20
#define STACK_SIZE (1 << 11)

struct VMState
{
  struct VMFunction *fun;
  uint32_t pc;
  Value registers[NUM_REGISTERS];
  VTable_T globals;
  std::unordered_map<std::string, Value> new_globals;
  Value literals[NUM_LITERALS];
  uint32_t num_literals;
  Activation call_stack[STACK_SIZE];
  uint32_t stack_length;
};

VMStateP newstate();           // allocate and initialize (to empty)
void freestatep(VMStateP *sp); // deallocate

int literal_slot(VMStateP state, Value literal);
// return index of literal in `literals`, adding if needed
// (at need, can be postponed to module 2)

Value literal_value(VMStateP state, unsigned index);
// Return the value at the given index. *Not* intended
// for use in `vmrun`, in which you don't want to pay the
// overhead of a function call.

int literal_count(VMStateP state);
// Returns N, the number of index values for which it
// is ok to call `literal_value` (range 0 to N-1)

#endif /* VMSTATE_INCLUDED */
