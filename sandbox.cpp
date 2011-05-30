/*
	SANDBOX.CPP

	Copyright (c) 1995  Shawn Halpenny
	All rights reserved.

	Main module for SandBox.
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

//Global variables
PTApplication pApp;
PTSandBoxWnd pMainWnd;
PTListBox pRoomLB, pMobLB, pObjLB, pHelpLB, pSocialLB;
PTMapWnd pMapWnd;
HFONT hCourier;
ifstream is;
ofstream os;
TList<TValPair> ActionList;
TList<TValPair> AffByList;
TList<TValPair> SpecFuncList;
TList<TValPair> ObjFlagsList;
TList<TValPair> ObjWearList;
TList<TValPair> OffensiveList;
TList<TValPair> ImmuneList;
TList<TValPair> ResistList;
TList<TValPair> VulnList;
TList<TValPair> FormsList;
TList<TValPair> PartsList;
TList<TValPair> DamTypeList;
TList<TValPair> SexList;
TList<TValPair> SizeList;
TList<TValPair> MaterialList;
TList<TValPair> AdjustmentsList;
TList<TValPair> ConditionList;
TList<TValPair> DirectionsList;
TList<TValPair> RoomFlagsList;
TList<TValPair> TerrainList;
TList<TValPair> ObjTypeList;
TList<TValPair> PosList;
TList<TValPair> RaceList;
TList<TValPair> DoorStateList;
TList<TValPair> DoorTypeList;
TList<TValPair> WeaponClassList;
TList<TValPair> WeaponTypeList;
TList<TValPair> ContFlagsList;
TList<TValPair> LiquidList;
TList<TValPair> SpellsList;

#ifndef ET
TList<TValPair> PortalTypeList;
#endif

TList<TValPair> ExitTypeList;
TList<TValPair> EWearList;

TSandBoxApp::TSandBoxApp(LPCSTR lpszClassName, HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow) :
 TApplication(lpszClassName, hInstance, hPrevInstance, lpszCmdLine, nCmdShow)
{
    pApp = this;
	InitAtomTable(73);	//enlarge the local atom table for future considerations
	AssignAccel(1);

	LOGFONT lf;
	_fmemset(&lf, 0, sizeof(lf));
	lf.lfHeight = 14;		//fine for 800x600 (a tad device-dependent)
	lf.lfWeight = FW_NORMAL;
    lstrcpy(lf.lfFaceName, "Courier New");
	 
	hCourier = CreateFontIndirect(&lf);
}	//TSandBoxApp::TSandBoxApp

TSandBoxApp::~TSandBoxApp()
{
	DeleteObject(hCourier);
}	//TSandBoxApp::~TSandBoxApp

void TSandBoxApp::InitMainWindow()
{
	pMainWindow = new TSandBoxWnd(NULL, "SandBox");
}	//TSandBoxApp::InitMainWindow

TSandBoxWnd::TSandBoxWnd(PTWindowObject pParent, LPCSTR lpszTitle) : TWindow(pParent, lpszTitle)
{
	pMainWnd = this;
	AssignMenu(1);
	AssignIcon(1);
	SetStyle(WS_OVERLAPPEDWINDOW, 0);
	WndClass.hbrBackground = (HBRUSH) GetStockObject(LTGRAY_BRUSH);

	pRoomListDlg = new TRoomListDlg(this, ID_LISTDLG);
	pMobListDlg = new TMobListDlg(this, ID_LISTDLG);
	pObjListDlg = new TObjListDlg(this, ID_LISTDLG);
    pHelpListDlg = new THelpListDlg(this, ID_LISTDLG);
    pSocialListDlg = new TSocialListDlg(this, ID_LISTDLG);
	pMapWnd = new TMapWnd(this);

	szFilename[0] = 0;	//no initial file
	bIsDirty = FALSE;
}	//TSandBoxWnd::TSandBoxWnd

int PASCAL WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	TSandBoxApp SandBoxApp("SANDBOX::MAIN", hInstance, hPrevInstance, lpszCmdLine, nCmdShow);
	if (ReadValues())	//if no error, run the thing.
		SandBoxApp.Run();

	return SandBoxApp.nStatus; 
}	//WinMain

/*
	Pop up a box to check if you want to save.  If yes...save it.  Return TRUE if save was
	done, FALSE if it wasn't.
*/
BOOL TSandBoxWnd::SaveIfDirty(LPCSTR pszCaption)
{
	TMessage DummyMsg;

	if (bIsDirty)
		switch (pApplication->MsgBox(this, MB_YESNOCANCEL | MB_ICONINFORMATION,
		 pszCaption, "Do you want to save the changes?"))
		{
			case IDYES:
				CMSave(DummyMsg);
				if (!DummyMsg.wParam)	//user didn't cancel save
					break;

			case IDCANCEL:
				return FALSE;
		}

	return TRUE;
}	//TSandBoxWnd::SaveIfDirty

