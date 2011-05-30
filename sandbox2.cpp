/*
	SANDBOX2.CPP

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

TFlagDlg::TFlagDlg(PTWindowObject pParent, int nID, PTBaseMob pM, PTFlags p1, PTFlags p2,
 PTFlags p3, PTFlags p4, PTFlags p5, PTFlags p6, PTFlags p7, PTFlags p8) : TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
    pMob = pM;
	pAct = p1;
	pAff = p2;
	pOff = p3;
	pImm = p4;
	pRes = p5;
	pVuln = p6;
	pForm = p7;
	pParts = p8;

	pLB = new TListBox(this, LBID_ACTION);
	//setting multi sel flag even though its a dlg control cuz the program has to
    //determine if it's a multisel box
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_AFFBY);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_OFFENSIVE);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_IMMUNE);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_RESIST);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_VULN);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_FORM);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_PARTS);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);
}	//TFlagDlg::TFlagDlg

LRESULT TFlagDlg::InitDialog()
{
	PTListBox pLB;

    pLB = (PTListBox) GetChild(LBID_ACTION);
	ActionList.ForEach(FillBox, pLB);
	SelectLB(pLB, ActionList, *pAct);
	pLB->SetFocus();

    pLB = (PTListBox) GetChild(LBID_AFFBY);
	AffByList.ForEach(FillBox, pLB);
    SelectLB(pLB, AffByList, *pAff);

	if (pMob->Type() == NEW)
    {
		pLB = (PTListBox) GetChild(LBID_OFFENSIVE);
		OffensiveList.ForEach(FillBox, pLB);
		SelectLB(pLB, OffensiveList, *pOff);

		pLB = (PTListBox) GetChild(LBID_IMMUNE);
		ImmuneList.ForEach(FillBox, pLB);
		SelectLB(pLB, ImmuneList, *pImm);

		pLB = (PTListBox) GetChild(LBID_RESIST);
		ResistList.ForEach(FillBox, pLB);
		SelectLB(pLB, ResistList, *pRes);

		pLB = (PTListBox) GetChild(LBID_VULN);
		VulnList.ForEach(FillBox, pLB);
		SelectLB(pLB, VulnList, *pVuln);

		pLB = (PTListBox) GetChild(LBID_FORM);
		FormsList.ForEach(FillBox, pLB);
		SelectLB(pLB, FormsList, *pForm);

		pLB = (PTListBox) GetChild(LBID_PARTS);
		PartsList.ForEach(FillBox, pLB);
		SelectLB(pLB, PartsList, *pParts);
	}
	else
	{
		GetChild(LBID_OFFENSIVE)->Enable(FALSE);
		GetChild(LBID_IMMUNE)->Enable(FALSE);
		GetChild(LBID_RESIST)->Enable(FALSE);
		GetChild(LBID_VULN)->Enable(FALSE);
		GetChild(LBID_FORM)->Enable(FALSE);
		GetChild(LBID_PARTS)->Enable(FALSE);
	}

	return 0;
}	//TFlagDlg::InitDialog

void TFlagDlg::Okay(RTMessage Msg)
{
	GetBoxFlags((PTListBox) GetChild(LBID_ACTION), ActionList, *pAct);
	GetBoxFlags((PTListBox) GetChild(LBID_AFFBY), AffByList, *pAff);
	if (pMob->Type() == NEW)
    {
		GetBoxFlags((PTListBox) GetChild(LBID_OFFENSIVE), OffensiveList, *pOff);
		GetBoxFlags((PTListBox) GetChild(LBID_IMMUNE), ImmuneList, *pImm);
		GetBoxFlags((PTListBox) GetChild(LBID_RESIST), ResistList, *pRes);
		GetBoxFlags((PTListBox) GetChild(LBID_VULN), VulnList, *pVuln);
		GetBoxFlags((PTListBox) GetChild(LBID_FORM), FormsList, *pForm);
		GetBoxFlags((PTListBox) GetChild(LBID_PARTS), PartsList, *pParts);
	}

	//chain to original
    TDialog::Okay(Msg);
}	//TFlagDlg::Okay

/*
	Same as other two fns, but takes a TFlags and selects all the right things.
*/
void SelectLB(PTListBox pLB, TList<TValPair>& L, TFlags& f)
{
	char sz[2], *p, szFlags[256];

	f.Get(szFlags);
	p = szFlags;
    sz[1] = 0;

	while (*p)
	{
		sz[0] = *p;
		SelectLB(pLB, L, sz);
        p++;
    }
}	//SelectLB

