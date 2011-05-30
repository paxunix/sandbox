/*
	TSTRING.CPP

	Copyright (c) 1994  Shawn Halpenny
	All rights reserved.

	Code for TString class declared in TSTRING.H.
*/

#ifdef _Windows
	#include <windows.h>
#endif

#ifndef _Windows
	#include <string.h>
	#define lstrcmp strcmp
	#define lstrcmpi stricmp
	#define lstrcpy strcpy
	#define lstrlen strlen
    #define
#endif

#include <stdlib.h>
#include "stream.h"
#include "tstring.h"

#ifdef _Windows
void* TString::Alloc(size_t u)
{
	HGLOBAL h = GlobalAlloc(GMEM_MOVEABLE | GMEM_DISCARDABLE, u);

	if (!h)
	{
		MessageBox(NULL, "Not enough memory to allocate string.\n"
						 "This application will try to terminate...", "Oops", MB_ICONSTOP | MB_OK);
        SendMessage(GetActiveWindow(), WM_CLOSE, 0, 0);
    	return NULL;
	}

	return GlobalLock(h);
}	//TString::Alloc

void TString::Dealloc(void *p)
{
	if (GlobalFree(GlobalUnlock(LOWORD(GlobalHandle(SELECTOROF(p))))))
	{
		MessageBox(NULL, "Can't free string memory.\n"
						 "This application will try to terminate...", "Oops", MB_ICONSTOP | MB_OK);
		SendMessage(GetActiveWindow(), WM_CLOSE, 0, 0);
	}
}	//TString::Dealloc
#endif

//Case sensitive if nCase !0
TString::TString(char const _FAR *p, int n)
{
	nCase = n;

	if (!p)
	{
		pszStr = "";
		nLen = 0;
	}
	else
	{
		nLen = lstrlen(p);

		if (nLen)
		{
			pszStr = (char *) Alloc(nLen + 1);
			lstrcpy(pszStr, p);
		}
		else
			pszStr = "";
	}
}	//TString::TString

TString::TString(const TString _FAR & src)
{
	nCase = src.nCase;
	nLen = src.nLen;

	if (nLen)
	{
		pszStr = (char *) Alloc(nLen + 1);

		lstrcpy(pszStr, src.pszStr);
	}
	else
		pszStr = "";
}	//TString::TString

TString::~TString()
{
	if (nLen)
		Dealloc(pszStr);
}	//TString::~TString

/*
	Compares the two strings.
*/
int TString::Compare(const TString _FAR & right) const
{
	if (nCase || right.nCase)
		return lstrcmp(pszStr, right.pszStr);
	else
		return lstrcmpi(pszStr, right.pszStr);
}	//TString::Compare

/*
	Returns !0 if they're not equal, 0 if they are.
*/
inline int TString::operator!=(const TString _FAR & right) const
{
	return Compare(right) != 0;
}	//TString::operator!=

/*
	Sets length to 0, case-insensitive, and TString to empty (not NULL),
	(frees memory associated with string).
*/
void TString::Kill()
{
	if (nLen)
		Dealloc(pszStr);

	pszStr = "";
	nCase = 0;
	nLen = 0;
}	//TString::Kill

TString _FAR & TString::operator=(const TString _FAR & src)
{
	if (*this != src)
	{
		if (nLen != src.nLen)
		{
			if (nLen)
				Dealloc(pszStr);

			nLen = src.nLen;
			pszStr = (char *) Alloc(nLen + 1);
		}

		if (nLen)
			lstrcpy(pszStr, src.pszStr);
	}

	return *this;
}	//TString::operator=

/*
	Returns !0 if they're equal, 0 if they're not.
*/
inline int TString::operator==(const TString _FAR & right) const
{
	return Compare(right) == 0;
}	//TString::operator==

/*
	Returns !0 if this < right, 0 if this >= right.
*/
inline int TString::operator<(const TString _FAR & right) const
{
	return Compare(right) < 0;
}	//TString::operator<

/*
	Returns !0 if this > right, 0 if this <= right.
*/
inline int TString::operator>(const TString _FAR & right) const
{
	return Compare(right) > 0;
}	//TString::operator>

/*
	Returns !0 if this <= right, 0 if this > right.
*/
inline int TString::operator<=(const TString _FAR & right) const
{
	return Compare(right) <= 0;
}	//TString::operator<=

/*
	Returns !0 if this >= right, 0 if this < right.
*/
inline int TString::operator>=(const TString _FAR & right) const
{
	return Compare(right) >= 0;
}	//TString::operator>=

/*
	Returns this, which has right concatenated to its end.
*/
TString _FAR & TString::operator+=(const TString _FAR & right)
{
	if (right.nLen)
	{
		char _FAR *p;
		int nOldLen = nLen;

		nLen += right.nLen;
		p = (char *) Alloc(nLen + 1);

		lstrcpy(p, pszStr);
		lstrcat(p, right.pszStr);

		if (nOldLen)
			Dealloc(pszStr);

		pszStr = p;
	}

	return *this;
}	//TString::operator+=

TString TString::operator+(const TString _FAR & right) const
{
	TString tmp(*this);

	tmp += right;
	return tmp;
}	//TString::operator+

//ch = str[i] or str[i] = ch.  Returns the terminating NULL if past end
//of TString.
char _FAR & TString::operator[](int i) const
{
	if (i > nLen)
		return pszStr[nLen];

	return pszStr[i];
}	//TString::operator[]

ofstream _FAR & operator<<(ofstream _FAR & os, TString _FAR & Str)
{
	os << Str.pszStr;
	return os;
}	//operator<<