void TSandBoxWnd::CMOpen(RTMessage)
{
	FileDlgData fdd;
    char sz[256];

	SaveIfDirty("Opening a New Area");	//don't care if was actually saved
	fdd.lpszFilters = "Area File (*.are)|*.are|";
	fdd.dwFilterIndex = 1;
	fdd.lpszFile = szFilename;
	fdd.dwMaxFile = 128;
	fdd.lpszFileTitle = NULL;
	fdd.lpszInitialDir = NULL;
	fdd.lpszDefExt = "are";

    //Show the Open dialog
    if (pApplication->FileOpen(this, fdd) != 0)
	{
		ClearData();
		SetCursor(LoadCursor(0, IDC_WAIT));
		is.open(szFilename);
		osLog.open(LOGFILE, _IOLBF, 1024);

		if (!Area.Read(is))
		{
			pApplication->MsgBox(this, MB_ICONEXCLAMATION | MB_OK, "Error in Area File",
			 "Check log file \"%s\" for details.\n", LOGFILE);
        	ClearData();
		}
		else
		{
            sprintf(sz, "SandBox : %s", szFilename);
			SetText(sz);
		}

        is.close();
		osLog.close();

        //Fill the list boxes now and redraw map
		Area.RoomList.ForEach(FillRoomDlg, pRoomLB);
		Area.MobList.ForEach(FillMobDlg, pMobLB);
		Area.ObjList.ForEach(FillObjDlg, pObjLB);
        Area.HelpList.ForEach(FillHelpDlg, pHelpLB);
        Area.SocialList.ForEach(FillSocialDlg, pSocialLB);
		UpdateLB(pRoomLB, FALSE);
		UpdateLB(pMobLB, FALSE);
		UpdateLB(pObjLB, FALSE);
        UpdateLB(pHelpLB, FALSE);
        UpdateLB(pSocialLB, FALSE);

		InvalidateRect(pMapWnd->hWindow, NULL, TRUE);
		UpdateWindow(pMapWnd->hWindow);

		SetCursor(LoadCursor(0, IDC_ARROW));
	}
}	//TSandBoxWnd::CMOpen

void TSandBoxWnd::CMSave(RTMessage Msg)
{
	if (bIsDirty)
		if (szFilename[0])	//if filename exists, save over it
		{
			DoSave();
			Msg.wParam = 0;
        }
		else
			CMSaveAs(Msg);	//otherwise get a filename
}	//TSandBoxWnd::CMSave

void TSandBoxWnd::CMDelMob(RTMessage)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;
	PTBaseMob pMob;
    int n = pMobLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Mobile",
		 "First select a mobile from the mobile list.");
		return;
    }

	pMobLB->GetSelString(sz, 5);
    pMob = (PTBaseMob) *pD;
	Area.MobList.Remove(pMob);

    //kill all resets for this mob
    Area.RoomList.ForEach(DelMobResets, pMob);
	delete pMob;
	pMobLB->DeleteString(n);
	pMainWnd->bIsDirty = TRUE;
    UpdateLB(pMobLB, TRUE);
}	//TSandBoxWnd::CMDelMob

/*
	Iterates through each mob reset for a room and kills the Mreset, G and E resets for that
    mob (p).
*/
void DelMobResets(PTRoom pRoom, void* p)
{
	PTBaseMob pMob = (PTBaseMob) p;
    PTMReset pMReset;

	//now kill the M resets themselves for pMob
	TListIterator<TMReset> Iter(pRoom->MobResetList);
	while (int(Iter))
	{
        pMReset = Iter++;
		if (pMReset->nVnum == pMob->nVnum)
		{
			//we can delete from the list we are iterating over because we are removing an
			//item we have already passed (i.e. the iterator is pointing to the next
			//item already)
			pRoom->MobResetList.Remove(pMReset);
			delete pMReset;		//also kills G and E resets
		}
	}
}	//DelMobResets

void TSandBoxWnd::CMDelObj(RTMessage)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;
	PTBaseObj pObj;
    int n = pObjLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Object",
		 "First select an object from the object list.");
		return;
    }

	pObjLB->GetSelString(sz, 5);
    pObj = (PTBaseObj) *pD;
	Area.ObjList.Remove(pObj);

	//kill all resets for this obj
	Area.RoomList.ForEach(DelObjResets, pObj);
	delete pObj;
	pObjLB->DeleteString(n);
	pMainWnd->bIsDirty = TRUE;
    UpdateLB(pObjLB, TRUE);
}	//TSandBoxWnd::CMDelObj

/*
	Iterates through each obj reset in pRoom and kills all resets assoc'd with obj p
*/
void DelObjResets(PTRoom pRoom, void* p)
{
	PTBaseObj pObj = (PTBaseObj) p;
	PTOReset pOReset;

	//now kill the O resets themselves for pObj
	TListIterator<TOReset> Iter(pRoom->ObjResetList);
	while (int(Iter))
	{
        pOReset = Iter++;
		if (pOReset->nVnum == pObj->nVnum)
		{
			//we can delete from the list we are iterating over because we are removing an
			//item we have already passed (i.e. the iterator is pointing to the next
			//item already)
			pRoom->ObjResetList.Remove(pOReset);
			delete pOReset;		//also kills P resets (can't put stuff in obj if it's not here)
		}
	}
}	//DelObjResets

