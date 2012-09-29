#include <assert.h>
#include <math.h>

#include "ejs-value.h"
#include "ejs-string.h"
#include "ejs-ops.h"

static EJSValue* _ejs_string_specop_get (EJSValue* obj, EJSValue* propertyName);
static EJSValue* _ejs_string_specop_get_own_property (EJSValue* obj, EJSValue* propertyName);
static EJSValue* _ejs_string_specop_get_property (EJSValue* obj, EJSValue* propertyName);
static void      _ejs_string_specop_put (EJSValue *obj, EJSValue* propertyName, EJSValue* val, EJSBool flag);
static EJSBool   _ejs_string_specop_can_put (EJSValue *obj, EJSValue* propertyName);
static EJSBool   _ejs_string_specop_has_property (EJSValue *obj, EJSValue* propertyName);
static EJSBool   _ejs_string_specop_delete (EJSValue *obj, EJSValue* propertyName, EJSBool flag);
static EJSValue* _ejs_string_specop_default_value (EJSValue *obj, const char *hint);
static void      _ejs_string_specop_define_own_property (EJSValue *obj, EJSValue* propertyName, EJSValue* propertyDescriptor, EJSBool flag);

extern EJSSpecOps _ejs_object_specops;

EJSSpecOps _ejs_string_specops = {
  "String",
  _ejs_string_specop_get,
  _ejs_string_specop_get_own_property,
  _ejs_string_specop_get_property,
  _ejs_string_specop_put,
  _ejs_string_specop_can_put,
  _ejs_string_specop_has_property,
  _ejs_string_specop_delete,
  _ejs_string_specop_default_value,
  _ejs_string_specop_define_own_property
};

EJSObject* _ejs_string_alloc_instance()
{
  return (EJSObject*)calloc(1, sizeof (EJSString));
}

EJSValue* _ejs_String;
static EJSValue*
_ejs_String_impl (EJSValue* env, EJSValue* _this, int argc, EJSValue **args)
{
  if (EJSVAL_IS_UNDEFINED(_this)) {
    if (argc > 0)
      return ToString(args[0]);
    else
      return _ejs_string_new_utf8("");
  }
  else {
    // called as a constructor
    _this->o.ops = &_ejs_string_specops;

    EJSString* str = (EJSString*)&_this->o;
    if (argc > 0) {
      str->primStr = ToString(args[0]);
    }
    else {
      str->primStr = _ejs_string_new_utf8("");
    }
    return _this;
  }
}

static EJSValue* _ejs_String_proto;
EJSValue*
_ejs_string_get_prototype()
{
  return _ejs_String_proto;
}

static EJSValue*
_ejs_String_prototype_toString (EJSValue* env, EJSValue* _this, int argc, EJSValue **args)
{
  EJSString *str = (EJSString*)_this;

  return _ejs_string_new_utf8 (EJSVAL_TO_STRING(str->primStr));
}

static EJSValue*
_ejs_String_prototype_charCodeAt (EJSValue* env, EJSValue* _this, int argc, EJSValue **args)
{
  EJSValue* primStr;

  if (EJSVAL_IS_STRING(_this)) {
    primStr = _this;
  }
  else {
    EJSString *str = (EJSString*)_this;
    primStr = str->primStr;
  }

  int idx = 0;
  if (argc > 0 && EJSVAL_IS_NUMBER(args[0])) {
    idx = (int)EJSVAL_TO_NUMBER(args[0]);
  }

  if (idx < 0 || idx >= EJSVAL_TO_STRLEN(primStr))
    return _ejs_nan;

  return _ejs_number_new (EJSVAL_TO_STRING(primStr)[idx]);
}

static EJSValue*
_ejs_String_prototype_split (EJSValue* env, EJSValue* _this, int argc, EJSValue **args)
{
  // for now let's just not split anything at all, return the original string as element0 of the array.

  EJSValue* rv = _ejs_array_new (1);
  _ejs_object_setprop (rv, _ejs_number_new (0), _this);
  return rv;
}

void
_ejs_string_init(EJSValue *global)
{
  _ejs_String = _ejs_function_new_utf8 (NULL, "String", (EJSClosureFunc)_ejs_String_impl);
  _ejs_String_proto = _ejs_object_new(NULL);

  _ejs_object_setprop_utf8 (_ejs_String,       "prototype",  _ejs_String_proto);

#define PROTO_METHOD(x) _ejs_object_setprop_utf8 (_ejs_String_proto, #x, _ejs_function_new_utf8 (NULL, #x, (EJSClosureFunc)_ejs_String_prototype_##x))

  PROTO_METHOD(charCodeAt);
  PROTO_METHOD(toString);
  PROTO_METHOD(split);

  _ejs_object_setprop_utf8 (global, "String", _ejs_String);
}

static EJSValue*
_ejs_string_specop_get (EJSValue* obj, EJSValue* propertyName)
{
  EJSString* estr = (EJSString*)&obj->o;

  // check if propertyName is an integer, or a string that we can convert to an int
  EJSBool is_index = FALSE;
  int idx = 0;
  if (EJSVAL_IS_NUMBER(propertyName)) {
    double n = EJSVAL_TO_NUMBER(propertyName);
    if (floor(n) == n) {
      idx = (int)n;
      is_index = TRUE;
    }
  }

  if (is_index) {
    if (idx < 0 || idx > EJSVAL_TO_STRLEN(estr->primStr))
      return _ejs_undefined;
    char c[2];
    c[1] = EJSVAL_TO_STRING(estr->primStr)[idx];
    c[2] = '\0';
    return _ejs_string_new_utf8 (c);
  }

  // we also handle the length getter here
  if (EJSVAL_IS_STRING(propertyName) && !strcmp ("length", EJSVAL_TO_STRING(propertyName))) {
    return _ejs_number_new (EJSVAL_TO_STRLEN(estr->primStr));
  }

  // otherwise we fallback to the object implementation
  return _ejs_object_specops.get (obj, propertyName);
}

static EJSValue*
_ejs_string_specop_get_own_property (EJSValue* obj, EJSValue* propertyName)
{
  return _ejs_object_specops.get_own_property (obj, propertyName);
}

static EJSValue*
_ejs_string_specop_get_property (EJSValue* obj, EJSValue* propertyName)
{
  return _ejs_object_specops.get_property (obj, propertyName);
}

static void
_ejs_string_specop_put (EJSValue *obj, EJSValue* propertyName, EJSValue* val, EJSBool flag)
{
  _ejs_object_specops.put (obj, propertyName, val, flag);
}

static EJSBool
_ejs_string_specop_can_put (EJSValue *obj, EJSValue* propertyName)
{
  return _ejs_object_specops.can_put (obj, propertyName);
}

static EJSBool
_ejs_string_specop_has_property (EJSValue *obj, EJSValue* propertyName)
{
  return _ejs_object_specops.has_property (obj, propertyName);
}

static EJSBool
_ejs_string_specop_delete (EJSValue *obj, EJSValue* propertyName, EJSBool flag)
{
  return _ejs_object_specops._delete (obj, propertyName, flag);
}

static EJSValue*
_ejs_string_specop_default_value (EJSValue *obj, const char *hint)
{
  return _ejs_object_specops.default_value (obj, hint);
}

static void
_ejs_string_specop_define_own_property (EJSValue *obj, EJSValue* propertyName, EJSValue* propertyDescriptor, EJSBool flag)
{
  _ejs_object_specops.define_own_property (obj, propertyName, propertyDescriptor, flag);
}