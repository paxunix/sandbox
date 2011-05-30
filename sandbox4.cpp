/*
	SANDBOX4.CPP

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

TLightDlg::TLightDlg(PTWindowObject pParent, int nID, int nT, long *l) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
	nType = nT;
    lValues = l;

	new TEdit(this, EID_HOURS, 6);
}	//TLightDlg::TLightDlg

LRESULT TLightDlg::InitDialog()
{
    PTEdit pE = (PTEdit) GetChild(EID_HOURS);

	if (nType == ITEM_MONEY)
	{
        SetDlgItemLong(hWindow, EID_HOURS, lValues[0], FALSE);
		SetText("Values for \"Money\"");
		SetDlgItemText(hWindow, SID_HOURS, "Amount of gold:");
	}
	else
		SetDlgItemLong(hWindow, EID_HOURS, lValues[2], TRUE);

	pE->LimitText(5);
	pE->SetSel(0, 32767);

	return 0;
}	//TLightDlg::InitDialog

void TLightDlg::Okay(RTMessage Msg)
{
	if (nType == ITEM_MONEY)
		lValues[0] = GetDlgItemLong(hWindow, EID_HOURS, NULL, FALSE);
	else
		lValues[2] = GetDlgItemLong(hWindow, EID_HOURS, NULL, TRUE);

	//chain to original
    TDialog::Okay(Msg);
}	//TLightDlg::Okay

TPotionDlg::TPotionDlg(PTWindowObject pParent, int nID, int nT, long* lV) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
    nType = nT;
    lValues = lV;

	new TComboBox(this, CBID_SPELL1);
	new TComboBox(this, CBID_SPELL2);
	new TComboBox(this, CBID_SPELL3);
	new TEdit(this, EID_LEVEL, 6);
}	//TPotionDlg::TPotionDlg

LRESULT TPotionDlg::InitDialog()
{
	PTComboBox pCB = (PTComboBox) GetChild(CBID_SPELL1);
    PTEdit pE;

	switch (nType)
	{
		case ITEM_SCROLL:
			SetText("Values for \"Scroll\"");
			break;

		case ITEM_PILL:
			SetText("Values for \"Pill\"");
	}

	SpellsList.ForEach(FillBox, pCB);
    SelectCB(pCB, SpellsList, lValues[1]);
	pCB->SetFocus();

	pCB = (PTComboBox) GetChild(CBID_SPELL2);
	SpellsList.ForEach(FillBox, pCB);
	SelectCB(pCB, SpellsList, lValues[2]);

	pCB = (PTComboBox) GetChild(CBID_SPELL3);
	SpellsList.ForEach(FillBox, pCB);
	SelectCB(pCB, SpellsList, lValues[3]);

	SetDlgItemLong(hWindow, EID_LEVEL, lValues[0], FALSE);
	pE = (PTEdit) GetChild(EID_LEVEL);
	pE->LimitText(5);

	return 0;
}	//TPotionDlg::InitDialog

void TPotionDlg::Okay(RTMessage Msg)
{
	PTComboBox pCB = (PTComboBox) GetChild(CBID_SPELL1);
    char sz[256];

	GetBoxSel(pCB, SpellsList, sz, 256);
	lValues[1] = strtol(sz, NULL, 10);

    pCB = (PTComboBox) GetChild(CBID_SPELL2);
	GetBoxSel(pCB, SpellsList, sz, 256);
	lValues[2] = strtol(sz, NULL, 10);

	pCB = (PTComboBox) GetChild(CBID_SPELL3);
	GetBoxSel(pCB, SpellsList, sz, 256);
	lValues[3] = strtol(sz, NULL, 10);

	lValues[0] = GetDlgItemLong(hWindow, EID_LEVEL, NULL, FALSE);

	//chain to original
	TDialog::Okay(Msg);
}	//TPotionDlg::Okay

TWandDlg::TWandDlg(PTWindowObject pParent, int nID, int nT, long* lV) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
    nType = nT;
    lValues = lV;

	new TEdit(this, EID_LEVEL, 6);
	new TEdit(this, EID_MAXCHARGES, 6);
	new TEdit(this, EID_CURRENTCHARGES, 6);
	new TComboBox(this, CBID_SPELL1);
}	//TWandDlg::TWandDlg

LRESULT TWandDlg::InitDialog()
{
	PTComboBox pCB = (PTComboBox) GetChild(CBID_SPELL1);
    PTEdit pE;

	if (nType == ITEM_SCROLL)
		SetText("Values for \"Staff\"");

	SetDlgItemLong(hWindow, EID_LEVEL, lValues[0], FALSE);
	pE = (PTEdit) GetChild(EID_LEVEL);
	pE->LimitText(5);
    pE->SetSel(0, 32767);
    pE->SetFocus();

	SetDlgItemLong(hWindow, EID_MAXCHARGES, lValues[1], FALSE);
	pE = (PTEdit) GetChild(EID_MAXCHARGES);
	pE->LimitText(5);

	SetDlgItemLong(hWindow, EID_CURRENTCHARGES, lValues[2], FALSE);
	pE = (PTEdit) GetChild(EID_CURRENTCHARGES);
	pE->LimitText(5);

	SpellsList.ForEach(FillBox, pCB);
    SelectCB(pCB, SpellsList, lValues[3]);

	return 0;
}	//TWandDlg::InitDialog

void TWandDlg::Okay(RTMessage Msg)
{
	PTComboBox pCB = (PTComboBox) GetChild(CBID_SPELL1);
    char sz[256];

	lValues[0] = GetDlgItemLong(hWindow, EID_LEVEL, NULL, FALSE);
	lValues[1] = GetDlgItemLong(hWindow, EID_MAXCHARGES, NULL, FALSE);
	lValues[2] = GetDlgItemLong(hWindow, EID_CURRENTCHARGES, NULL, FALSE);

	GetBoxSel(pCB, SpellsList, sz, 256);
	lValues[3] = strtol(sz, NULL, 10);

	//chain to original
	TDialog::Okay(Msg);
}	//TWandDlg::Okay


TMobResetDlg::TMobResetDlg(PTWindowObject pParent, int nID, PTMReset p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pMReset = p;

	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_LIMIT, 6);
	new TListBox(this, LBID_GRESETS);
	new TListBox(this, LBID_ERESETS);
}	//TMobResetDlg::TMobResetDlg

LRESULT TMobResetDlg::InitDialog()
{
    PTListBox pLB;
	PTEdit pE;
    char sz[256];

	pE = (PTEdit) GetChild(EID_VNUM);
	SetDlgItemInt(hWindow, EID_VNUM, pMReset->nVnum, FALSE);
	pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_LIMIT);
	SetDlgItemInt(hWindow, EID_LIMIT, pMReset->nLimit, TRUE);
	pE->LimitText(5);

	//fill the G and E reset LBs
	TListIterator<TGReset> GIter(pMReset->GResetList);
	pLB = (PTListBox) GetChild(LBID_GRESETS);
    PTBaseObj pObj;
	PTGReset pG;
	while (int(GIter))
	{
        pG = GIter++;
        pObj = Area.FindObj(pG->nVnum);
		sprintf(sz, "%d : %s", pG->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);
	}

	TListIterator<TEReset> EIter(pMReset->EResetList);
	pLB = (PTListBox) GetChild(LBID_ERESETS);
	PTEReset pER;
	while (int(EIter))
	{
        pER = EIter++;
		pObj = Area.FindObj(pER->nVnum);
		sprintf(sz, "%d : %s", pER->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);
	}

	return 0;
}	//TMobResetDlg::InitDialog

void TMobResetDlg::Okay(RTMessage Msg)
{
	pMReset->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);
	pMReset->nLimit = GetDlgItemInt(hWindow, EID_LIMIT, NULL, TRUE);

	//chain to original
    TDialog::Okay(Msg);
}	//TMobResetDlg::Okay

TObjResetDlg::TObjResetDlg(PTWindowObject pParent, int nID, PTOReset p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pOReset = p;

	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_LIMIT, 6);
	new TListBox(this, LBID_PRESETS);
}	//TObjResetDlg::TObjResetDlg

LRESULT TObjResetDlg::InitDialog()
{
    PTListBox pLB;
	PTEdit pE;
    char sz[256];

	pE = (PTEdit) GetChild(EID_VNUM);
	SetDlgItemInt(hWindow, EID_VNUM, pOReset->nVnum, FALSE);
	pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_LIMIT);
	SetDlgItemInt(hWindow, EID_LIMIT, pOReset->nLimit, TRUE);
	pE->LimitText(5);

	//fill the P reset LB
	TListIterator<TPReset> PIter(pOReset->PResetList);
	pLB = (PTListBox) GetChild(LBID_PRESETS);
    PTBaseObj pObj;
	PTPReset pP;
	while (int(PIter))
	{
        pP = PIter++;
        pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);
	}

	return 0;
}	//TObjResetDlg::InitDialog

void TObjResetDlg::Okay(RTMessage Msg)
{
	pOReset->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);
	pOReset->nLimit = GetDlgItemInt(hWindow, EID_LIMIT, NULL, TRUE);

	//chain to original
    TDialog::Okay(Msg);
}	//TObjResetDlg::Okay

TPResetDlg::TPResetDlg(PTWindowObject pParent, int nID, PTPReset p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pPReset = p;

	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_LIMIT, 6);
	new TEdit(this, EID_AMOUNT, 6); 
}	//TPResetDlg::TPResetDlg

LRESULT TPResetDlg::InitDialog()
{
	PTEdit pE;

	pE = (PTEdit) GetChild(EID_VNUM);
	SetDlgItemInt(hWindow, EID_VNUM, pPReset->nVnum, FALSE);
	pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_LIMIT);
	SetDlgItemInt(hWindow, EID_LIMIT, pPReset->nLimit, TRUE);
	pE->LimitText(5);

	pE = (PTEdit) GetChild(EID_AMOUNT);
	SetDlgItemInt(hWindow, EID_AMOUNT, pPReset->nAmount, TRUE);
	pE->LimitText(5);

	return 0;
}	//TPResetDlg::InitDialog

void TPResetDlg::Okay(RTMessage Msg)
{
	pPReset->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);
	pPReset->nLimit = GetDlgItemInt(hWindow, EID_LIMIT, NULL, TRUE);
	pPReset->nAmount = GetDlgItemInt(hWindow, EID_AMOUNT, NULL, FALSE);

	//chain to original
    TDialog::Okay(Msg);
}	//TPResetDlg::Okay

TGResetDlg::TGResetDlg(PTWindowObject pParent, int nID, PTGReset p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pGReset = p;

	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_LIMIT, 6);
	new TEdit(this, EID_AMOUNT, 6);
    new TListBox(this, LBID_PRESETS);
}	//TGResetDlg::TGResetDlg

LRESULT TGResetDlg::InitDialog()
{
	PTEdit pE;
	PTListBox pLB;
    char sz[256];

	pE = (PTEdit) GetChild(EID_VNUM);
	SetDlgItemInt(hWindow, EID_VNUM, pGReset->nVnum, FALSE);
	pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_LIMIT);
	SetDlgItemInt(hWindow, EID_LIMIT, pGReset->nLimit, TRUE);
	pE->LimitText(5);

	pE = (PTEdit) GetChild(EID_AMOUNT);
	SetDlgItemInt(hWindow, EID_AMOUNT, pGReset->nAmount, TRUE);
	pE->LimitText(5);

	//fill the P reset LB
	TListIterator<TPReset> PIter(pGReset->PResetList);
	pLB = (PTListBox) GetChild(LBID_PRESETS);
    PTBaseObj pObj;
	PTPReset pP;
	while (int(PIter))
	{
        pP = PIter++;
        pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);
	}

	return 0;
}	//TGResetDlg::InitDialog

void TGResetDlg::Okay(RTMessage Msg)
{
	pGReset->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);
	pGReset->nLimit = GetDlgItemInt(hWindow, EID_LIMIT, NULL, TRUE);
	pGReset->nAmount = GetDlgItemInt(hWindow, EID_AMOUNT, NULL, FALSE);

	//chain to original
    TDialog::Okay(Msg);
}	//TGResetDlg::Okay

TEResetDlg::TEResetDlg(PTWindowObject pParent, int nID, PTEReset p) : TDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pEReset = p;

	new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_LIMIT, 6);
	new TEdit(this, EID_AMOUNT, 6);
	new TListBox(this, LBID_WEARFLAGS);
	new TListBox(this, LBID_PRESETS);
}	//TEResetDlg::TEResetDlg

LRESULT TEResetDlg::InitDialog()
{
	PTEdit pE;
	PTListBox pLB;
	char sz[256];

	pE = (PTEdit) GetChild(EID_VNUM);
	SetDlgItemInt(hWindow, EID_VNUM, pEReset->nVnum, FALSE);
	pE->LimitText(5);
	pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_LIMIT);
	SetDlgItemInt(hWindow, EID_LIMIT, pEReset->nLimit, TRUE);
	pE->LimitText(5);

	pE = (PTEdit) GetChild(EID_AMOUNT);
	SetDlgItemInt(hWindow, EID_AMOUNT, pEReset->nAmount, TRUE);
	pE->LimitText(5);

    //fill the wear locations LB and select one
	pLB = (PTListBox) GetChild(LBID_WEARFLAGS);
	EWearList.ForEach(FillBox, pLB);
    SelectLB(pLB, EWearList, pEReset->nWearLoc);

	//fill the P reset LB
	TListIterator<TPReset> PIter(pEReset->PResetList);
	pLB = (PTListBox) GetChild(LBID_PRESETS);
    PTBaseObj pObj;
	PTPReset pP;
	while (int(PIter))
	{
        pP = PIter++;
        pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char *) pObj->sName : "<non-area object>");
		pLB->AddString(sz);
	}

	return 0;
}	//TEResetDlg::InitDialog

void TEResetDlg::Okay(RTMessage Msg)
{
	char sz[256];

	pEReset->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);
	pEReset->nLimit = GetDlgItemInt(hWindow, EID_LIMIT, NULL, TRUE);
	pEReset->nAmount = GetDlgItemInt(hWindow, EID_AMOUNT, NULL, FALSE);

	//get wear location from LB
	GetBoxSel((PTListBox) GetChild(LBID_WEARFLAGS), EWearList, sz, 256);
    pEReset->nWearLoc = atoi(sz);

	//chain to original
    TDialog::Okay(Msg);
}	//TEResetDlg::Okay

/*
	Iteration func that adds pHelp to the list box.
*/
void FillHelpDlg(PTHelp pHelp, void *p)
{
	PTListBox pLB = (PTListBox) p;

	pLB->AddString((LPCSTR) pHelp);
}	//FillHelpDlg

