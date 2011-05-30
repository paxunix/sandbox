/*
	AREAIOR.CPP

	Copyright (c) 1994  Shawn Halpenny
	All rights reserved.

	Module containing class code for Sandbox's area file I/O.
	The "read" sections and misc.
*/

#ifdef _Windows
	#include <windows.h>
#endif

#ifndef _Windows
	#include <string.h>
	#define lstrcmp strcmp
	#define lstrlen strlen
	#define
#endif

#include <ctype.h>
#include "stream.h"
#include "llist.h"
#include "areaio.h"
#include "tstring.h"

//Global variables
char *szSections[] = {  //used when outputting to osLog
	{ "#AREA:  " },
	{ "#HELPS:  " },
	{ "#MOBILES:  " },
	{ "#MOBOLD:  " },
	{ "#OBJECTS:  " },
	{ "#OBJOLD:  " },
	{ "#ROOMS:  " },
	{ "#RESETS:  " },
	{ "#SHOPS:  " },
	{ "#SPECIALS:  " },
	{ "#SOCIALS:  " } };

ofstream osLog;
TArea Area;

/*
	If the last char of s is not '\n', adds a newline to the string, else
	do nothing (don't add if string is empty).
*/
void AddNewline(TString& s)
{
	static TString sNewline("\n");
	int nLen;

	if ((nLen = s.Len()) == 0)
		return;

	if (s[nLen - 1] != '\n')
		s += sNewline;
}   //AdjustString

/*
	Removes all newlines at end of s.
*/
void KillNewline(TString& s)
{
	char *p = &s[s.Len() - 1];

	while (*p == '\r' || *p == '\n')
		p--;

	*(p + 1) = 0;   //don't adjust string len, cuz it doesn't matter
}   //KillNewline

/*
	Fn to read a number (have to use it since area file #s aren't
	necessarily all digits :(  Note that they have no whitespace in them.
*/
long freadNum(ifstream& is)
{
	long number = 0;
	int sign = 0;
	char c;

	is >> ws;
	c = is.get();
	if (c == '+')
		c = is.get();
	else
	if (c == '-')
	{
		sign = 1;
		c = is.get();
	}

	if (!isdigit(c))
		return 0;

	while (isdigit(c))
	{
		number = number * 10 + c - '0';
		c = is.get();
	}

	if (sign)
		number = 0 - number;

	if (c == '|')
		number += freadNum(is);
	else
	if (!isdigit(c))
		is.putback(c);

	return number;
}   //freadNum

//Class code
void TArea::Clear()
{
	sTitle = "";
	RoomList.RemoveAll(TRUE);
	ObjList.RemoveAll(TRUE);
	MobList.RemoveAll(TRUE);
	HelpList.RemoveAll(TRUE);
	SocialList.RemoveAll(TRUE);
}   //TArea::Clear

