/*
	SANDBOX3.CPP

	Copyright (c) 1995  Shawn Halpenny
	All rights reserved.

	Support module for SandBox.
*/

#include <wolf.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <mem.h>
#include "stream.h"
#include "areaio.h"

#pragma hdrstop
#include "sandbox.h"

TWeaponDlg::TWeaponDlg(PTWindowObject pParent, int nID, PTBaseObj pO, int nT, long* l) : TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
	lValues = l;
	pObj = pO;

	new TComboBox(this, CBID_WEAPONCLASS);
	new TComboBox(this, CBID_DAMTYPE);

	pLB = new TListBox(this, LBID_WEAPONTYPE);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	new TEdit(this, EID_NUMDICE, 6);
    new TEdit(this, EID_DICETYPE, 6);
}	//TWeaponDlg::TWeaponDlg

LRESULT TWeaponDlg::InitDialog()
{
    PTListBox pLB;
	PTComboBox pCB;
	PTEdit pE;

    pCB = (PTComboBox) GetChild(CBID_WEAPONCLASS);
	WeaponClassList.ForEach(FillBox, pCB);
	SelectCB(pCB, WeaponClassList, lValues[0]);
    pCB->SetFocus();

	pCB = (PTComboBox) GetChild(CBID_DAMTYPE);
	DamTypeList.ForEach(FillBox, pCB);
	SelectCB(pCB, DamTypeList, lValues[3]);

	pLB = (PTListBox) GetChild(LBID_WEAPONTYPE);
	if (pObj->Type() == NEW)
	{
		TFlags F;

		WeaponTypeList.ForEach(FillBox, pLB);
        F.Set(lValues[4]);
		SelectLB(pLB, WeaponTypeList, F);
	}
	else
    	pCB->Enable(FALSE);

	pE = (PTEdit) GetChild(EID_NUMDICE);
	SetDlgItemLong(hWindow, EID_NUMDICE, lValues[1], FALSE);
	pE->LimitText(5);

	pE = (PTEdit) GetChild(EID_DICETYPE);
	SetDlgItemLong(hWindow, EID_DICETYPE, lValues[2], FALSE);
	pE->LimitText(5);

	return 0;
}	//TWeaponDlg::InitDialog

void TWeaponDlg::Okay(RTMessage Msg)
{
    PTListBox pLB;
	PTComboBox pCB = (PTComboBox) GetChild(CBID_WEAPONCLASS);
    char sz[256];

	GetBoxSel(pCB, WeaponClassList, sz, 256);
	lValues[0] = strtol(sz, NULL, 10);

    pCB = (PTComboBox) GetChild(CBID_DAMTYPE);
	GetBoxSel(pCB, DamTypeList, sz, 256);
	lValues[3] = strtol(sz, NULL, 10);

	pLB = (PTListBox) GetChild(LBID_WEAPONTYPE);
	if (pObj->Type() == NEW)
	{
		TFlags F;

		GetBoxFlags(pLB, WeaponTypeList, F);
    	lValues[4] = F.Get();
	}

	lValues[1] = GetDlgItemLong(hWindow, EID_NUMDICE, NULL, FALSE);
	lValues[2] = GetDlgItemLong(hWindow, EID_DICETYPE, NULL, FALSE);

	//chain to original
	TDialog::Okay(Msg);
}	//TWeaponDlg::Okay

TGenericDlg::TGenericDlg(PTWindowObject pParent, int nID, PTBaseObj pO, char *psz, int nT, long* lV) :
 TDialog(pParent, nID) 
{
	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
	lValues = lV;
	pObj = pO;
    pszName = psz;

	new TEdit(this, EID_V0, 256);
    new TEdit(this, EID_V1, 256);
	new TEdit(this, EID_V2, 256);
    new TEdit(this, EID_V3, 256);
	new TEdit(this, EID_V4, 256);
}	//TGenericDlg::TGenericDlg

LRESULT TGenericDlg::InitDialog()
{
	PTEdit pE;
	char sz[256];

	sprintf(sz, "Values for \"%s\"", pszName);
    SetText(sz);

	pE = (PTEdit) GetChild(EID_V0);
    SetDlgItemLong(hWindow, EID_V0, lValues[0], TRUE);
	pE->LimitText(255);
    pE->SetFocus();

	pE = (PTEdit) GetChild(EID_V1);
    SetDlgItemLong(hWindow, EID_V1, lValues[1], TRUE);
	pE->LimitText(255);

	pE = (PTEdit) GetChild(EID_V2);
    SetDlgItemLong(hWindow, EID_V2, lValues[2], TRUE);
	pE->LimitText(255);

	pE = (PTEdit) GetChild(EID_V3);
    SetDlgItemLong(hWindow, EID_V3, lValues[3], TRUE);
	pE->LimitText(255);

	if (pObj->Type() == NEW)
    	SetDlgItemLong(hWindow, EID_V4, lValues[4], TRUE);
	else
    	GetChild(EID_V4)->Enable(FALSE);

	return 0;
}	//TGenericDlg::InitDialog

