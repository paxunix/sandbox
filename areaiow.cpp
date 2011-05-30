/*
	AREAIOW.CPP

	Copyright (c) 1994  Shawn Halpenny
	All rights reserved.

	Module containing class code for Sandbox's area file I/O.
	The "write" sections.
*/

#ifdef _Windows
	#include <windows.h>
#endif

#include <ctype.h>
#include "stream.h"
#include "llist.h"
#include "areaio.h"
#include "tstring.h"

/*
	Writes an entire area to os.  Returns TRUE on success, FALSE on
	failure.
*/
BOOL TArea::Write(ofstream& os)
{
	TListIterator<TRoom> RoomListIter(RoomList);
	TListIterator<TBaseObj> ObjListIter(ObjList);
	TListIterator<TBaseMob> MobListIter(MobList);
	TListIterator<THelp> HelpListIter(HelpList);
	TListIterator<TSocial> SocialsListIter(SocialList);
	PTBaseMob pMob;
	PTBaseObj pObj;

	KillNewline(sTitle);

    if (sTitle.Len())
		os << "#AREA\t" << sTitle << "~\n\n\n";

	if (!HelpList.IsEmpty())
	{
		os << "#HELPS\n";
		while (int(HelpListIter))
			if (!(HelpListIter++)->Write(os))
				return FALSE;

		os << "0 $~\n\n\n";
	}

	if (!MobList.IsEmpty())
	{
		os << "#MOBILES\n";
		while (int(MobListIter))
		{
			pMob = MobListIter++;
			if (pMob->Type() == NEW)
				if (!pMob->Write(os))
					return FALSE;
		}

		os << "#0\n\n\n";

		os << "#MOBOLD\n";
		MobListIter.Restart();
		while (int(MobListIter))
		{
			pMob = MobListIter++;
			if (pMob->Type() == OLD)
				if (!pMob->Write(os))
					return FALSE;
		}

		os << "#0\n\n\n";
	}

	if (!ObjList.IsEmpty())
	{
		os << "#OBJECTS\n";
		while (int(ObjListIter))
		{
			pObj = ObjListIter++;
			if (pObj->Type() == NEW)
				if (!pObj->Write(os))
					return FALSE;
		}
		os << "#0\n\n\n";

		os << "#OBJOLD\n";
		ObjListIter.Restart();
		while (int(ObjListIter))
		{
			pObj = ObjListIter++;
			if (pObj->Type() == OLD)
				if (!pObj->Write(os))
					return FALSE;
		}

		os << "#0\n\n\n";
	}

	if (!RoomList.IsEmpty())
	{
		os << "#ROOMS\n";
		while (int(RoomListIter))
			if (!(RoomListIter++)->Write(os))
				return FALSE;

		os << "#0\n\n\n";

		os << "#RESETS\n";
		RoomListIter.Restart();
		while (int(RoomListIter))
			if (!(RoomListIter++)->WriteResets(os))
				return FALSE;

		os << "S\n\n\n";
	}

	if (!MobList.IsEmpty())
	{
		os << "#SHOPS\n";
		MobListIter.Restart();
		while (int(MobListIter))
		{
			pMob = MobListIter++;
			if (pMob->pShop)
				if (!(pMob->pShop)->Write(os, pMob->nVnum))
					return FALSE;
		}

		os << "0\n\n\n";

		os << "#SPECIALS\n";
		MobListIter.Restart();
		while (int(MobListIter))
		{
			pMob = MobListIter++;
			if (pMob->sSpecial[0])  //if it has a special func
				os << "M " << pMob->nVnum << ' ' << pMob->sSpecial << '\n';
		}

		os << "S\n\n\n";
	}

	if (!SocialList.IsEmpty())
	{
		os << "#SOCIALS\n";

		while (int(SocialsListIter))
			if (!(SocialsListIter++)->Write(os))
				return FALSE;

    	os << "#0\n\n\n";
    }

	os << "#$\n";

	if (!os)
	{
		osLog << "Error writing file\n";
		return FALSE;
	}
	else
		osLog << "Area successfully written.\n";

	return TRUE;
}   //TArea::Write