void TSandBoxWnd::CMDelRoom(RTMessage)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;
	PTRoom pRoom;
    int n = pRoomLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Room",
		 "First select a room from the room list.");
		return;
    }

	pRoomLB->GetSelString(sz, 5);
    pRoom = (PTRoom) *pD;
	Area.RoomList.Remove(pRoom);	//all resets are destroyed too
	delete pRoom;

	pRoomLB->DeleteString(n);
	pMainWnd->bIsDirty = TRUE;
    UpdateLB(pRoomLB, TRUE);
}	//TSandBoxWnd::CMDelRoom

/*
	Do the save.
*/
void TSandBoxWnd::DoSave()
{
	SetCursor(LoadCursor(0, IDC_WAIT));
	os.open(szFilename, _IOFBF);
	osLog.open(LOGFILE, _IOLBF, 1024);

	if (!Area.Write(os))
		pApplication->MsgBox(this, MB_ICONEXCLAMATION | MB_OK, "Error Writing Area File",
		 "Check the log file\n%s\nfor details.", LOGFILE);

	os.close();
	osLog.close();
    bIsDirty = FALSE;
	SetCursor(LoadCursor(0, IDC_ARROW));
}	//TSandBoxWnd::DoSave

void TSandBoxWnd::CMSaveAs(RTMessage)
{
	FileDlgData fdd;
	MSG Msg;
    char sz[256];

	fdd.lpszFilters = "Area File (*.are)|*.are|";
	fdd.dwFilterIndex = 1;
	fdd.lpszFile = szFilename;
	fdd.dwMaxFile = 128;
	fdd.lpszFileTitle = NULL;
	fdd.lpszInitialDir = NULL;
	fdd.lpszDefExt = "are";

	//Set Msg.wParam to TRUE if the save was canceled, FALSE if not canceled.  Changing
	//wParam doesn't matter since we are processing the message.  Reason:  some functions
	//call this function explicitly and need to know if save went through.  Can't change
	//lResult because it is used only for Windows.
	Msg.wParam = !pApplication->FileSave(this, fdd);
	if (!Msg.wParam)
	{
		DoSave();
        sprintf(sz, "SandBox : %s", szFilename);
		SetText(sz);
	}
}	//TSandBoxWnd::CMSaveAs

void TSandBoxWnd::CMExit(RTMessage)
{
    SendMessage(hWindow, WM_CLOSE, 0, 0);
}	//TSandBoxWnd::CMExit

void TSandBoxWnd::CMArrange(RTMessage)
{
	RECT r;

    GetClientRect(hWindow, &r);
	SizeAndPos((LPPOINT) &r.right);
}	//TSandBoxWnd::CMArrange

BOOL TSandBoxWnd::CanClose()
{
	if (!TWindowObject::CanClose())	//make sure children say ok to close
		return FALSE;

    return SaveIfDirty("Exiting SandBox");
}	//TSandBoxWnd::CanClose

void TSandBoxWnd::CMNew(RTMessage)
{
	SaveIfDirty("New Area");
    ClearData();
}	//TSandBoxWnd::CMNew

/*
	Size and position child windows.
*/
void TSandBoxWnd::SizeAndPos(LPPOINT lpPt)
{
	RECT r;
	int nHeight, nWidth;

	//only get size for one of them, since they are all same size
	GetWindowRect(pRoomListDlg->hWindow, &r);
	nHeight = r.bottom - r.top;
	nWidth = r.right - r.left;
	r.left = r.top = r.right = 0;
	r.bottom = nHeight;
	ClientToScreen(hWindow, (LPPOINT) &r);
	ClientToScreen(hWindow, (LPPOINT) &r.right);

	//Position and size the modeless dlgs and map window
	SetWindowPos(pRoomListDlg->hWindow, NULL, r.left, r.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(pHelpListDlg->hWindow, NULL, r.left + nWidth, r.top, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(pMobListDlg->hWindow, NULL, r.left, r.top + nHeight, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(pSocialListDlg->hWindow, NULL, r.left + nWidth, r.top + nHeight, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(pObjListDlg->hWindow, NULL, r.left, r.top + nHeight * 2, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
	SetWindowPos(pMapWnd->hWindow, NULL, nWidth, 0, lpPt->x - nWidth, lpPt->y, SWP_NOACTIVATE | SWP_NOZORDER);

	//make them visible
	pRoomListDlg->Show();
	pMobListDlg->Show();
	pObjListDlg->Show();
	pHelpListDlg->Show();
    pSocialListDlg->Show();
	pMapWnd->Show();
}	//TSandBoxWnd::SizeAndPos

void TSandBoxWnd::WMSize(RTMessage Msg)
{
	POINT pt = MAKEPOINT(Msg.lParam);

	if (Msg.wParam != SIZE_MINIMIZED)
		SizeAndPos(&pt);
}	//TSandBoxWnd::WMSize

void TSandBoxWnd::WMCreate(RTMessage Msg)
{
	//chain to original
	TWindow::WMCreate(Msg);

	pRoomListDlg->Create();
	pMobListDlg->Create();
	pObjListDlg->Create();
	pHelpListDlg->Create();
	pSocialListDlg->Create();
}	//TSandBoxWnd::WMCreate

TRoomListDlg::TRoomListDlg(PTWindowObject pParent, int nID) : TModelessDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pRoomLB = new TListBox(this, LBID_LIST);
}	//TRoomListDlg::TRoomListDlg

void TRoomListDlg::IDRoomLB(RTMessage Msg)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pRoomLB->GetSelString(sz, 5);
		if (pApplication->ExecDialog(new TRoomAttrDlg(pMainWnd, ID_ROOMATTRDLG, (PTRoom) *pD)) == IDOK)
		{
			//redraw room list to show change
			pMainWnd->bIsDirty = TRUE;
			UpdateLB(pRoomLB, TRUE);
		}
    }
}	//TRoomListDlg::IDRoomLB

TMobListDlg::TMobListDlg(PTWindowObject pParent, int nID) : TModelessDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pMobLB = new TListBox(this, LBID_LIST);
}	//TMobListDlg::TMobListDlg

TObjListDlg::TObjListDlg(PTWindowObject pParent, int nID) : TModelessDialog(pParent, nID)
{
	SetFlags(WB_TRANSFER, FALSE);
	pObjLB = new TListBox(this, LBID_LIST);
}	//TObjListDlg::TObjListDlg

void TObjListDlg::IDObjLB(RTMessage Msg)
{
	char sz[5];
	DWORD* pD = (DWORD*) sz;

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pObjLB->GetSelString(sz, 5);
		if (pApplication->ExecDialog(new TObjAttrDlg(pMainWnd, ID_OBJATTRDLG, (PTBaseObj) *pD)) == IDOK)
		{
			//redraw obj list to show change
			pMainWnd->bIsDirty = TRUE;
			UpdateLB(pObjLB, TRUE);
		}
    }
}	//TObjListDlg::IDObjLB