void TGenericDlg::Okay(RTMessage Msg)
{
	char sz[256];
    TFlags F;

    //we'll translate all the values as flags, since that's what they are
	GetDlgItemText(hWindow, EID_V0, sz, 256);
    Killws(sz);	//can't have whitespace in flags
	F.Set(sz);
    lValues[0] = F.Get();

	GetDlgItemText(hWindow, EID_V1, sz, 256);
    Killws(sz);
	F.Set(sz);
    lValues[1] = F.Get();

	GetDlgItemText(hWindow, EID_V2, sz, 256);
    Killws(sz);
	F.Set(sz);
    lValues[2] = F.Get();

	GetDlgItemText(hWindow, EID_V3, sz, 256);
    Killws(sz);
	F.Set(sz);
    lValues[3] = F.Get();

    if (pObj->Type() == NEW)
	{
		GetDlgItemText(hWindow, EID_V4, sz, 256);
	    Killws(sz);
		F.Set(sz);
		lValues[4] = F.Get();
    }

	//chain to original
	TDialog::Okay(Msg);	
}	//TGenericDlg::Okay

/*
	Removes all whitespace from psz.
*/
void Killws(char *psz)
{
	char *pCur = psz;

	while (*psz)
		if (isspace(*psz))
			psz++;
		else
		{
			*pCur = *psz;
			pCur++;
        }

	*pCur = 0;
}	//Killws

void SetDlgItemLong(HWND hWindow, int nID, long lVal, BOOL bSign)
{
	char sz[40];

    if (bSign)
		sprintf(sz, "%ld", lVal);
	else
		sprintf(sz, "%lu", lVal);

    SetDlgItemText(hWindow, nID, sz);
}	//SetDlgItemLong

long GetDlgItemLong(HWND hWindow, int nID, void*, BOOL bSign)
{
	char sz[40];

	GetDlgItemText(hWindow, nID, sz, 40);
    return bSign ? strtol(sz, NULL, 10) : strtoul(sz, NULL, 10);
}	//GetDlgItemLong

TFoodDlg::TFoodDlg(PTWindowObject pParent, int nID, int nT, long* lV) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
    lValues = lV;

	new TEdit(this, EID_HOURS, 6);
    new TCheckBox(this, CHID_POISONED);
}	//TFoodDlg::TFoodDlg

LRESULT TFoodDlg::InitDialog()
{
	PTEdit pE = (PTEdit) GetChild(EID_HOURS);

	SetDlgItemLong(hWindow, EID_HOURS, lValues[0], FALSE);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	PTCheckBox pCH = (PTCheckBox) GetChild(CHID_POISONED);
    pCH->SetCheck(lValues[3] ? BF_CHECKED : BF_UNCHECKED);

    return 0;
}	//TFoodDlg::InitDialog

void TFoodDlg::Okay(RTMessage Msg)
{
	lValues[0] = GetDlgItemLong(hWindow, EID_HOURS, NULL, FALSE);

	PTCheckBox pCH = (PTCheckBox) GetChild(CHID_POISONED);
    lValues[3] = (pCH->GetCheck() == BF_CHECKED ? 1 : 0); 

	//chain to orig
    TDialog::Okay(Msg);
}	//TFoodDlg::Okay

TContainerDlg::TContainerDlg(PTWindowObject pParent, int nID, int nT, long* lV) :
 TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
	lValues = lV;

	new TEdit(this, EID_CAPACITY, 6);
	new TEdit(this, EID_KEY, 6);
	pLB = new TListBox(this, LBID_FLAGS);
    pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);
}	//TContainerDlg::TContainerDlg

LRESULT TContainerDlg::InitDialog()
{
    PTEdit pE = (PTEdit) GetChild(EID_CAPACITY);
	PTListBox pLB = (PTListBox) GetChild(LBID_FLAGS);
    TFlags F;

	SetDlgItemLong(hWindow, EID_CAPACITY, lValues[0], FALSE);
    pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_KEY);
	SetDlgItemLong(hWindow, EID_KEY, lValues[2], TRUE);
	pE->LimitText(5);

	ContFlagsList.ForEach(FillBox, pLB);
    F.Set(lValues[1]);
    SelectLB(pLB, ContFlagsList, F);

	return 0;
}	//TContainerDlg::InitDialog

void TContainerDlg::Okay(RTMessage Msg)
{
	TFlags F;

	lValues[0] = GetDlgItemLong(hWindow, EID_CAPACITY, NULL, FALSE);
	lValues[2] = GetDlgItemLong(hWindow, EID_KEY, NULL, TRUE);

	GetBoxFlags((PTListBox) GetChild(LBID_FLAGS), ContFlagsList, F);
    lValues[1] = F.Get();

	//chain to orig
    TDialog::Okay(Msg);
}	//TContainerDlg::Okay