/*
	Reads and parses an area file from is.  The "#AREA" has not been
	consumed.
*/
BOOL TArea::Read(ifstream& is)
{
	char szIn[256], ch;

	while (is.good())
	{
		is >> szIn;
		if (szIn[0] != '#')
		{
			osLog << "Expecting '#'\n";
			return FALSE;
		}

		if (!lstrcmp(szIn, "#AREA"))
		{
			is >> ws;
			is.getline(szIn, 256, '~');
			sTitle = szIn;
		}
		else
		if (!lstrcmp(szIn, "#HELPS"))
		{
			PTHelp pHelp;

			while (is.good())
			{
				pHelp = new THelp;
				if (!pHelp->Read(is))
				{
					delete pHelp;
					return FALSE;
				}
				else
				if (pHelp->sKeywords[0] != '$') //if keyword == "$", done helps
					AddHelp(pHelp);
				else
				{
					delete pHelp;
					osLog << szSections[nHELPS] << "Successful read\n";
					break;
				}
			}
		}
		else
		if (!lstrcmp(szIn, "#MOBILES"))
		{
			PTMobile pMob;

			is >> ch;
			while (is.good() && ch == '#' && is.peek() != '0')
			{
				is.putback(ch);
				pMob = new TMobile;
				if (!pMob->Read(is))
				{
					delete pMob;
					return FALSE;
				}

				if (FindMob(pMob->nVnum))
				{
					osLog << szSections[nMOBILES] << "Duplicate mobile vnum = " << pMob->nVnum<< '\n';
					delete pMob;
				}
				else
					AddMob(pMob);

				is >> ch;
			}

			is >> ch;   //skip the '0' we peeked at
		}
		else
		if (!lstrcmp(szIn, "#MOBOLD"))
		{
			PTMobOld pMob;

			is >> ch;
			while (is.good() && ch == '#' && is.peek() != '0')
			{
				is.putback(ch);
				pMob = new TMobOld;
				if (!pMob->Read(is))
				{
					delete pMob;
					return FALSE;
				}

				if (FindMob(pMob->nVnum))
				{
					osLog << szSections[nMOBOLD] << "Duplicate mobile vnum = " << pMob->nVnum << '\n';
					delete pMob;
				}
				else
					AddMob(pMob);

				is >> ch;
			}

			is >> ch;   //skip the '0' we peeked at
		}
		else
		if (!lstrcmp(szIn, "#OBJECTS"))
		{
			PTObject pObj;

			is >> ch;
			while (is.good() && ch == '#' && is.peek() != '0')
			{
				pObj = new TObject;
				is.putback(ch);
				if (!pObj->Read(is))
				{
					delete pObj;
					return FALSE;
				}

				if (FindObj(pObj->nVnum))
				{
					osLog << szSections[nOBJECTS] << "Duplicate object vnum = " << pObj->nVnum << '\n';
					delete pObj;
				}
				else
					AddObject(pObj);

				is >> ch;
			}

			is >> ch;   //skip peeked-at '0'
		}
		else
		if (!lstrcmp(szIn, "#OBJOLD"))
		{
			PTObjOld pObj;

			is >> ch;
			while (is.good() && ch == '#' && is.peek() != '0')
			{
				pObj = new TObjOld;
				is.putback(ch);
				if (!pObj->Read(is))
				{
					delete pObj;
					return FALSE;
				}

				if (FindObj(pObj->nVnum))
				{
					osLog << szSections[nOBJOLD] << "Duplicate object vnum = " << pObj->nVnum << '\n';
					delete pObj;
				}
				else
					AddObject(pObj);

				is >> ch;
			}

			is >> ch;   //skip peeked-at '0'
		}
		else
		if (!lstrcmp(szIn, "#ROOMS"))
		{
			PTRoom pRoom;

			is >> ch;
			while (is.good() && ch == '#' && is.peek() != '0')
			{
				pRoom = new TRoom();
				is.putback(ch);
				if (!pRoom->Read(is))
				{
					delete pRoom;
					return FALSE;
				}

				if (FindRoom(pRoom->nVnum))
				{
					osLog << szSections[nROOMS] << "Duplicate room vnum = " << pRoom->nVnum << '\n';
					delete pRoom;
				}
				else
					AddRoom(pRoom);

				is >> ch;
			}

			is >> ch;   //skip peeked-at '0'
		}
		else
		if (!lstrcmp(szIn, "#RESETS"))
		{
			PTRoom pRoom;
			PTMReset pMReset = NULL;
			PTBaseObj pLastObj = NULL;
			PResetObjType Type;
			void *pReset;

			while (is.good())
			{
				is >> ch;
				switch (ch)
				{
					case 'M':
					{
						int nVnum, nLimit, nRoomVnum;

						freadNum(is);
						nVnum = (int) freadNum(is);
						nLimit = (int) freadNum(is);
						nRoomVnum = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						pRoom = FindRoom(nRoomVnum);
						if (!pRoom)
							osLog << "'M' reset:  Bad room vnum = " << nRoomVnum << '\n';
						else
						{
							if (!FindMob(nVnum))
								osLog << "'M' reset:  Non-area mobile vnum = " << nVnum << '\n';

							pRoom->AddMReset((pMReset = new TMReset(nVnum, nLimit)));
						}
					}
					break;

					case 'G':
					{
						int nAmount, nVnum, nLimit;

						nAmount = (int) freadNum(is);
						nVnum = (int) freadNum(is);
						nLimit = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						if (!pMReset)
						{
							osLog << "'G' reset:  No preceding 'M' reset\n";
							return FALSE;
						}

						if (!(pLastObj = FindObj(nVnum)))
							osLog << "'G' reset:  Non-area object vnum = " << nVnum << '\n';

						Type = G_RESET;
						pMReset->AddGReset((PTGReset) (pReset = new TGReset(nAmount, nVnum, nLimit)));
					}
					break;

					case 'E':
					{
						int nAmount, nVnum, nLimit, nLoc;

						nAmount = (int) freadNum(is);
						nVnum = (int) freadNum(is);
						nLimit = (int) freadNum(is);
						nLoc = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						if (!pMReset)
						{
							osLog << "'E' reset:  No preceding 'M' reset\n";
							return FALSE;
						}

						if (!(pLastObj = FindObj(nVnum)))
							osLog << "'E' reset:  Non-area object vnum = " << nVnum << '\n';

						Type = E_RESET;
						pMReset->AddEReset((PTEReset) (pReset = new TEReset(nAmount, nVnum, nLimit, nLoc)));
					}
					break;

					case 'O':
					{
						int nVnum, nLimit, nRoomVnum;

						freadNum(is);
						nVnum = (int) freadNum(is);
						nLimit = (int) freadNum(is);
						nRoomVnum = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment
						pRoom = FindRoom(nRoomVnum);
						if (!pRoom)
							osLog << "'O' reset:  Bad room vnum = " << nRoomVnum << '\n';
						else
						{
							if (!(pLastObj = FindObj(nVnum)))
								osLog << "'O' reset:  Non-area object vnum = " << nVnum << '\n';

							Type = O_RESET;
							pRoom->AddOReset((PTOReset) (pReset = new TOReset(nVnum, nLimit)));
						}
					}
					break;

					case 'P':
					{
						int nAmount, nVnum, nLimit, nAcceptorVnum;

						nAmount = (int) freadNum(is);
						nVnum = (int) freadNum(is);
						nLimit = (int) freadNum(is);
						nAcceptorVnum = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						if (!pLastObj)
						{
							osLog << "'P' reset:  No last object loaded with 'E', 'G', or 'O' resets.\n";
							return FALSE;
						}

						if (pLastObj->nVnum != nAcceptorVnum)
							osLog << "'P' reset:  Container-object (" << nAcceptorVnum << ") different than in last 'E', 'G', or 'O' reset\n";
						else
						{
							if (!FindObj(nVnum))
								osLog << "'P' reset:  Non-area object vnum = " << nVnum << '\n';

							if (Type == O_RESET)
								((PTOReset) pReset)->AddPReset(new TPReset(nAmount, nVnum, nLimit));

							if (Type == E_RESET)
								((PTEReset) pReset)->AddPReset(new TPReset(nAmount, nVnum, nLimit));

							if (Type == G_RESET)
								((PTGReset) pReset)->AddPReset(new TPReset(nAmount, nVnum, nLimit));
						}
					}
					break;

					case 'R':
					{
						int nRoomVnum, nLastDoor;

						freadNum(is);
						nRoomVnum = (int) freadNum(is);
						nLastDoor = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						pRoom = FindRoom(nRoomVnum);
						if (!pRoom)
							osLog << "'R' reset:  Bad room vnum = " << nRoomVnum << '\n';
						else
							pRoom->RReset(nLastDoor);
					}
					break;

					case 'D':
					{
						int nRoomVnum, nDoor, nState;
						PTDoor pDoor;

						freadNum(is);
						nRoomVnum = (int) freadNum(is);
						nDoor = (int) freadNum(is);
						nState = (int) freadNum(is);
						is.ignore(256, '\n');   //skip comment

						pRoom = FindRoom(nRoomVnum);
						if (!pRoom)
							osLog << "'D' reset:  Bad room vnum = " << nRoomVnum << '\n';
						else
						{
							pDoor = pRoom->FindDoor(nDoor);

							if (!pDoor)
								osLog << "'D' reset:  Bad door number = " << nDoor << '\n';
							else
								pDoor->nState = nState;
						}
					}
					break;

					case '*':
						is.ignore(256, '\n');
						break;

					case 'S':
						goto DoneResets;

					default:
					{
						osLog << szSections[nRESETS] << "Unknown reset\n";
						return FALSE;
					}
				}
			}

			DoneResets:
			osLog << szSections[nRESETS] << "Successful read\n";
		}
		else
		if (!lstrcmp(szIn, "#SHOPS"))
		{
			int nVnum;
			PTBaseMob pMob;
			PTShop pShop;

			is >> ws;
			while (is.good() && is.peek() != '0')
			{
				nVnum = (int) freadNum(is);
				pMob = FindMob(nVnum);
				if (!pMob)
				{
					osLog << szSections[nSHOPS] << "Non-area mobile vnum = " << nVnum << '\n';
					if (MessageBox(NULL, "Non-area mobile shopkeeper in #SHOPS section.\n"
										 "Continue?  (If yes, this shopkeeper is ignored).",
					 "Parse Error", MB_ICONINFORMATION | MB_YESNO) == IDNO)
						return FALSE;
				}

				pShop = new TShop;
				pShop->Read(is);
				if (pMob)
					pMob->pShop = pShop;
				else
					delete pMob;

				is >> ws;
			}

			is >> ch;   //skip the 0 we peeked at
			if (ch != '0')
			{
				osLog << szSections[nSHOPS] << "Expecting '0'\n";
				return FALSE;
			}

			osLog << szSections[nSHOPS] << "Successful read\n";
		}
		else
		if (!lstrcmp(szIn, "#SPECIALS"))
		{
			PTBaseMob pMob;
			int nVnum;

			is >> ch;
			while (is.good())
			{
				switch (ch)
				{
					case 'M':
						nVnum = (int) freadNum(is);
						is >> szIn;
						is.ignore(256, '\n');   //skip comment
						pMob = FindMob(nVnum);
						if (!pMob)
							osLog << szSections[nSPECIALS] << "Bad mobile vnum = " << nVnum << '\n';
						else
							pMob->sSpecial = szIn;

						break;

					case '*':
						is.ignore(256, '\n');   //skip comment
						break;

					case 'S':
						goto DoneSpecials;

					default:
						osLog << szSections[nSPECIALS] << "Expecting 'M'\n";
						return FALSE;
				}

				is >> ch;
			}

			DoneSpecials:
			osLog << szSections[nSPECIALS] << "Successful read\n";
		}
		else
		if (!lstrcmp(szIn, "#SOCIALS"))
		{
			PTSocial pSocial;

			is >> ch;
			while (is.good() && ch != '#' && is.peek() != '0')
			{
				pSocial = new TSocial;
				is.putback(ch);
				if (!pSocial->Read(is))
				{
					delete pSocial;
					return FALSE;
				}

				AddSocial(pSocial);
				is >> ch;
			}

			is >> ch;   //skip peeked-at '0'
		}
		else
		if (!lstrcmp(szIn, "#$"))
			break;
		else
		{
			osLog << "Unknown '#' directive\n";
			return FALSE;
		}
	}

	if (!is)
	{
		osLog << "Problem parsing area file\n";
		return FALSE;
	}

	return TRUE;
}   //TArea::Read