/*
	Takes a multiple selection list box.  The values of all selected items from L are put
    into F.
*/
void GetBoxFlags(PTListBox pLB, TList<TValPair>& L, TFlags& F)
{
	char sz[40], szFlg[40] = "";
	int i, n, *pnIndices;

	n = pLB->GetSelCount();
	if (n == 0)
	{
		F.Set((long) 0);
		return;
	}

    pnIndices = new int[n];
	pLB->GetSelIndices(pnIndices, n);
	for (i=0; i<n; i++)
	{
		GetBoxSel(pLB, L, sz, 256, pnIndices[i]);
        lstrcat(szFlg, sz);
	}

    F.Set(szFlg);
}	//GetBoxFlags

/*
	Invalidates and updates pLB.  Also sets the window text on the pLB's parent window to
	reflect the number of items in pLB.  If bRedraw is TRUE, the LB is redrawn.  
*/
void UpdateLB(PTListBox pLB, BOOL bRedraw)
{
	char sz[256];
    int n = pLB->GetCount();

    if (bRedraw)
	{
		InvalidateRect(pLB->hWindow, NULL, TRUE);
		UpdateWindow(pLB->hWindow);
    }

	if (pLB == pMobLB)
	{
        sprintf(sz, "Mobiles : %d", n);
		pLB->pParent->SetText(sz);
	}
	else
	if (pLB == pRoomLB)
	{
		sprintf(sz, "Rooms : %d", n);
		pLB->pParent->SetText(sz);
	}
	else
	if (pLB == pObjLB)
	{
		sprintf(sz, "Objects : %d", n);
		pLB->pParent->SetText(sz);
	}
	else
	if (pLB == pHelpLB)
	{
		sprintf(sz, "Helps : %d", n);
		pLB->pParent->SetText(sz);
	}
	else
	if (pLB == pSocialLB)
	{
		sprintf(sz, "Socials : %d", n);
		pLB->pParent->SetText(sz);
	}
}	//UpdateLB


LRESULT TRoomListDlg::InitDialog()
{
    SetText("Rooms : 0");
	Area.RoomList.ForEach(FillRoomDlg, pRoomLB);
	return 0;
}	//TRoomListDlg::InitDialog

void TRoomListDlg::WMDrawItem(RTMessage Msg)
{
	LPDRAWITEMSTRUCT lpD = (LPDRAWITEMSTRUCT) Msg.lParam;
	PTRoom pRoom = (PTRoom) lpD->itemData;
	char sz[256];
    int n;

	if ((lpD->itemAction & ODA_FOCUS))
		return;

	PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
	 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, WHITENESS);

	if ((lpD->itemAction & ODA_SELECT) || (lpD->itemAction & ODA_DRAWENTIRE))
	{
    	n = sprintf(sz, "(%d) %s", pRoom->nVnum, (const char*) pRoom->sName);
		TextOut(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top, sz, n);
	}

	if ((lpD->itemState & ODS_SELECTED))
		PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
		 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, PATINVERT);
}	//TRoomListDlg::WMDrawItem

void TMobListDlg::IDMobLB(RTMessage Msg)
{
    char sz[5];
	DWORD* pD = (DWORD*) sz;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pMobLB->GetSelString(sz, 5);
		if (pApplication->ExecDialog(new TMobAttrDlg(pMainWnd, ID_MOBATTRDLG, (PTBaseMob) *pD)) == IDOK)
		{
			//redraw mob list to show change
			pMainWnd->bIsDirty = TRUE;
            UpdateLB(pMobLB, TRUE);
		}
    }
}	//TMobListDlg::IDMobLB

LRESULT TMobListDlg::InitDialog()
{
    SetText("Mobs : 0");
	Area.MobList.ForEach(FillMobDlg, pMobLB);
	return 0;
}	//TMobListDlg::InitDialog

void TMobListDlg::WMDrawItem(RTMessage Msg)
{
	LPDRAWITEMSTRUCT lpD = (LPDRAWITEMSTRUCT) Msg.lParam;
	PTBaseMob pMob = (PTBaseMob) lpD->itemData;
	char sz[256];
    int n;

	if ((lpD->itemAction & ODA_FOCUS))
		return;

	PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
	 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, WHITENESS);

	if ((lpD->itemAction & ODA_SELECT) || (lpD->itemAction & ODA_DRAWENTIRE))
	{
    	n = sprintf(sz, "(%d) %s", pMob->nVnum, (const char*) pMob->sShortDesc);
		TextOut(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top, sz, n);
	}

	if ((lpD->itemState & ODS_SELECTED))
		PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
		 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, PATINVERT);
}	//TMobListDlg::WMDrawItem

LRESULT TObjListDlg::InitDialog()
{
    SetText("Objects : 0");
	Area.ObjList.ForEach(FillObjDlg, pObjLB);
	return 0;
}	//TObjListDlg::InitDialog