TDrinkDlg::TDrinkDlg(PTWindowObject pParent, int nID, int nT, long* lV) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
	lValues = lV;

	new TEdit(this, EID_CAPACITY, 6);
	new TEdit(this, EID_CURCAPACITY, 6);
    new TCheckBox(this, CHID_POISONED);
    new TComboBox(this, CBID_LIQUIDTYPE);
}	//TDrinkDlg::TDrinkDlg

LRESULT TDrinkDlg::InitDialog()
{
	PTEdit pE = (PTEdit) GetChild(EID_CAPACITY);

	SetDlgItemLong(hWindow, EID_CAPACITY, lValues[0], FALSE);
	pE->SetSel(0, 32767);
	pE->LimitText(5);
	pE->SetFocus();

    pE = (PTEdit) GetChild(EID_CURCAPACITY);
	SetDlgItemLong(hWindow, EID_CURCAPACITY, lValues[1], FALSE);
    pE->LimitText(5);

	PTComboBox pCB = (PTComboBox) GetChild(CBID_LIQUIDTYPE);
	LiquidList.ForEach(FillBox, pCB);
	SelectCB(pCB, LiquidList, lValues[2]);

	((PTCheckBox) GetChild(CHID_POISONED))->SetCheck(lValues[3] ? BF_CHECKED : BF_UNCHECKED);

    return 0;
}	//TDrinkDlg::InitDialog

void TDrinkDlg::Okay(RTMessage Msg)
{
	char sz[256];

	lValues[0] = GetDlgItemLong(hWindow, EID_CAPACITY, NULL, FALSE);
	lValues[1] = GetDlgItemLong(hWindow, EID_CURCAPACITY, NULL, FALSE);
	GetBoxSel((PTListBox) GetChild(CBID_LIQUIDTYPE), LiquidList, sz, 256);
	lValues[2] = strtol(sz, NULL, 10);
	lValues[3] = ((PTCheckBox) GetChild(CHID_POISONED))->GetCheck() == BF_CHECKED ? 1 : 0; 

	//chain to orig
    TDialog::Okay(Msg);
}	//TDrinkDlg::Okay

#ifndef ET
TPortalDlg::TPortalDlg(PTWindowObject pParent, int nID, int nT, long* lV) : TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
	lValues = lV;

	new TEdit(this, EID_NUMUSES, 6);
	new TEdit(this, EID_VNUM, 6);
	pLB = new TListBox(this, LBID_STATE);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_TYPE);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);
}	//TPortalDlg::TPortalDlg

LRESULT TPortalDlg::InitDialog()
{
    PTEdit pE = (PTEdit) GetChild(EID_NUMUSES);
	PTListBox pLB;
    TFlags F;

	SetDlgItemLong(hWindow, EID_NUMUSES, lValues[0], FALSE);
    pE->LimitText(5);

	SetDlgItemLong(hWindow, EID_VNUM, lValues[3], TRUE);
    pE = (PTEdit) GetChild(EID_VNUM);
	pE->LimitText(5);

	pLB = (PTListBox) GetChild(LBID_STATE);
	ExitTypeList.ForEach(FillBox, pLB);
    F.Set(lValues[1]);
	SelectLB(pLB, ExitTypeList, F);
    pLB->SetFocus();

	pLB = (PTListBox) GetChild(LBID_TYPE);
	PortalTypeList.ForEach(FillBox, pLB);
    F.Set(lValues[2]);
	SelectLB(pLB, PortalTypeList, F);

	return 0;
}	//TPortalDlg::InitDialog

void TPortalDlg::Okay(RTMessage Msg)
{
	TFlags F;

	lValues[3] = GetDlgItemLong(hWindow, EID_VNUM, NULL, TRUE);
	lValues[0] = GetDlgItemLong(hWindow, EID_NUMUSES, NULL, FALSE);

	GetBoxFlags((PTListBox) GetChild(LBID_STATE), ExitTypeList, F);
	lValues[1] = F.Get();

	GetBoxFlags((PTListBox) GetChild(LBID_TYPE), PortalTypeList, F);
	lValues[2] = F.Get();

	//chain to orig
    TDialog::Okay(Msg);
}	//TPortalDlg::Okay
#endif //ET

TAdjustDlg::TAdjustDlg(PTWindowObject pParent, int nID, char *p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	psz = p;

    new TEdit(this, EID_ADJUSTBY, 7);
}	//TAdjustDlg::TAdjustDlg

