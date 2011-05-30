/*
	SANDBOX.H

	Copyright (c) 1994-1995  Shawn Halpenny
	All rights reserved.
*/

#ifndef __SANDBOX_H
#define __SANDBOX_H

#include "sbox-id.h"	//resource IDs

#define LOGFILE "sandbox.log"
#define ITEM_LIGHT		      1
#define ITEM_SCROLL		      2
#define ITEM_WAND		      3
#define ITEM_STAFF		      4
#define ITEM_WEAPON		      5
#define ITEM_TREASURE		      8
#define ITEM_ARMOR		      9
#define ITEM_POTION		     10
#define ITEM_CLOTHING		     11
#define ITEM_FURNITURE		     12
#define ITEM_TRASH		     13
#define ITEM_CONTAINER		     15
#define ITEM_DRINK_CON		     17
#define ITEM_KEY		     18
#define ITEM_FOOD		     19
#define ITEM_MONEY		     20
#define ITEM_BOAT		     22
#define ITEM_CORPSE_NPC		     23
#define ITEM_CORPSE_PC		     24
#define ITEM_FOUNTAIN		     25
#define ITEM_PILL		     26
#define ITEM_PROTECT		     27
#define ITEM_MAP		     28
#define ITEM_PORTAL                  29

_CLASSDEF(TSandBoxApp)
_CLASSDEF(TSandBoxWnd)
_CLASSDEF(TRoomListDlg)
_CLASSDEF(TMobListDlg)
_CLASSDEF(TObjListDlg)
_CLASSDEF(TMapWnd)
_CLASSDEF(TValPair)
_CLASSDEF(TMobAttrDlg)
_CLASSDEF(TShopDlg)
_CLASSDEF(TDiceDlg)
_CLASSDEF(TACDlg)
_CLASSDEF(TFlagDlg)
_CLASSDEF(TObjAttrDlg)
_CLASSDEF(TAdjustDlg)
_CLASSDEF(TExDescDlg)
_CLASSDEF(TLightDlg)
_CLASSDEF(TPotionDlg)
_CLASSDEF(TWandDlg)
_CLASSDEF(TWeaponDlg)
_CLASSDEF(TGenericDlg)
_CLASSDEF(TFoodDlg)
_CLASSDEF(TContainerDlg)
_CLASSDEF(TDrinkDlg)
_CLASSDEF(TPortalDlg)
_CLASSDEF(TRoomAttrDlg)
_CLASSDEF(TDoorDlg)
_CLASSDEF(TRoomAttrDlg)
_CLASSDEF(TMobResetDlg)
_CLASSDEF(TObjResetDlg)
_CLASSDEF(TPResetDlg)
_CLASSDEF(TEResetDlg)
_CLASSDEF(TGResetDlg)
_CLASSDEF(THelpListDlg)
_CLASSDEF(THelpDlg)
_CLASSDEF(TSocialListDlg)
_CLASSDEF(TSocialDlg)

class TValPair
{
public:
	TString sName;	//string user selects this value by
    ATOM Atom;		//the value (although case-insensitive, it won't matter)

	TValPair(const char*, const char*);
	~TValPair() { DeleteAtom(Atom); }
};

class TSandBoxApp : public TApplication
{
public:
	TSandBoxApp(LPCSTR, HINSTANCE, HINSTANCE, LPSTR, int);
    ~TSandBoxApp();
	virtual void InitMainWindow();
};

class TSandBoxWnd : public TWindow
{
	char szFilename[128];

	void DoSave();
	BOOL SaveIfDirty(LPCSTR);
	void SizeAndPos(LPPOINT);

public:
	PTModelessDialog pRoomListDlg, pMobListDlg, pObjListDlg, pHelpListDlg,
	 pSocialListDlg;
	BOOL bIsDirty;

	TSandBoxWnd(PTWindowObject, LPCSTR);
	virtual void CMOpen(RTMessage) = [CM_FIRST + MENU_OPEN];
	virtual void CMSave(RTMessage) = [CM_FIRST + MENU_SAVE];
	virtual void CMSaveAs(RTMessage) = [CM_FIRST + MENU_SAVEAS];
    virtual void CMExit(RTMessage) = [CM_FIRST + MENU_EXIT]; 
    virtual void CMNew(RTMessage) = [CM_FIRST + MENU_NEW];
    virtual void CMArrange(RTMessage) = [CM_FIRST + MENU_ARRANGE];
	virtual void WMSize(RTMessage) = [WM_FIRST + WM_SIZE];
    virtual void WMCreate(RTMessage) = [WM_FIRST + WM_CREATE];
    virtual void CMAbout(RTMessage) = [CM_FIRST + MENU_ABOUT];
    virtual void CMAddMob(RTMessage) = [CM_FIRST + MENU_MOBILE];
    virtual void CMDelMob(RTMessage) = [CM_FIRST + MENU_DELMOBILE];
	virtual void CMDelObj(RTMessage) = [CM_FIRST + MENU_DELOBJECT];
	virtual void CMAddRoom(RTMessage) = [CM_FIRST + MENU_ROOM];
	virtual void CMDelRoom(RTMessage) = [CM_FIRST + MENU_DELROOM];
	virtual void CMAddObj(RTMessage) = [CM_FIRST + MENU_OBJECT];
    virtual void CMName(RTMessage) = [CM_FIRST + MENU_NAME];
	virtual void CMAddHelp(RTMessage) = [CM_FIRST + MENU_ADDHELP];
	virtual void CMDelHelp(RTMessage) = [CM_FIRST + MENU_DELHELP];
	virtual void CMAddSocial(RTMessage) = [CM_FIRST + MENU_ADDSOCIAL];
	virtual void CMDelSocial(RTMessage) = [CM_FIRST + MENU_DELSOCIAL];
	BOOL CanClose();
};