THelpListDlg::THelpListDlg(PTWindowObject pParent, int nID) : TModelessDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pHelpLB = new TListBox(this, LBID_LIST);
}	//THelpListDlg::THelpListDlg

LRESULT THelpListDlg::InitDialog()
{
    SetText("Helps : 0");
	Area.HelpList.ForEach(FillHelpDlg, pHelpLB);
	return 0;
}	//THelpListDlg::InitDialog

void THelpListDlg::WMDrawItem(RTMessage Msg)
{
	LPDRAWITEMSTRUCT lpD = (LPDRAWITEMSTRUCT) Msg.lParam;
	PTHelp pHelp = (PTHelp) lpD->itemData;
	char sz[256];
    int n;

	if ((lpD->itemAction & ODA_FOCUS))
		return;

	PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
	 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, WHITENESS);

	if ((lpD->itemAction & ODA_SELECT) || (lpD->itemAction & ODA_DRAWENTIRE))
	{
    	n = sprintf(sz, "(%d) %s", pHelp->nLevel, (const char*) pHelp->sKeywords);
		TextOut(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top, sz, n);
	}

	if ((lpD->itemState & ODS_SELECTED))
		PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
		 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, PATINVERT);
}	//THelpListDlg::WMDrawItem

void THelpListDlg::IDHelpLB(RTMessage Msg)
{
    char sz[5];
	DWORD* pD = (DWORD*) sz;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pHelpLB->GetSelString(sz, 5);
		if (pApplication->ExecDialog(new THelpDlg(pMainWnd, ID_HELPDLG, (PTHelp) *pD)) == IDOK)
		{
			//redraw help list to show change
			pMainWnd->bIsDirty = TRUE;
            UpdateLB(pHelpLB, TRUE);
		}
    }
}	//THelpListDlg::IDHelpLB