LRESULT TAdjustDlg::InitDialog()
{
    PTEdit pE;
	char *p = (psz + lstrlen(psz) - 1);

	if (!isdigit(*p))	//if last char isn't digit, new adjustment
		SetDlgItemInt(hWindow, EID_ADJUSTBY, 0, TRUE);
	else
    {
		//as long as last chars in str are digits, we are fine
		while (p > psz && isdigit(*p))
			p--;

    	//p points to '-' or '+'
		SetDlgItemInt(hWindow, EID_ADJUSTBY, atoi(p), TRUE);

		*(p - 1) = 0;	//remove the single space
	}

	SetText(psz);

	pE = (PTEdit) GetChild(EID_ADJUSTBY);
	pE->SetSel(0, 32767);
    pE->LimitText(6);
	pE->SetFocus();
	return 0;
}	//TAdjustDlg::InitDialog

void TAdjustDlg::Okay(RTMessage Msg)
{
	int n;
    char sz[256];

    //Can't use GetDlgItemInt since it doesn't understand '+'
	n = GetDlgItemText(hWindow, EID_ADJUSTBY, sz, 256);
	n = atoi(sz);

	GetText(sz, 256);
    if (n != 0)
		sprintf(psz, "%s %+d", sz, n);
	else
		lstrcpy(psz, sz);

	//chain to original
    TDialog::Okay(Msg);
}	//TAdjustDlg::Okay

TExDescDlg::TExDescDlg(PTWindowObject pParent, int nID, PTESection p) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
	pES = p;

	new TEdit(this, EID_KEYWORDS, 256);
    new TEdit(this, EID_DESC, 4096);
}	//TExDescDlg::TExDescDlg

LRESULT TExDescDlg::InitDialog()
{
	PTEdit pE = (PTEdit) GetChild(EID_KEYWORDS);

    pE->SetText(pES->sKeywords);
	pE->LimitText(255);
    pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_DESC);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->SetText(pES->sDesc);
    pE->LimitText(4095);

	return 0;
}	//TExDescDlg::InitDialog

void TExDescDlg::Okay(RTMessage Msg)
{
	char sz[4096];

	GetChild(EID_KEYWORDS)->GetText(sz, 256);
	if (!sz[0])	//don't want null keywords
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Extended Description",
		 "You must have a keyword string.");
		return;
	}

	pES->sKeywords = sz;

	GetChild(EID_DESC)->GetText(sz, 4096);
	pES->sDesc = sz;

	//chain to original
    TDialog::Okay(Msg);
}	//TExDescDlg::Okay

TDoorDlg::TDoorDlg(PTWindowObject pParent, int nID, PTDoor p, const char *psz) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
	pDoor = p;
    pszDir = psz;

	new TEdit(this, EID_KEY, 6);
    new TEdit(this, EID_TOROOM, 6);
	new TComboBox(this, CBID_DOORTYPE);
	new TComboBox(this, CBID_DOORRESET);
	new TEdit(this, EID_KEYWORDS, 256);
    new TEdit(this, EID_DESC, 4096);
}	//TDoorDlg::TDoorDlg

LRESULT TDoorDlg::InitDialog()
{
	PTEdit pE;
	PTComboBox pCB;

    SetText(pszDir);

	pE = (PTEdit) GetChild(EID_TOROOM);
	SetDlgItemInt(hWindow, EID_TOROOM, pDoor->nToRoomVnum, FALSE);
    pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_KEY);
	SetDlgItemInt(hWindow, EID_KEY, pDoor->nKey, TRUE);
    pE->LimitText(5);

	pCB = (PTComboBox) GetChild(CBID_DOORTYPE);
	DoorTypeList.ForEach(FillBox, pCB);
	SelectCB(pCB, DoorTypeList, pDoor->nLock);

	pCB = (PTComboBox) GetChild(CBID_DOORRESET);
	DoorStateList.ForEach(FillBox, pCB);
	SelectCB(pCB, DoorStateList, pDoor->nState);

	pE = (PTEdit) GetChild(EID_KEYWORDS);
	pE->LimitText(255);
	pE->SetText(pDoor->sKeywords);

	pE = (PTEdit) GetChild(EID_DESC);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->LimitText(4095);
    pE->SetText(pDoor->sDesc);

	return 0;
}	//TDoorDlg::InitDialog

void TDoorDlg::Okay(RTMessage Msg)
{
    PTComboBox pCB;
	char sz[4096];

	pDoor->nToRoomVnum = GetDlgItemInt(hWindow, EID_TOROOM, NULL, FALSE);
	pDoor->nKey = GetDlgItemInt(hWindow, EID_KEY, NULL, TRUE);

	pCB = (PTComboBox) GetChild(CBID_DOORTYPE);
	GetBoxSel(pCB, DoorTypeList, sz, 256);
    pDoor->nLock = atoi(sz);

	pCB = (PTComboBox) GetChild(CBID_DOORRESET);
	GetBoxSel(pCB, DoorStateList, sz, 256);
    pDoor->nState = atoi(sz);

	GetChild(EID_KEYWORDS)->GetText(sz, 256);
	pDoor->sKeywords = sz;

	GetChild(EID_DESC)->GetText(sz, 4096);
    pDoor->sDesc = sz;

	//chain to original
    TDialog::Okay(Msg);
}	//TDoorDlg::Okay