void TObjListDlg::WMDrawItem(RTMessage Msg)
{
	LPDRAWITEMSTRUCT lpD = (LPDRAWITEMSTRUCT) Msg.lParam;
	PTBaseObj pObj = (PTBaseObj) lpD->itemData;
	char sz[256];
    int n;

	if ((lpD->itemAction & ODA_FOCUS))
		return;

	PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
	 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, WHITENESS);

	if ((lpD->itemAction & ODA_SELECT) || (lpD->itemAction & ODA_DRAWENTIRE))
	{
		n = sprintf(sz, "(%d) %s", pObj->nVnum, (const char*) pObj->sName);
		TextOut(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top, sz, n);
	}

	if ((lpD->itemState & ODS_SELECTED))
		PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
		 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, PATINVERT);
}	//TObjListDlg::WMDrawItem

TMobAttrDlg::TMobAttrDlg(PTWindowObject pParent, int nID, PTBaseMob pMobile) : TDialog(pParent, nID) 
{
    SetFlags(WB_TRANSFER, FALSE);
	new TEdit(this, EID_ALIGN, 6);
	new TEdit(this, EID_LEVEL, 7);
	new TEdit(this, EID_SHORTDESC, 256);
	new TEdit(this, EID_DESC, 4096);
	new TPushButton(this, PBID_HP);
	new TEdit(this, EID_HITBONUS, 7);
	new TComboBox(this, CBID_MATERIAL);
	new TComboBox(this, CBID_DEFPOS);
	new TPushButton(this, PBID_DAMAGE); 
	new TCheckBox(this, CHID_SHOP);
	new TComboBox(this, CBID_SIZE);
	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_GOLD, 11);
    new TEdit(this, EID_KEYWORDS, 256);
	new TEdit(this, EID_LONGDESC, 4096);
	new TComboBox(this, CBID_STARTPOS);
	new TComboBox(this, CBID_DAMTYPE);
	new TPushButton(this, PBID_MANA);
    new TPushButton(this, PBID_AC);
	new TComboBox(this, CBID_SEX);
	new TComboBox(this, CBID_RACE);
	new TComboBox(this, CBID_SPECFUNC);

	pMob = pMobile;
}	//TMobAttrDlg::TMobAttrDlg

