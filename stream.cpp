/*
	STREAM.CPP
	Copyright (c) 1995  Shawn Halpenny
	All rights reserved.

    Code for STREAM.H classes.
*/

#include <stdio.h>
#include <ctype.h>
#include "stream.h"

ifstream::ifstream()
{
	pf = NULL;
	pBuf = NULL;
}	//ifstream::ifstream

ifstream::~ifstream()
{
    close();
}	//ifstream::~ifstream

ifstream& ws(ifstream& is)
{
	int c;

	if (!is.good())
		return is;

	do {
		c = fgetc(is.pf);
    } while (isspace(c));

	ungetc(c, is.pf);
	return is;
}	//ws

int ifstream::good()
{
	if (!pf || ferror(pf) || feof(pf))
		return 0;

	return -1;
}	//ifstream::good

void ifstream::open(char *pszFile, int nBufSize)
{
	if (pf)
		return;

	pf = fopen(pszFile, "rt");
	if (pf)
	{
    	pBuf = new char[nBufSize]; 
    	setvbuf(pf, pBuf, _IOFBF, nBufSize);
    }
}	//ifstream::open

void ifstream::close()
{
	if (pf)
	{
		fclose(pf);
        pf = NULL;
		delete [] pBuf;
    }
}	//ifstream::close

int ifstream::operator!()
{
	return !good();
}	//ifstream::operator!

ifstream& ifstream::operator>>(ifstream& (*f)(ifstream&))
{
	if (!good())
		return *this;

	return (f)(*this);
}	//ifstream::operator>>

ifstream& ifstream::operator>>(char *p)
{
    int c;
	char *cur = p;

	if (!good())
		return *this;

    ws(*this);
	while (!isspace((c = fgetc(pf))))
	{
		*cur = c;
		cur++;
	}

	*cur = 0;
	ungetc(c, pf);
    return *this;
}	//ifstream::operator>>

ifstream& ifstream::operator>>(char& ch)
{
	if (!good())
		return *this;

	ws(*this);
	ch = fgetc(pf);
	return *this;
}	//ifstream::operator>>

ifstream& ifstream::operator>>(int& n)
{
	int c, nSign = 1;

	if (!good())
		return *this;

	n = 0;
	ws(*this);
	c = fgetc(pf);
	if (c == '-')
		nSign = -1;
	else
	if (c == '+')
    	nSign = 1;
	else
		ungetc(c, pf);

	while (isdigit((c = fgetc(pf))))
		n = n * 10 + c - '0';

	n *= nSign;
	ungetc(c, pf);
	return *this;
}	//ifstream::operator>>

ifstream& ifstream::operator>>(unsigned int& n)
{
	int c;

	if (!good())
		return *this;

	n = 0;
	ws(*this);
	while (isdigit((c = fgetc(pf))))
		n = n * 10 + c - '0';

	ungetc(c, pf);
	return *this;
}	//ifstream::operator>>

ifstream& ifstream::operator>>(long& n)
{
	int c;
	long nSign = 1;

	if (!good())
		return *this;

	n = 0;
	ws(*this);
	c = fgetc(pf);
	if (c == '-')
		nSign = -1;
	else
	if (c == '+')
    	nSign = 1;
	else
		ungetc(c, pf);

	while (isdigit((c = fgetc(pf))))
		n = n * 10 + c - '0';

	n *= nSign;
	ungetc(c, pf);
	return *this;
}	//ifstream::operator>>

ifstream& ifstream::operator>>(unsigned long& n)
{
	int c;

	if (!good())
		return *this;

	n = 0;
	ws(*this);
	while (isdigit((c = fgetc(pf))))
		n = n * 10 + c - '0';

	ungetc(c, pf);
	return *this;
}	//ifstream::operator>>

ifstream& ifstream::ignore(int nNum, int ch)
{
	if (!good())
		return *this;

	while (nNum > 0)
	{
        nNum--;
        if (fgetc(pf) == ch)
			break;
	};

    return *this;
}	//ifstream::ignore

int ifstream::peek()
{
	int c;

	if (!good())
		return -1;

	c = fgetc(pf);
    ungetc(c, pf);

    return c;
}	//ifstream::peek

/*
	n includes space for the terminating null.
*/
ifstream& ifstream::getline(char *p, int n, char ch)
{
	int c = ch + 1;

	if (!good())
		return *this;

	while (c != ch && n > 1)
    {
		*p = c = fgetc(pf);
		if (c == '\n')
		{
			*p = c = '\r';	//Windoze edit controls are stupid and need a '\r' to break lines
			p++;
			*p = c = '\n';
			n--;
		}

		p++;
		n--;
	}

	if (*(p-1) == ch)
		*(p-1) = 0;
    else
	    *p = 0;

	return *this;
}	//ifstream::getline

ofstream::ofstream()
{
	pf = NULL;
	pBuf = NULL;
}	//ofstream::ofstream

ofstream::~ofstream()
{
	close();
}	//ofstream::~ofstream

int ofstream::good()
{
	if (!pf || ferror(pf))
		return 0;

	return -1;
}	//ofstream::good

/* nHow is _IOFBF, _IOLBF, or _IONBF as def'd in stdio.h */
void ofstream::open(char *pszFile, int nHow, int nBufSize)
{
	if (pf)
		return;

    pf = fopen(pszFile, "wt");
	if (pf)
	{
    	pBuf = new char[nBufSize]; 
    	setvbuf(pf, pBuf, nHow, nBufSize);
    }
}	//ofstream::open

void ofstream::close()
{
	if (pf)
	{
		fclose(pf);
		pf = NULL;
		delete [] pBuf;
    }
}	//ofstream::close

int ofstream::operator!()
{
    return !good();
}	//ofstream::operator!

ofstream& ofstream::operator<<(ofstream& (*f)(ofstream &))
{
	if (!good())
		return *this;

	return (f)(*this);
}	//ofstream::operator<<

ofstream& ofstream::operator<<(char c)
{
	if (!good())
		return *this;

	fputc(c, pf);
    return *this;
}	//ofstream::operator<<

ofstream& ofstream::operator<<(long n)
{
	if (!good())
		return *this;

	fprintf(pf, "%ld", n);
	return *this;
}	//ofstream::operator<<

ofstream& ofstream::operator<<(unsigned long n)
{
	if (!good())
		return *this;

	fprintf(pf, "%lu", n);
	return *this;
}	//ofstream::operator<<

ofstream& ofstream::operator<<(const char *p)
{
    const char *pE = p;
	int n = 0;

	if (!good())
		return *this;

	while (*pE)
	{
        n++;
		pE++;
    }

	fwrite(p, 1, n, pf);
	return *this;
}