TRoomAttrDlg::TRoomAttrDlg(PTWindowObject pParent, int nID, PTRoom p) : TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
    pRoom = p;

	new TEdit(this, EID_VNUM, 6);
	new TCheckBox(this, CHID_RANDEXIT);
	new TComboBox(this, CBID_RANDEXIT);
	new TEdit(this, EID_NAME, 256);
	new TEdit(this, EID_DESC, 4096);

	pLB = new TListBox(this, LBID_FLAGS);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	new TComboBox(this, CBID_TERRAIN);
	new TListBox(this, LBID_DOORS);
	new TListBox(this, LBID_EXDESC);
	new TListBox(this, LBID_MRESETS);
    new TListBox(this, LBID_ORESETS);
}	//TRoomAttrDlg::TRoomAttrDlg

LRESULT TRoomAttrDlg::InitDialog()
{
	PTEdit pE;
	PTComboBox pCB;
	PTListBox pLB;
    char sz[512], sz2[512];

	SetDlgItemInt(hWindow, EID_VNUM, pRoom->nVnum, FALSE);
	pE = (PTEdit) GetChild(EID_VNUM);
	pE->SetSel(0, 32767);
    pE->LimitText(5);
    pE->SetFocus();

	pCB = (PTComboBox) GetChild(CBID_RANDEXIT);
	DirectionsList.ForEach(FillBox, pCB);
	if (pRoom->nRReset != NO_R_RESET)	//we have an 'R' reset for this room
	{
		//NOTE:  it's very important to not change the order of the directions in values.dat
		SelectCB(pCB, DirectionsList, pRoom->nRReset);
		((PTCheckBox) GetChild(CHID_RANDEXIT))->SetCheck(BF_CHECKED);
	}

	pE = (PTEdit) GetChild(EID_NAME);
	pE->LimitText(255);
	pE->SetText(pRoom->sName);

	pE = (PTEdit) GetChild(EID_DESC);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->LimitText(4095);
    pE->SetText(pRoom->sDesc);

	pLB = (PTListBox) GetChild(LBID_FLAGS);
	RoomFlagsList.ForEach(FillBox, pLB);
	SelectLB(pLB, RoomFlagsList, pRoom->Flags);

	pCB = (PTComboBox) GetChild(CBID_TERRAIN);
	TerrainList.ForEach(FillBox, pCB);
	SelectCB(pCB, TerrainList, pRoom->nTerrain);

	//put exdescs in internal TESection list so mods can be done without changing pRoom's list
	TListIterator<TESection> EDIter(pRoom->EList);
    PTESection pES, pEOld;
    pLB = (PTListBox) GetChild(LBID_EXDESC);
	while (int(EDIter))
	{
        pES = EDIter++;
		pLB->AddString(pES->sKeywords);
		pEOld = new TESection;
		pEOld->sKeywords = pES->sKeywords;	//duplicate the stuff, so that ~TList doesn't kill originals
        pEOld->sDesc = pES->sDesc;
		ExDescList.Add(pEOld);
    }

	//fill door LB
	TListIterator<TValPair> DirIter(DirectionsList);
    TListIterator<TDoor> DoorIter(pRoom->DoorList);
	PTDoor pD;
    PTValPair pV;
	pLB = (PTListBox) GetChild(LBID_DOORS);
	while (int(DirIter))
	{
        pV = DirIter++;
        lstrcpy(sz, pV->sName);

        DoorIter.Restart();
		while (int(DoorIter))
		{
			pD = DoorIter++;
            GetAtomName(pV->Atom, sz2, 512);
			if (pD->nDir == atoi(sz2))	//if this door goes in our current direction
			{
				PTRoom pR = Area.FindRoom(pD->nToRoomVnum);
				sprintf(sz, "%s : (%d) %s", (const char *) pV->sName, pD->nToRoomVnum,
				 pR ? (const char *) pR->sName : "<non-area room>");
				break;	//no more doors in this direction of course
			}
		}

        pLB->AddString(sz);	//add it to the LB
	}

	//and store doors in internal TDoor list
	DoorIter.Restart();
	while (int(DoorIter))
	{
		//duplicate the stuff so that ~TList doesn't kill originals
		pD = new TDoor;
		*pD = *(DoorIter++);
		DoorList.Add(pD);
    }

    //prep the M and O reset LBs
	TListIterator<TMReset> MIter(pRoom->MobResetList);
	pLB = (PTListBox) GetChild(LBID_MRESETS);
	PTBaseMob pMob;
    PTMReset pMReset, pNewM;
	while (int(MIter))
	{
        pMReset = MIter++;
		pMob = Area.FindMob(pMReset->nVnum);
		sprintf(sz, "%d : %s", pMReset->nVnum, pMob ? (const char*) pMob->sShortDesc : "<non-area mobile>");
		pLB->AddString(sz);

		//now dupe each M reset and put in internal list to prevent mod'ing of pRoom's Mresets
		pNewM = new TMReset(pMReset->nVnum, pMReset->nLimit);
		MResetList.Add(pNewM);
		//now dupe each G and E reset for this MReset; blocked so iters are killed out of scope
		{
			TListIterator<TGReset> GIter(pMReset->GResetList);
			PTGReset pGReset, pNewG;

			while (int(GIter))
			{
				pGReset = GIter++;
				pNewG = new TGReset(pGReset->nAmount, pGReset->nVnum, pGReset->nLimit);
				pNewM->AddGReset(pNewG);

				//now dupe each P reset for this GReset; blocked so iterator is killed out of scope
				{
					TListIterator<TPReset> PIter(pGReset->PResetList);
					PTPReset pPReset;

					while (int(PIter))
					{
						pPReset = PIter++;
                        //add P reset to dup'd GReset
						pNewG->AddPReset(new TPReset(pPReset->nAmount, pPReset->nVnum, pPReset->nLimit));
					}
				}
			}

			TListIterator<TEReset> EIter(pMReset->EResetList);
			PTEReset pEReset, pNewE;

			while (int(EIter))
			{
				pEReset = EIter++;
				pNewE = new TEReset(pEReset->nAmount, pEReset->nVnum, pEReset->nLimit, pEReset->nWearLoc);
				pNewM->AddEReset(pNewE);

				//now dupe each P reset for this EReset; blocked so iterator is killed out of scope
				{
					TListIterator<TPReset> PIter(pEReset->PResetList);
					PTPReset pPReset;
					while (int(PIter))
					{
						pPReset = PIter++;
                        //add P reset to dup'd EReset
						pNewE->AddPReset(new TPReset(pPReset->nAmount, pPReset->nVnum, pPReset->nLimit));
					}
				}
			}
		}
    }

	TListIterator<TOReset> OIter(pRoom->ObjResetList);
	pLB = (PTListBox) GetChild(LBID_ORESETS);
	PTBaseObj pObj;
    PTOReset pOReset, pNewO;
	while (int(OIter))
	{
        pOReset = OIter++;
		pObj = Area.FindObj(pOReset->nVnum);
		sprintf(sz, "%d : %s", pOReset->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);

		//now dupe each O reset and put in internal list to prevent mod'ing of pRoom's Oresets
		pNewO = new TOReset(pOReset->nVnum, pOReset->nLimit);
		OResetList.Add(pNewO);
		//now dupe each P reset for this OReset; blocked so iterator is killed out of scope
		{
			TListIterator<TPReset> PIter(pOReset->PResetList);
			PTPReset pPReset;
			while (int(PIter))
			{
				pPReset = PIter++;
				pNewO->AddPReset(new TPReset(pPReset->nAmount, pPReset->nVnum, pPReset->nLimit));
			}
        }
	}

	return 0;
}	//TRoomAttrDlg::InitDialog