LRESULT TMobAttrDlg::InitDialog()
{
	PTEdit pE;
	PTCheckBox pChB;
    PTComboBox pCB;
    char sz[80];

    //init the data that gets passed to flags, dice, etc. dlgs
	NewAction = pMob->ActionFlg;
	NewAffect = pMob->AffectFlg;
	if (pMob->pShop)
		NewShop = *(pMob->pShop);

	//init the controls
	pE = (PTEdit) GetChild(EID_VNUM);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_VNUM, pMob->nVnum, FALSE);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_KEYWORDS);
    pE->LimitText(255);
    pE->SetText(pMob->sKeywords);

	pE = (PTEdit) GetChild(EID_SHORTDESC);
	pE->LimitText(255);
    pE->SetText(pMob->sShortDesc);

	pE = (PTEdit) GetChild(EID_LONGDESC);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->LimitText(4095);
    pE->SetText(pMob->sLongDesc);

	pE = (PTEdit) GetChild(EID_DESC);
    SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0); 
	pE->LimitText(4095);
	pE->SetText(pMob->sDesc);

	pE = (PTEdit) GetChild(EID_ALIGN);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_ALIGN, pMob->nAlign, TRUE);

	pE = (PTEdit) GetChild(EID_LEVEL);
    pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_LEVEL, pMob->nLevel, FALSE);

	pE = (PTEdit) GetChild(EID_GOLD);
    pE->LimitText(10);
    SetDlgItemLong(hWindow, EID_GOLD, pMob->lGold, FALSE);

    pCB = (PTComboBox) GetChild(CBID_STARTPOS);
	PosList.ForEach(FillBox, pCB);
	SelectCB(pCB, PosList, pMob->nStartPos);

	pCB = (PTComboBox) GetChild(CBID_DEFPOS);
	PosList.ForEach(FillBox, pCB);
    SelectCB(pCB, PosList, pMob->nDefPos);

	pCB = (PTComboBox) GetChild(CBID_SEX);
	SexList.ForEach(FillBox, pCB);
	SelectCB(pCB, SexList, pMob->nSex);

	pCB = (PTComboBox) GetChild(CBID_SPECFUNC);
	SpecFuncList.ForEach(FillBox, pCB);
    if (!pMob->sSpecial.Len())	//no special function, then select "none" in CB
		SelectCB(pCB, SpecFuncList, "none");
	else
		SelectCB(pCB, SpecFuncList, pMob->sSpecial);

	if (pMob->pShop)
	{
		pChB = (PTCheckBox) GetChild(CHID_SHOP);
		pChB->SetCheck(BF_CHECKED);
	}

    //disable stuff that old-style mobs don't have
	if (pMob->Type() == OLD)
	{
		GetChild(CBID_RACE)->Enable(FALSE);
		GetChild(EID_HITBONUS)->Enable(FALSE);
		GetChild(PBID_HP)->Enable(FALSE);
		GetChild(PBID_MANA)->Enable(FALSE);
		GetChild(PBID_DAMAGE)->Enable(FALSE);
		GetChild(CBID_DAMTYPE)->Enable(FALSE);
		GetChild(PBID_AC)->Enable(FALSE);
		GetChild(CBID_SIZE)->Enable(FALSE);
		GetChild(CBID_MATERIAL)->Enable(FALSE);
	}
	else
	{
		PTMobile pMobile = (PTMobile) pMob;

		//init the data that gets passed to flags, dice, etc. dlgs
		NewHP = pMobile->HP;
		NewMana = pMobile->Mana;
		NewDamage = pMobile->Damage;
		NewAC = pMobile->AC;
		NewOff = pMobile->OffBits;
		NewImm = pMobile->Immune;
		NewRes = pMobile->Resist;
		NewVuln = pMobile->Vuln;
		NewForm = pMobile->Form;
		NewParts = pMobile->Parts;

		pCB = (PTComboBox) GetChild(CBID_RACE);
		RaceList.ForEach(FillBox, pCB);
		SelectCB(pCB, RaceList, pMobile->sRace);

		pE = (PTEdit) GetChild(EID_HITBONUS);
		pE->LimitText(6);
		SetDlgItemInt(hWindow, EID_HITBONUS, pMobile->nHitBonus, TRUE);

		sprintf(sz, "%dd%d+%d", pMobile->HP.nNum, pMobile->HP.nType, pMobile->HP.nBase);
		GetChild(PBID_HP)->SetText(sz);

		sprintf(sz, "%dd%d+%d", pMobile->Mana.nNum, pMobile->Mana.nType, pMobile->Mana.nBase);
		GetChild(PBID_MANA)->SetText(sz);

		sprintf(sz, "%dd%d+%d", pMobile->Damage.nNum, pMobile->Damage.nType, pMobile->Damage.nBase);
		GetChild(PBID_DAMAGE)->SetText(sz);

		pCB = (PTComboBox) GetChild(CBID_DAMTYPE);
		DamTypeList.ForEach(FillBox, pCB);
		SelectCB(pCB, DamTypeList, pMobile->nDamType);

		sprintf(sz, "P:%d  B:%d  S:%d  M:%d", pMobile->AC.nPierce, pMobile->AC.nBash,
		 pMobile->AC.nSlash, pMobile->AC.nMagic);
		GetChild(PBID_AC)->SetText(sz);

		pCB = (PTComboBox) GetChild(CBID_SIZE);
		SizeList.ForEach(FillBox, pCB);
		sprintf(sz, "%c", pMobile->cSize);
		SelectCB(pCB, SizeList, sz);

		pCB = (PTComboBox) GetChild(CBID_MATERIAL);
		MaterialList.ForEach(FillBox, pCB);
		if (!pMobile->sMaterial.Len())
			pCB->SetSelIndex(0);
        else
			SelectCB(pCB, MaterialList, pMobile->sMaterial);
	}
	    
	return 0;
}	//TMobAttrDlg::InitDialog