TMapWnd::TMapWnd(PTWindowObject pParent) : TWindow(pParent, "", 100)
{
	SetFlags(WB_TRANSFER, FALSE);
    SetFlags(WB_AUTOCREATE, TRUE);
	WndClass.lpszClassName = "CHILD::MAPWND";
    WndClass.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH);
    SetStyle(WS_CHILD | WS_BORDER | WS_HSCROLL | WS_VSCROLL, 0);
}	//TMapWnd::TMapWnd

/*
	Iteration func that adds pRoom to the list box.
*/
void FillRoomDlg(PTRoom pRoom, void *p)
{
	PTListBox pLB = (PTListBox) p;

    pLB->AddString((LPCSTR) pRoom);
}	//FillRoomDlg

/*
	Iteration func that adds pMob to the list box.
*/
void FillMobDlg(PTBaseMob pMob, void *p)
{
	PTListBox pLB = (PTListBox) p;

	pLB->AddString((LPCSTR) pMob);
}	//FillMobDlg

/*
	Iteration func that adds pObj to the list box.
*/
void FillObjDlg(PTBaseObj pObj, void *p)
{
	PTListBox pLB = (PTListBox) p;

	pLB->AddString((LPCSTR) pObj);
}	//FillObjDlg

TObjAttrDlg::TObjAttrDlg(PTWindowObject pParent, int nID, PTBaseObj p) : TDialog(pParent, nID)
{
	PTListBox pLB;

	SetFlags(WB_TRANSFER, FALSE);
    pObj = p;

    new TEdit(this, EID_VNUM, 6);
	new TEdit(this, EID_WEIGHT, 6);
	new TEdit(this, EID_VALUE, 6);
	new TEdit(this, EID_LEVEL, 6);
	new TEdit(this, EID_KEYWORDS, 256);
	new TEdit(this, EID_NAME, 256);
	new TEdit(this, EID_LONGDESC, 4096);
	new TComboBox(this, CBID_TYPE);
	new TComboBox(this, CBID_MATERIAL);
	new TComboBox(this, CBID_CONDITION);
	new TListBox(this, LBID_ADJUST);

	pLB = new TListBox(this, LBID_FLAGS);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	pLB = new TListBox(this, LBID_WEARFLAGS);
	pLB->SetStyle(pLB->dwStyle | LBS_MULTIPLESEL, 0);

	new TListBox(this, LBID_EXDESC);
}	//TObjAttrDlg::TObjAttrDlg