void TRoomAttrDlg::Okay(RTMessage Msg)
{
	PTEdit pE;
	PTComboBox pCB;
	PTCheckBox pCH = (PTCheckBox) GetChild(CHID_RANDEXIT);
	PTListBox pLB;
	char sz[4096];
    PTRoom pCheckRoom;

	pRoom->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);

	pCheckRoom = Area.FindRoom(pRoom->nVnum);
	if (pCheckRoom && pCheckRoom != pRoom && pCheckRoom->nVnum == pRoom->nVnum)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Add Room",
		 "A room with vnum = %d already exists.", pRoom->nVnum);
		return;
	}

	if (pCH->GetCheck() == BF_CHECKED)	//'R' reset?
    {
		pCB = (PTComboBox) GetChild(CBID_RANDEXIT);
		GetBoxSel(pCB, DirectionsList, sz, 80);
		pRoom->nRReset = atoi(sz);
	}
	else
    	pRoom->nRReset = NO_R_RESET;

	pE = (PTEdit) GetChild(EID_NAME);
	pE->GetText(sz, 256);
	pRoom->sName = sz;

	pE = (PTEdit) GetChild(EID_DESC);
	pE->GetText(sz, 4096);
	pRoom->sDesc = sz;

	pLB = (PTListBox) GetChild(LBID_FLAGS);
	GetBoxFlags(pLB, RoomFlagsList, pRoom->Flags);

	pCB = (PTComboBox) GetChild(CBID_TERRAIN);
	GetBoxSel(pCB, TerrainList, sz, 80);
	pRoom->nTerrain = atoi(sz);

	//get ex descs
	pRoom->EList.RemoveAll(TRUE);	//kill old ones
	TListIterator<TESection> Iter(ExDescList);
	while (int(Iter))	//iterate over the ex descs in the LB
        pRoom->EList.Add(Iter++);

	//empty the internal list here, so that ~TList doesn't destroy its data (since the data
	//now "belongs" to pRoom->EList) when this is destroyed
    ExDescList.RemoveAll(FALSE);

	//handle doors
	pRoom->DoorList.RemoveAll(TRUE);	//kill old doors
    TListIterator<TDoor> DIter(DoorList);
	while (int(DIter))
		pRoom->DoorList.Add(DIter++);

	//empty the internal list here, so that ~TList doesn't destroy its data (since the data
	//now "belongs" to pRoom->DoorList) when this is destroyed
	DoorList.RemoveAll(FALSE);

	//grab the O and M resets now
	TListIterator<TOReset> OIter(OResetList);
    pRoom->ObjResetList.RemoveAll(TRUE);	//kill old ones
	while (int(OIter))
		pRoom->ObjResetList.Add(OIter++);

	//empty the internal list here, so that ~TList doesn't destroy its data (since the data
	//now "belongs" to pRoom->ObjResetList) when this is destroyed
	OResetList.RemoveAll(FALSE);

	TListIterator<TMReset> MIter(MResetList);
    pRoom->MobResetList.RemoveAll(TRUE);	//kill old ones
	while (int(MIter))
		pRoom->MobResetList.Add(MIter++);

	//empty the internal list here, so that ~TList doesn't destroy its data (since the data
	//now "belongs" to pRoom->MobResetList) when this is destroyed
	MResetList.RemoveAll(FALSE);

	//chain to original
    TDialog::Okay(Msg);
}	//TRoomAttrDlg::Okay