class TRoomListDlg : public TModelessDialog
{
public:
	TRoomListDlg(PTWindowObject, int);
	LRESULT InitDialog();
    virtual void WMDrawItem(RTMessage) = [WM_FIRST + WM_DRAWITEM];
	virtual void IDRoomLB(RTMessage) = [ID_FIRST + LBID_LIST];
};

class TMobListDlg : public TModelessDialog
{
public:
	TMobListDlg(PTWindowObject, int);
	LRESULT InitDialog();
	virtual void WMDrawItem(RTMessage) = [WM_FIRST + WM_DRAWITEM];
	virtual void IDMobLB(RTMessage) = [ID_FIRST + LBID_LIST];
};

class TObjListDlg : public TModelessDialog
{
public:
	TObjListDlg(PTWindowObject, int);
	LRESULT InitDialog();
	virtual void WMDrawItem(RTMessage) = [WM_FIRST + WM_DRAWITEM];
	virtual void IDObjLB(RTMessage) = [ID_FIRST + LBID_LIST];
};

class TMapWnd : public TWindow
{
public:
	TMapWnd(PTWindowObject);
};

class TMobAttrDlg : public TDialog
{
	PTBaseMob pMob;
	TDice NewHP, NewMana, NewDamage;
	TArmorClass NewAC;
	TShop NewShop;
	TFlags NewAction, NewAffect, NewOff, NewImm, NewRes, NewVuln,
     NewForm, NewParts;

public:
	TMobAttrDlg(PTWindowObject, int, PTBaseMob);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDShop(RTMessage) = [ID_FIRST + CHID_SHOP];
	virtual void IDHP(RTMessage) = [ID_FIRST + PBID_HP];
	virtual void IDMana(RTMessage) = [ID_FIRST + PBID_MANA];
	virtual void IDDamage(RTMessage) = [ID_FIRST + PBID_DAMAGE];
	virtual void IDAC(RTMessage) = [ID_FIRST + PBID_AC];
	virtual void IDFlags(RTMessage) = [ID_FIRST + PBID_FLAGS];
};