/*
	Searches RoomList and returns pointer to room with Vnum = nVnum.  NULL is
	returned if not found.
*/
PTRoom TArea::FindRoom(int nVnum)
{
	PTRoom pRoom;
	TListIterator<TRoom> RoomListIter(RoomList);

	while (int(RoomListIter))
		if ((pRoom = RoomListIter++)->nVnum == nVnum)
			return pRoom;

	return NULL;
}   //TArea::FindRoom

/*
	Searches ObjList and returns pointer to obj with Vnum = nVnum.  NULL is
	returned if not found.
*/
PTBaseObj TArea::FindObj(int nVnum)
{
	PTBaseObj pObj;
	TListIterator<TBaseObj> ObjListIter(ObjList);

	while (int(ObjListIter))
		if ((pObj = ObjListIter++)->nVnum == nVnum)
			return pObj;

	return NULL;
}   //TArea::FindObj

/*
	Returns pointer to the TMobile with Vnum = n.  Return NULL if mobile
	doesn't exist.
*/
PTBaseMob TArea::FindMob(int nVnum)
{
	PTBaseMob pMob;
	TListIterator<TBaseMob> MobListIter(MobList);

	while (int(MobListIter))
		if ((pMob = MobListIter++)->nVnum == nVnum)
			return pMob;

	return NULL;
}   //TArea::FindMob