void TRoomAttrDlg::IDDelExDesc(RTMessage)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_EXDESC);
	PTESection pE;
	int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Extra Description",
		 "Select an extra description first.");
		return;
	}

    //remove it from internal list
	pE = ExDescList.nth(n);
    ExDescList.Remove(pE);
    delete pE;
	pLB->DeleteString(n);	//and from LB

	//redraw the LB now
    UpdateWindow(pLB->hWindow);
}	//TRoomAttrDlg::IDDelExDesc

void TRoomAttrDlg::IDAddExDesc(RTMessage)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_EXDESC);
	PTESection pE = new TESection;

	pE->sKeywords = "<none>";
	pE->sDesc = "<none>";

	if (pApplication->ExecDialog(new TExDescDlg(this, ID_EXDESCDLG, pE)) == IDOK)
	{
		//add new
		pLB->AddString(pE->sKeywords);	//to LB
        ExDescList.Add(pE);		//and to internal list

		//redraw LB
		UpdateWindow(pLB->hWindow);
	}
	else
		delete pE;
}	//TRoomAttrDlg::IDAddExDesc

void TRoomAttrDlg::IDExDescLB(RTMessage Msg)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_EXDESC);
    PTESection pE;
    int n = pLB->GetSelIndex();

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
        pE = ExDescList.nth(n);
		if (pApplication->ExecDialog(new TExDescDlg(this, ID_EXDESCDLG, pE)) == IDOK)
		{
            //remove old and insert new
			pLB->DeleteString(n);
			pLB->InsertString(pE->sKeywords, n);

			//redraw LB
			UpdateWindow(pLB->hWindow);
		}
	}
}	//TRoomAttrDlg::IDExDescLB

void TRoomAttrDlg::IDDoorLB(RTMessage Msg)
{
    PTListBox pLB;
	PTDoor pD = NULL;
    PTValPair pV;
	TListIterator<TDoor> Iter(DoorList);	//iterate over internal door list
	char sz[256];
	BOOL bNewDoor = TRUE;
    int n;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		//find the correct door
		pLB = (PTListBox) GetChild(LBID_DOORS);
		GetBoxSel(pLB, DirectionsList, sz, 256);
		while (int(Iter))
		{
			pD = Iter++;
			if (pD->nDir == atoi(sz))	//this is the one
			{
                bNewDoor = FALSE;
				break;
            }
		}

		if (bNewDoor)	//no door in list
		{
			//so make a new one
			pD = new TDoor;

			pV = DoorStateList.nth(0);	//default reset door state
			GetAtomName(pV->Atom, sz, 256);
			pD->nState = atoi(sz);

			pD->nDir = atoi(sz);
			pD->sDesc = "<none>";
			pD->sKeywords = "<none>";

            //default door type
			pV = DoorTypeList.nth(0);
			GetAtomName(pV->Atom, sz, 256);
			pD->nLock = atoi(sz);

			pD->nKey = -1;
			pD->nToRoomVnum = 0;
		}

		n = pLB->GetSelIndex();
        pV = DirectionsList.nth(n);
		if (pApplication->ExecDialog(new TDoorDlg(this, ID_DOORDLG, pD, (const char*) pV->sName)) == IDOK)
		{
			if (bNewDoor)
            	DoorList.Add(pD);

            //update door LB
			PTRoom pR = Area.FindRoom(pD->nToRoomVnum);
			sprintf(sz, "%s : (%d) %s", (const char *) pV->sName, pD->nToRoomVnum,
			 pR ? (const char *) pR->sName : "<non-area room>");  //exist or not?

			pLB->DeleteString(n);
			pLB->InsertString(sz, n);
		}
		else
		if (bNewDoor)
			delete pD; 
	}
}	//TRoomAttrDlg::IDDoorLB