/*
	Writes a THelp object to os.  Assumes "#HELPS" has been
	written.
*/
BOOL THelp::Write(ofstream& os)
{
	KillNewline(sKeywords);
	AddNewline(sText);

	os << nLevel << ' ' << sKeywords << "~\n" <<
	 sText << "~\n\n";

	if (!os)
	{
		osLog << szSections[nHELPS] << "Error writing file\n";
		return FALSE;
	}

	return TRUE;
}   //THelp::Write

/*
	Writes a TDice object to os.
*/
BOOL TDice::Write(ofstream& os)
{
	os << nNum << 'd' << nType << '+' << nBase;
	if (!os)
	{
		osLog << szSections[nMOBILES] << "Error writing dice\n";
		return FALSE;
	}

	return TRUE;
}   //TDice::Write

/*
	Writes a TArmorClass object to os.
*/
BOOL TArmorClass::Write(ofstream& os)
{
	os << nPierce << ' ' << nBash << ' ' << nSlash << ' ' << nMagic;
	if (!os)
	{
		osLog << szSections[nMOBILES] << "Error writing armor class\n";
		return FALSE;
	}

	return TRUE;
}   //TArmorClass::Write

/*
	Writes a mobile to os.  Assumes "#MOBILES" has been written.
*/
BOOL TMobile::Write(ofstream& os)
{
	KillNewline(sKeywords);
	KillNewline(sShortDesc);
	AddNewline(sLongDesc);
	AddNewline(sDesc);
	KillNewline(sRace);

	os << '#' << nVnum << '\n' <<
	 sKeywords << "~\n" <<
	 sShortDesc << "~\n" <<
	 sLongDesc << "~\n" <<
	 sDesc << "~\n" <<
	 sRace << "~\n" <<
	 ActionFlg << ' ' << AffectFlg << ' ' << nAlign << " S\n" <<
	 nLevel << ' ' << nHitBonus << ' ';

	HP.Write(os);
	os << ' ';
	Mana.Write(os);
	os << ' ';
	Damage.Write(os);
	os << ' ' << nDamType << '\n';

	AC.Write(os);
	os << '\n' <<
	 OffBits << ' ' << Immune << ' ' << Resist << ' ' << Vuln << '\n' <<
	 nStartPos << ' ' << nDefPos << ' ' << nSex << ' ' << lGold << '\n' <<
	 Form << ' ' << Parts << ' ' << cSize << ' ';

	if (!sMaterial.Len())	//just in case
		os << "unknown";
	else
		os << sMaterial;

	os << "\n\n";

	if (!os)
	{
		osLog << szSections[nMOBILES] << "Error writing file\n";
		return FALSE;
	}

	return TRUE;
}   //TMobile::Write

/*
	Writes TESection object to os.
*/
BOOL TESection::Write(ofstream& os)
{
	KillNewline(sKeywords);
	AddNewline(sDesc);

	os << "E\n" <<
	 sKeywords << "~\n" <<
	 sDesc << "~\n";

	if (!os)
	{
		osLog << "Error writing 'E'xtended section\n";
		return FALSE;
	}

	return TRUE;
}   //TExtra::Write

/*
	Writes a TASection object to os.
*/
BOOL TASection::Write(ofstream& os)
{
	os << "A\n" <<
	 nType << ' ' << nVal << '\n';

	if (!os)
	{
		osLog << szSections[nOBJECTS] << "Error writing 'A'djustment section\n";
		return FALSE;
	}

	return TRUE;
}   //TASection::Write

/*
	Writes a TObject to os.  Assumes "#OBJECTS" has been written
*/
BOOL TObject::Write(ofstream& os)
{
	TListIterator<TESection> EListIter(EList);
	TListIterator<TASection> AListIter(AList);

	KillNewline(sKeywords);
	KillNewline(sName);
	KillNewline(sLongDesc);
	KillNewline(sMaterial);

	if (!sMaterial.Len())
		sMaterial = "unknown";

	os << '#' << nVnum << '\n' <<
	 sKeywords << "~\n" <<
	 sName << "~\n" <<
	 sLongDesc << "~\n" <<
	 sMaterial << "~\n" <<
	 nType << ' ' << Flags << ' ' << WearFlags << '\n' <<
	 //NOTE:  although the Values[] are flags, when written, they are
	 //usually numeric, so that's the way we'll do it (as opposed to A-Zs)
	 Values[0].Get() << ' ' << Values[1].Get() << ' ' << Values[2].Get() << ' ' <<
	 Values[3].Get() << ' ' << Values[4].Get() << '\n' <<
	 nLevel << ' ' << nWeight << ' ' << lValue << ' ' << cCondition << '\n';

	while (int(EListIter))
		(EListIter++)->Write(os);

	while (int(AListIter))
		(AListIter++)->Write(os);

	if (!os)
	{
		osLog << szSections[nOBJECTS] << "Error writing file\n";
		return FALSE;
	}
	else
		os << '\n';

	return TRUE;
}   //TObject::Write