LRESULT TObjAttrDlg::InitDialog()
{
	PTEdit pE;
	PTListBox pLB;
	PTComboBox pCB;
	char sz[256];

	pE = (PTEdit) GetChild(EID_VNUM);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_VNUM, pObj->nVnum, FALSE);
	pE->SetSel(0, 32767); 
	pE->SetFocus();

	pE = (PTEdit) GetChild(EID_WEIGHT);
	pE->LimitText(5);
	SetDlgItemInt(hWindow, EID_WEIGHT, pObj->nWeight, FALSE);

	pE = (PTEdit) GetChild(EID_VALUE);
    pE->LimitText(10);
    SetDlgItemLong(hWindow, EID_VALUE, pObj->lValue, TRUE);

	pE = (PTEdit) GetChild(EID_KEYWORDS);
    pE->LimitText(255);
    pE->SetText(pObj->sKeywords);

	pE = (PTEdit) GetChild(EID_NAME);
	pE->LimitText(255);
    pE->SetText(pObj->sName);

	pE = (PTEdit) GetChild(EID_LONGDESC);
	SendMessage(pE->hWindow, WM_SETFONT, (WPARAM) hCourier, 0);
	pE->LimitText(4095);
    pE->SetText(pObj->sLongDesc);

	pCB = (PTComboBox) GetChild(CBID_TYPE);
	ObjTypeList.ForEach(FillBox, pCB);
	SelectCB(pCB, ObjTypeList, pObj->nType);

	pLB = (PTListBox) GetChild(LBID_FLAGS);
	ObjFlagsList.ForEach(FillBox, pLB);
	SelectLB(pLB, ObjFlagsList, pObj->Flags);

	pLB = (PTListBox) GetChild(LBID_WEARFLAGS);
	ObjWearList.ForEach(FillBox, pLB);
	SelectLB(pLB, ObjWearList, pObj->WearFlags);

	//handle the adjustments
	TListIterator<TASection> Iter(pObj->AList);
	TListIterator<TValPair> PairIter(AdjustmentsList);
	PTValPair pV;
	PTASection pA;
    BOOL bFound = FALSE;

	pLB = (PTListBox) GetChild(LBID_ADJUST);
	while (int(PairIter))	//for each available adjustment
	{
        pV = PairIter++;
        Iter.Restart();
		while (int(Iter))	//for each adjustment on this obj 
		{
			pA = Iter++;
            GetAtomName(pV->Atom, sz, 256);
			if (atoi(sz) == pA->nType)	//this one have a value?
			{
				sprintf(sz, "%s %+d", (const char*) pV->sName, pA->nVal);
				pLB->AddString(sz);
				bFound = TRUE;
            	break;
            }
			else
				bFound = FALSE;
		}

		if (!bFound)
        {
			lstrcpy(sz, pV->sName);
			pLB->AddString(sz);
		}
    }

	//put exdescs in internal TESection list so mods can be done without directly modifying pObj
	TListIterator<TESection> EDIter(pObj->EList);
    PTESection pES, pEOld;
    pLB = (PTListBox) GetChild(LBID_EXDESC);
	while (int(EDIter))
	{
        pES = EDIter++;
		pLB->AddString(pES->sKeywords);
		pEOld = new TESection;
		pEOld->sKeywords = pES->sKeywords;
        pEOld->sDesc = pES->sDesc;
		ExDescList.Add(pEOld);
    }

	if (pObj->Type() == NEW)
	{
		PTObject pObject = (PTObject) pObj;

		pCB = (PTComboBox) GetChild(CBID_MATERIAL);
		MaterialList.ForEach(FillBox, pCB);
		if (!pObject->sMaterial.Len())
			pCB->SetSelIndex(0);
        else
			SelectCB(pCB, MaterialList, pObject->sMaterial);

		pE = (PTEdit) GetChild(EID_LEVEL);
    	pE->LimitText(5);
		SetDlgItemInt(hWindow, EID_LEVEL, pObject->nLevel, FALSE);

		pCB = (PTComboBox) GetChild(CBID_CONDITION);
		ConditionList.ForEach(FillBox, pCB);
		sprintf(sz, "%c", pObject->cCondition);
		SelectCB(pCB, ConditionList, sz);

		//save values for the value dlg
		for (int i=0; i<5; i++)
			aNewFlags[i] = pObject->Values[i];
	}
	else	//object is old-style
	{
		GetChild(CBID_MATERIAL)->Enable(FALSE);
		GetChild(EID_LEVEL)->Enable(FALSE);
        GetChild(CBID_CONDITION)->Enable(FALSE);

		//save values for the value dlg
		for (int i=0; i<4; i++)
			alNewValues[i] = ((PTObjOld) pObj)->lValues[i];
    }

	return 0;
}	//TObjAttrDlg::InitDialog

