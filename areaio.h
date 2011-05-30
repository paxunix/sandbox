/*
	AREAIO.H

	Copyright (c) 1994  Shawn Halpenny
	All rights reserved.

	Header file declaring classes for Sandbox area file I/O.
	Class code is in AREAIO.CPP.
*/

#ifndef __AREAIO_H
#define __AREAIO_H

#ifndef __WINDOWS_H
#include <windows.h>
#endif

#ifndef ___DEFS_H
#include <_defs.h>
#endif

#ifndef __LLIST_H
#include "llist.h"
#endif

#ifndef __STREAM_H
#include "stream.h"
#endif

#ifndef __TSTRING_H
#include "tstring.h"
#endif

//Type definitions
enum { nAREA, nHELPS, nMOBILES, nMOBOLD,
	   nOBJECTS, nOBJOLD, nROOMS, nRESETS,
	   nSHOPS, nSPECIALS, nSOCIALS };

enum ObjType { OLD, NEW };
enum PResetObjType { E_RESET, O_RESET, G_RESET };

_CLASSDEF(TArea)
_CLASSDEF(TMobile)
_CLASSDEF(TObject)
_CLASSDEF(TRoom)
_CLASSDEF(THelp)
_CLASSDEF(TDice)
_CLASSDEF(TArmorClass)
_CLASSDEF(TESection)
_CLASSDEF(TASection)
_CLASSDEF(TDoor)
_CLASSDEF(TShop)
_CLASSDEF(TMReset)
_CLASSDEF(TEReset)
_CLASSDEF(TGReset)
_CLASSDEF(TOReset)
_CLASSDEF(TPReset)
_CLASSDEF(TBaseMob)
_CLASSDEF(TBaseObj)
_CLASSDEF(TMobOld)
_CLASSDEF(TObjOld)
_CLASSDEF(TFlags)
_CLASSDEF(TSocial)

class TFlags
{
public:
	long lFlags;

	friend ofstream & operator<<(ofstream &, TFlags &);
	friend ifstream & operator>>(ifstream &, TFlags &);
	void Set(long l) { lFlags = l; }
	long Set(const char *);
	long Get() { return lFlags; }
	void Get(char *);
};

class TOReset
{
public:
	TList<TPReset> PResetList;
	int nVnum;
	int nLimit;

	TOReset(int n1, int n2) { nVnum = n1; nLimit = n2; }
	void AddPReset(PTPReset p)
	{
		PResetList.Add(p);
	}

	BOOL WriteResets(ofstream &, int);
};

class TDoor
{
public:
	int nDir;
	TString sDesc;
	TString sKeywords;
	int nLock;	//the lock # from the #ROOM declaration
    int nState;	//the state number from the 'D' #RESET section.  -1 if none. 
	int nKey;
	int nToRoomVnum;

	TDoor() { nState = -1; }
	TDoor& operator=(const TDoor&);
	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	BOOL WriteReset(ofstream &, int);
};

class TMReset
{
public:
	TList<TGReset> GResetList;
	TList<TEReset> EResetList;
	int nVnum;
	int nLimit;

	TMReset(int n1, int n2) { nVnum = n1; nLimit = n2; }
	BOOL WriteResets(ofstream &, int);
	void Fill(int, int);
	void AddGReset(PTGReset p)
	{
		GResetList.Add(p);
	}

	void AddEReset(PTEReset p)
	{
		EResetList.Add(p);
	}
};

class TESection
{
public:
	TString sKeywords;
	TString sDesc;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

#define NO_R_RESET -1

class TRoom
{
public:
	TList<TDoor> DoorList;
	TList<TESection> EList;
	TList<TMReset> MobResetList;
	TList<TOReset> ObjResetList;
	int nRReset;    //if !=NO_R_RESET, R reset with nRReset=last_door_num
	int nVnum;
	TString sName;
	TString sDesc;
	TFlags Flags;
	int nTerrain;

	TRoom() { nRReset = NO_R_RESET; }
	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	BOOL WriteResets(ofstream &);
	void AddMReset(PTMReset p)
	{
		MobResetList.Add(p);
	}

	void AddOReset(PTOReset p)
	{
		ObjResetList.Add(p);
	}

	void AddESection(PTESection p)
	{
		EList.Add(p);
	}

	void AddDoor(PTDoor p)
	{
		DoorList.Add(p);
	}

	void RReset(int n1) { nRReset = n1; }
	PTDoor FindDoor(int);
};

class TBaseObj
{
public:
	TList<TESection> EList;
	TList<TASection> AList;
	int nVnum;
	TString sKeywords;
	TString sName;
	TString sLongDesc;
	int nType;
	TFlags Flags;
	TFlags WearFlags;
	int nWeight;
	long lValue;