void TRoomAttrDlg::IDDelDoor(RTMessage)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_DOORS);
	int n = pLB->GetSelIndex();
    PTDoor pD;
    PTValPair pV;
    char sz[256];

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Door",
		 "Select a door first.");
		return;
	}

    //remove it from internal list
	//find the correct door
	pLB = (PTListBox) GetChild(LBID_DOORS);
	GetBoxSel(pLB, DirectionsList, sz, 256);
	TListIterator<TDoor> Iter(DoorList);
    BOOL bFound = FALSE;
	while (int(Iter))
	{
		pD = Iter++;
		if (pD->nDir == atoi(sz))	//this is the one
		{
            bFound = TRUE;
			break;
        }
	}

    if (bFound)
	{
	    DoorList.Remove(pD);	//remove from list
		delete pD;	//kill it
		pLB->DeleteString(n);	//and from LB

		pV = DirectionsList.nth(n);	//get the name of the direction we del'd
		pLB->InsertString(pV->sName, n); //and put it back in list
	}
	else
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Door",
		 "There is no door in that direction.");
		return;
    }

	//redraw the LB now
    UpdateWindow(pLB->hWindow);
}	//TRoomAttrDlg::IDDelDoor

void TRoomAttrDlg::IDMResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTMReset pM;
    PTBaseMob pMob;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_MRESETS);
		n = pLB->GetSelIndex();
		pM = MResetList.nth(n);

		if (pApplication->ExecDialog(new TMobResetDlg(this, ID_MRESETDLG, pM)) == IDOK)
		{
			//redraw list to show change
			pMob = Area.FindMob(pM->nVnum);
			sprintf(sz, "%d : %s", pM->nVnum, pMob ? (const char*) pMob->sShortDesc : "<non-area mobile>");
            pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TRoomAttrDlg::IDMResetLB

void TRoomAttrDlg::IDOResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTOReset pO;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_ORESETS);
		n = pLB->GetSelIndex();
		pO = OResetList.nth(n);

		if (pApplication->ExecDialog(new TObjResetDlg(this, ID_ORESETDLG, pO)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pO->nVnum);
			sprintf(sz, "%d : %s", pO->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TRoomAttrDlg::IDOResetLB

void TRoomAttrDlg::IDDelMReset(RTMessage)
{
	PTMReset pM;
	PTListBox pLB = (PTListBox) GetChild(LBID_MRESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'M' Reset",
		 "First select a mobile reset from the list.");
		return;
    }

    //remove it from the internal list
	pM = MResetList.nth(n);
    MResetList.Remove(pM);
    delete pM;	//and kill it (the G and E resets will be killed too)
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TRoomAttrDlg::IDDelMReset

void TRoomAttrDlg::IDDelOReset(RTMessage)
{
	PTOReset pO;
	PTListBox pLB = (PTListBox) GetChild(LBID_ORESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'O' Reset",
		 "First select an object reset from the list.");
		return;
    }

    //remove it from the internal list
	pO = OResetList.nth(n);
    OResetList.Remove(pO);
    delete pO;	//and kill it (the P resets will be killed too)
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TRoomAttrDlg::IDDelOReset

void TRoomAttrDlg::IDAddMReset(RTMessage)
{
	PTMReset pM = new TMReset(0, -1);
	PTBaseMob pMob;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_MRESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TMobResetDlg(this, ID_MRESETDLG, pM)) == IDOK)
	{
        //save the changes
        MResetList.Add(pM);
		pMob = Area.FindMob(pM->nVnum);
		sprintf(sz, "%d : %s", pM->nVnum, pMob ? (const char*) pMob->sShortDesc : "<non-area mobile>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
    	delete pM;
}	//TRoomAttrDlg::IDAddMReset

void TRoomAttrDlg::IDAddOReset(RTMessage)
{
	PTOReset pO = new TOReset(0, -1);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_ORESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TObjResetDlg(this, ID_ORESETDLG, pO)) == IDOK)
	{
        //save the changes
        OResetList.Add(pO);
		pObj = Area.FindObj(pO->nVnum);
		sprintf(sz, "%d : %s", pO->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
		delete pO;
}	//TRoomAttrDlg::IDAddOReset