void TEResetDlg::IDAddPReset(RTMessage)
{
	PTPReset pP = new TPReset(1, 0, -1);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
	{
        //save the changes
        pEReset->PResetList.Add(pP);
		pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
		delete pP;
}	//TEResetDlg::IDAddPReset

void TEResetDlg::IDDelPReset(RTMessage)
{
    PTPReset pP;
	PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'P' Reset",
		 "First select a reset from the list.");
		return;
    }

    //remove it from the internal list
	pEReset->PResetList.Remove(pP = pEReset->PResetList.nth(n));
    delete pP;	//and kill it
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TEResetDlg::IDDelPReset

void TEResetDlg::IDPResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTPReset pP;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_PRESETS);
		n = pLB->GetSelIndex();
		pP = pEReset->PResetList.nth(n);

		if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pP->nVnum);
			sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TEResetDlg::IDPResetLB

void TGResetDlg::IDAddPReset(RTMessage)
{
	PTPReset pP = new TPReset(1, 0, -1);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
	{
        //save the changes
        pGReset->PResetList.Add(pP);
		pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
		delete pP;
}	//TGResetDlg::IDAddPReset

void TGResetDlg::IDDelPReset(RTMessage)
{
    PTPReset pP;
	PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'P' Reset",
		 "First select a reset from the list.");
		return;
    }

    //remove it from the internal list
	pGReset->PResetList.Remove(pP = pGReset->PResetList.nth(n));
    delete pP;	//and kill it
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TGResetDlg::IDDelPReset