/*
	Reads the #HELPS section from is.  Assumes "#HELPS" has been
	consumed.
*/
BOOL THelp::Read(ifstream& is)
{
	char szIn[4096];

	nLevel = (int) freadNum(is);
	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;
	if (szIn[0] == '$') //end of helps
		return TRUE;    //have to let caller realize we're done helps

	is >> ws;
	is.getline(szIn, 4096, '~');
	if (!is)
	{
		osLog << szSections[nHELPS] << "Error reading file\n";
		return FALSE;
	}

	sText = szIn;

	//log success
	osLog << szSections[nHELPS] << "Successful read.  Keywords: (" << sKeywords << ")\n";

	return TRUE;
}   //THelp::Read

/*
	Reads a TDice object from is.
*/
BOOL TDice::Read(ifstream& is)
{
	char ch;

	nNum = (int) freadNum(is);
	is >> ch;
	nType = (int) freadNum(is);
	is >> ch;
	nBase = (int) freadNum(is);
	if (!is)
	{
		osLog << szSections[nMOBILES] << "Error reading dice\n";
		return FALSE;
	}

	return TRUE;
}   //TDice::Read

/*
	Reads a TArmorClass object from is.
*/
BOOL TArmorClass::Read(ifstream& is)
{
	nPierce = (int) freadNum(is);
	nBash = (int) freadNum(is);
	nSlash = (int) freadNum(is);
	nMagic = (int) freadNum(is);
	if (!is)
	{
		osLog << szSections[nMOBILES] << "Error reading armor class\n";
		return FALSE;
	}

	return TRUE;
}   //TArmorClass::Read