TDoor& TDoor::operator=(const TDoor& src)
{
	if (&src != this)
    {
		nDir = src.nDir;
		sDesc = src.sDesc;
		sKeywords = src.sKeywords;
		nLock = src.nLock;
		nState = src.nState;
		nKey = src.nKey;
		nToRoomVnum = src.nToRoomVnum;
	}

	return *this;
}	//TDoor::operator=

/*
	Writes a TDoor object to os.
*/
BOOL TDoor::Write(ofstream& os)
{
	AddNewline(sDesc);
	KillNewline(sKeywords);

	os << 'D' << nDir << '\n' <<
	 sDesc << "~\n" <<
	 sKeywords << "~\n" <<
	 nLock << ' ' << nKey << ' ' << nToRoomVnum << '\n';

	if (!os)
	{
		osLog << szSections[nROOMS] << "Error writing door\n";
		return FALSE;
	}

	return TRUE;
}   //TDoor::Write

TShop::TShop()
{
	for (int i=0; i<5; i++)
		nTradeTypes[i] = 0;

	nBuyMarkup = nSellMarkdown = nOpenHour = nCloseHour = 0;
}	//TShop::TShop

TShop& TShop::operator=(const TShop& src)
{
	if (&src != this)
    {
		for (int i=0; i<5; i++)
			nTradeTypes[i] = src.nTradeTypes[i];

		nBuyMarkup = src.nBuyMarkup;
		nSellMarkdown = src.nSellMarkdown;
		nOpenHour = src.nOpenHour;
		nCloseHour = src.nCloseHour;
	}

	return *this;
}	//TShop::operator=

/*
	Writes a TShop object to os.  Assumes #SHOPS has been written.
*/
BOOL TShop::Write(ofstream& os, int nMobVnum)
{
	os << nMobVnum << ' ' << nTradeTypes[0] << ' ' << nTradeTypes[1] << ' ' <<
	 nTradeTypes[2] << ' ' << nTradeTypes[3] << ' ' << nTradeTypes[4] << ' ' <<
	 nBuyMarkup << ' ' << nSellMarkdown << ' ' << nOpenHour  << ' ' <<
	 nCloseHour << '\n';

	if (!os)
	{
		osLog << szSections[nSHOPS] << "Error writing file\n";
		return FALSE;
	}

	return TRUE;
}   //TShop::Write

/*
	Writes a TRoom object to os.  Assumes #ROOMS has been written.
*/
BOOL TRoom::Write(ofstream& os)
{
	TListIterator<TDoor> DoorListIter(DoorList);
	TListIterator<TESection> EListIter(EList);

	KillNewline(sName);
	AddNewline(sDesc);

	os << '#' << nVnum << '\n' <<
	 sName << "~\n" <<
	 sDesc << "~\n" <<
	 "0 " << Flags << ' ' << nTerrain << '\n';

	while (int(DoorListIter))
		if (!(DoorListIter++)->Write(os))
			break;

	while (int(EListIter))
		if (!(EListIter++)->Write(os))
			break;

	if (!os)
	{
		osLog << szSections[nROOMS] << "Error writing file\n";
		return FALSE;
	}
	else
		os << "S\n\n";

	return TRUE;
}   //TRoom::Write

/*
	Writes the resets for the room to os.
*/
BOOL TRoom::WriteResets(ofstream& os)
{
	TListIterator<TMReset> MobResetListIter(MobResetList);
	TListIterator<TOReset> ObjResetListIter(ObjResetList);
	TListIterator<TDoor> DoorListIter(DoorList);

	while (int(MobResetListIter))
		if (!(MobResetListIter++)->WriteResets(os, nVnum))
			return FALSE;

	while (int(ObjResetListIter))
		if (!(ObjResetListIter++)->WriteResets(os, nVnum))
			return FALSE;

	while (int(DoorListIter))
		if (!(DoorListIter++)->WriteReset(os, nVnum))
			return FALSE;

	if (nRReset != NO_R_RESET)
		os << "R 0 " << nVnum << ' ' << nRReset << "\t;" << sName << '\n';

	return !os ? FALSE : TRUE;
}   //TRoom::WriteResets