void TGResetDlg::IDPResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTPReset pP;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_PRESETS);
		n = pLB->GetSelIndex();
		pP = pGReset->PResetList.nth(n);

		if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pP->nVnum);
			sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TGResetDlg::IDPResetLB

THelpDlg::THelpDlg(PTWindowObject pParent, int nID, PTHelp p) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
	pHelp = p;

    new TEdit(this, EID_LEVEL, 6);
	new TEdit(this, EID_KEYWORDS, 256);
    new TEdit(this, EID_HELPTEXT, 4096);
}	//THelpDlg::THelpDlg

LRESULT THelpDlg::InitDialog()
{
	PTEdit pE = (PTEdit) GetChild(EID_LEVEL);

    SetDlgItemInt(hWindow, EID_LEVEL, pHelp->nLevel, TRUE);
	pE->LimitText(5);
    pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_KEYWORDS);
	pE->SetText(pHelp->sKeywords);
    pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_HELPTEXT);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->SetText(pHelp->sText);
    pE->LimitText(4095);

	return 0;
}	//THelpDlg::InitDialog

void THelpDlg::Okay(RTMessage Msg)
{
	char sz[4096];

    pHelp->nLevel = GetDlgItemInt(hWindow, EID_LEVEL, NULL, TRUE);

	GetChild(EID_KEYWORDS)->GetText(sz, 256);
	if (!sz[0])	//don't want null keywords
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Help Text",
		 "You must have a keyword string.");
		return;
	}

	pHelp->sKeywords = sz;

	GetChild(EID_HELPTEXT)->GetText(sz, 4096);
	pHelp->sText = sz;

	//chain to original
    TDialog::Okay(Msg);
}	//THelpDlg::Okay