	virtual BOOL Read(ifstream &) = 0;
	virtual BOOL Write(ofstream &) = 0;
	virtual ObjType Type() = 0;
	void AddESection(PTESection p)
	{
		EList.Add(p);
	}

	void AddASection(PTASection p)
	{
		AList.Add(p);
	}
};

class TShop
{
public:
	int nTradeTypes[5];
	int nBuyMarkup;
	int nSellMarkdown;
	int nOpenHour;
	int nCloseHour;

    TShop();
	BOOL Read(ifstream &);
	BOOL Write(ofstream &, int);
	TShop& operator=(const TShop&);
};

class TBaseMob
{
public:
	int nVnum;
	TString sKeywords;
	TString sShortDesc;
	TString sLongDesc;
	TString sDesc;
	TFlags ActionFlg;
	TFlags AffectFlg;
	int nAlign;
	int nLevel;
	long lGold;
	int nStartPos;
	int nDefPos;
	int nSex;
	PTShop pShop;   //valid if this mob is a shopkeeper
	TString sSpecial;   //non-empty if this mob has special function

	TBaseMob() { pShop = NULL; }
	~TBaseMob() { delete pShop; }
	virtual BOOL Read(ifstream &) = 0;
	virtual BOOL Write(ofstream &) = 0;
	virtual ObjType Type() = 0;
};

class THelp
{
public:
	int nLevel;
	TString sKeywords;
	TString sText;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

class TArea
{
public:
	TList<TRoom> RoomList;
	TList<TBaseObj> ObjList;
	TList<TBaseMob> MobList;
	TList<THelp> HelpList;
	TList<TSocial> SocialList;
	TString sTitle;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	void Clear();
	void AddRoom(PTRoom p)
	{
		RoomList.Add(p);
	}

	void AddObject(PTBaseObj p)
	{
		ObjList.Add(p);
	}

	void AddMob(PTBaseMob p)
	{
		MobList.Add(p);
	}

	void AddHelp(PTHelp p)
	{
		HelpList.Add(p);
	}

	void AddSocial(PTSocial p)
	{
		SocialList.Add(p);
	}

	PTRoom FindRoom(int);
	PTBaseObj FindObj(int);
	PTBaseMob FindMob(int);
};

class TMobOld : public TBaseMob
{
public:
	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	ObjType Type() { return OLD; }
};

class TObjOld : public TBaseObj
{
public:
	long lValues[4];

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	ObjType Type() { return OLD; }
};

class TDice
{
public:
	int nNum;
	int nType;
	int nBase;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

class TArmorClass
{
public:
	int nPierce;
	int nBash;
	int nSlash;
	int nMagic;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

class TMobile : public TBaseMob
{
public:
	TString sRace;
	int nHitBonus;
	TDice HP;
	TDice Mana;
	TDice Damage;
	int nDamType;
	TArmorClass AC;
	TFlags OffBits;
	TFlags Immune;
	TFlags Resist;
	TFlags Vuln;
	char cSize;
	TFlags Form;
	TFlags Parts;
	TString sMaterial;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	ObjType Type() { return NEW; }
};

class TASection
{
public:
	int nType;
	int nVal;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

class TObject : public TBaseObj
{
public:
	TString sMaterial;
	TFlags Values[5];
	int nLevel;
	char cCondition;

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
	ObjType Type() { return NEW; }
};

class TGReset
{
public:
	TList<TPReset> PResetList;	//cuz what if this is a reset for container? 
	int nAmount;
	int nVnum;
	int nLimit;

	TGReset(int n1, int n2, int n3) { nAmount = n1; nVnum = n2, nLimit = n3; }
	void AddPReset(PTPReset p)
	{
		PResetList.Add(p);
	}

	BOOL WriteReset(ofstream &);
};

class TEReset
{
public:
	TList<TPReset> PResetList;
	int nAmount;
	int nVnum;
	int nLimit;
	int nWearLoc;

	TEReset(int n1, int n2, int n3, int n4) { nAmount = n1; nVnum = n2; nLimit = n3; nWearLoc = n4; }
	void AddPReset(PTPReset p)
	{
		PResetList.Add(p);
	}

	BOOL WriteReset(ofstream &);
};

class TPReset
{
public:
	int nAmount;
	int nVnum;
	int nLimit;

	TPReset(int n1, int n2, int n3) { nAmount = n1; nVnum = n2; nLimit = n3; }
	BOOL WriteReset(ofstream &, int);
};

class TSocial
{
public:
	TString sName;
    TString sStrs[8];

	BOOL Read(ifstream &);
	BOOL Write(ofstream &);
};

//Function protos
void AddNewline(RTString);
void KillNewline(RTString);
long freadNum(ifstream&);

//Global Vars
extern ofstream osLog;  //the log stream for parser information
extern TArea Area;		//the area object for (cuz there can be only one)
extern char *szSections[];	//array of section names for osLog output
#endif  //__AREAIO_H