void TMobAttrDlg::Okay(RTMessage Msg)
{
	PTEdit pE;
	char sz[4096];
    PTBaseMob pCheckMob;

    //get data in controls into pMob
	pMob->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);

	//check for duplicate vnum
    pCheckMob = Area.FindMob(pMob->nVnum);
	if (pCheckMob && pCheckMob != pMob && pCheckMob->nVnum == pMob->nVnum)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Add Mobile",
		 "A mobile with vnum = %d already exists.", pMob->nVnum);
		return;
	}

	pE = (PTEdit) GetChild(EID_KEYWORDS);
	if (pE->IsModified())
	{
		pE->GetText(sz, 256);
		pMob->sKeywords = sz;
	}

	pE = (PTEdit) GetChild(EID_SHORTDESC);
	if (pE->IsModified())
	{
		pE->GetText(sz, 256);
		pMob->sShortDesc = sz;
	}

	pE = (PTEdit) GetChild(EID_LONGDESC);
	if (pE->IsModified())
	{
		pE->GetText(sz, 4096);
		pMob->sLongDesc = sz;
	}

	pE = (PTEdit) GetChild(EID_DESC);
	if (pE->IsModified())
	{
		pE->GetText(sz, 4096);
		pMob->sDesc = sz;
	}

	pMob->nAlign = GetDlgItemInt(hWindow, EID_ALIGN, NULL, TRUE);
	pMob->nLevel = GetDlgItemInt(hWindow, EID_LEVEL, NULL, FALSE);

	pMob->lGold = GetDlgItemLong(hWindow, EID_GOLD, NULL, FALSE);

	GetBoxSel((PTComboBox) GetChild(CBID_STARTPOS), PosList, sz, 256);
	pMob->nStartPos = atoi(sz);
	GetBoxSel((PTComboBox) GetChild(CBID_DEFPOS), PosList, sz, 256);
	pMob->nDefPos = atoi(sz);
	GetBoxSel((PTComboBox) GetChild(CBID_SEX), SexList, sz, 256);
	pMob->nSex = atoi(sz);
	GetBoxSel((PTComboBox) GetChild(CBID_SPECFUNC), SpecFuncList, sz, 256);
	if (!lstrcmpi(sz, "none"))	//is "none"
		sz[0] = 0;	//empty the string

	pMob->sSpecial = sz;
	pMob->ActionFlg = NewAction;
	pMob->AffectFlg = NewAffect;

    delete pMob->pShop;		//must destroy the shop
	if (IsDlgButtonChecked(hWindow, CHID_SHOP))
	{
		pMob->pShop = new TShop;
		*(pMob->pShop) = NewShop;
	}
	else
    	pMob->pShop = NULL;

	if (pMob->Type() == NEW)
	{
        PTMobile pMobile = (PTMobile) pMob; 
		GetBoxSel((PTComboBox) GetChild(CBID_RACE), RaceList, sz, 256);
		pMobile->sRace = sz;
		GetChild(EID_HITBONUS)->GetText(sz, 256);
		pMobile->nHitBonus = atoi(sz);
		GetBoxSel((PTComboBox) GetChild(CBID_DAMTYPE), DamTypeList, sz, 256);
		pMobile->nDamType = atoi(sz);
		GetBoxSel((PTComboBox) GetChild(CBID_SIZE), SizeList, sz, 256);
		pMobile->cSize = sz[0];
		GetBoxSel((PTComboBox) GetChild(CBID_MATERIAL), MaterialList, sz, 256);
		pMobile->sMaterial = sz;

		pMobile->HP = NewHP;
		pMobile->Mana = NewMana;
		pMobile->Damage = NewDamage;
		pMobile->AC = NewAC;
		pMobile->OffBits = NewOff;
		pMobile->Immune = NewImm;
		pMobile->Resist = NewRes;
		pMobile->Vuln = NewVuln;
		pMobile->Form = NewForm;
		pMobile->Parts = NewParts;
	}

	//Chain to original
	TDialog::Okay(Msg);
}	//TMobAttrDlg::Okay

void TMobAttrDlg::IDShop(RTMessage)
{
	PTCheckBox pCH = (PTCheckBox) GetChild(CHID_SHOP);

	if (pCH->GetCheck() == BF_CHECKED)
		if (pApplication->ExecDialog(new TShopDlg(this, ID_SHOPDLG, &NewShop)) == IDCANCEL)
			pCH->SetCheck(BF_UNCHECKED);
}	//TMobAttrDlg::IDShop

void TMobAttrDlg::IDHP(RTMessage)
{
	char sz[256];

	if (pApplication->ExecDialog(new TDiceDlg(this, ID_DICEDLG, &NewHP, "Hit Points")) == IDOK)
	{
		sprintf(sz, "%dd%d+%d", NewHP.nNum, NewHP.nType, NewHP.nBase);
		GetChild(PBID_HP)->SetText(sz);
    }
}	//TMobAttrDlg::IDHP

void TMobAttrDlg::IDMana(RTMessage)
{
	char sz[256];

	if (pApplication->ExecDialog(new TDiceDlg(this, ID_DICEDLG, &NewMana, "Mana")) == IDOK)
	{
		sprintf(sz, "%dd%d+%d", NewMana.nNum, NewMana.nType, NewMana.nBase);
		GetChild(PBID_MANA)->SetText(sz);
    }
}	//TMobAttrDlg::IDMana

void TMobAttrDlg::IDDamage(RTMessage)
{
	char sz[256];

	if (pApplication->ExecDialog(new TDiceDlg(this, ID_DICEDLG, &NewDamage, "Hit Points")) == IDOK)
	{
		sprintf(sz, "%dd%d+%d", NewDamage.nNum, NewDamage.nType, NewDamage.nBase);
		GetChild(PBID_DAMAGE)->SetText(sz);
    }
}	//TMobAttrDlg::IDDamage

void TMobAttrDlg::IDAC(RTMessage)
{
	char sz[256];

	if (pApplication->ExecDialog(new TACDlg(this, ID_ACDLG, &NewAC)) == IDOK)
	{
		sprintf(sz, "P:%d  B:%d  S:%d  M:%d", NewAC.nPierce, NewAC.nBash, NewAC.nSlash, NewAC.nMagic);
		GetChild(PBID_AC)->SetText(sz);
    }
}	//TMobAttrDlg::IDAC

void TMobAttrDlg::IDFlags(RTMessage)
{
	pApplication->ExecDialog(new TFlagDlg(this, ID_FLAGDLG, pMob, &NewAction, &NewAffect, &NewOff,
	 &NewImm, &NewRes, &NewVuln, &NewForm, &NewParts)); 
}	//TMobAttrDlg::IDFlags