/*
	Reads a mobile from is.  Assumes "#MOBILES" has been consumed.
*/
BOOL TMobile::Read(ifstream& is)
{
	char ch, szIn[4096];
	int nLen;

	is >> ch;
	if (ch != '#')
	{
		osLog << szSections[nMOBILES] << "Expecting '#'\n";
		return FALSE;
	}

	nVnum = (int) freadNum(is);
	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sShortDesc = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sLongDesc = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sDesc = szIn;

	//CHANGE:  should ensure race is valid
	is >> ws;
	is.getline(szIn, 256, '~');
	sRace = szIn;

	//CHANGE: have to ensure action are valid
	is >> ActionFlg;

	//CHANGE: have to ensure affected_by are valid
	is >> AffectFlg;

	nAlign = (int) freadNum(is);
	is >> ch; //should verify alignment too
	if (ch != 'S')
	{
		osLog << szSections[nMOBILES] << "Expecting 'S'\n";
		return FALSE;
	}

	nLevel = (int) freadNum(is);
	nHitBonus = (int) freadNum(is);
	if (!HP.Read(is))
		return FALSE;

	if (!Mana.Read(is))
		return FALSE;

	if (!Damage.Read(is))
		return FALSE;

	nDamType = (int) freadNum(is);
	if (!AC.Read(is))
		return FALSE;

	//CHANGE:  verify off_bits
	is >> OffBits >> Immune >> Resist >> Vuln;
	nStartPos = (int) freadNum(is);
	nDefPos = (int) freadNum(is);
	nSex = (int) freadNum(is);
	lGold = (int) freadNum(is);
	is >> Form >> Parts >> cSize;

	//NOTE:  The material can have whitespace in it, as long as the name is
	//then surrounded by double/single quotes.  e.g. "burnished steel" is a
	//valid word for which just reading a string won't work.  Get all chars
	//up to and including the one that matches the opening quote type.
	is >> szIn[0];
	szIn[1] = 0;
	if (szIn[0] == '\'' || szIn[0] == '\"')
	{
		is.getline(&szIn[1], 256, szIn[0]);
		nLen = lstrlen(szIn);
		szIn[nLen] = szIn[0];
		szIn[nLen + 1] = 0;
	}
	else
	{
		is.putback(szIn[0]);
		is >> szIn;
	}

	//if material is '0', no worries, since string is already "" (saves space)
	if (szIn[0] == '0')
		sMaterial = "unknown";
	else
		sMaterial = szIn;

	if (!is)
	{
		osLog << szSections[nMOBILES] << "Error reading file\n";
		return FALSE;
	}

	osLog << szSections[nMOBILES] << "Successful read.  Vnum = " << nVnum << '\n';

	return TRUE;
}   //TMobile::Read