void TSandBoxWnd::CMDelHelp(RTMessage)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;
	PTHelp pHelp;
    int n = pHelpLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Help",
		 "First select an item from the help keyword list.");
		return;
    }

	pHelpLB->GetSelString(sz, 5);
    pHelp = (PTHelp) *pD;

	Area.HelpList.Remove(pHelp);
	delete pHelp;
    pHelpLB->DeleteString(n);

	pMainWnd->bIsDirty = TRUE;
    UpdateLB(pHelpLB, TRUE);
}	//TSandBoxWnd::CMDelHelp

void TSandBoxWnd::CMAddHelp(RTMessage)
{
	PTHelp pHelp = new THelp;

	pHelp->nLevel = -1;
	pHelp->sKeywords = "<none>";
	pHelp->sText = "<none>";

	//bring up the dlg
	if (pApplication->ExecDialog(new THelpDlg(this, ID_HELPDLG, pHelp)) == IDOK)
	{
		Area.HelpList.Add(pHelp);
        pHelpLB->AddString((LPSTR) pHelp);
        UpdateLB(pHelpLB, TRUE);
    	pMainWnd->bIsDirty = TRUE;
	}
	else
		delete pHelp;
}	//TSandBoxWnd::CMAddHelp

/*
	Iteration func that adds pSocial to the list box.
*/
void FillSocialDlg(PTSocial pSocial, void *p)
{
	PTListBox pLB = (PTListBox) p;

	pLB->AddString((LPCSTR) pSocial);
}	//FillSocialDlg