/*
	Puts the atom text associated with pLB's currently selected item into psz.  cb is
    max buffer size (including terminating NULL).  L is the list to check for the atom.
	Works with comboboxes too.
	If nIndex is not -1, the nIndex'th element is processed instead.
*/
void GetBoxSel(PTListBox pLB, TList<TValPair>& L, char *psz, int cb, int nIndex)
{
	int n;
	PTValPair pV;

	if (nIndex == -1)
	{
		n = pLB->GetSelIndex();
		if (n == -1)
		{
			psz = "";
			return;
		}
    }
	else
       	n = nIndex;

	pV = L.nth(n);
	GetAtomName(pV->Atom, psz, cb);
}	//GetBoxSel

/*
	Adds string in pPair to combo/listbox p
*/
void FillBox(PTValPair pPair, void* p)
{
	PTListBox pLB = (PTListBox) p;

	pLB->AddString(pPair->sName);
}	//FillBox

/*
	Selects the item in pLB whose value in L matches psz.  Works for both single and multi
    selection list boxes.
*/
void SelectLB(PTListBox pLB, TList<TValPair>& L, const char *psz)
{
	TListIterator<TValPair> Iter(L);
	int nIndex = 0;

	while (int(Iter))
	{
		//just remember that atoms are case insensitive
		if (FindAtom(psz) == (Iter++)->Atom)
		{
			if ((pLB->dwStyle & LBS_MULTIPLESEL))
				pLB->SetSelIndices(&nIndex, 1, TRUE);
			else
				pLB->SetSelIndex(nIndex);

        	break;
		}

		nIndex++;
    }
}	//SelectCB

/*
	Selects the item in pCB whose value in L matches psz.
*/
void SelectCB(PTComboBox pCB, TList<TValPair>& L, const char *psz)
{
	TListIterator<TValPair> Iter(L);
	int nIndex = 0;

	while (int(Iter))
	{
		//just remember that atoms are case insensitive
		if (FindAtom(psz) == (Iter++)->Atom)
		{
            pCB->SetSelIndex(nIndex);
			break;
		}

		nIndex++;
    }
}	//SelectCB

//same thing overloaded for int values
void SelectLB(PTListBox pLB, TList<TValPair>& L, long l)
{
	char sz[30];

	sprintf(sz, "%ld", l);
    SelectLB(pLB, L, sz);
}	//SelectLB

//same thing overloaded for int values
void SelectCB(PTComboBox pCB, TList<TValPair>& L, long l)
{
	char sz[30];

	sprintf(sz, "%ld", l);
	SelectCB(pCB, L, sz);
}	//SelectCB

TShopDlg::TShopDlg(PTWindowObject pParent, int nID, PTShop p) : TDialog(pParent, nID)
{
	PTListBox pLB;

    SetFlags(WB_TRANSFER, FALSE);
	pShop = p;
	pLB = new TListBox(this, LBID_TRADETYPES);
	//setting multi sel flag even though its a dlg control cuz the program has to
    //determine if it's a multisel box
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	new TEdit(this, EID_BUYMARKUP, 5);  
	new TEdit(this, EID_SELLMARKDOWN, 5);
	new TEdit(this, EID_OPENHR, 2);
	new TEdit(this,	EID_CLOSEHR, 2);
}	//TShopDlg::TShopDlg

LRESULT TShopDlg::InitDialog()
{
	PTEdit pE;
	PTListBox pLB;

	pLB = (PTListBox) GetChild(LBID_TRADETYPES);
	pLB->SetFocus();
	ObjTypeList.ForEach(FillBox, pLB);
	for (int i=0; i<5; i++)
		SelectLB(pLB, ObjTypeList, pShop->nTradeTypes[i]);

	pE = (PTEdit) GetChild(EID_BUYMARKUP);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_BUYMARKUP, pShop->nBuyMarkup, FALSE);

	pE = (PTEdit) GetChild(EID_SELLMARKDOWN);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_SELLMARKDOWN, pShop->nSellMarkdown, FALSE);

	pE = (PTEdit) GetChild(EID_OPENHR);
	pE->LimitText(2);
	SetDlgItemInt(hWindow, EID_OPENHR, pShop->nOpenHour, FALSE);

	pE = (PTEdit) GetChild(EID_CLOSEHR);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_CLOSEHR, pShop->nCloseHour, FALSE);

    return 0;
}	//TShopDlg::InitDialog

