/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 * vim: set ts=4 sw=4 et tw=99 ft=cpp:
 */
#ifndef _ejs_closureenv_h_
#define _ejs_closureenv_h_

#include "ejs-object.h"

struct _EJSClosureEnv {
    GCObjectHeader gc_header;
    uint32_t length; /* this will be followed by @length ejsvals */
    ejsval slots[1];
};

ejsval  _ejs_closureenv_new (ejsval length);
ejsval _ejs_closureenv_get_slot (ejsval env, ejsval slot);
ejsval* _ejs_closureenv_get_slot_ref (ejsval env, ejsval slot);

#endif /* _ejs_closureenv_h */