void TObjAttrDlg::Okay(RTMessage Msg)
{
	PTListBox pLB;
	PTASection pA;
	PTBaseObj pCheckObj;
	char sz[4096], *p;
    int i, n;

    //get data from controls
	pObj->nVnum = GetDlgItemInt(hWindow, EID_VNUM, NULL, FALSE);

	//check for duplicate vnum
    pCheckObj = Area.FindObj(pObj->nVnum);
	if (pCheckObj && pCheckObj != pObj && pCheckObj->nVnum == pObj->nVnum)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Add Object",
		 "An object with vnum = %d already exists.", pObj->nVnum);
		return;
	}

	pObj->nWeight = GetDlgItemInt(hWindow, EID_WEIGHT, NULL, FALSE);

	GetChild(EID_VALUE)->GetText(sz, 256);
	pObj->lValue = strtol(sz, NULL, 10);

	GetDlgItemText(hWindow, EID_KEYWORDS, sz, 256);
	pObj->sKeywords = sz;

	GetDlgItemText(hWindow, EID_NAME, sz, 256);
	pObj->sName = sz;

	GetDlgItemText(hWindow, EID_LONGDESC, sz, 4096);
    pObj->sLongDesc = sz;
			  
	GetBoxSel((PTComboBox) GetChild(CBID_TYPE), ObjTypeList, sz, 256);
	pObj->nType = atoi(sz);

	GetBoxFlags((PTListBox) GetChild(LBID_FLAGS), ObjFlagsList, pObj->Flags);
	GetBoxFlags((PTListBox) GetChild(LBID_WEARFLAGS), ObjWearList, pObj->WearFlags);

	//get the adjustments
	pObj->AList.RemoveAll(TRUE);	//kill all old ones
    pLB = (PTListBox) GetChild(LBID_ADJUST);
    n = pLB->GetCount();
	for (i=0; i<n; i++)	//loop through the adjust list box strings
	{
		pLB->GetString(sz, i, 256);
		p = (sz + lstrlen(sz) - 1);

		//as long as last chars of str are digits, we are fine
		while (p > sz && isdigit(*p))
			p--;

		if (*p == '-' || *p == '+')
        {
			pA = new TASection;
			pA->nVal = atoi(p);
			GetBoxSel(pLB, AdjustmentsList, sz, 256, i);
			pA->nType = atoi(sz);
        	pObj->AList.Add(pA);
		}
    }

	//get ex descs
	pObj->EList.RemoveAll(TRUE);	//kill old ones
	TListIterator<TESection> Iter(ExDescList);
	while (int(Iter))	//iterate over the ex descs in the LB
        pObj->EList.Add(Iter++);

	//empty the internal list here, so that ~TList doesn't destroy its data (since the data
	//now "belongs" to pObj->EList) when this is destroyed
    ExDescList.RemoveAll(FALSE);

	//handle new stuff
	if (pObj->Type() == NEW)
	{
		for (i=0; i<5; i++)
			((PTObject) pObj)->Values[i] = aNewFlags[i];

		GetBoxSel((PTComboBox) GetChild(CBID_MATERIAL), MaterialList, sz, 256);
		((PTObject) pObj)->sMaterial = sz;

		GetBoxSel((PTComboBox) GetChild(CBID_CONDITION), ConditionList, sz, 256);
		((PTObject) pObj)->cCondition = sz[0];

		((PTObject) pObj)->nLevel = GetDlgItemInt(hWindow, EID_LEVEL, NULL, FALSE);
    }
	else	//object is old-style
		for (i=0; i<4; i++)
			((PTObjOld) pObj)->lValues[i] = alNewValues[i];

	//chain to original
    TDialog::Okay(Msg);
}	//TObjAttrDlg::Okay

void TObjAttrDlg::IDAdjustLB(RTMessage Msg)
{
	PTListBox pLB = (PTListBox) GetChild(LBID_ADJUST);
	char sz[256];
    int n = pLB->GetSelIndex();

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB->GetSelString(sz, 256);
		if (pApplication->ExecDialog(new TAdjustDlg(this, ID_ADJUSTDLG, sz)) == IDOK)
		{
            //remove old and insert new
			pLB->DeleteString(n);
            pLB->InsertString(sz, n);

			//redraw LB
			UpdateWindow(pLB->hWindow);
		}
	}
}	//TObjAttrDlg::IDAdjustLB

void TObjAttrDlg::IDDelExDesc(RTMessage)
{
    PTListBox pLB = (PTListBox) GetChild(LBID_EXDESC);
	int n = pLB->GetSelIndex();
    PTESection pE;

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete Extra Description",
		 "Select an extra description first.");
		return;
	}

	pE = ExDescList.nth(n);
    ExDescList.Remove(pE);
    delete pE;
	pLB->DeleteString(n);

    UpdateWindow(pLB->hWindow);
}	//TObjAttrDlg::IDDelExDesc

void TObjAttrDlg::IDAddExDesc(RTMessage)
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
}	//TObjAttrDlg::IDAddExDesc

void TObjAttrDlg::IDExDescLB(RTMessage Msg)
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
}	//TObjAttrDlg::IDExDescLB

void TSandBoxWnd::CMAddRoom(RTMessage)
{
	static int nLastRoomVnum = 1;
	PTRoom pRoom = new TRoom;
	char szVal[256];
	PTValPair pV;

	//init defaults
	pRoom->nVnum = nLastRoomVnum;
	pRoom->Flags.Set((long) 0);
	pV = TerrainList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pRoom->nTerrain = atoi(szVal);

	//bring up the dlg
	if (pApplication->ExecDialog(new TRoomAttrDlg(pMainWnd, ID_ROOMATTRDLG, pRoom)) == IDOK)
	{
		Area.RoomList.Add(pRoom);
        pRoomLB->AddString((LPSTR) pRoom);
        UpdateLB(pRoomLB, TRUE);
		nLastRoomVnum = pRoom->nVnum + 1;
		pMainWnd->bIsDirty = TRUE;
	}
	else
		delete pRoom;
}	//TSandBoxWnd::CMAddRoom