void TShopDlg::Okay(RTMessage Msg)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_TRADETYPES);
	int i, n, nIndices[5];
    char sz[20];

	n = pLB->GetSelCount();
	if (n > 5 || n < 1)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION, "Shop Error",
		 "You must have at least one trade type,\n"
		 "and no more than five.");
        return;
	}

	pLB->GetSelIndices(nIndices, 5);
	_fmemset(pShop->nTradeTypes, 0, sizeof(pShop->nTradeTypes));
	for (i=0; i<n; i++)
	{
		GetBoxSel(pLB, ObjTypeList, sz, 20, nIndices[i]);
		pShop->nTradeTypes[i] = atoi(sz);
	}

	pShop->nBuyMarkup = GetDlgItemInt(hWindow, EID_BUYMARKUP, NULL, FALSE);
	pShop->nSellMarkdown = GetDlgItemInt(hWindow, EID_SELLMARKDOWN, NULL, FALSE);
	pShop->nOpenHour = GetDlgItemInt(hWindow, EID_OPENHR, NULL, FALSE);
	pShop->nCloseHour = GetDlgItemInt(hWindow, EID_CLOSEHR, NULL, FALSE);

	if (pShop->nOpenHour < 0 || pShop->nOpenHour > 23 ||
	 pShop->nCloseHour < 0 || pShop->nCloseHour > 23)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION, "Shop Error",
		 "Shop hours can range from 0 to 23.");
        return;
	}

	//chain to original
    TDialog::Okay(Msg);
}	//TShopDlg::Okay

TDiceDlg::TDiceDlg(PTWindowObject pParent, int nID, PTDice p, LPSTR lpszC) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
    lpszCaption = lpszC;
	pDice = p;

	new TEdit(this, EID_NUMDICE, 5);
	new TEdit(this, EID_DICETYPE, 5);
	new TEdit(this, EID_DICEBASE, 5);
}	//TDiceDlg::TDiceDlg

LRESULT TDiceDlg::InitDialog()
{
    PTEdit pE;

	SetText(lpszCaption);

	pE = (PTEdit) GetChild(EID_NUMDICE);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_NUMDICE, pDice->nNum, FALSE);
	pE->SetSel(0, 32767);
    pE->SetFocus();

	pE = (PTEdit) GetChild(EID_DICETYPE);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_DICETYPE, pDice->nType, FALSE);

	pE = (PTEdit) GetChild(EID_DICEBASE);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_DICEBASE, pDice->nBase, FALSE);

    return 0;
}	//TDiceDlg::InitDialog

void TDiceDlg::Okay(RTMessage Msg)
{
	pDice->nNum = GetDlgItemInt(hWindow, EID_NUMDICE, NULL, FALSE);
	pDice->nType = GetDlgItemInt(hWindow, EID_DICETYPE, NULL, FALSE);
	pDice->nBase = GetDlgItemInt(hWindow, EID_DICEBASE, NULL, FALSE);

	//Chain to original
    TDialog::Okay(Msg);
}	//TDiceDlg::Okay

TACDlg::TACDlg(PTWindowObject pParent, int nID, PTArmorClass p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pAC = p;

	new TEdit(this, EID_PIERCE, 6);
	new TEdit(this, EID_BASH, 6);
	new TEdit(this, EID_SLASH, 6);
	new TEdit(this, EID_MAGIC, 6);
}	//TACDlg::TACDlg

LRESULT TACDlg::InitDialog()
{
	PTEdit pE;

	pE = (PTEdit) GetChild(EID_PIERCE);
	pE->LimitText(6);
	SetDlgItemInt(hWindow, EID_PIERCE, pAC->nPierce, TRUE);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_BASH);
	pE->LimitText(6);
	SetDlgItemInt(hWindow, EID_BASH, pAC->nBash, TRUE);

	pE = (PTEdit) GetChild(EID_SLASH);
	pE->LimitText(6);
	SetDlgItemInt(hWindow, EID_SLASH, pAC->nSlash, TRUE);

	pE = (PTEdit) GetChild(EID_MAGIC);
	pE->LimitText(6);
	SetDlgItemInt(hWindow, EID_MAGIC, pAC->nMagic, TRUE);

    return 0;
}	//TACDlg::InitDialog

void TACDlg::Okay(RTMessage Msg)
{
	pAC->nPierce = GetDlgItemInt(hWindow, EID_PIERCE, NULL, TRUE);
	pAC->nBash = GetDlgItemInt(hWindow, EID_BASH, NULL, TRUE);
	pAC->nSlash = GetDlgItemInt(hWindow, EID_SLASH, NULL, TRUE);
	pAC->nMagic = GetDlgItemInt(hWindow, EID_MAGIC, NULL, TRUE);

	//chain to original
    TDialog::Okay(Msg);
}	//TACDlg::Okay


TValPair::TValPair(const char *p, const char *v)
{
	sName = p;
    Atom = AddAtom(v);
}	//TValPair::TValPair

/*
	Kills whitespace at end of string.
*/
void KillEndSpace(char* pszStr)
{
	char *p = &pszStr[lstrlen(pszStr) - 1];

	while (p >= pszStr && isspace(*p))
		p--;

	p[1] = 0;
}	//KillEndSpace