/*
	Reads a TESection object from is.  Assumes 'E' has been consumed.
*/
BOOL TESection::Read(ifstream& is)
{
	char szIn[4096];

	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sDesc = szIn;

	if (!is)
	{
		osLog << "Error reading 'E'xtended section\n";
		return FALSE;
	}

	return TRUE;
}   //TExtra::Read

/*
	Reads a TASection object from is.  Assumes 'A' has been consumed.
*/
BOOL TASection::Read(ifstream& is)
{
	nType = (int) freadNum(is);
	nVal = (int) freadNum(is);
	if (!is)
	{
		osLog << szSections[nOBJECTS] << "Error reading 'A'djustment section\n";
		return FALSE;
	}

	return TRUE;
}   //TAsection::Read

/*
	Reads a TObject from is.  Assumes "#OBJECTS" has been consumed.
*/
BOOL TObject::Read(ifstream& is)
{
	char szIn[4096], ch;
	PTESection pE;
	PTASection pA;
	BOOL bStill = TRUE;

	is >> ch;
	nVnum = (int) freadNum(is);
	if (ch != '#')
	{
		osLog << szSections[nOBJECTS] << "Expecting '#'\n";
		return FALSE;
	}

	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sName = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sLongDesc = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sMaterial = szIn;

	nType = (int) freadNum(is);
	is >> Flags >> WearFlags >> Values[0] >> Values[1] >>
	 Values[2] >> Values[3] >> Values[4];
	nLevel = (int) freadNum(is);
	nWeight = (int) freadNum(is);
	lValue = freadNum(is);
	is >> cCondition;

	//Read extended and affect sections
	while (bStill)
	{
		is >> ch;
		switch (ch)
		{
			case 'E':
				pE = new TESection();
				if (pE->Read(is))
					AddESection(pE);
				else
					return FALSE;

				break;

			case 'A':
				pA = new TASection();
				if (pA->Read(is))
					AddASection(pA);
				else
					return FALSE;

				break;

			default:
				bStill = FALSE;
				is.putback(ch);
		}
	}

	//log success
	osLog << szSections[nOBJECTS] << "Successful read.  Vnum = " << nVnum << '\n';

	return TRUE;
}   //TObject::Read

/*
	Reads in a TDoor object.  Assumes the 'D' has been consumed.
*/
BOOL TDoor::Read(ifstream& is)
{
	char szIn[256];

	nDir = (int) freadNum(is);
	is >> ws;
	is.getline(szIn, 256, '~');
	sDesc = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	nLock = (int) freadNum(is);
	nKey = (int) freadNum(is);
	nToRoomVnum = (int) freadNum(is);
	if (!is)
	{
		osLog << szSections[nROOMS] << "Error reading door\n";
		return FALSE;
	}

	return TRUE;
}   //TDoor::Read

