/*
 *  Copyright (c) 2020-2022 Thakee Nathees
 *  Copyright (c) 2021-2022 Pocketlang Contributors
 *  Distributed Under The MIT License
 */

#ifndef PK_AMALGAMATED
#include "libs.h"
#endif

// A DUMMY MODULE TO TEST NATIVE INTERFACE AND CLASSES.

typedef struct {
  double val;
} Dummy;

void* _newDummy(PKVM* vm) {
  Dummy* dummy = pkRealloc(vm, NULL, sizeof(Dummy));
  ASSERT(dummy != NULL, "pkRealloc failed.");
  dummy->val = 0;
  return dummy;
}

void _deleteDummy(PKVM* vm, void* ptr) {
  pkRealloc(vm, ptr, 0);
}

DEF(_dummyInit, "") {
  double val;
  if (!pkValidateSlotNumber(vm, 1, &val)) return;

  Dummy* self = (Dummy*) pkGetSelf(vm);
  self->val = val;
}

DEF(_dummyGetter, "") {
  const char* name = pkGetSlotString(vm, 1, NULL);
  Dummy* self = (Dummy*)pkGetSelf(vm);
  if (strcmp("val", name) == 0) {
    pkSetSlotNumber(vm, 0, self->val);
    return;
  }
}

DEF(_dummySetter, "") {
  const char* name = pkGetSlotString(vm, 1, NULL);
  Dummy* self = (Dummy*)pkGetSelf(vm);
  if (strcmp("val", name) == 0) {
    double val;
    if (!pkValidateSlotNumber(vm, 2, &val)) return;
    self->val = val;
    return;
  }
}

DEF(_dummyAdd, "") {
  Dummy* self = (Dummy*) pkGetSelf(vm);

  pkReserveSlots(vm, 4); // Now we have slots [0, 1, 2, 3].

  pkPlaceSelf(vm, 2); // slot[2] = self
  pkGetClass(vm, 2, 2); // slot[2] = Dummy class.

  // slots[1] = other.
  if (!pkValidateSlotInstanceOf(vm, 1, 2)) return;
  Dummy* other = (Dummy*) pkGetSlotNativeInstance(vm, 1);

  // slot[3] = self.val + other.val
  pkSetSlotNumber(vm, 3, self->val + other->val);

  // slot[0] = Dummy(slot[3]) => return value.
  if (!pkNewInstance(vm, 2, 0, 1, 3)) return;
}

DEF(_dummyEq, "") {

  // TODO: Currently there is no way of getting another native instance
  // So, it's impossible to check self == other. So for now checking with
  // number.
  double value;
  if (!pkValidateSlotNumber(vm, 1, &value)) return;

  Dummy* self = (Dummy*)pkGetSelf(vm);
  pkSetSlotBool(vm, 0, value == self->val);
}

DEF(_dummyGt, "") {

  // TODO: Currently there is no way of getting another native instance
  // So, it's impossible to check self == other. So for now checking with
  // number.
  double value;
  if (!pkValidateSlotNumber(vm, 1, &value)) return;

  Dummy* self = (Dummy*)pkGetSelf(vm);
  pkSetSlotBool(vm, 0, self->val > value);
}

DEF(_dummyMethod,
  "Dummy.a_method(n1:num, n2:num) -> num\n"
  "A dummy method to check dummy method calls. Will take 2 number arguments "
  "and return the multiplication.") {

  double n1, n2;
  if (!pkValidateSlotNumber(vm, 1, &n1)) return;
  if (!pkValidateSlotNumber(vm, 2, &n2)) return;
  pkSetSlotNumber(vm, 0, n1 * n2);

}

DEF(_dummyFunction,
  "dummy.afunc(s1:str, s2:str) -> str\n"
  "A dummy function the'll return s2 + s1.") {

  const char *s1, *s2;
  if (!pkValidateSlotString(vm, 1, &s1, NULL)) return;
  if (!pkValidateSlotString(vm, 2, &s2, NULL)) return;

  pkSetSlotStringFmt(vm, 0, "%s%s", s2, s1);
}

DEF(_dummyCallNative,
  "dummy.call_native(f:fn) -> void\n"
  "Calls the function 'f' with arguments 'foo', 42, false.") {
  if (!pkValidateSlotType(vm, 1, PK_CLOSURE)) return;

  pkReserveSlots(vm, 5); // Now we have slots [0, 1, 2, 3, 4].
  pkSetSlotString(vm, 2, "foo");
  pkSetSlotNumber(vm, 3, 42);
  pkSetSlotBool(vm, 4, false);

  // slot[0] = slot[1](slot[2], slot[3], slot[4])
  if (!pkCallFunction(vm, 1, 3, 2, 0)) return;
}

DEF(_dummyCallMethod,
  "dummy.call_method(o:Obj, method:str, a1, a2) -> \n"
  "Calls the method int the object [o] with two arguments [a1] and [a2].") {
  const char* method;
  if (!pkValidateSlotString(vm, 2, &method, NULL)) return;

  // slots = [null, o, method, a1, a2]

  if (!pkCallMethod(vm, 1, method, 2, 3, 0)) return;
}

/*****************************************************************************/
/* MODULE REGISTER                                                           */
/*****************************************************************************/

void registerModuleDummy(PKVM* vm) {

  PkHandle* dummy = pkNewModule(vm, "dummy");

  pkModuleAddFunction(vm, dummy, "afunc", _dummyFunction, 2);
  pkModuleAddFunction(vm, dummy, "call_native", _dummyCallNative, 1);
  pkModuleAddFunction(vm, dummy, "call_method", _dummyCallMethod, 4);

  PkHandle* cls_dummy = pkNewClass(vm, "Dummy", NULL, dummy,
                                   _newDummy, _deleteDummy);
  pkClassAddMethod(vm, cls_dummy, "_init",    _dummyInit,   1);
  pkClassAddMethod(vm, cls_dummy, "@getter",  _dummyGetter, 1);
  pkClassAddMethod(vm, cls_dummy, "@setter",  _dummySetter, 2);
  pkClassAddMethod(vm, cls_dummy, "+",        _dummyAdd,    1);
  pkClassAddMethod(vm, cls_dummy, "==",       _dummyEq,     1);
  pkClassAddMethod(vm, cls_dummy, ">",        _dummyGt,     1);
  pkClassAddMethod(vm, cls_dummy, "a_method", _dummyMethod, 2);
  pkReleaseHandle(vm, cls_dummy);

  pkRegisterModule(vm, dummy);
  pkReleaseHandle(vm, dummy);
}