/*
	Adds a TValPair to the given list. 
*/
void ReadAndAddVal(ifstream& is, TList<TValPair>& L)
{
	PTValPair p;
    char szName[40], szVal[80]; 

    is >> ws;
	is.getline(szName, 40, '=');	//read name to '='
    KillEndSpace(szName);

    is >> ws;
	is.getline(szVal, 80);	//read value to EOL
	KillEndSpace(szVal);

	if (!lstrcmp(szVal, "\"\""))	//if value is empty string, value = name
		lstrcpy(szVal, szName);

    is >> ws;	//ready for next val

	p = new TValPair(szName, szVal);
	L.Add(p);	
}	//ReadAndAddVal

/*
	Reads "values.dat" into memory.  Returns 0 if fail, -1 if success.
*/
int ReadValues()
{
	char ch, szIn[80];
	ifstream is;

	is.open("values.dat");
	if (!is.good())
	{
		MessageBox(0, "Can't open \"values.dat\"", "Very Bad Thing",
		 MB_OK | MB_ICONEXCLAMATION);
		return 0;
    }

	SetCursor(LoadCursor(0, IDC_WAIT));

L1:	if (!is.good())
	{
		SetCursor(LoadCursor(0, IDC_ARROW));
		return -1;
    }

	is >> ch;
	if (ch != '[')
	{
		MessageBox(0, "Missing '[' in \"values.dat\"", "Parse Error",
		 MB_OK | MB_ICONEXCLAMATION);
		return 0;
	}

    is >> ws;
    is.getline(szIn, 80, ']');	//get up to ']'
	KillEndSpace(szIn);

    //handle the category
	if (!lstrcmpi(szIn, "Action"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ActionList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Affby"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, AffByList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "SpecFunc"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, SpecFuncList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "ObjFlags"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ObjFlagsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "ObjWear"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ObjWearList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Offensive"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, OffensiveList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Immune"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ImmuneList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Resist"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ResistList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Vuln"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, VulnList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Forms"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, FormsList);

        goto L1; 
    }
	else
	if (!lstrcmpi(szIn, "Parts"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, PartsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "DamType"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, DamTypeList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Sex"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, SexList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Size"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, SizeList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Material"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, MaterialList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Adjustments"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, AdjustmentsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Condition"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ConditionList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Directions"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, DirectionsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "RoomFlags"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, RoomFlagsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Terrain"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, TerrainList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "ObjType"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ObjTypeList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Pos"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, PosList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Race"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, RaceList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "DoorState"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, DoorStateList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "DoorType"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, DoorTypeList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "WeaponClass"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, WeaponClassList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "WeaponType"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, WeaponTypeList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "ContFlags"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ContFlagsList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Liquids"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, LiquidList);

        goto L1; 
	}
	else
	if (!lstrcmpi(szIn, "Spells"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, SpellsList);

        goto L1; 
	}
	else

    #ifndef ET
	if (!lstrcmpi(szIn, "PortalTypes"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, PortalTypeList);

        goto L1;
	}
	else
	#endif

	if (!lstrcmpi(szIn, "E_Wear"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, EWearList);

        goto L1;
	}
    else
	if (!lstrcmpi(szIn, "ExitTypes"))
	{
		int ch;

		while ((ch = is.peek()) != '[' && ch != -1)
			ReadAndAddVal(is, ExitTypeList);

        goto L1;
	}
    else
		MessageBox(0, "Unknown category in \"values.dat\"", "Parse Error",
		 MB_OK | MB_ICONEXCLAMATION);

	SetCursor(LoadCursor(0, IDC_ARROW));
	return 0;
}	//ReadValues

/*
	Calls Area.Clear() to kill all the data.  Resets the list boxes.  Clears the map.
*/
void ClearData()
{
    SetCursor(LoadCursor(0, IDC_WAIT));
	Area.Clear();
	SendMessage(pRoomLB->hWindow, LB_RESETCONTENT, 0, 0);
	SendMessage(pMobLB->hWindow, LB_RESETCONTENT, 0, 0);
	SendMessage(pObjLB->hWindow, LB_RESETCONTENT, 0, 0);
	SendMessage(pHelpLB->hWindow, LB_RESETCONTENT, 0, 0);
    SendMessage(pSocialLB->hWindow, LB_RESETCONTENT, 0, 0);
	UpdateLB(pRoomLB, FALSE);
	UpdateLB(pMobLB, FALSE);
	UpdateLB(pObjLB, FALSE);
	UpdateLB(pHelpLB, FALSE);
	UpdateLB(pSocialLB, FALSE);

	InvalidateRect(pMapWnd->hWindow, NULL, TRUE);
	UpdateWindow(pMapWnd->hWindow);
	SetCursor(LoadCursor(0, IDC_ARROW));
	pMainWnd->SetText("SandBox");
	pMainWnd->bIsDirty = FALSE;
}	//ClearData
