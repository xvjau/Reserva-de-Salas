/*
 *
 *  C++ Portable Types Library (PTypes)
 *  Version 2.1.0  Released 15-May-2007
 *
 *  Copyright (C) 2001-2007 Hovik Melikyan
 *
 *  http://www.melikyan.com/ptypes/
 *
 */

#include "ptypes.h"


PTYPES_BEGIN


exception::exception(const char* imsg)
    : message(imsg) 
{
}


exception::exception(const string& imsg) 
    : message(imsg) 
{
}


exception::~exception() 
{
}


PTYPES_END