TSocialListDlg::TSocialListDlg(PTWindowObject pParent, int nID) : TModelessDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pSocialLB = new TListBox(this, LBID_LIST);
}	//TSocialListDlg::TSocialListDlg

LRESULT TSocialListDlg::InitDialog()
{
    SetText("Socials : 0");
	Area.SocialList.ForEach(FillSocialDlg, pHelpLB);
	return 0;
}	//TSocialListDlg::InitDialog

void TSocialListDlg::WMDrawItem(RTMessage Msg)
{
	LPDRAWITEMSTRUCT lpD = (LPDRAWITEMSTRUCT) Msg.lParam;
	PTSocial pSocial = (PTSocial) lpD->itemData;
	if ((lpD->itemAction & ODA_FOCUS))
		return;

	PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
	 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, WHITENESS);

	if ((lpD->itemAction & ODA_SELECT) || (lpD->itemAction & ODA_DRAWENTIRE))
		TextOut(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top, pSocial->sName, pSocial->sName.Len());

	if ((lpD->itemState & ODS_SELECTED))
		PatBlt(lpD->hDC, (lpD->rcItem).left, (lpD->rcItem).top,
		 (lpD->rcItem).right - (lpD->rcItem).left, (lpD->rcItem).bottom - (lpD->rcItem).top, PATINVERT);
}	//TSocialListDlg::WMDrawItem

void TSocialListDlg::IDSocialLB(RTMessage Msg)
{
    char sz[5];
	DWORD* pD = (DWORD*) sz;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pSocialLB->GetSelString(sz, 5);
		if (pApplication->ExecDialog(new TSocialDlg(pMainWnd, ID_SOCIALDLG, (PTSocial) *pD)) == IDOK)
		{
			//redraw help list to show change
			pMainWnd->bIsDirty = TRUE;
            UpdateLB(pSocialLB, TRUE);
		}
    }
}	//TSocialListDlg::IDSocialLB

TSocialDlg::TSocialDlg(PTWindowObject pParent, int nID, PTSocial p) : TDialog(pParent, nID)
{
    SetFlags(WB_TRANSFER, FALSE);
	pSocial = p;

	new TEdit(this, EID_NAME, 256);
	new TEdit(this, EID_CHARNOARG, 256);
	new TEdit(this, EID_OTHERSNOARG, 256);
	new TEdit(this, EID_CHARARGFOUND, 256);
	new TEdit(this, EID_OTHERSARGFOUND, 256);
	new TEdit(this, EID_VICTIMARGFOUND, 256);
	new TEdit(this, EID_CHARARGNOTFOUND, 256);
	new TEdit(this, EID_CHARSELF, 256);
	new TEdit(this, EID_OTHERSSELF, 256);
}	//TSocialDlg::TSocialDlg