/*
	Reads a TShop object from is.  Assumes "#SHOPS" and the mobile vnum
	have been consumed.
*/
BOOL TShop::Read(ifstream& is)
{
	nTradeTypes[0] = (int) freadNum(is);
	nTradeTypes[1] = (int) freadNum(is);
	nTradeTypes[2] = (int) freadNum(is);
	nTradeTypes[3] = (int) freadNum(is);
	nTradeTypes[4] = (int) freadNum(is);
	nBuyMarkup = (int) freadNum(is);
	nSellMarkdown = (int) freadNum(is);
	nOpenHour = (int) freadNum(is);
	nCloseHour = (int) freadNum(is);
	is.ignore(256, '\n');   //skip the comment, if any
	if (!is)
	{
		osLog << szSections[nSHOPS] << "Error reading file\n";
		return FALSE;
	}

	return TRUE;
}   //TShop::Read

/*
	Reads a TRoom object from is.  Assumes #ROOMS has been consumed.
*/
BOOL TRoom::Read(ifstream& is)
{
	char szIn[4096], ch;
	PTDoor pDoor;
	PTESection pE;

	is >> ch;
	if (ch != '#')
	{
		osLog << szSections[nROOMS] << "Expecting '#'\n";
		return FALSE;
	}

	nVnum = (int) freadNum(is);
	is >> ws;
	is.getline(szIn, 256, '~');
	sName = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sDesc = szIn;

	is >> szIn >> Flags;
	nTerrain = (int) freadNum(is);

	is >> ch;
	while (ch != 'S')
	{
		switch (ch)
		{
			case 'D':
				pDoor = new TDoor;
				pDoor->Read(is);
				AddDoor(pDoor);
				break;

			case 'E':
				pE = new TESection;
				pE->Read(is);
				AddESection(pE);
				break;

			default:
				return FALSE;
		}

		is >> ch;
	}

	if (!is)
	{
		osLog << szSections[nROOMS] << "Error reading file\n";
		return FALSE;
	}

	//log success
	osLog << szSections[nROOMS] << "Successful read.  Vnum = " << nVnum << '\n';

	return TRUE;
}   //TRoom::Read

/*
	Returns pointer to the TDoor object that goes in direction nDir.  NULL,
	if door not found.
*/
PTDoor TRoom::FindDoor(int nDir)
{
	TListIterator<TDoor> DoorListIter(DoorList);
	PTDoor pDoor;

	while (int(DoorListIter))
		if ((pDoor = DoorListIter++)->nDir == nDir)
			return pDoor;

	return NULL;
}   //TRoom::FindDoor

/*
	Reads an old-style mob from is.  Assumes "#MOBOLD" has been consumed.
*/
BOOL TMobOld::Read(ifstream& is)
{
	char ch, szIn[4096];

	is >> ch;
	if (ch != '#')
	{
		osLog << szSections[nMOBOLD] << "Expecting '#'\n";
		return FALSE;
	}

	nVnum = (int) freadNum(is);
	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sShortDesc = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sLongDesc = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sDesc = szIn;

	//CHANGE: have to ensure action are valid
	is >> ActionFlg;

	//CHANGE: have to ensure affected_by are valid
	is >> AffectFlg;

	nAlign = (int) freadNum(is);
	is >> ch; //should verify alignment too
	if (ch != 'S')
	{
		osLog << szSections[nMOBOLD] << "Expecting 'S'\n";
		return FALSE;
	}

	nLevel = (int) freadNum(is);

	//unused stuff
	freadNum(is);
	freadNum(is);
	freadNum(is);
	is >> ch;
	freadNum(is);
	is >> ch;
	freadNum(is);
	freadNum(is);
	is >> ch;
	freadNum(is);
	is >> ch;
	freadNum(is);
	lGold = freadNum(is);
	freadNum(is);
	nStartPos = (int) freadNum(is);
	nDefPos = (int) freadNum(is);
	nSex = (int) freadNum(is);

	if (!is)
	{
		osLog << szSections[nMOBOLD] << "Error reading file\n";
		return FALSE;
	}

	//log success
	osLog << szSections[nMOBOLD] << "Successful read.  Vnum = " << nVnum << '\n';

	return TRUE;
}   //TMobOld::Read