class TShopDlg : public TDialog
{
	PTShop pShop;

public:
	TShopDlg(PTWindowObject, int, PTShop);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TDiceDlg : public TDialog
{
	PTDice pDice;
    LPSTR lpszCaption;

public:
	TDiceDlg(PTWindowObject, int, PTDice, LPSTR);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TACDlg : public TDialog
{
	PTArmorClass pAC;

public:
	TACDlg(PTWindowObject, int, PTArmorClass);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TFlagDlg : public TDialog
{
	PTFlags pAct, pAff, pOff, pImm, pRes, pVuln, pForm, pParts;
	PTBaseMob pMob;

public:
	TFlagDlg(PTWindowObject, int, PTBaseMob, PTFlags, PTFlags, PTFlags,
	 PTFlags, PTFlags, PTFlags, PTFlags, PTFlags);
	LRESULT InitDialog();
    void Okay(RTMessage);
};

class TObjAttrDlg : public TDialog
{
	PTBaseObj pObj;
	union {	//store the values for the object
		TFlags aNewFlags[5];
		long alNewValues[5];
    };
    TList<TESection> ExDescList;

public:
	TObjAttrDlg(PTWindowObject, int, PTBaseObj);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDAdjustLB(RTMessage) = [ID_FIRST + LBID_ADJUST];
    virtual void IDDelExDesc(RTMessage) = [ID_FIRST + PBID_DELEXDESC];
	virtual void IDAddExDesc(RTMessage) = [ID_FIRST + PBID_ADDEXDESC];
	virtual void IDExDescLB(RTMessage) = [ID_FIRST + LBID_EXDESC];
	virtual void IDValues(RTMessage) = [ID_FIRST + PBID_VALUES];
	virtual void IDTypeCB(RTMessage) = [ID_FIRST + CBID_TYPE];
};

class TAdjustDlg : public TDialog
{
	char *psz;
    int nVal;

public:
	TAdjustDlg(PTWindowObject, int, char*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TExDescDlg : public TDialog
{
	PTESection pES;

public:
	TExDescDlg(PTWindowObject, int, PTESection);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TLightDlg : public TDialog
{
	long *lValues;
	int nType;

public:
	TLightDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TPotionDlg : public TDialog
{
	long *lValues;
	int nType;
	 
public:
	TPotionDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TWandDlg : public TDialog
{
	long *lValues;
	int nType;
	 
public:
	TWandDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TWeaponDlg : public TDialog
{
	long *lValues;
	int nType;
    PTBaseObj pObj;
	 
public:
	TWeaponDlg(PTWindowObject, int, PTBaseObj, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TGenericDlg : public TDialog
{
	long *lValues;
	int nType;
	PTBaseObj pObj;
    char *pszName;
	 
public:
	TGenericDlg(PTWindowObject, int, PTBaseObj, char *, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TFoodDlg : public TDialog
{
	long *lValues;
	int nType;

public:
	TFoodDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TContainerDlg : public TDialog
{
	long *lValues;
	int nType;

public:
	TContainerDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TDrinkDlg : public TDialog
{
	long *lValues;
	int nType;

public:
	TDrinkDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TPortalDlg : public TDialog
{
	long *lValues;
	int nType;

public:
	TPortalDlg(PTWindowObject, int, int, long*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TDoorDlg : public TDialog
{
	PTDoor pDoor;
	const char *pszDir;

public:
	TDoorDlg(PTWindowObject, int, PTDoor, const char*);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TRoomAttrDlg : public TDialog
{
	PTRoom pRoom;
    char *pszDir;
	TList<TESection> ExDescList;
	TList<TDoor> DoorList;
	TList<TMReset> MResetList;
    TList<TOReset> OResetList;

public:
	TRoomAttrDlg(PTWindowObject, int, PTRoom);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDDelExDesc(RTMessage) = [ID_FIRST + PBID_DELEXDESC];
	virtual void IDAddExDesc(RTMessage) = [ID_FIRST + PBID_ADDEXDESC];
	virtual void IDExDescLB(RTMessage) = [ID_FIRST + LBID_EXDESC];
	virtual void IDDoorLB(RTMessage) = [ID_FIRST + LBID_DOORS];
	virtual void IDDelDoor(RTMessage) = [ID_FIRST + PBID_DELDOOR];
    virtual void IDMResetLB(RTMessage) = [ID_FIRST + LBID_MRESETS];
	virtual void IDDelMReset(RTMessage) = [ID_FIRST + PBID_DELMRESET];
	virtual void IDDelOReset(RTMessage) = [ID_FIRST + PBID_DELORESET];
	virtual void IDOResetLB(RTMessage) = [ID_FIRST + LBID_ORESETS];
	virtual void IDAddMReset(RTMessage) = [ID_FIRST + PBID_ADDMRESET];
	virtual void IDAddOReset(RTMessage) = [ID_FIRST + PBID_ADDORESET];
};

class TMobResetDlg : public TDialog
{
	PTMReset pMReset;

public:
	TMobResetDlg(PTWindowObject, int, PTMReset);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDAddGReset(RTMessage) = [ID_FIRST + PBID_ADDGRESET];
	virtual void IDAddEReset(RTMessage) = [ID_FIRST + PBID_ADDERESET];
	virtual void IDDelGReset(RTMessage) = [ID_FIRST + PBID_DELGRESET];
	virtual void IDDelEReset(RTMessage) = [ID_FIRST + PBID_DELERESET];
	virtual void IDGResetLB(RTMessage) = [ID_FIRST + LBID_GRESETS];
	virtual void IDEResetLB(RTMessage) = [ID_FIRST + LBID_ERESETS];
};

class TObjResetDlg : public TDialog
{
	PTOReset pOReset;

public:
	TObjResetDlg(PTWindowObject, int, PTOReset);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDAddPReset(RTMessage) = [ID_FIRST + PBID_ADDPRESET];
	virtual void IDDelPReset(RTMessage) = [ID_FIRST + PBID_DELPRESET];
	virtual void IDPResetLB(RTMessage) = [ID_FIRST + LBID_PRESETS];
};

class TPResetDlg : public TDialog
{
	PTPReset pPReset;

public:
	TPResetDlg(PTWindowObject, int, PTPReset);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TEResetDlg : public TDialog
{
	PTEReset pEReset;

public:
	TEResetDlg(PTWindowObject, int, PTEReset);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDAddPReset(RTMessage) = [ID_FIRST + PBID_ADDPRESET];
	virtual void IDDelPReset(RTMessage) = [ID_FIRST + PBID_DELPRESET];
	virtual void IDPResetLB(RTMessage) = [ID_FIRST + LBID_PRESETS];
};

class TGResetDlg : public TDialog
{
	PTGReset pGReset;

public:
	TGResetDlg(PTWindowObject, int, PTGReset);
	LRESULT InitDialog();
	void Okay(RTMessage);
	virtual void IDAddPReset(RTMessage) = [ID_FIRST + PBID_ADDPRESET];
	virtual void IDDelPReset(RTMessage) = [ID_FIRST + PBID_DELPRESET];
	virtual void IDPResetLB(RTMessage) = [ID_FIRST + LBID_PRESETS];
};

class THelpListDlg : public TModelessDialog
{
public:
	THelpListDlg(PTWindowObject, int);
	LRESULT InitDialog();
    virtual void WMDrawItem(RTMessage) = [WM_FIRST + WM_DRAWITEM];
	virtual void IDHelpLB(RTMessage) = [ID_FIRST + LBID_LIST];
};

class THelpDlg : public TDialog
{
	PTHelp pHelp;

public:
	THelpDlg(PTWindowObject, int, PTHelp);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

class TSocialListDlg : public TModelessDialog
{
public:
	TSocialListDlg(PTWindowObject, int);
	LRESULT InitDialog();
    virtual void WMDrawItem(RTMessage) = [WM_FIRST + WM_DRAWITEM];
	virtual void IDSocialLB(RTMessage) = [ID_FIRST + LBID_LIST];
};

class TSocialDlg : public TDialog
{
	PTSocial pSocial;

public:
	TSocialDlg(PTWindowObject, int, PTSocial);
	LRESULT InitDialog();
	void Okay(RTMessage);
};

//Global fns
int ReadValues();
void ReadAndAddVal(ifstream&, TList<TValPair>&);
void KillEndSpace(char*);
void FillRoomDlg(PTRoom, void*);
void FillMobDlg(PTBaseMob, void*);
void FillObjDlg(PTBaseObj, void*);
void FillHelpDlg(PTHelp, void*);
void FillSocialDlg(PTSocial, void*);
void ClearData();
void FillBox(PTValPair, void*);
void SelectLB(PTListBox, TList<TValPair>&, const char *);
void SelectCB(PTComboBox, TList<TValPair>&, const char *);
void SelectLB(PTListBox, TList<TValPair>&, long);
void SelectCB(PTComboBox, TList<TValPair>&, long);
void SelectLB(PTListBox, TList<TValPair>&, TFlags&);
void GetBoxSel(PTListBox, TList<TValPair>&, char *, int, int=-1);
void GetBoxFlags(PTListBox, TList<TValPair>&, TFlags&);
void DelMobResets(PTRoom, void*);
void DelObjResets(PTRoom, void*);
void UpdateLB(PTListBox, BOOL);
void Killws(char *);
void SetDlgItemLong(HWND, int, long, BOOL);
long GetDlgItemLong(HWND, int, void*, BOOL); 

//global vars
extern PTApplication pApp;
extern PTSandBoxWnd pMainWnd;
extern PTListBox pRoomLB, pMobLB, pObjLB, pHelpLB, pSocialLB;
extern PTMapWnd pMapWnd;
extern HFONT hCourier;
extern ifstream is;
extern ofstream os;
extern TList<TValPair> ActionList;
extern TList<TValPair> AffByList;
extern TList<TValPair> SpecFuncList;
extern TList<TValPair> ObjFlagsList;
extern TList<TValPair> ObjWearList;
extern TList<TValPair> OffensiveList;
extern TList<TValPair> ImmuneList;
extern TList<TValPair> ResistList;
extern TList<TValPair> VulnList;
extern TList<TValPair> FormsList;
extern TList<TValPair> PartsList;
extern TList<TValPair> DamTypeList;
extern TList<TValPair> SexList;
extern TList<TValPair> SizeList;
extern TList<TValPair> MaterialList;
extern TList<TValPair> AdjustmentsList;
extern TList<TValPair> ConditionList;
extern TList<TValPair> DirectionsList;
extern TList<TValPair> RoomFlagsList;
extern TList<TValPair> TerrainList;
extern TList<TValPair> ObjTypeList;
extern TList<TValPair> PosList;
extern TList<TValPair> RaceList;
extern TList<TValPair> DoorStateList;
extern TList<TValPair> DoorTypeList;
extern TList<TValPair> WeaponClassList;
extern TList<TValPair> WeaponTypeList;
extern TList<TValPair> ContFlagsList;
extern TList<TValPair> LiquidList;
extern TList<TValPair> SpellsList;
extern TList<TValPair> PortalTypeList;
extern TList<TValPair> ExitTypeList;
extern TList<TValPair> EWearList;

#endif	//__SANDBOX_H
