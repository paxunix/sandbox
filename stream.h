/*
	STREAM.H
	Copyright (c) 1995  Shawn Halpenny
	All rights reserved.

    Input and output stream wrapper classes.  Tailored especially for Sandbox.
*/

#ifndef __STREAM_H
#define __STREAM_H

#ifndef __STDIO_H
#include <stdio.h>
#endif

class ifstream
{
	FILE *pf;
	char *pBuf;

public:
	ifstream();
	~ifstream();
	void close();
	void open(char *, int=16384);
	int operator!();
	friend ifstream& ws(ifstream&);
	ifstream& operator>>(ifstream& (*f)(ifstream&));
	ifstream& operator>>(char*);
	ifstream& operator>>(char&);
	ifstream& operator>>(int&);
	ifstream& operator>>(unsigned int&);
	ifstream& operator>>(long&);
	ifstream& operator>>(unsigned long&);
    ifstream& ignore(int=1, int=EOF);
	ifstream& putback(char c) { ungetc(c, pf); return *this; }
    int peek();
	int get() { return fgetc(pf); }
	int good();
	ifstream& getline(char *, int, char = '\n');
};

class ofstream
{
	FILE *pf;
	char *pBuf;

public:
	ofstream();
	~ofstream();
	void close();
	void open(char *, int, int=16384);
	int operator!();
	ofstream& operator<<(char);
	ofstream& operator<<(int n) { return *this << (long) n; }
	ofstream& operator<<(unsigned int n) { return *this << (unsigned long) n; }
	ofstream& operator<<(long);
	ofstream& operator<<(unsigned long);
	ofstream& operator<<(const char *);
	ofstream& operator<<(ofstream& (*f)(ofstream &));
	int good();
};

#endif	//__STREAM_H