void TSandBoxWnd::CMAddObj(RTMessage)
{
	static int nLastObjVnum = 1;
	PTObject pObj = new TObject;
	char szVal[256];
    PTValPair pV;

    //init defaults
	pObj->nVnum = nLastObjVnum;

	pV = ObjTypeList.nth(0);	//no hardcoding values, so use first one
	GetAtomName(pV->Atom, szVal, 256);
	pObj->nType = atoi(szVal);

	pObj->Flags.Set((long) 0);
	pObj->WearFlags.Set((long) 0);
	pObj->nWeight = 0;
    pObj->lValue = 0;

	pV = MaterialList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pObj->sMaterial = szVal;

	for (int i=0; i<5; i++)
    	pObj->Values[i].Set((long) 0);

	pObj->nLevel = 1;

	pV = ConditionList.nth(0);
	GetAtomName(pV->Atom, szVal, 256);
	pObj->cCondition = szVal[0];

	//bring up the dlg
	if (pApplication->ExecDialog(new TObjAttrDlg(this, ID_OBJATTRDLG, pObj)) == IDOK)
	{
		Area.ObjList.Add(pObj);
        pObjLB->AddString((LPSTR) pObj);
        UpdateLB(pObjLB, TRUE);
		nLastObjVnum = pObj->nVnum + 1;
		pMainWnd->bIsDirty = TRUE;
	}
	else
		delete pObj;
}	//TSandBoxWnd::CMAddObj

void TObjAttrDlg::IDValues(RTMessage)
{
	char sz[256];
	int nType;
	long l[5];
    PTDialog pDialog;

	//this is the only part that it hard-coded to take use different dialogs for each
	//obj type.  If the object isn't a supported type, a simple dialog is brought up that
	//allows entering of the five values

	if (((PTComboBox) GetChild(CBID_TYPE))->GetSelIndex() < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Values",
		 "Choose an object type first.");
		return;
	}

	GetBoxSel((PTComboBox) GetChild(CBID_TYPE), ObjTypeList, sz, 256);
	nType = atoi(sz);

	l[0] = pObj->Type() == NEW ? aNewFlags[0].Get() : alNewValues[0];
	l[1] = pObj->Type() == NEW ? aNewFlags[1].Get() : alNewValues[1];
	l[2] = pObj->Type() == NEW ? aNewFlags[2].Get() : alNewValues[2];
	l[3] = pObj->Type() == NEW ? aNewFlags[3].Get() : alNewValues[3];
    l[4] = pObj->Type() == NEW ? aNewFlags[4].Get() : 0;

	switch (nType)
	{
        case ITEM_LIGHT:
		case ITEM_MONEY:
			pDialog = new TLightDlg(this, ID_LIGHTDLG, nType, l);
            break;

        case ITEM_FOOD:
			pDialog = new TFoodDlg(this, ID_FOODDLG, nType, l);
			break;

		case ITEM_WAND:
		case ITEM_STAFF:
			pDialog = new TWandDlg(this, ID_WANDDLG, nType, l);
            break;

		case ITEM_WEAPON:
			pDialog = new TWeaponDlg(this, ID_WEAPONDLG, pObj, nType, l);
			break;

        case ITEM_TREASURE:
		case ITEM_CLOTHING:
        case ITEM_FURNITURE:
        case ITEM_TRASH:
		case ITEM_KEY:
		case ITEM_BOAT:
		case ITEM_FOUNTAIN:
		case ITEM_PROTECT:
			pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Object Type",
			 "No values are needed for that type of object.");
			return;

		case ITEM_ARMOR:
		{
			TArmorClass AC;

			AC.nPierce = (int) (pObj->Type() == NEW ? aNewFlags[0].Get() : alNewValues[0]);
			AC.nBash = (int) (pObj->Type() == NEW ? aNewFlags[1].Get() : alNewValues[1]);
			AC.nSlash = (int) (pObj->Type() == NEW ? aNewFlags[2].Get() : alNewValues[2]);
			AC.nMagic = (int) (pObj->Type() == NEW ? aNewFlags[3].Get() : alNewValues[3]);

			if (pApplication->ExecDialog(new TACDlg(this, ID_ACDLG, &AC)) == IDOK)
				if (pObj->Type() == NEW)
				{
					aNewFlags[0].Set(AC.nPierce);
					aNewFlags[1].Set(AC.nBash);
					aNewFlags[2].Set(AC.nSlash);
					aNewFlags[3].Set(AC.nMagic);
				}
				else
				{
					alNewValues[0] = AC.nPierce;
					alNewValues[1] = AC.nBash;
					alNewValues[2] = AC.nSlash;
					alNewValues[3] = AC.nMagic;
				}
		}
        return;

        case ITEM_SCROLL:
		case ITEM_POTION:
		case ITEM_PILL:
			pDialog = new TPotionDlg(this, ID_POTIONDLG, nType, l);
            break;

		case ITEM_CONTAINER:
			pDialog = new TContainerDlg(this, ID_CONTAINERDLG, nType, l);
            break;

		case ITEM_DRINK_CON:
			pDialog = new TDrinkDlg(this, ID_DRINKDLG, nType, l);
			break;

        #ifndef ET
		case ITEM_PORTAL:
			pDialog = new TPortalDlg(this, ID_PORTALDLG, nType, l);
			break;
        #endif

		default: //generic values dlg
			((PTComboBox) GetChild(CBID_TYPE))->GetSelString(sz, 256);
			pDialog = new TGenericDlg(this, ID_GENERICDLG, pObj, sz, nType, l);
	}

	if (pApplication->ExecDialog(pDialog) == IDCANCEL)
		return;

	if (pObj->Type() == NEW)
	{
		aNewFlags[0].Set(l[0]);
		aNewFlags[1].Set(l[1]);
		aNewFlags[2].Set(l[2]);
		aNewFlags[3].Set(l[3]);
    	aNewFlags[4].Set(l[4]);
	}
	else
	{
		alNewValues[0] = l[0];
		alNewValues[1] = l[1];
		alNewValues[2] = l[2];
		alNewValues[3] = l[3];
	}
}	//TObjAttrDlg::IDValues