LRESULT TSocialDlg::InitDialog()
{
	PTEdit pE = (PTEdit) GetChild(EID_NAME);

	pE->SetText(pSocial->sName);
	pE->LimitText(256);
    pE->SetSel(0, 32767);
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_CHARNOARG);
	pE->SetText(pSocial->sStrs[0]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_OTHERSNOARG);
	pE->SetText(pSocial->sStrs[1]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_CHARARGFOUND);
	pE->SetText(pSocial->sStrs[2]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_OTHERSARGFOUND);
	pE->SetText(pSocial->sStrs[3]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_VICTIMARGFOUND);
	pE->SetText(pSocial->sStrs[4]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_CHARARGNOTFOUND);
	pE->SetText(pSocial->sStrs[5]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_CHARSELF);
	pE->SetText(pSocial->sStrs[6]);
	pE->LimitText(256);

	pE = (PTEdit) GetChild(EID_OTHERSSELF);
	pE->SetText(pSocial->sStrs[7]);
	pE->LimitText(256);

	return 0;
}	//TSocialDlg::InitDialog

void TSocialDlg::Okay(RTMessage Msg)
{
	char sz[4096];

	PTEdit pE = (PTEdit) GetChild(EID_NAME);

	pE->GetText(sz, 4096);
    pSocial->sName = sz;

	pE = (PTEdit) GetChild(EID_CHARNOARG);
	pE->GetText(sz, 4096);
	pSocial->sStrs[0] = sz;

	pE = (PTEdit) GetChild(EID_OTHERSNOARG);
	pE->GetText(sz, 4096);
	pSocial->sStrs[1] = sz;

	pE = (PTEdit) GetChild(EID_CHARARGFOUND);
	pE->GetText(sz, 4096);
	pSocial->sStrs[2] = sz;

	pE = (PTEdit) GetChild(EID_OTHERSARGFOUND);
	pE->GetText(sz, 4096);
	pSocial->sStrs[3] = sz;

	pE = (PTEdit) GetChild(EID_VICTIMARGFOUND);
	pE->GetText(sz, 4096);
	pSocial->sStrs[4] = sz;

	pE = (PTEdit) GetChild(EID_CHARARGNOTFOUND);
	pE->GetText(sz, 4096);
	pSocial->sStrs[5] = sz;

	pE = (PTEdit) GetChild(EID_CHARSELF);
	pE->GetText(sz, 4096);
	pSocial->sStrs[6] = sz;

	pE = (PTEdit) GetChild(EID_OTHERSSELF);
	pE->GetText(sz, 4096);
	pSocial->sStrs[7] = sz;

	//chain to original
    TDialog::Okay(Msg);
}	//TSocialDlg::Okay

void TSandBoxWnd::CMDelSocial(RTMessage)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;
	PTSocial pSocial;
    int n = pSocialLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Social",
		 "First select an item from the social list.");
		return;
    }

	pSocialLB->GetSelString(sz, 5);
    pSocial = (PTSocial) *pD;

	Area.SocialList.Remove(pSocial);
	delete pSocial;
    pSocialLB->DeleteString(n);

	pMainWnd->bIsDirty = TRUE;
    UpdateLB(pSocialLB, TRUE);
}	//TSandBoxWnd::CMDelSocial

void TSandBoxWnd::CMAddSocial(RTMessage)
{
	PTSocial pSocial = new TSocial;

	//Social inits itself to all empty strings.

	//bring up the dlg
	if (pApplication->ExecDialog(new TSocialDlg(this, ID_SOCIALDLG, pSocial)) == IDOK)
	{
		Area.SocialList.Add(pSocial);
        pSocialLB->AddString((LPSTR) pSocial);
        UpdateLB(pSocialLB, TRUE);
    	pMainWnd->bIsDirty = TRUE;
	}
	else
		delete pSocial;
}	//TSandBoxWnd::CMAddSocial