/*
	Write M, G, E resets for the mob to os.  nRoomVnum is the room the mob is
	put in at reset.
*/
BOOL TMReset::WriteResets(ofstream& os, int nRoomVnum)
{
	TListIterator<TGReset> GResetListIter(GResetList);
	TListIterator<TEReset> EResetListIter(EResetList);

	os << "M 0 " << nVnum << ' ' << nLimit << ' ' << nRoomVnum << "\t;";

	if (Area.FindMob(nVnum))	//can load non-area mobs (i.e. we dont know the shdesc)
		os << Area.FindMob(nVnum)->sShortDesc;
	else
		os << "<non-area mobile>";

    //room will always be in area
	os << " into " << Area.FindRoom(nRoomVnum)->sName << '\n';

	while (int(GResetListIter))
		if (!(GResetListIter++)->WriteReset(os))
			return FALSE;

	while (int(EResetListIter))
		if (!(EResetListIter++)->WriteReset(os))
			return FALSE;

	return !os ? FALSE : TRUE;
}   //TMReset::WriteReset

/*
	Writes a G reset to os.  
*/
BOOL TGReset::WriteReset(ofstream& os)
{
	os << "G " << nAmount << ' ' << nVnum << ' ' << nLimit << "\t;";

	if (Area.FindObj(nVnum))	//can be giving a non-area obj (i.e. don't know name)
		os << Area.FindObj(nVnum)->sName;
	else
		os << "<non-area object>";

    os << '\n';

	return !os ? FALSE : TRUE;
}   //TGReset::WriteReset

/*
	Writes an E reset to os.
*/
BOOL TEReset::WriteReset(ofstream& os)
{
	os << "E " << nAmount << ' ' << nVnum << ' ' << nLimit << ' ' << nWearLoc << "\t;";

	if (Area.FindObj(nVnum)) //can equip non-area obj
		os << Area.FindObj(nVnum)->sName;
	else
		os << "<non-area object>";

    os << '\n';

	return !os ? FALSE : TRUE;
}   //TEReset::WriteReset

/*
	Writes an O,P reset to os.  nRoomVnum is the room the object is put in
	at reset.
*/
BOOL TOReset::WriteResets(ofstream& os, int nRoomVnum)
{
	TListIterator<TPReset> PResetListIter(PResetList);

	os << "O 0 " << nVnum << ' ' << nLimit << ' ' << nRoomVnum << "\t;";

	if (Area.FindObj(nVnum))	//can load non-area obj
		os << Area.FindObj(nVnum)->sName;
	else
    	os << "<mon-area object>";

	os << " into " << Area.FindRoom(nRoomVnum)->sName << '\n';

	//write P resets for this obj
	while (int(PResetListIter))
		if (!(PResetListIter++)->WriteReset(os, nVnum))
			return FALSE;

	return !os ? FALSE : TRUE;
}   //TOReset::WriteReset

/*
	Writes a P reset to os.  nAcceptorVnum is the vnum of the container
	object (that was last loaded with 'O' reset.
*/
BOOL TPReset::WriteReset(ofstream& os, int nAcceptorVnum)
{
	os << "P " << nAmount << ' ' << nVnum << ' ' << nLimit << ' ' << nAcceptorVnum << "\t;";

	if (Area.FindObj(nVnum))	//can load non-area objs
		os << Area.FindObj(nVnum)->sName;
	else
		os << "<non-area object>";

	os << " into ";
    if (Area.FindObj(nAcceptorVnum))
		os << Area.FindObj(nAcceptorVnum)->sName;
	else
    	os << "<non-area object>";

	os << '\n';

	return !os ? FALSE : TRUE;
}   //TPReset::WriteReset

/*
	Writes a D reset to os.  nRoomVnum is the vnum of the room with the door.
*/
BOOL TDoor::WriteReset(ofstream& os, int nRoomVnum)
{
	PTRoom pRoom;

	if (nState != -1)
	{
		os << "D 0 " << nRoomVnum << ' ' << nDir << ' ' << nState << "\t;";
		pRoom = Area.FindRoom(nRoomVnum);
		os << "Door with keywords: (" << sKeywords << ") in " << pRoom->sName << '\n';
	}

	return !os ? FALSE : TRUE;
}   //TDoor::WriteReset

