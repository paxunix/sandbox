/*
	TSTRING.H

	Copyright (c) 1994  Shawn Halpenny
	All rights reserved.

	Header file for TString class.
	Will work with Windows or DOS without modification.
*/

#ifndef __TSTRING_H
#define __TSTRING_H

#ifndef __STREAM_H
#include "stream.h"
#endif

#ifndef ___DEFS_H
#include <_defs.h>
#endif

#ifndef __STDLIB_H
#include <stdlib.h>
#endif

_CLASSDEF(TString)
class TString
{
	int nLen;
	int nCase;	//if !0 for either operand, the comparison is case-sens
	char _FAR *pszStr;

	int Compare(const TString _FAR &) const;

public:
	TString(const char _FAR * = "", int = 0);
	TString(const TString _FAR &);
    #ifdef _Windows
	void* Alloc(size_t);
    void Dealloc(void *);
	#endif
	virtual ~TString();
	operator const char _FAR * () const { return pszStr; }
	void Case(int n) { nCase = n; }
	void Kill();
	int Len() const { return nLen; }
	TString _FAR & operator=(const TString _FAR &);
	int operator==(const TString _FAR &) const;
	int operator!=(const TString _FAR &) const;
	int operator<(const TString _FAR &) const;
	int operator>(const TString _FAR &) const;
	int operator<=(const TString _FAR &) const;
	int operator>=(const TString _FAR &) const;
	TString _FAR & operator+=(const TString _FAR &);
	TString operator+(const TString _FAR &) const;
	char _FAR & operator[](int) const;
	friend ofstream _FAR & operator<<(ofstream _FAR &, TString _FAR &);
};

#endif //__TSTRING_H