void TObjAttrDlg::IDTypeCB(RTMessage Msg)
{
	//push the values button if the item type changes
	if (Msg.LP.Hi == CBN_SELCHANGE)
		IDValues(Msg);	
}	//TObjAttrDlg::IDTypeCB

void TSandBoxWnd::CMName(RTMessage)
{
	char sz[255];

	lstrcpy(sz, Area.sTitle);
	if (pApplication->Input(this, sz, 255, "Area name:") == IDOK)
	{
		Area.sTitle = sz;
		pMainWnd->bIsDirty = TRUE;
    }
}	//TSandBoxWnd::CMName

void TMobResetDlg::IDAddGReset(RTMessage)
{
	PTGReset pG = new TGReset(1, 0, -1);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_GRESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TGResetDlg(this, ID_GRESETDLG, pG)) == IDOK)
	{
        //save the changes
        pMReset->GResetList.Add(pG);
		pObj = Area.FindObj(pG->nVnum);
		sprintf(sz, "%d : %s", pG->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
    	delete pG;
}	//TMobResetDlg::IDAddGReset

void TMobResetDlg::IDAddEReset(RTMessage)
{
	PTEReset pE = new TEReset(1, 0, -1, 0);
	PTBaseObj pObj;
	char sz[256];
    PTListBox pLB = (PTListBox) GetChild(LBID_ERESETS);

	//bring up the dlg
	if (pApplication->ExecDialog(new TEResetDlg(this, ID_ERESETDLG, pE)) == IDOK)
	{
        //save the changes
        pMReset->EResetList.Add(pE);
		pObj = Area.FindObj(pE->nVnum);
		sprintf(sz, "%d : %s", pE->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");

        pLB->AddString(sz);
		UpdateWindow(pLB->hWindow);
    }
	else
		delete pE;
}	//TMobResetDlg::IDAddEReset

void TMobResetDlg::IDDelGReset(RTMessage)
{
    PTGReset pG;
	PTListBox pLB = (PTListBox) GetChild(LBID_GRESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'G' Reset",
		 "First select a reset from the list.");
		return;
    }

    //remove it from the internal list
	pMReset->GResetList.Remove(pG = pMReset->GResetList.nth(n));
    delete pG;	//and kill it
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TMobResetDlg::IDDelGReset

void TMobResetDlg::IDDelEReset(RTMessage)
{
    PTEReset pE;
	PTListBox pLB = (PTListBox) GetChild(LBID_ERESETS);
    int n = pLB->GetSelIndex();

	if (n < 0)
	{
		pApplication->MsgBox(this, MB_ICONINFORMATION | MB_OK, "Delete 'E' Reset",
		 "First select a reset from the list.");
		return;
    }

    //remove it from the internal list
	pMReset->EResetList.Remove(pE = pMReset->EResetList.nth(n));
    delete pE;	//and kill it
	pLB->DeleteString(n);

	UpdateWindow(pLB->hWindow);
}	//TMobResetDlg::IDDelEReset

void TMobResetDlg::IDGResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTGReset pG;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_GRESETS);
		n = pLB->GetSelIndex();
		pG = pMReset->GResetList.nth(n);

		if (pApplication->ExecDialog(new TGResetDlg(this, ID_GRESETDLG, pG)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pG->nVnum);
			sprintf(sz, "%d : %s", pG->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TMobResetDlg::IDGResetLB

void TMobResetDlg::IDEResetLB(RTMessage Msg)
{
	PTListBox pLB;
	PTEReset pE;
    PTBaseObj pObj;
	int n;
    char sz[256];

	if (Msg.LP.Hi == LBN_DBLCLK)
	{
		pLB = (PTListBox) GetChild(LBID_ERESETS);
		n = pLB->GetSelIndex();
		pE = pMReset->EResetList.nth(n);

		if (pApplication->ExecDialog(new TEResetDlg(this, ID_ERESETDLG, pE)) == IDOK)
		{
			//redraw list to show change
			pObj = Area.FindObj(pE->nVnum);
			sprintf(sz, "%d : %s", pE->nVnum, pObj ? (const char*) pObj->sName : "<non-area object>");
			pLB->DeleteString(n);
			pLB->InsertString(sz, n);

			UpdateWindow(pLB->hWindow);
		}
    }
}	//TMobResetDlg::IDEResetLB