/*
	Writes an old-style mob to os.  Assumes "#MOBOLD" has been written.
*/
BOOL TMobOld::Write(ofstream& os)
{
	KillNewline(sKeywords);
	KillNewline(sShortDesc);
	AddNewline(sLongDesc);
	AddNewline(sDesc);

	os << '#' << nVnum << '\n' <<
	 sKeywords << "~\n" <<
	 sShortDesc << "~\n" <<
	 sLongDesc << "~\n" <<
	 sDesc << "~\n" <<
	 ActionFlg << ' ' << AffectFlg << ' ' << nAlign << " S\n" <<
	 nLevel;

	os << " 0 0 0d0+0 0d0+0 \n" <<
	 lGold << " 0\n" <<
	 nStartPos << ' ' << nDefPos << ' ' << nSex << "\n\n";

	if (!os)
	{
		osLog << szSections[nMOBOLD] << "Error writing file\n";
		return FALSE;
	}

	return TRUE;
}   //TMobOld::Write

/*
	Writes an old-style object to os.  Assumes "#OBJOLD" has been written.
*/
BOOL TObjOld::Write(ofstream& os)
{
	TListIterator<TESection> EListIter(EList);
	TListIterator<TASection> AListIter(AList);

	KillNewline(sKeywords);
	KillNewline(sName);
	KillNewline(sLongDesc);

	os << '#' << nVnum << '\n' <<
	 sKeywords << "~\n" <<
	 sName << "~\n" <<
	 sLongDesc << "~\n~\n" <<
	 nType << ' ' << Flags << ' ' << WearFlags << '\n' <<
	 lValues[0] << ' ' << lValues[1] << ' ' << lValues[2] << ' ' <<
	 lValues[3] << " \n" <<
	 nWeight << ' ' << lValue << " 0\n";

	while (int(EListIter))
		(EListIter++)->Write(os);

	while (int(AListIter))
		(AListIter++)->Write(os);

	if (!os)
	{
		osLog << szSections[nOBJOLD] << "Error writing file\n";
		return FALSE;
	}
	else
		os << '\n';

	return TRUE;
}   //TObjOld::Write

/*
	Writes the ASCII equivalent of Flags to os.
*/
ofstream& operator<<(ofstream& os, TFlags& Flags)
{
	char sz[40];

	Flags.Get(sz);
	os << sz;

	return os;
}   //operator<<

/*
	Reads in a single social.  Assumes the "#SOCIALS" has been consumed.
*/
BOOL TSocial::Read(ifstream &is)
{
	char sz[256], ch;
    int i=0;

	is >> sz;
	sName = sz;
	is.ignore(256, '\n');

	while (is.peek() != '#' && i < 8)
	{
		is.getline(sz, 256);	//read in a social line
		if (sz[0] == '$' && (!sz[1] || isspace(sz[1])))	//social line?
            sStrs[i] = "";	//no text for that part of social
		else
		{
			sStrs[i] = sz;
			KillNewline(sStrs[i]);
        }

		i++;
	}

	is >> ch;
	if (ch != '#' || is.peek() == '0')	//if last social, we ate the '#' from the "#0"
		is.putback(ch);
		    
	if (!is.good())
		return FALSE;

	//log success
	osLog << szSections[nSOCIALS] << "Successful read.  Social: " << sName << '\n';
	return TRUE;
}	//TSocial::Read

BOOL TSocial::Write(ofstream &os)
{
	int i, nLast;

	os << sName << '\n';

	for (i=0; i<8; i++)
		if (sStrs[i].Len())
			nLast = i;

	for (i=0; i<=nLast; i++)
		if (sStrs[i].Len())
		{
			os << sStrs[i] << '\n';
		}
		else
			os << "$\n";		//no social text for slot

	if (nLast != 7)	//didn't use all slots?
		os << "#\n";

	os << '\n';	//line between each social

	if (!os)
	{
		osLog << szSections[nSOCIALS] << "Error writing file\n";
		return FALSE;
	}

	return TRUE;
}	//TSocial::Write
