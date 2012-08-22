#ifndef _FBROWSEFORM_H_
#define _FBROWSEFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "PublicFunctions.h"

class FavoritesListItem : public Osp::Base::Object {
public:
	FavoritesListItem(Osp::Base::String path, Osp::Base::String displaypath, int id);
	virtual ~FavoritesListItem(void);

public:
	Osp::Base::String path;
	Osp::Base::String displaypath;
	int id;
};

class FBrowseForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::ICustomItemEventListener,
	public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FBrowseForm(bool fromplaylist);
	virtual ~FBrowseForm(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int APLAY = 1;
	static const int AENQUENE = 2;
	static const int ABACK = 3;
	static const int AFAVORITES = 4;
	static const int AHOME = 5;
	static const int AADDTOFAV = 6;
	static const int AREMOVEFROMFAV = 7;
	static const int AFAVORITESCLOSE = 8;
	static const int AFAVORITESCLOSEFROMLIST = 9;
	static const int APLAYFROMFAVORITES = 10;
	static const int AENQUENEFROMFAVORITES = 11;

	static const int ID_LIST_TEXT = 102;
	static const int ID_LIST_ICON = 103;

	Osp::Ui::Controls::CustomListItemFormat* __pCustomListItemFormat;
	Osp::Ui::Controls::CustomListItemFormat* __pCustomListItemFormatD;

	static const int BOXBTN = 4;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	Osp::Base::String curdir;
	Osp::Base::String prevdir;

	Osp::Ui::Controls::CustomList * UList_;
	Osp::Graphics::Bitmap * foldericon;
	Osp::Graphics::Bitmap * folderupicon;
	Osp::Graphics::Bitmap * filedefico;
	Osp::Graphics::Bitmap * filemusicico;
	Osp::Graphics::Bitmap * filevideoico;

	Osp::Ui::Controls::ContextMenu * UListContextmenu_;
	Osp::Ui::Controls::ContextMenu * UListContextmenuFav_;
	int curselitemid;

	bool fromplaylist;

	bool favoritesmode;

	PublicFunctions * publicfunc_;

	Osp::Base::Collection::ArrayList * favoriteslist;

public:
	Osp::Base::String GetFolderDisplayName(Osp::Base::String dir);
	result AddListItem(int itemid, Osp::Base::String itemText, Osp::Graphics::Bitmap* pBitmapNormal);
	void FillListItems();
	void LoadFavoritesList();
	void LoadList(Osp::Base::String dir);
	virtual result OnDraw();
	void OnActionPerformed(const Osp::Ui::Control&, int);
	void OnTouchDoublePressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusIn(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchFocusOut(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchLongPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchMoved(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchPressed(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	void OnTouchReleased(const Osp::Ui::Control&, const Osp::Graphics::Point&, const Osp::Ui::TouchEventInfo&);
	virtual void OnItemStateChanged(const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status);
	virtual void OnItemStateChanged(const Osp::Ui::Control& source, int index, int itemId, int elementId, Osp::Ui::ItemStatus status);
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
};

#endif