/*
	Reads an old-style object from is.  Assumes "#OBJOLD" has been consumed.
*/
BOOL TObjOld::Read(ifstream& is)
{
	char szIn[4096], ch;
	PTESection pE;
	PTASection pA;
	BOOL bStill = TRUE;

	is >> ch;
	nVnum = (int) freadNum(is);
	if (ch != '#')
	{
		osLog << szSections[nOBJOLD] << "Expecting '#'\n";
		return FALSE;
	}

	is >> ws;
	is.getline(szIn, 256, '~');
	sKeywords = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');
	sName = szIn;

	is >> ws;
	is.getline(szIn, 4096, '~');
	sLongDesc = szIn;

	is >> ws;
	is.getline(szIn, 256, '~');

	nType = (int) freadNum(is);
	is >> Flags >> WearFlags;
	lValues[0] = freadNum(is);
	lValues[1] = freadNum(is);
	lValues[2] = freadNum(is);
	lValues[3] = freadNum(is);
	nWeight = (int) freadNum(is);
	lValue = freadNum(is);
	freadNum(is);

	//Read extended and affect sections
	while (bStill)
	{
		is >> ch;
		switch (ch)
		{
			case 'E':
				pE = new TESection();
				if (pE->Read(is))
					AddESection(pE);
				else
					return FALSE;

				break;

			case 'A':
				pA = new TASection();
				if (pA->Read(is))
					AddASection(pA);
				else
					return FALSE;

				break;

			default:
				bStill = FALSE;
				is.putback(ch);
		}
	}

	//log success
	osLog << szSections[nOBJOLD] << "Successful read.  Vnum = " << nVnum << '\n';

	return TRUE;
}   //TObjOld::Read

/*
	Sets lFlags to the int equivalent of the flags in pszFlags.  pszFlags is
	assumed to contain no whitespace.
*/
long TFlags::Set(const char *pszFlags)
{
	long lNum = 0;
	int nSign = 1;

	if (!*pszFlags)
		return 0;

	//take care of signs
	if (*pszFlags == '-')
		nSign = -1;

	if (*pszFlags == '+')
		pszFlags++;

	while (*pszFlags && isalpha(*pszFlags))
	{
		lNum += (isupper(*pszFlags) ? (1L << (*pszFlags - 'A')) :
		 (67108864L << (*pszFlags - 'a')));
		if (islower(*pszFlags))
			pszFlags++;

		pszFlags++; //lowercase chars are dup'd (see merc.h)
	}

	while (*pszFlags && isdigit(*pszFlags))
	{
		lNum = lNum * 10 + *pszFlags - '0';
		pszFlags++;
	}

	if (*pszFlags == '|')
		lNum += TFlags::Set(++pszFlags);

	lFlags = lNum;
	return lFlags * nSign;
}   //TFlags::Set

/*
	Fills pszFlags with the NULL-terminated ASCII flag form of lFlags.
	Assumes pszFlags is large enough to accomodate the string.  If
	the value is <0, flags is written numerically instead.
*/
void TFlags::Get(char *pszFlags)
{
	char i, *p = pszFlags;
	long l;

	if (lFlags < 0)
	{
		sprintf(pszFlags, "%ld", lFlags);
		return;
	}

	for (i='A'; i<='Z'; i++)
	{
		l = 1L << (i - 'A');
		if ((lFlags & l))
		{
			*p = i;
			p++;
		}
	}

	for (i='a'; i<='e'; i++)    //'f' would be 32-bit (the sign)...a no no
	{
		l = 67108864L << (i - 'a');
		if ((lFlags & l))
		{
			*p = i;
			p++;
			*p = i; //See NOTE in TFlags::Set re: dup'd lcase char
			p++;
		}
	}

	*p = 0;
	if (!pszFlags[0])   //empty string->no flags->"0"
	{
		pszFlags[0] = '0';
		pszFlags[1] = 0;
	}
}   //TFlags::Get

/*
	Reads a set of flags from is.  Flags have no whitespace and consist
	of (A-Z and a-e) or (0-9 and |).  The result is stored in Flags.
*/
ifstream& operator>>(ifstream& is, TFlags& Flags)
{
	char sz[40] = "";

	is >> sz;
	Flags.Set(sz);

	return is;
}   //operator>>