void TObjResetDlg::IDAddPReset(RTMessage)
{
	PTPReset pP = new TPReset(1, 0, -1);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
	{
        //save the changes
        pOReset->PResetList.Add(pP);
		pObj = Area.FindObj(pP->nVnum);
		sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
		delete pP;
}	//TObjResetDlg::IDAddPReset

void TObjResetDlg::IDDelPReset(RTMessage)
{
    PTPReset pP;
	PTListBox pLB = (PTListBox) GetChild(LBID_PRESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'P' Reset",
		 "First select a reset from the list.");
		return;
    }

    //remove it from the internal list
	pOReset->PResetList.Remove(pP = pOReset->PResetList.nth(n));
    delete pP;	//and kill it
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TObjResetDlg::IDDelPReset

void TObjResetDlg::IDPResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTPReset pP;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_PRESETS);
		n = pLB->GetSelIndex();
		pP = pOReset->PResetList.nth(n);

		if (pApplication->ExecDialog(new TPResetDlg(this, ID_PRESETDLG, pP)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pP->nVnum);
			sprintf(sz, "%d : %s", pP->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TObjResetDlg::IDPResetLB

void TSandBoxWnd::CMAbout(RTMessage)
{
	pApplication->ExecDialog(new TDialog(this, ID_ABOUTDLG));
}	//TSandBoxWnd::CMAbout

void TSandBoxWnd::CMAddMob(RTMessage)
{
    static int nLastMobVnum = 1;
	PTMobile pMob = new TMobile;
	char szVal[256];
    PTValPair pV;

    //init old defaults
	pMob->nVnum = nLastMobVnum;
	pMob->ActionFlg.Set((long) 0);
	pMob->AffectFlg.Set((long) 0);
	pMob->nAlign = 0;
	pMob->nLevel = 1;
	pMob->lGold = 0;

    //don't want to hard code values, so just pull first ones out of lists
	pV = PosList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->nStartPos = pMob->nDefPos = atoi(szVal);

	pV = SexList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->nSex = atoi(szVal);

	pMob->pShop = NULL;

	//init new defaults
	pMob->nHitBonus = 0;
	pMob->HP.nNum = pMob->HP.nType = pMob->Mana.nNum = pMob->Mana.nType = pMob->Damage.nNum =
	 pMob->Damage.nType = 1;
	pMob->HP.nBase = pMob->Mana.nBase = pMob->Damage.nBase = 0;

	pV = DamTypeList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->nDamType = atoi(szVal);

	pMob->AC.nPierce = pMob->AC.nBash = pMob->AC.nSlash = pMob->AC.nMagic = 0;
	pMob->OffBits.Set((long) 0);
	pMob->Immune.Set((long) 0);
	pMob->Resist.Set((long) 0);
	pMob->Vuln.Set((long) 0);

    pV = SizeList.nth(0); 
	GetAtomName(pV->Atom, szVal, 256);
	pMob->cSize = szVal[0];

	pMob->Form.Set((long) 0);
	pMob->Parts.Set((long) 0);

	pV = MaterialList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->sMaterial = szVal;

	pV = RaceList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->sRace = szVal;

	pV = SpecFuncList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pMob->sSpecial = szVal;

	//bring up the dlg
	if (pApplication->ExecDialog(new TMobAttrDlg(this, ID_MOBATTRDLG, pMob)) == IDOK)
	{
		Area.MobList.Add(pMob);
        pMobLB->AddString((LPSTR) pMob);
        UpdateLB(pMobLB, TRUE);
		nLastMobVnum = pMob->nVnum + 1;
    	pMainWnd->bIsDirty = TRUE;
	}
	else
    	delete pMob;
}	//TSandBoxWnd::CMAddMob
