/*
 Copyright (C) 2012  Boštjan Mrak

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "FClientsForm.h"
#include "ButtonCostumEl.h"
#include "FormMgr.h"

using namespace Osp::Base;
using namespace Osp::Ui;
using namespace Osp::Ui::Controls;
using namespace Osp::Net;
using namespace Osp::Net::Http;
using namespace Osp::Xml;
using namespace Osp::Media;
using namespace Osp::Graphics;
using namespace Osp::App;
using namespace Osp::Net::Wifi;

FClientsForm::FClientsForm(bool isfromstart)
{
	this->isfromstart = isfromstart;
}

FClientsForm::~FClientsForm(void)
{
}

bool
FClientsForm::Initialize()
{
	this->Construct(FORM_STYLE_NORMAL|FORM_STYLE_INDICATOR);
	this->SetName(L"FClientsForm");
	this->SetOrientation(ORIENTATION_PORTRAIT);

	return true;
}

result
FClientsForm::OnInitializing(void)
{
	result r = E_SUCCESS;

	FormMgr *pFormMgr = static_cast<FormMgr *>(Application::GetInstance()->GetAppFrame()->GetFrame()->GetControl("FormMgr"));
	drawingclass_ = pFormMgr->drawingclass_;
	publicfunc_ = pFormMgr->publicfunc_;

	UList_ = new List();
	UList_->Construct(Rectangle(0,50,480,572), LIST_STYLE_NORMAL, LIST_ITEM_DOUBLE_IMAGE_TEXT_TEXT, 50, 50, 60, 370);
    UList_->SetTextColorOfEmptyList(Color::COLOR_WHITE);
    UList_->SetTextOfEmptyList(L"No clients. Please add!");
    UList_->EnableTextSlide(LIST_ITEM_TEXT1);
    UList_->AddItemEventListener(*this);

    UListContextmenu_ = new ContextMenu;
	UListContextmenu_->Construct(Point(50,50), CONTEXT_MENU_STYLE_LIST);
	UListContextmenu_->AddItem(L"Edit", AEDIT);
	UListContextmenu_->AddItem(L"Remove", AREMOVE);
	UListContextmenu_->AddActionEventListener(*this);

	UList_->AddTouchEventListener(*this);

	curselitemid = 0;

    AddControl(*UList_);

	buttons_ = new Osp::Base::Collection::ArrayList;

	ButtonCostumEl * addbtn = new ButtonCostumEl(Rectangle(140,667,128,124), AADD, 14, BOXBTN, L"Add");
	buttons_->Add(*addbtn);
	ButtonCostumEl * backbtn = new ButtonCostumEl(Rectangle(12,667,128,124), ABACK, 10, BOXBTN, L"Back");
	buttons_->Add(*backbtn);
	ButtonCostumEl * helpbtn = new ButtonCostumEl(Rectangle(268,667,128,124), AHELP, 4, BOXBTN, L"Help");
	buttons_->Add(*helpbtn);

	currpressedbtn = -1;
	this->AddTouchEventListener(*this);

	pRequest = null;
	pSession = null;
	pTransaction = null;

	CheckConnectionTimer_ = new Osp::Base::Runtime::Timer;
	CheckConnectionTimer_->Construct(*this);
	CheckConnectionTimerStarted = false;

	CheckNextClientTimer_ = new Osp::Base::Runtime::Timer;
	CheckNextClientTimer_->Construct(*this);
	CheckNextClientTimerStarted = false;

	CheckConnectionProgressTimer_ = new Osp::Base::Runtime::Timer;
	CheckConnectionProgressTimer_->Construct(*this);

	CheckForMyIpTimer_ = new Osp::Base::Runtime::Timer;
	CheckForMyIpTimer_->Construct(*this);
	CheckForMyIpTimerStarted = false;

	sendingcommand = false;

	clientlist = new Osp::Base::Collection::ArrayList;
    clientlist->Construct();

    Image* image_ = new Image();
    image_->Construct();

    connectedicon = image_->DecodeN(L"/Res/connectedico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    disconnectedicon = image_->DecodeN(L"/Res/disconnectedico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    protectedicon = image_->DecodeN(L"/Res/protectedico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    newconnectedicon = image_->DecodeN(L"/Res/newconnectedico.png", BITMAP_PIXEL_FORMAT_ARGB8888);
    newprotectedicon = image_->DecodeN(L"/Res/newprotectedicon.png", BITMAP_PIXEL_FORMAT_ARGB8888);

    delete image_;

    Osp::Io::Database * db_ = new Osp::Io::Database();
    if (!Osp::Io::Database::Exists(L"/Home/data.db")) {
		db_->Construct((L"/Home/data.db"), true);
		db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS CLIENTS (ID INTEGER PRIMARY KEY AUTOINCREMENT, IP TEXT, NAME TEXT, LASTDIR TEXT)"),true);
		db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS FAVORITES (ID INTEGER PRIMARY KEY AUTOINCREMENT, CLIENTID INTEGER, DIR TEXT)"),true);
	} else {
		db_->Construct((L"/Home/data.db"), true);
		db_->ExecuteSql((L"CREATE TABLE IF NOT EXISTS FAVORITES (ID INTEGER PRIMARY KEY AUTOINCREMENT, CLIENTID INTEGER, DIR TEXT)"),true);
	}
    Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT ID, IP, NAME FROM CLIENTS ORDER BY ID");
    if(!pEnum_) {
		//no records
	} else {
		pEnum_->Reset();
		while (pEnum_->MoveNext() ==  E_SUCCESS) {
			ClientListItem * clientlistitem_ = new ClientListItem(0, L"", L"", false);
			pEnum_->GetIntAt(0, clientlistitem_->id);
			if (pEnum_->GetColumnSize(1) > 0) {
				pEnum_->GetStringAt(1, clientlistitem_->ip);
			}
			if (pEnum_->GetColumnSize(2) > 0) {
				pEnum_->GetStringAt(2, clientlistitem_->name);
			}
			clientlist->Add(*clientlistitem_);
		}
	}
    delete pEnum_;
    delete db_;

    currcheckclientindex = 0;
    currcheckclienttime = 0;
    connectinginprogress = false;
    FillListItems();

    addeditpopup_ = new Popup();
	addeditpopup_->Construct(L"IDP_POPUP1");
	Osp::Ui::Controls::Button * addeditpopupSaveButton_ = static_cast<Button *>(addeditpopup_->GetControl(L"IDC_BUTTON1"));
	addeditpopupSaveButton_->SetActionId(ASAVE);
	addeditpopupSaveButton_->AddActionEventListener(*this);
	Osp::Ui::Controls::Button * addeditpopupCancelButton_ = static_cast<Button *>(addeditpopup_->GetControl(L"IDC_BUTTON2"));
	addeditpopupCancelButton_->SetActionId(ACANCEL);
	addeditpopupCancelButton_->AddActionEventListener(*this);

	connectingpopup_ = new Popup();
	connectingpopup_->Construct(L"IDP_POPUP2");
	Osp::Ui::Controls::Button * connectingpopupCancelButton_ = static_cast<Button *>(connectingpopup_->GetControl(L"IDC_BUTTON1"));
	connectingpopupCancelButton_->SetActionId(ACANCELCHECK);
	connectingpopupCancelButton_->AddActionEventListener(*this);

	ischeckinginprogress = false;
	startupchecklast = false;

	pwifimanager.Construct(*this);
	if (pwifimanager.IsConnected()) {
		FillPotentialClients();
		if (!isfromstart) {
			CheckingClients();
		}
	}

	return r;
}

void FClientsForm::FillPotentialClients() {
	if (publicfunc_->GetMyIp() != L"") {
		if (CheckForMyIpTimerStarted == true) CheckForMyIpTimer_->Cancel();
		CheckForMyIpTimerStarted = false;
		//get last number of my ip, get lower ips -10 and higer ips +10, only ones without already in clients list
		int ippart1 = -1;
		int ippart2 = -1;
		int ippart3 = -1;
		int ippart4 = -1;
		int ippartint = 0;
		String ippartstr;
		Osp::Base::Utility::StringTokenizer ipparse(publicfunc_->GetMyIp(), L".");
		int ippartnum = 1;
		while (ipparse.HasMoreTokens()) {
			ipparse.GetNextToken(ippartstr);
			ippartstr.Trim();
			if (Osp::Base::Integer::Parse(ippartstr,ippartint) == E_SUCCESS) {
				if (ippartnum == 1) {
					ippart1 = ippartint;
				} else if (ippartnum == 2) {
					ippart2 = ippartint;
				} else if (ippartnum == 3) {
					ippart3 = ippartint;
				} else if (ippartnum == 4) {
					ippart4 = ippartint;
				} else {
					ippart4 = -1;
				}
			} else {
				break;
			}
			ippartnum++;
		}
		Osp::Base::String checkips = L"";
		Osp::Base::String checkip1 = L"";
		Osp::Base::String checkip2 = L"";
		checkips.Append(ippart1);
		checkips.Append(L".");
		checkips.Append(ippart2);
		checkips.Append(L".");
		checkips.Append(ippart3);
		checkips.Append(L".");
		int virtualid = -1;
		ClientListItem * clientlistitem_ = null;
		for (int checki=1;checki <= 10;checki++) {
			if (((ippart4-checki) > 0) && ((ippart4+checki) < 250)) {
				checkip1 = checkips + Osp::Base::Integer::ToString(ippart4-checki);
				checkip2 = checkips + Osp::Base::Integer::ToString(ippart4+checki);
				bool checkip1exists = false;
				bool checkip2exists = false;
				Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
				while (pEnum->MoveNext() == E_SUCCESS) {
					clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
					if (clientlistitem_->ip == checkip1) {
						checkip1exists = true;
					} else if (clientlistitem_->ip == checkip2) {
						checkip2exists = true;
					}
				}
				delete pEnum;

				if (checkip1exists == false) {
					ClientListItem * clientlistitem1_ = new ClientListItem(virtualid, checkip1, L"new *", false);
					clientlistitem1_->isvirtual = true;
					clientlistitem1_->isprotected = false;
					clientlistitem1_->ischecked = false;
					clientlist->Add(*clientlistitem1_);
					AppLog("check %S", checkip1.GetPointer());
				}
				virtualid--;
				if (checkip2exists == false) {
					ClientListItem * clientlistitem2_ = new ClientListItem(virtualid, checkip2, L"new *", false);
					clientlistitem2_->isvirtual = true;
					clientlistitem2_->isprotected = false;
					clientlistitem2_->ischecked = false;
					clientlist->Add(*clientlistitem2_);
					AppLog("check %S", checkip2.GetPointer());
				}
				virtualid--;
			}

		}
		UList_->SetTextOfEmptyList(L"Scanning network...");
		this->RequestRedraw();
	} else {
		if (CheckForMyIpTimerStarted == true) CheckForMyIpTimer_->Cancel();
		CheckForMyIpTimerStarted = false;
		if (CheckForMyIpTimerStarted == false) {
			CheckForMyIpTimer_->Start(200);
			CheckForMyIpTimerStarted = true;
		}
	}
}

void FClientsForm::StartUpCheck() {
	if (publicfunc_->selectedclientid > 0) {
		startupchecklast = true;
		if (pwifimanager.IsConnected()) {
			Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
			ClientListItem * clientlistitem_ = null;
			while (pEnum->MoveNext() == E_SUCCESS) {
				clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
				if (clientlistitem_->id == publicfunc_->selectedclientid) {
					curselitemid = clientlistitem_->id;
					this->OnActionPerformed(*this, ACONNECT);
					break;
				}
			}
			delete pEnum;
		} else {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("No WiFi", "Please connect to WiFi network!", MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
		}
	}
}

void FClientsForm::CheckingClients() {
	ischeckinginprogress = true;
	if (CheckNextClientTimerStarted == true) {
		CheckNextClientTimer_->Cancel();
		CheckNextClientTimerStarted = false;
	}
	if (clientlist->GetCount() > 0) {
		if (pwifimanager.IsConnected()) {
			if (currcheckclientindex >= clientlist->GetCount()) {
				currcheckclientindex = 0;
			}
			Osp::System::SystemTime::GetTicks(currcheckclienttime);
			ClientListItem * clientlistitem_ = static_cast<ClientListItem *> (clientlist->GetAt(currcheckclientindex));
			PingClient(clientlistitem_->ip);
		}
	}
}

void FClientsForm::CheckingClientsStop() {
	if (ischeckinginprogress == true) {
	if (CheckNextClientTimerStarted == true) {
		CheckNextClientTimer_->Cancel();
		CheckNextClientTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (CheckConnectionProgressTimer_ != null) {
		CheckConnectionProgressTimer_->Cancel();
	}
	sendingcommand = false;
	if (pSession != null) {
		if (pTransaction != null) {
			pSession->CancelTransaction(*pTransaction);
			pSession->CloseAllTransactions();
		}
		delete pTransaction;
		pTransaction = null;
		delete pSession;
		pSession = null;
	}
	ischeckinginprogress = false;
	}
}

void FClientsForm::PingClientResponse(bool status, bool isforbidden) {
	if (CheckNextClientTimerStarted == true) {
		CheckNextClientTimer_->Cancel();
		CheckNextClientTimerStarted = false;
	}
	if (connectinginprogress) {
		if (status == true) {
			this->OnActionPerformed(*this,ACONNECTRESPONSEON);
		} else {
			if (isforbidden == true) {
				this->OnActionPerformed(*this,ACONNECTRESPONSEOFFFORBIDDEN);
			} else {
				this->OnActionPerformed(*this,ACONNECTRESPONSEOFF);
			}
		}
	} else {
		if (clientlist->GetCount() > 0) {
			if (currcheckclientindex >= clientlist->GetCount()) {
				currcheckclientindex = 0;
			}
			ClientListItem * clientlistitem_ = static_cast<ClientListItem *> (clientlist->GetAt(currcheckclientindex));
			clientlistitem_->isconnected = status;
			clientlistitem_->isprotected = isforbidden;
			clientlistitem_->ischecked = true;
			if ((clientlistitem_->isvirtual) && ((clientlistitem_->isconnected) || (clientlistitem_->isprotected))) {
				if (UList_->GetItemIndexFromItemId(clientlistitem_->id) < 0) {
					if (clientlistitem_->isconnected) {
						UList_->AddItem(&clientlistitem_->name, &clientlistitem_->ip, newconnectedicon, null, clientlistitem_->id);
					} else if (clientlistitem_->isprotected) {
						UList_->AddItem(&clientlistitem_->name, &clientlistitem_->ip, newprotectedicon, null, clientlistitem_->id);
					}
				}
			}
			int listitemindex = UList_->GetItemIndexFromItemId(clientlistitem_->id);
			if (listitemindex >= 0) {
				if (clientlistitem_->isvirtual) {
					if (clientlistitem_->isconnected) {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, newconnectedicon, null, clientlistitem_->id);
					} else if (clientlistitem_->isprotected) {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, newprotectedicon, null, clientlistitem_->id);
					} else {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
					}
				} else {
					if (clientlistitem_->isconnected) {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, connectedicon, null, clientlistitem_->id);
					} else if (clientlistitem_->isprotected) {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, protectedicon, null, clientlistitem_->id);
					} else {
						UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
					}
				}
				UList_->RefreshItem(listitemindex);
			}
			this->RequestRedraw();
			if (currcheckclientindex == clientlist->GetCount()-1) {
				currcheckclientindex = 0;
			} else {
				currcheckclientindex++;
			}
			long long nowchecktime;
			Osp::System::SystemTime::GetTicks(nowchecktime);
			if ((nowchecktime-currcheckclienttime) > CHECKNEXTCLIENTTIMEOUT) {
				CheckingClients();
			} else {
				CheckNextClientTimerStarted = true;
				if ((nowchecktime-currcheckclienttime) > 0) {
					CheckNextClientTimer_->Start(CHECKNEXTCLIENTTIMEOUT-(nowchecktime-currcheckclienttime));
				} else {
					CheckNextClientTimer_->Start(CHECKNEXTCLIENTTIMEOUT);
				}
			}
		}
	}
}

void FClientsForm::ResetChekingClients(int fromindex) {
	CheckingClientsStop();
	currcheckclientindex = fromindex;
	CheckingClients();
}

void FClientsForm::SaveClient(int id, Osp::Base::String ip, Osp::Base::String name) {
	Osp::Io::Database * db_ = new Osp::Io::Database();
	db_->Construct((L"/Home/data.db"), true);
	Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT * FROM CLIENTS WHERE ID=" + Osp::Base::Integer::ToString(id));
	if(!pEnum_) {
		if (id < 0) {
			int index = UList_->GetItemIndexFromItemId(id);
			UList_->RemoveItemAt(index);
			this->RequestRedraw();
			Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
			ClientListItem * clientlistitem_ = null;
			index = 0;
			while (pEnum->MoveNext() == E_SUCCESS) {
				clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
				if (clientlistitem_->id == id) {
					break;
				}
				index++;
			}
			delete pEnum;
			clientlist->RemoveAt(index,true);
		}
		db_->BeginTransaction();
		Osp::Io::DbStatement * pStmt_ = db_->CreateStatementN(L"INSERT INTO CLIENTS (ID, IP, NAME) VALUES (NULL, ?, ?)");
		pStmt_->BindString(0, ip);
		pStmt_->BindString(1, name);
		Osp::Io::DbEnumerator * pEnum2_ = db_->ExecuteStatementN(*pStmt_);
		db_->CommitTransaction();
		delete pEnum2_;
		Osp::Io::DbEnumerator * pEnumgetid_ = db_->QueryN(L"SELECT last_insert_rowid()");
		if(!(!pEnumgetid_)) {
			pEnumgetid_->Reset();
			pEnumgetid_->MoveNext();
			pEnumgetid_->GetIntAt(0, id);
		}
		delete pEnumgetid_;
		delete pStmt_;

		ClientListItem * clientlistitem_ = new ClientListItem(id, ip, name, false);
		clientlist->Add(*clientlistitem_);
		UList_->AddItem(&clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
		if (UList_->GetItemCount() > 2) {
			UList_->ScrollToTop(UList_->GetItemCount()-2);
		} else {
			UList_->ScrollToTop();
		}
		this->RequestRedraw();
		ResetChekingClients(clientlist->GetCount()-1);
	} else {
		db_->BeginTransaction();
		Osp::Io::DbStatement * pStmt_ = db_->CreateStatementN(L"UPDATE CLIENTS SET IP = ?, NAME = ? WHERE ID=?");
		pStmt_->BindString(0, ip);
		pStmt_->BindString(1, name);
		pStmt_->BindInt(2, id);
		Osp::Io::DbEnumerator * pEnum2_ = db_->ExecuteStatementN(*pStmt_);
		db_->CommitTransaction();
		delete pEnum2_;
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		int curclientindex = 0;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == id) {
				clientlistitem_->ip = ip;
				clientlistitem_->name = name;
				clientlistitem_->isconnected = false;
				break;
			}
			curclientindex++;
		}
		delete pEnum;
		int listitemindex = UList_->GetItemIndexFromItemId(id);
		if (listitemindex >= 0) {
			UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
			UList_->RefreshItem(listitemindex);
		}
		this->RequestRedraw();
		ResetChekingClients(curclientindex);
	}
	delete pEnum_;
	delete db_;
}

void FClientsForm::DeleteClient(int id) {
	Osp::Io::Database * db_ = new Osp::Io::Database();
	db_->Construct((L"/Home/data.db"), true);
	Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT * FROM CLIENTS WHERE ID=" + Osp::Base::Integer::ToString(id));
	if (!(!pEnum_)) {
		db_->ExecuteSql(L"DELETE FROM CLIENTS WHERE ID=" + Osp::Base::Integer::ToString(id), true);
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		int clientlistitemindexfound = -1;
		int clientlistitemindex = 0;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == id) {
				clientlistitemindexfound = clientlistitemindex;
				break;
			}
			clientlistitemindex++;
		}
		delete pEnum;
		if (clientlistitemindexfound > -1) {
			clientlist->RemoveAt(clientlistitemindexfound,true);
		}
		int listitemindex = UList_->GetItemIndexFromItemId(id);
		if (listitemindex >= 0) {
			UList_->RemoveItemAt(listitemindex);
		}
		this->RequestRedraw();
	}
	delete pEnum_;
	delete db_;
}

void FClientsForm::FillListItems() {
	Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
	ClientListItem * clientlistitem_ = null;
	while (pEnum->MoveNext() == E_SUCCESS) {
		clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
		if (clientlistitem_->isconnected) {
			UList_->AddItem(&clientlistitem_->name, &clientlistitem_->ip, connectedicon, null, clientlistitem_->id);
		} else {
			UList_->AddItem(&clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
		}
	}
	delete pEnum;
	UList_->ScrollToTop();
	this->RequestRedraw();
}

void FClientsForm::PingClient(Osp::Base::String ip) {
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (sendingcommand == false) {
		sendingcommand = true;
		if (pSession != null) {
			if (pTransaction != null) {
				pSession->CancelTransaction(*pTransaction);
				pSession->CloseAllTransactions();
			}
			delete pTransaction;
			pTransaction = null;
			delete pSession;
			pSession = null;
		}
		if (pSession == null) {
			pSession = new HttpSession();
			pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, ip, null);
		}
		if (pTransaction != null) {
			pSession->CancelTransaction(*pTransaction);
			pSession->CloseAllTransactions();
			delete pTransaction;
			pTransaction = null;
		}
		pTransaction = pSession->OpenTransactionN();
		if (pTransaction) {
			pTransaction->AddHttpTransactionListener(*this);
			pRequest = pTransaction->GetRequest();
			if (pRequest) {
				pRequest->SetUri(ip + L":8080/requests/status.xml");
				pRequest->SetMethod(NET_HTTP_METHOD_GET);
				pTransaction->Submit();
			}
		}
	}
	CheckConnectionTimerStarted = true;
	if (startupchecklast) {
		CheckConnectionTimer_->Start(CLIENTSTATUSCHECKTIMEOUT*2);
		if (connectinginprogress == true) {
			Osp::Ui::Controls::Progress * connectingpopupprogressbar_ = static_cast<Progress *>(connectingpopup_->GetControl(L"IDC_PROGRESS1"));
			connectingpopupprogressbar_->SetRange(0,(CLIENTSTATUSCHECKTIMEOUT*2/100)-10);
			CheckConnectionProgressTimer_->Start(100);
		}
	} else {
		CheckConnectionTimer_->Start(CLIENTSTATUSCHECKTIMEOUT);
		if (connectinginprogress == true) {
			Osp::Ui::Controls::Progress * connectingpopupprogressbar_ = static_cast<Progress *>(connectingpopup_->GetControl(L"IDC_PROGRESS1"));
			connectingpopupprogressbar_->SetRange(0,(CLIENTSTATUSCHECKTIMEOUT/100)-10);
			CheckConnectionProgressTimer_->Start(100);
		}
	}
}

void FClientsForm::OnWifiActivated(result r) {

}
void FClientsForm::OnWifiConnected(const Osp::Base::String &ssid, result r) {
	FillPotentialClients();
	if (startupchecklast) {
		StartUpCheck();
	} else {
		CheckingClients();
	}
}
void FClientsForm::OnWifiDeactivated (result r) {

}
void FClientsForm::OnWifiDisconnected (void) {
	CheckingClientsStop();
	Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
	ClientListItem * clientlistitem_ = null;
	int listitemindex = -1;
	while (pEnum->MoveNext() == E_SUCCESS) {
		clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
		clientlistitem_->isconnected = false;
		listitemindex = UList_->GetItemIndexFromItemId(clientlistitem_->id);
		if (listitemindex >= 0) {
			UList_->SetItemAt(listitemindex, &clientlistitem_->name, &clientlistitem_->ip, disconnectedicon, null, clientlistitem_->id);
			UList_->RefreshItem(listitemindex);
		}
	}
	delete pEnum;
	this->RequestRedraw();
}
void FClientsForm::OnWifiRssiChanged (long rssi) {

}
void FClientsForm::OnWifiScanCompletedN(const Osp::Base::Collection::IList *pWifiBssInfoList, result r) {

}

void FClientsForm::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (CheckNextClientTimer_->Equals(timer)) {
		CheckNextClientTimerStarted = false;
		CheckingClients();
	} else if (CheckConnectionTimer_->Equals(timer)) {
		CheckConnectionTimerStarted = false;
		sendingcommand = false;
		PingClientResponse(false, false);
	} else if (CheckConnectionProgressTimer_->Equals(timer)) {
		if (connectinginprogress == true) {
			Osp::Ui::Controls::Progress * connectingpopupprogressbar_ = static_cast<Progress *>(connectingpopup_->GetControl(L"IDC_PROGRESS1"));
			int curprogress = connectingpopupprogressbar_->GetValue();
			int minval, maxval;
			connectingpopupprogressbar_->GetRange(minval,maxval);
			if (curprogress < maxval) {
				curprogress++;
				connectingpopupprogressbar_->SetValue(curprogress);
				connectingpopupprogressbar_->Draw();
				connectingpopupprogressbar_->Show();
				CheckConnectionProgressTimer_->Start(100);
			}
		}
	} else if (CheckForMyIpTimer_->Equals(timer)) {
		CheckForMyIpTimerStarted = false;
		FillPotentialClients();
	}
}

result
FClientsForm::OnDraw() {
	Osp::Graphics::Canvas* pCanvas_ = GetCanvasN();
	if(pCanvas_ != null) {
		//draw bg
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(0,0,480,800), Rectangle(0,0,480,800));
		pCanvas_->FillRectangle(Color(0,0,0,50),Rectangle(0,660,480,140));
		//draw title
		pCanvas_->FillRectangle(Color(0,0,0,100),Rectangle(0,38,480,50));
		pCanvas_->FillRectangle(Color(255,255,255,100),Rectangle(0,38+50-2,480,2));
		drawingclass_->FDrawTextToCanvas(pCanvas_,Rectangle(0,38,480,50),L"Clients",28,FONT_STYLE_BOLD,false,Color::COLOR_WHITE,TEXT_ALIGNMENT_CENTER,TEXT_ALIGNMENT_MIDDLE);
		//draw icon
		drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(419,668,61,124), Rectangle(0,800,61,124));
		//draw buttons
		ButtonCostumEl* button_;
		bool isvisible;
		Rectangle btnicorect;
		Point btnicoshift;
		for (int n=0; n<buttons_->GetCount(); n++) {
			isvisible = true;
			button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
			if (button_->type == BOXBTN) {
				if (isvisible) {
					if ((button_->pressed) || (button_->toogle)) {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0+button_->position.height,button_->position.width,button_->position.height));
					} else {
						drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x,button_->position.y,button_->position.width,button_->position.height), Rectangle(818,0,button_->position.width,button_->position.height));
					}
					btnicorect.SetBounds(818,256,90,44);
					btnicoshift.SetPosition(0,0);
					if (button_->toogle) {
						btnicorect.y = btnicorect.y + (button_->icoindextoogle * btnicorect.height);
					} else {
						btnicorect.y = btnicorect.y + (button_->icoindex * btnicorect.height);
					}
					drawingclass_->FDrawSpriteToCanvas(pCanvas_, Rectangle(button_->position.x+19+btnicoshift.x,button_->position.y+38+btnicoshift.y,btnicorect.width,btnicorect.height), btnicorect);
				}
			}
		}
	}
	delete pCanvas_;
	return E_SUCCESS;
}

void FClientsForm::OnTransactionReadyToRead(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int availableBodyLen)
{

}

void FClientsForm::OnTransactionAborted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, result r)
{
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	sendingcommand = false;
	PingClientResponse(false, false);
}

void FClientsForm::OnTransactionReadyToWrite(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int recommendedChunkSize)
{

}

void FClientsForm::OnTransactionHeaderCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int headerLen, bool bAuthRequired)
{

}

void FClientsForm::OnTransactionCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction)
{
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	sendingcommand = false;
	if (httpTransaction.GetResponse()->GetStatusCode() == NET_HTTP_STATUS_OK) {
		ByteBuffer * pBuffer = httpTransaction.GetResponse()->ReadBodyN();
		char *charBuff = (char*) pBuffer->GetPointer();

		xmlDocPtr pDocument = Osp::Xml::xmlParseMemory(charBuff, pBuffer->GetCapacity());
		if (pDocument == null) {
			PingClientResponse(false, false);
		} else {

			xmlNodePtr pRoot = xmlDocGetRootElement(pDocument);
			xmlNodePtr pCurrentElement = null;

			Osp::Base::String nodeName, nodeValue;
			Osp::Base::String apiversion = L"";
			Osp::Base::String statetest = L"";
			if (pRoot) {
				if (pRoot->children) {
					for(pCurrentElement = pRoot->children; pCurrentElement; pCurrentElement = pCurrentElement->next) {
						if (pCurrentElement->type == XML_ELEMENT_NODE) {
							Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->name, nodeName);
							if((pCurrentElement->children) != null) {
								if (strlen((char*)pCurrentElement->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->children->content, nodeValue);
								}
							}
							if (nodeName == L"apiversion") {
								apiversion = nodeValue;
							} else if (nodeName == L"state") {
								statetest = nodeValue;
							}
						}
					}
				}
			}
			AppLog("apiversion %S", apiversion.GetPointer());
			if (statetest != L"") {
				PingClientResponse(true, false);
			} else {
				PingClientResponse(false, false);
			}
		}
		xmlFreeDoc(pDocument);
		delete pBuffer;
	} else if (httpTransaction.GetResponse()->GetStatusCode() == NET_HTTP_STATUS_FORBIDDEN) {
		PingClientResponse(false, true);
	} else {
		PingClientResponse(false, false);
	}
}

void FClientsForm::OnTransactionCertVerificationRequiredN(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, String* pCert)
{
	PingClientResponse(false, false);
}

result
FClientsForm::OnTerminating(void)
{
	result r = E_SUCCESS;

	if (CheckConnectionTimer_ != null) {
		if (CheckConnectionTimerStarted == true) {
			CheckConnectionTimer_->Cancel();
			CheckConnectionTimerStarted = false;
		}
		delete CheckConnectionTimer_;
		CheckConnectionTimer_ = null;
	}
	if (CheckNextClientTimer_ != null) {
		if (CheckNextClientTimerStarted == true) {
			CheckNextClientTimer_->Cancel();
			CheckNextClientTimerStarted = false;
		}
		delete CheckNextClientTimer_;
		CheckNextClientTimer_ = null;
	}
	if (CheckConnectionProgressTimer_ != null) {
		CheckConnectionProgressTimer_->Cancel();
		delete CheckConnectionProgressTimer_;
		CheckConnectionProgressTimer_ = null;
	}
	if (CheckForMyIpTimer_ != null) {
		if (CheckForMyIpTimerStarted) CheckForMyIpTimer_->Cancel();
		delete CheckForMyIpTimer_;
		CheckForMyIpTimer_ = null;
	}
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}

	delete pTransaction;
	delete pSession;

	clientlist->RemoveAll(true);
	delete clientlist;

	buttons_->RemoveAll(true);
	delete buttons_;

	delete connectedicon;
	delete disconnectedicon;
	delete protectedicon;
	delete newconnectedicon;
	delete newprotectedicon;

	delete UListContextmenu_;

	delete addeditpopup_;
	delete connectingpopup_;

	return r;
}

void
FClientsForm::OnActionPerformed(const Control& source, int actionId)
{
	if (actionId == AADD) {
		startupchecklast = false;
		CheckingClientsStop();
		curselitemid = 0;
		addeditpopup_->SetTitleText(L"Add client");
		Osp::Ui::Controls::EditField * ipfieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD1"));
		ipfieldel_->SetText(L"");
		ipfieldel_->RequestRedraw();
		Osp::Ui::Controls::EditField * namefieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD2"));
		namefieldel_->SetText(L"");
		namefieldel_->RequestRedraw();
		addeditpopup_->SetShowState(true);
		addeditpopup_->Show();
	} else if ((actionId == AEDIT) || (actionId == ACONNECTNEW)) {
		startupchecklast = false;
		CheckingClientsStop();
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == curselitemid) {
				break;
			}
		}
		delete pEnum;
		if (actionId == ACONNECTNEW) {
			addeditpopup_->SetTitleText(L"Add new client");
		} else {
			addeditpopup_->SetTitleText(L"Edit client");
		}
		Osp::Ui::Controls::EditField * ipfieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD1"));
		ipfieldel_->SetText(clientlistitem_->ip);
		ipfieldel_->RequestRedraw();
		Osp::Ui::Controls::EditField * namefieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD2"));
		namefieldel_->SetText(clientlistitem_->name);
		namefieldel_->RequestRedraw();
		addeditpopup_->SetShowState(true);
		addeditpopup_->Show();
	} else if (actionId == AREMOVE) {
		startupchecklast = false;
		DeleteClient(curselitemid);
	} else if (actionId == ACONNECT) {
		Osp::Ui::Controls::Progress * connectingpopupprogressbar_ = static_cast<Progress *>(connectingpopup_->GetControl(L"IDC_PROGRESS1"));
		connectingpopupprogressbar_->SetValue(0);
		connectingpopup_->SetShowState(true);
		connectingpopup_->Show();
		CheckingClientsStop();
		connectinginprogress = true;
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == curselitemid) {
				break;
			}
		}
		delete pEnum;
		PingClient(clientlistitem_->ip);
		startupchecklast = false;
	} else if ((actionId == ACONNECTRESPONSEOFF) || (actionId == ACONNECTRESPONSEOFFFORBIDDEN)) {
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		int curclientlistitemsel = 0;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == curselitemid) {
				break;
			}
			curclientlistitemsel++;
		}
		delete pEnum;
		connectinginprogress = false;
		connectingpopup_->SetShowState(false);
		this->RequestRedraw();
		MessageBox msgbox;
		int modalResult = 0;
		if (actionId == ACONNECTRESPONSEOFFFORBIDDEN) {
			msgbox.Construct("Access forbidden!", "Please read help how to enable access!", MSGBOX_STYLE_OK, 10000);
		} else {
			msgbox.Construct("Not connected!", "Client is not connected!", MSGBOX_STYLE_OK, 10000);
		}
		msgbox.ShowAndWait(modalResult);
		ResetChekingClients(curclientlistitemsel);
	} else if (actionId == ACANCELCHECK) {
		CheckingClientsStop();
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		int curclientlistitemsel = 0;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == curselitemid) {
				break;
			}
			curclientlistitemsel++;
		}
		delete pEnum;
		connectinginprogress = false;
		connectingpopup_->SetShowState(false);
		this->RequestRedraw();
		ResetChekingClients(curclientlistitemsel);
	} else if (actionId == ACONNECTRESPONSEON) {
		connectinginprogress = false;
		Osp::Base::Collection::IEnumerator * pEnum = clientlist->GetEnumeratorN();
		ClientListItem * clientlistitem_ = null;
		while (pEnum->MoveNext() == E_SUCCESS) {
			clientlistitem_ = static_cast<ClientListItem *> (pEnum->GetCurrent());
			if (clientlistitem_->id == curselitemid) {
				break;
			}
		}
		delete pEnum;
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		publicfunc_->SelectClient(clientlistitem_->id);
		pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
	} else if (actionId == ASAVE) {
		Osp::Base::String saveip;
		Osp::Base::String savename;
		Osp::Ui::Controls::EditField * ipfieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD1"));
		saveip = ipfieldel_->GetText();
		saveip.Trim();
		Osp::Ui::Controls::EditField * namefieldel_ = static_cast<EditField *>(addeditpopup_->GetControl(L"IDC_EDITFIELD2"));
		savename = namefieldel_->GetText();
		savename.Trim();
		Osp::Base::String validationerrmsg = L"";
		if (saveip == L"") {
			validationerrmsg = L"Enter IP!";
		} else if (savename == L"") {
			validationerrmsg = L"Enter name!";
		} else {
			int ippart1 = -1;
			int ippart2 = -1;
			int ippart3 = -1;
			int ippart4 = -1;
			int ippartint = 0;
			String ippartstr;
			Osp::Base::Utility::StringTokenizer ipparse(saveip, L".");
			int ippartnum = 1;
			while (ipparse.HasMoreTokens()) {
				ipparse.GetNextToken(ippartstr);
				ippartstr.Trim();
				if (Osp::Base::Integer::Parse(ippartstr,ippartint) == E_SUCCESS) {
					if (ippartnum == 1) {
						ippart1 = ippartint;
					} else if (ippartnum == 2) {
						ippart2 = ippartint;
					} else if (ippartnum == 3) {
						ippart3 = ippartint;
					} else if (ippartnum == 4) {
						ippart4 = ippartint;
					} else {
						ippart4 = -1;
					}
				} else {
					break;
				}
				ippartnum++;
			}
			if ((ippart1 == 192) && (ippart2 == 168) && ((ippart3 >= 0) && (ippart3 <= 255)) && ((ippart4 >= 0) && (ippart4 <= 255))) {
				//valid ip
			} else if ((ippart1 == 172) && ((ippart2 >= 16) && (ippart2 <= 31)) && ((ippart3 >= 0) && (ippart3 <= 255)) && ((ippart4 >= 0) && (ippart4 <= 255))) {
				//valid ip
			} else if ((ippart1 == 10) && ((ippart2 >= 0) && (ippart2 <= 255)) && ((ippart3 >= 0) && (ippart3 <= 255)) && ((ippart4 >= 0) && (ippart4 <= 255))) {
				//valid ip
			} else {
				validationerrmsg = L"Invalid IP!";
			}
		}
		if (validationerrmsg != L"") {
			MessageBox msgbox;
			int modalResult = 0;
			msgbox.Construct("Validate", validationerrmsg, MSGBOX_STYLE_OK, 10000);
			msgbox.ShowAndWait(modalResult);
			return;
		} else {
			SaveClient(curselitemid, saveip, savename);
			addeditpopup_->SetShowState(false);
			this->RequestRedraw();
			CheckingClients();
		}
	} else if (actionId == ACANCEL) {
		addeditpopup_->SetShowState(false);
		this->RequestRedraw();
		CheckingClients();
	} else if (actionId == ABACK) {
		if (isfromstart) {
			Application::GetInstance()->Terminate();
		} else {
			Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
			pFormMgr->SendUserEvent(FormMgr::REQUEST_MAINFORM, null);
		}
	} else if (actionId == AHELP) {
		startupchecklast = false;
		Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
		FormMgr *pFormMgr = static_cast<FormMgr *>(pFrame->GetControl("FormMgr"));
		if (isfromstart) {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_HELPFORMCLIENTSSTART, null);
		} else {
			pFormMgr->SendUserEvent(FormMgr::REQUEST_HELPFORMCLIENTS, null);
		}
	}
}

void
FClientsForm::OnTouchDoublePressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FClientsForm::OnTouchFocusIn(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FClientsForm::OnTouchFocusOut(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{

}

void
FClientsForm::OnTouchLongPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	if(&source==UList_) {
			if (UList_->GetItemIndexFromPosition(currentPosition) >= 0) {
				curselitemid = UList_->GetItemIdAt(UList_->GetItemIndexFromPosition(currentPosition));
				if (curselitemid > 0) {
					UListContextmenu_->SetPosition(currentPosition.x, currentPosition.y+GetClientAreaBounds().y+UList_->GetBounds().y);
					UListContextmenu_->SetShowState(true);
					UListContextmenu_->Show();
				}
			}
	}
}

void
FClientsForm::OnTouchMoved(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (!button_->pressposition.Contains(currentPosition)) {
			if (button_->pressed) {
				button_->pressed = false;
				buttons_->SetAt(*button_,currpressedbtn,false);
				currpressedbtn = -1;
				needsredraw = true;
			}
		}
	}
	bool isvisible = true;
	for (int n=0; n<buttons_->GetCount(); n++) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(n));
		isvisible = true;
		if ((isvisible) && (button_->pressposition.Contains(currentPosition))) {
			if (!button_->pressed) {
				button_->pressed = true;
				buttons_->SetAt(*button_,n,false);
				currpressedbtn = n;
				needsredraw = true;
			}
			break;
		}
	}
	if (needsredraw) {
		RequestRedraw();
	}
}

void
FClientsForm::OnTouchPressed(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	CheckingClientsStop();
	currpressedbtn = -1;
	OnTouchMoved(source, currentPosition, touchInfo);
}

void
FClientsForm::OnTouchReleased(const Control &source, const Osp::Graphics::Point &currentPosition, const TouchEventInfo &touchInfo)
{
	bool needsredraw = false;
	ButtonCostumEl* button_;
	if (currpressedbtn >= 0) {
		button_ = static_cast<ButtonCostumEl *> (buttons_->GetAt(currpressedbtn));
		if (button_->pressed) {
			button_->pressed = false;
			buttons_->SetAt(*button_,currpressedbtn,false);
			this->OnActionPerformed(source, button_->action);
			needsredraw = true;
		}
	}
	currpressedbtn = -1;
	if (needsredraw) {
		RequestRedraw();
	}
	CheckingClients();
}

void
FClientsForm::OnItemStateChanged (const Osp::Ui::Control &source, int index, int itemId, Osp::Ui::ItemStatus status)
{
	if (status == ITEM_SELECTED) {
		curselitemid = itemId;
		if (curselitemid < 0) {
			this->OnActionPerformed(source, ACONNECTNEW);
		} else {
			this->OnActionPerformed(source, ACONNECT);
		}
	}
}
