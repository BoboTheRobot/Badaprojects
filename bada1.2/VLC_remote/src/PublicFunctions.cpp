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
/*
 * PublicFunctions.cpp
 *
 *  Created on: 2.4.2011
 *      Author: mrak
 */

#include "PublicFunctions.h"

using namespace Osp::Base;
using namespace Osp::Net;
using namespace Osp::Net::Http;
using namespace Osp::Xml;
using namespace Osp::App;
using namespace Osp::Net::Wifi;
using namespace Osp::Base::Utility;

FileListItem::FileListItem(Osp::Base::String filename, Osp::Base::String name, Osp::Base::String fileext, bool isfolder) {
	this->filename = filename;
	this->name = name;
	this->fileext = fileext;
	this->isfolder = isfolder;
}

FileListItem::~FileListItem(void) {

}

ClientListItem::ClientListItem(int id, Osp::Base::String ip, Osp::Base::String name, bool isconnected) {
	this->id = id;
	this->ip = ip;
	this->name = name;
	this->isconnected = isconnected;
	this->ischecked = false;
	this->isprotected = false;
	this->isvirtual = false;
}

ClientListItem::~ClientListItem(void) {

}

PlayListItem::PlayListItem(Osp::Base::String title, Osp::Base::String artist, Osp::Base::String uri, int id, bool iscurrent) {
	this->title = title;
	this->artist = artist;
	this->id = id;
	this->uri = uri;
	this->iscurrent = iscurrent;
}

PlayListItem::~PlayListItem(void) {

}

PublicFunctions::PublicFunctions() {
	clientip = L"";
	clientname = L"";
	lastdir = L"~";
	VLCAPIVER = 1;
	selectedclientid = -1;
	String value;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"selectedclient", value) == E_KEY_NOT_FOUND) {
		selectedclientid = -1;
	} else {
		if (IsFailed(Integer::Parse(value, selectedclientid))) {
			selectedclientid = 0;
		}
	}
	if (selectedclientid > 0) {
		ReadClient(selectedclientid, clientip, clientname, lastdir);
	}

	pRequest = null;
	pSession = null;
	pTransaction = null;

	vlcoldstatus.isfullscreen = false;
	vlcoldstatus.mediaartist = L"START****";
	vlcoldstatus.mediacurtime = -1;
	vlcoldstatus.medialength = -1;
	vlcoldstatus.medianowplaying = L"START****";
	vlcoldstatus.mediatitle = L"START****...";
	vlcoldstatus.volume = -1;
	vlcoldstatus.state = "EMPTY";
	vlccurstatus.mediatitle = L"Connecting...";
	vlccurstatus.mediaoutputtitlestr = L"Connecting...";

	RefreshTimer_ = new Osp::Base::Runtime::Timer;
	RefreshTimer_->Construct(*this);
	RefreshTimerStarted = false;
	CheckConnectionTimer_ = new Osp::Base::Runtime::Timer;
	CheckConnectionTimer_->Construct(*this);
	CheckConnectionTimerStarted = false;

	playlist = new Osp::Base::Collection::ArrayList;
    playlist->Construct();

	filelist = new Osp::Base::Collection::ArrayList;
    filelist->Construct();

    validfileext = L"a52,aac,ac3,dts,flac,m4a,m4p,mka,mod,mp1,mp2,mp3,oma,oga,spx,tta,wav,wma,xm,asf,avi,divx,dv,flv,gxf,m1v,m2v,m2ts,m4v,mkv,mov,mp4,mpeg,mpeg1,mpeg2,mpeg4,mpg,mts,mxf,ogg,ogm,ogx,ogv,ts,vob,wmv,asx,b4s,ifo,m3u,pls,vlc,xspf";
    validfileextmusic = L"aac,ac3,flac,m4a,mod,mp1,mp2,mp3,oga,wav,wma";
    validfileextvideo = L"avi,divx,flv,m1v,m2v,m2ts,m4v,mkv,mov,mp4,mpeg,mpeg1,mpeg2,mpeg4,mpg,ogg,ogv,ts,vob,wmv";

	pwifimanager.Construct(*this);

	GetIP_ = null;

	iswificonnected = pwifimanager.IsConnected();
	if (iswificonnected) {
		StartListening();
		GetIP_ = new GetIP();
	}
}

PublicFunctions::~PublicFunctions() {
	if (RefreshTimer_ != null) {
		if (RefreshTimerStarted == true) {
			RefreshTimer_->Cancel();
		}
		delete RefreshTimer_;
		RefreshTimer_ = null;
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimer_ != null) {
		if (CheckConnectionTimerStarted == true) {
			CheckConnectionTimer_->Cancel();
		}
		delete CheckConnectionTimer_;
		CheckConnectionTimer_ = null;
		CheckConnectionTimerStarted = false;
	}
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}
	delete pTransaction;
	delete pSession;

	playlist->RemoveAll(true);
	delete playlist;
	filelist->RemoveAll(true);
	delete filelist;

	if (GetIP_ != null) {
		delete GetIP_;
		GetIP_ = null;
	}
}

void PublicFunctions::StartListening() {
	if (islistening == false) {
		islistening = true;
		sendingcommand = false;
		if (RefreshTimerStarted == true) {
			RefreshTimer_->Cancel();
			RefreshTimerStarted = false;
		}
		if (CheckConnectionTimerStarted == true) {
			CheckConnectionTimer_->Cancel();
			CheckConnectionTimerStarted = false;
		}
		vlcoldstatus.mediatitle = L"START...";
		vlccurstatus.mediatitle = L"Connecting...";
		vlccurstatus.mediaartist = L"";
		vlccurstatus.medianowplaying = L"";
		RefreshDataUpdate();
		RefreshTimerStarted = true;
		RefreshTimer_->Start(REFTIMEOUT);
	}
}

void PublicFunctions::StopListening() {
	if (islistening == true) {
		islistening = false;
		if (CheckConnectionTimerStarted == true) {
			CheckConnectionTimer_->Cancel();
			CheckConnectionTimerStarted = false;
		}
		if (RefreshTimerStarted == true) {
			RefreshTimer_->Cancel();
			RefreshTimerStarted = false;
		}
		vlccurstatus.mediatitle = L"Connection lost!";
		vlccurstatus.mediaartist = L"";
		vlccurstatus.medianowplaying = L"";
		RefreshDataUpdate();
		sendingcommand = false;
		if (pTransaction != null) {
			pSession->CancelTransaction(*pTransaction);
			pSession->CloseAllTransactions();
		}
		delete pTransaction;
		pTransaction = null;
		delete pSession;
		pSession = null;
	}
}

void PublicFunctions::LoadPlaylist() {
	sendingcommand = false;
	trytoregetplaylist = 0;
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}
	delete pTransaction;
	pTransaction = null;
	islistening = false;
	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (sendingcommand == false) {
		if (clientip != L"") {
		if (pSession == null) {
			pSession = new HttpSession();
			pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, clientip, null);
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
				currentrequesturi = clientip + L":8080/requests/playlist.xml";
				pRequest->SetUri(currentrequesturi);
				pRequest->SetMethod(NET_HTTP_METHOD_GET);
				pTransaction->Submit();
			}
		}
		}
	}
	CheckConnectionTimerStarted = true;
	CheckConnectionTimer_->Start(STATUSCHECKTIMEOUT);
}

void PublicFunctions::OnWifiActivated(result r) {

}
void PublicFunctions::OnWifiConnected(const Osp::Base::String &ssid, result r) {
	iswificonnected = true;
	StartListening();
	Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->GetCurrentForm()->SendUserEvent(EVENTWIFICONNECTED,null);
	if (GetIP_ == null) {
		GetIP_ = new GetIP();
	}
}
void PublicFunctions::OnWifiDeactivated (result r) {

}
void PublicFunctions::OnWifiDisconnected (void) {
	iswificonnected = false;
	StopListening();
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}
	delete pTransaction;
	pTransaction = null;
	delete pSession;
	pSession = null;
	Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->GetCurrentForm()->SendUserEvent(EVENTWIFIDISCONNECTED,null);
	if (GetIP_ != null) {
		delete GetIP_;
		GetIP_ = null;
	}
}
void PublicFunctions::OnWifiRssiChanged (long rssi) {

}
void PublicFunctions::OnWifiScanCompletedN(const Osp::Base::Collection::IList *pWifiBssInfoList, result r) {

}

void PublicFunctions::OnTimerExpired(Osp::Base::Runtime::Timer& timer) {
	if (pwifimanager.IsConnected()) {
		if (RefreshTimer_->Equals(timer)) {
			trytoregetplaylist = 0;
			RefreshTimerStarted = false;
			if (CheckConnectionTimerStarted == true) {
				CheckConnectionTimer_->Cancel();
				CheckConnectionTimerStarted = false;
			}
			if (clientip != L"") {
			if (pSession == null) {
				pSession = new HttpSession();
				pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, clientip, null);
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
					currentrequesturi = clientip + L":8080/requests/status.xml";
					pRequest->SetUri(currentrequesturi);
					pRequest->SetMethod(NET_HTTP_METHOD_GET);
					pTransaction->Submit();
				}
			}
			CheckConnectionTimerStarted = true;
			CheckConnectionTimer_->Start(STATUSCHECKTIMEOUT);
			}
		} else if (CheckConnectionTimer_->Equals(timer)) {
			CheckConnectionTimerStarted = false;
			Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			pFrame->GetCurrentForm()->SendUserEvent(EVENTCONNECTIONLOST,null);

			AppLog("connection lost");
			vlccurstatus.mediatitle = L"Connection lost!";
			vlccurstatus.mediaartist = L"";
			vlccurstatus.medianowplaying = L"";
			RefreshDataUpdate();
			if (sendingcommand == true) {
				sendingcommand = false;
			}
			RefreshTimerStarted = true;
			RefreshTimer_->Start(REFTIMEOUT);
		}
	} else {
		if (CheckConnectionTimerStarted == true) {
			CheckConnectionTimer_->Cancel();
			CheckConnectionTimerStarted = false;
		}
		if (RefreshTimerStarted == true) {
			RefreshTimer_->Cancel();
			RefreshTimerStarted = false;
		}
		RefreshTimerStarted = true;
		RefreshTimer_->Start(REFTIMEOUT);
	}
}

void PublicFunctions::ClearPlayList() {
	SendCommand(L"pl_empty");
	playlist->RemoveAll(true);
}

void PublicFunctions::LoadBrowseList(Osp::Base::String dir) {
	if ((VLCAPIVER >= 2) && (dir == L"~")) dir = L"file://~";
	AppLog("dir %S", dir.GetPointer());
	trytoregetplaylist = 0;
	sendingcommand = false;
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}
	delete pTransaction;
	pTransaction = null;
	islistening = false;
	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (sendingcommand == false) {
		if (clientip != L"") {
		if (pSession == null) {
			pSession = new HttpSession();
			pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, clientip, null);
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
				if (VLCAPIVER >= 2) {
					currentrequesturi = clientip + L":8080/requests/browse.xml?uri=" + prepareurl(dir);
				} else {
					currentrequesturi = clientip + L":8080/requests/browse.xml?dir=" + prepareurl(dir);
				}
				pRequest->SetUri(currentrequesturi);
				pRequest->SetMethod(NET_HTTP_METHOD_GET);
				pTransaction->Submit();
			}
		}
		}
	}
	CheckConnectionTimerStarted = true;
	CheckConnectionTimer_->Start(STATUSCHECKTIMEOUT);
}

void PublicFunctions::SendCommand(Osp::Base::String command, bool withoutreset) {
	if (withoutreset == false) {
	trytoregetplaylist = 0;
	}
	if (pwifimanager.IsConnected()) {
	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (sendingcommand == false) {
		sendingcommand = true;
		if (clientip != L"") {
		if (pSession == null) {
			pSession = new HttpSession();
			pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, clientip, null);
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
				currentrequesturi = clientip + L":8080/requests/status.xml?command=" + command;
				pRequest->SetUri(currentrequesturi);
				pRequest->SetMethod(NET_HTTP_METHOD_GET);
				pTransaction->Submit();
			}
		}
		}
	}
	CheckConnectionTimerStarted = true;
	CheckConnectionTimer_->Start(STATUSCHECKTIMEOUT);
	}
}

void PublicFunctions::SendCommand(Osp::Base::String command) {
	SendCommand(command, false);
}

void PublicFunctions::SendCommandVLM(Osp::Base::String command) {
	trytoregetplaylist = 0;
	if (pwifimanager.IsConnected()) {
	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (sendingcommand == false) {
		sendingcommand = true;
		if (clientip != L"") {
		if (pSession == null) {
			pSession = new HttpSession();
			pSession->Construct(NET_HTTP_SESSION_MODE_NORMAL, null, clientip, null);
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
				currentrequesturi = clientip + L":8080/requests/vlm_cmd.xml?command=" + command;
				pRequest->SetUri(currentrequesturi);
				pRequest->SetMethod(NET_HTTP_METHOD_GET);
				pTransaction->Submit();
			}
		}
		}
	}
	CheckConnectionTimerStarted = true;
	CheckConnectionTimer_->Start(STATUSCHECKTIMEOUT);
	}
}

Osp::Base::String PublicFunctions::FormateDate(int timestamp) {
	Osp::Base::DateTime tmpdate;
	Osp::Base::String returnstr = L"0";
	if (timestamp > 0) {
	tmpdate.SetValue((long long)(timestamp * 1000));
	Osp::Base::String hourpart(L"0"), minutepart(L"0"), secpart(L"0");
	hourpart = Osp::Base::Integer::ToString(tmpdate.GetHour());
	minutepart = Osp::Base::Integer::ToString(tmpdate.GetMinute());
	secpart = Osp::Base::Integer::ToString(tmpdate.GetSecond());
	if (secpart.GetLength() > 2) secpart.SubString(0,2,secpart);
	if (secpart.GetLength() < 2) secpart = L"0" + secpart;
	if (tmpdate.GetHour() > 0) {
		if (minutepart.GetLength() < 2) minutepart = L"0" + minutepart;
		returnstr = hourpart + L":" + minutepart + L":" + secpart;
	} else {
		returnstr = minutepart + L":" + secpart;
	}
	} else {
		returnstr = L"0:00";
	}
	return returnstr;
}

void PublicFunctions::RefreshDataUpdate() {
	statusdataischanged = false;

	if ((vlcoldstatus.isfullscreen != vlccurstatus.isfullscreen) ||
			(vlcoldstatus.mediaartist != vlccurstatus.mediaartist) ||
			(vlcoldstatus.mediacurtime != vlccurstatus.mediacurtime) ||
			(vlcoldstatus.medialength != vlccurstatus.medialength) ||
			(vlcoldstatus.medianowplaying != vlccurstatus.medianowplaying) ||
			(vlcoldstatus.mediatitle != vlccurstatus.mediatitle) ||
			(vlcoldstatus.state != vlccurstatus.state) ||
			(vlcoldstatus.volume != vlccurstatus.volume)
		) {
		statusdataischanged = true;
	}
	if (statusdataischanged) {
		if ((vlcoldstatus.mediacurtime != vlccurstatus.mediacurtime) || (vlcoldstatus.medialength != vlccurstatus.medialength)) {
			vlccurstatus.mediacurtimestr = FormateDate(vlccurstatus.mediacurtime);
			vlccurstatus.medialengthstr = FormateDate(vlccurstatus.medialength);
			vlccurstatus.mediaremainingtimestr = L"-" + FormateDate(vlccurstatus.medialength - vlccurstatus.mediacurtime);
			if (vlccurstatus.mediaremainingtimestr == L"-0:00") {
				vlccurstatus.mediaremainingtimestr = L"";
			}
			vlccurstatus.mediatimestr = vlccurstatus.mediacurtimestr;
			if (vlccurstatus.medialengthstr != L"0:00") {
				vlccurstatus.mediatimestr.Append(L" / " + vlccurstatus.medialengthstr);
			}
		}
		vlccurstatus.mediaoutputtitlestr = L"";
		vlccurstatus.mediaoutputtitlestr.Append(vlccurstatus.medianowplaying);
		vlccurstatus.mediaoutputtitlestr.Trim();
		if (vlccurstatus.mediaoutputtitlestr != L"") vlccurstatus.mediaoutputtitlestr.Append(L"\n");
		vlccurstatus.mediaoutputtitlestr.Append(vlccurstatus.mediaartist);
		vlccurstatus.mediaoutputtitlestr.Trim();
		if (vlccurstatus.mediaoutputtitlestr != L"") vlccurstatus.mediaoutputtitlestr.Append(L"\n");
		vlccurstatus.mediaoutputtitlestr.Append(vlccurstatus.mediatitle);
		vlccurstatus.mediaoutputtitlestr.Trim();

		vlccurstatus.volumeproc = ((double)vlccurstatus.volume / 512.0f);
		if (vlccurstatus.volumeproc < 0) vlccurstatus.volumeproc = 0;
		if (vlccurstatus.volumeproc > 1) vlccurstatus.volumeproc = 1;
		if (vlccurstatus.medialength > 0) {
			vlccurstatus.seekproc = ((double)vlccurstatus.mediacurtime / (double)vlccurstatus.medialength);
		} else {
			vlccurstatus.seekproc = 0;
		}
		if (vlccurstatus.seekproc < 0) vlccurstatus.seekproc = 0;
		if (vlccurstatus.seekproc > 1) vlccurstatus.seekproc = 1;

	}

	Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
	pFrame->GetCurrentForm()->SendUserEvent(EVENTREFRESHDATAUPDATE,null);

}

void PublicFunctions::RefreshDataUpdateRecived() {
	vlcoldstatus.isfullscreen = vlccurstatus.isfullscreen;
	vlcoldstatus.mediaartist = vlccurstatus.mediaartist;
	vlcoldstatus.mediacurtime = vlccurstatus.mediacurtime;
	vlcoldstatus.medialength = vlccurstatus.medialength;
	vlcoldstatus.medianowplaying = vlccurstatus.medianowplaying;
	vlcoldstatus.mediatitle = vlccurstatus.mediatitle;
	vlcoldstatus.state = vlccurstatus.state;
	vlcoldstatus.volume = vlccurstatus.volume;
	vlcoldstatus.seekproc = vlccurstatus.seekproc;
	vlcoldstatus.mediaoutputtitlestr = vlccurstatus.mediaoutputtitlestr;
	statusdataischanged = false;
}

void PublicFunctions::OnTransactionReadyToRead(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int availableBodyLen)
{

}

void PublicFunctions::OnTransactionAborted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, result r)
{
	AppLog("OnTransactionAborted");
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	vlccurstatus.mediatitle = L"Connection lost!";
	vlccurstatus.mediaartist = L"";
	vlccurstatus.medianowplaying = L"";
	RefreshDataUpdate();
	sendingcommand = false;
	RefreshTimerStarted = true;
	RefreshTimer_->Start(REFTIMEOUT);
}

void PublicFunctions::OnTransactionReadyToWrite(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int recommendedChunkSize)
{

}

void PublicFunctions::OnTransactionHeaderCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int headerLen, bool bAuthRequired)
{

}

void PublicFunctions::OnTransactionCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction)
{
	if (sendingcommand == true) {
		sendingcommand = false;
		if (trytoregetplaylist == 1) {
			trytoregetplaylist = 2;
			SendCommand(L"pl_stop", true);
		} else if (trytoregetplaylist == 2) {
			LoadPlaylist();
		} else {
			Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
			pFrame->GetCurrentForm()->SendUserEvent(EVENTCOMMANDRETURN,null);
		}
	} else {
		Osp::Base::String nodeValue = L"";
		Osp::Base::String nodeName = L"";

		int requesturichecki = -1;

		ByteBuffer * pBuffer = httpTransaction.GetResponse()->ReadBodyN();
		xmlDocPtr pDocument = null;
		char *charBuff = (char*) pBuffer->GetPointer();

		pDocument = Osp::Xml::xmlParseMemory(charBuff, pBuffer->GetCapacity());

		if (currentrequesturi.IndexOf(L"/status.xml",0,requesturichecki) == E_SUCCESS) {
			if (pDocument == null) {
				AppLog("xml response is null");
			} else {

				vlccurstatus.mediatitle = L"";
				vlccurstatus.mediaartist = L"";
				vlccurstatus.medianowplaying = L"";

				xmlNodePtr pRoot = xmlDocGetRootElement(pDocument);
				xmlNodePtr pCurrentElement = null;

				if (pRoot) {
					if (pRoot->children) {
						for(pCurrentElement = pRoot->children; pCurrentElement; pCurrentElement = pCurrentElement->next) {
							if (pCurrentElement->type == XML_ELEMENT_NODE) {
								Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->name, nodeName);
								if ((nodeName == L"volume") || (nodeName == L"length") || (nodeName == L"time") || (nodeName == L"state") || (nodeName == L"fullscreen") || (nodeName == L"apiversion")) {
									nodeValue = L"";
									if((pCurrentElement->children) != null) {
										if (strlen((char*)pCurrentElement->children->content) > 0) {
											Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->children->content, nodeValue);
										}
									}
									if (nodeName == L"volume") {
										Osp::Base::Integer::Parse(nodeValue, vlccurstatus.volume);
									}
									if (nodeName == L"length") {
										Osp::Base::Integer::Parse(nodeValue, vlccurstatus.medialength);
									}
									if (nodeName == L"time") {
										Osp::Base::Integer::Parse(nodeValue, vlccurstatus.mediacurtime);
									}
									if (nodeName == L"state") {
										vlccurstatus.state = nodeValue;
									}
									if (nodeName == L"fullscreen") {
										vlccurstatus.isfullscreen = (nodeValue == L"1");
									}
									if (nodeName == L"apiversion") {
										Osp::Base::Integer::Parse(nodeValue, VLCAPIVER);
									}
								}
							}
						}
					}
				}
				xmlXPathContextPtr xpathCtx = xmlXPathNewContext(pDocument);
				xmlXPathObjectPtr xpathObj = null;
				if (VLCAPIVER >= 2) {
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/category[@name='meta']/info[@name='title']", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediatitle);
								}
							}
						}
					}
					xmlXPathFreeObject(xpathObj);
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/category[@name='meta']/info[@name='artist']", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediaartist);
								}
							}
						}
					}
					xmlXPathFreeObject(xpathObj);
					if (vlccurstatus.mediaartist == L"") {
						xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/category[@name='meta']/info[@name='showName']", xpathCtx);
						if (xpathObj) {
							if (xpathObj->nodesetval->nodeNr > 0) {
								if (xpathObj->nodesetval->nodeTab[0]->children != null) {
									if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
										Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediaartist);
									}
								}
							}
						}
						xmlXPathFreeObject(xpathObj);
					}
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/category[@name='meta']/info[@name='now_playing']", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.medianowplaying);
								}
							}
						}
					}
					xmlXPathFreeObject(xpathObj);
					if (vlccurstatus.mediatitle == L"") {
						xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/category[@name='meta']/info[@name='filename']", xpathCtx);
						if (xpathObj) {
							if (xpathObj->nodesetval->nodeNr > 0) {
								if (xpathObj->nodesetval->nodeTab[0]->children != null) {
									if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
										Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediatitle);
									}
								}
							}
						}
						xmlXPathFreeObject(xpathObj);
					}
					vlccurstatus.mediatitle = GetTitleFromUri(DecodeXMLentities(vlccurstatus.mediatitle));
					vlccurstatus.mediaartist = DecodeXMLentities(vlccurstatus.mediaartist);
					vlccurstatus.medianowplaying = DecodeXMLentities(vlccurstatus.medianowplaying);
				} else {
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/meta-information/title", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediatitle);
								}
							}
						}
					}
					xmlXPathFreeObject(xpathObj);
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/meta-information/artist", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.mediaartist);
								}
							}
						}
					}
					xmlXPathFreeObject(xpathObj);
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/information/meta-information/now_playing", xpathCtx);
					if (xpathObj) {
						if (xpathObj->nodesetval->nodeNr > 0) {
							if (xpathObj->nodesetval->nodeTab[0]->children != null) {
								if (strlen((char*)xpathObj->nodesetval->nodeTab[0]->children->content) > 0) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)xpathObj->nodesetval->nodeTab[0]->children->content, vlccurstatus.medianowplaying);
								}
							}
						}
					}
					vlccurstatus.mediatitle = GetTitleFromUri(DecodeXMLentities(vlccurstatus.mediatitle));
					vlccurstatus.mediaartist = DecodeXMLentities(vlccurstatus.mediaartist);
					vlccurstatus.medianowplaying = DecodeXMLentities(vlccurstatus.medianowplaying);
				}
				xmlXPathFreeObject(xpathObj);
				xmlXPathFreeContext(xpathCtx);


				RefreshDataUpdate();
			}
		} else if (currentrequesturi.IndexOf(L"/playlist.xml",0,requesturichecki) == E_SUCCESS) {
			playlist->RemoveAll(true);
			if (pDocument == null) {
				if (trytoregetplaylist == 0) {
					trytoregetplaylist = 1;
					SendCommand(L"pl_play", true);
				} else {
					trytoregetplaylist = 0;
					Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
					pFrame->GetCurrentForm()->SendUserEvent(EVENTREFRESHDATAUPDATEPLAYLIST,null);
				}
			} else {
				trytoregetplaylist = 0;
				Osp::Base::String mediaartist;
				Osp::Base::String mediatitle;
				Osp::Base::String medianame;
				Osp::Base::String mediauri;
				int mediaid;
				bool iscurrent;
				xmlXPathContextPtr xpathCtx = xmlXPathNewContext(pDocument);
				xmlXPathObjectPtr xpathObj = null;
				xpathObj = xmlXPathEvalExpression((xmlChar*)"//node/node[@id='2']/leaf", xpathCtx);
				if (xpathObj) {
					for(int inode = 0; inode < xpathObj->nodesetval->nodeNr; inode++) {
						mediaartist = L"";
						mediatitle = L"";
						medianame = L"";
						mediauri = L"";
						iscurrent = false;
						mediaid = 0;
						xmlAttr *attr = xpathObj->nodesetval->nodeTab[inode]->properties;
						while ( attr ) {
							Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->name, nodeName);
							if (strlen((char*)attr->children->content) > 0) {
								Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->children->content, nodeValue);
							} else {
								nodeValue = L"";
							}
							if ((nodeName == L"current") && (nodeValue == L"current")) {
								iscurrent = true;
							}
							if (nodeName == L"id") {
								Osp::Base::Integer::Parse(nodeValue,mediaid);
							}
							if (nodeName == L"name") {
								medianame = nodeValue;
							}
							if (nodeName == L"uri") {
								mediauri = nodeValue;
							}
							attr = attr->next;
						}
						xmlNodePtr pCurrentElement = null;
						if (xpathObj->nodesetval->nodeTab[inode]->children) {
							for(pCurrentElement = xpathObj->nodesetval->nodeTab[inode]->children; pCurrentElement; pCurrentElement = pCurrentElement->next) {
								if (pCurrentElement->type == XML_ELEMENT_NODE) {
									Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->name, nodeName);
									if ((nodeName == L"title") || (nodeName == L"artist")) {
										nodeValue = L"";
										if((pCurrentElement->children) != null) {
											if (strlen((char*)pCurrentElement->children->content) > 0) {
												Osp::Base::Utility::StringUtil::Utf8ToString ((char*)pCurrentElement->children->content, nodeValue);
											}
										}
										if (nodeName == L"title") {
											mediatitle = GetTitleFromUri(DecodeXMLentities(nodeValue));
										}
										if (nodeName == L"artist") {
											mediaartist = DecodeXMLentities(nodeValue);
										}
									}
								}
							}
						}
						if (mediaid > 0) {
							if (mediaartist == L"") mediaartist = medianame;
							if (mediatitle == L"") mediatitle = GetTitleFromUri(mediauri);
							PlayListItem * playlistitem_ = new PlayListItem(mediatitle, mediaartist, mediauri, mediaid, iscurrent);
							playlist->Add(*playlistitem_);
						}
					}
				}
				xmlXPathFreeObject(xpathObj);
				xmlXPathFreeContext(xpathCtx);
				Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				pFrame->GetCurrentForm()->SendUserEvent(EVENTREFRESHDATAUPDATEPLAYLIST,null);
			}
		} else if (currentrequesturi.IndexOf(L"/browse.xml",0,requesturichecki) == E_SUCCESS) {
			if (pDocument == null) {
				Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				pFrame->GetCurrentForm()->SendUserEvent(EVENTREFRESHDATAUPDATEBROWSE,null);
			} else {
				filelist->RemoveAll(true);
				Osp::Base::String filepath;
				Osp::Base::String fileext;
				Osp::Base::String filename;
				xmlXPathContextPtr xpathCtx = xmlXPathNewContext(pDocument);
				xmlXPathObjectPtr xpathObj = null;
				if (VLCAPIVER >= 2) {
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/element[@type='dir']", xpathCtx);
				} else {
					xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/element[@type='directory']", xpathCtx);
				}
				if (xpathObj) {
					for(int inode = 0; inode < xpathObj->nodesetval->nodeNr; inode++) {
						filepath = L"";
						filename = L"";
						xmlAttr *attr = xpathObj->nodesetval->nodeTab[inode]->properties;
						while ( attr ) {
							Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->name, nodeName);
							if (strlen((char*)attr->children->content) > 0) {
								Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->children->content, nodeValue);
							} else {
								nodeValue = L"";
							}
							if (VLCAPIVER >= 2) {
								if (nodeName == L"uri") {
									filepath = nodeValue;
								}
							} else {
								if (nodeName == L"path") {
									filepath = nodeValue;
								}
							}
							if (nodeName == L"name") {
								filename = nodeValue;
							}
							attr = attr->next;
						}
						if (filepath != L"") {
							FileListItem * filelistitem_ = new FileListItem(filepath, filename, L"", true);
							if ((filename == L"..") && (filelist->GetCount() > 0)) {
								filelist->InsertAt(*filelistitem_, 0);
							} else {
								filelist->Add(*filelistitem_);
							}
						}
					}
				}
				xmlXPathFreeObject(xpathObj);
				xpathObj = xmlXPathEvalExpression((xmlChar*)"//root/element[@type='file']", xpathCtx);
				if (xpathObj) {
					for(int inode = 0; inode < xpathObj->nodesetval->nodeNr; inode++) {
						filepath = L"";
						filename = L"";
						fileext = L"";
						xmlAttr *attr = xpathObj->nodesetval->nodeTab[inode]->properties;
						while ( attr ) {
							Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->name, nodeName);
							if (strlen((char*)attr->children->content) > 0) {
								Osp::Base::Utility::StringUtil::Utf8ToString ((char*)attr->children->content, nodeValue);
							} else {
								nodeValue = L"";
							}
							if (nodeName == L"path") {
								filepath = nodeValue;
							}
							if (nodeName == L"name") {
								filename = nodeValue;
							}
							if (nodeName == L"extension") {
								fileext = nodeValue;
							}
							attr = attr->next;
						}
						if (filepath != L"") {
							if ((fileext == L"") && (filename.GetLength() > 4)) {
								int extpos = 0;
								filename.LastIndexOf(L".",filename.GetLength()-1,extpos);
								filename.SubString(extpos+1,fileext);
							}
							FileListItem * filelistitem_ = new FileListItem(filepath, filename, fileext, false);
							filelist->Add(*filelistitem_);
						}
					}
				}
				xmlXPathFreeObject(xpathObj);
				xmlXPathFreeContext(xpathCtx);
				Osp::Ui::Controls::Frame *pFrame = Application::GetInstance()->GetAppFrame()->GetFrame();
				pFrame->GetCurrentForm()->SendUserEvent(EVENTREFRESHDATAUPDATEBROWSE,null);
			}
		}
		xmlFreeDoc(pDocument);
		delete pBuffer;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimerStarted = false;
		CheckConnectionTimer_->Cancel();
	}
	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
	}
	RefreshTimerStarted = true;
	RefreshTimer_->Start(REFTIMEOUT);
}

void PublicFunctions::OnTransactionCertVerificationRequiredN(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, String* pCert)
{

}

void PublicFunctions::ReadClient(int id, Osp::Base::String & ip, Osp::Base::String & name, Osp::Base::String & lastdir) {
	Osp::Io::Database * db_ = new Osp::Io::Database();
	if (Osp::Io::Database::Exists(L"/Home/data.db")) {
		db_->Construct((L"/Home/data.db"), true);
		Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT ID,IP,NAME,LASTDIR FROM CLIENTS WHERE ID=" + Osp::Base::Integer::ToString(id));
		if(!(!pEnum_)) {
			pEnum_->Reset();
			if (pEnum_->MoveNext() ==  E_SUCCESS) {
				if (pEnum_->GetColumnSize(1) > 0) {
					pEnum_->GetStringAt(1, ip);
				}
				if (pEnum_->GetColumnSize(2) > 0) {
					pEnum_->GetStringAt(2, name);
				}
				if (pEnum_->GetColumnSize(3) > 0) {
					pEnum_->GetStringAt(3, lastdir);
				}
			}
		}
		delete pEnum_;
	}
	delete db_;
}

void PublicFunctions::SetCurClientLastDir(Osp::Base::String lastdir) {
	this->lastdir = lastdir;
	if (selectedclientid > 0) {
		Osp::Io::Database * db_ = new Osp::Io::Database();
		if (Osp::Io::Database::Exists(L"/Home/data.db")) {
			db_->Construct((L"/Home/data.db"), true);
			Osp::Io::DbEnumerator * pEnum_ = db_->QueryN(L"SELECT * FROM CLIENTS WHERE ID=" + Osp::Base::Integer::ToString(selectedclientid));
			if(!(!pEnum_)) {
				db_->BeginTransaction();
				Osp::Io::DbStatement * pStmt_ = db_->CreateStatementN(L"UPDATE CLIENTS SET LASTDIR = ? WHERE ID=?");
				pStmt_->BindString(0, lastdir);
				pStmt_->BindInt(1, selectedclientid);
				db_->ExecuteStatementN(*pStmt_);
				db_->CommitTransaction();
				delete pStmt_;
			}
			delete pEnum_;
		}
		delete db_;
	}
}

void PublicFunctions::SelectClient(int id) {
	this->selectedclientid = id;
	String value;
	if (Application::GetInstance()->GetAppRegistry()->Get(L"selectedclient", value) == E_KEY_NOT_FOUND) {
		Application::GetInstance()->GetAppRegistry()->Add(L"selectedclient", selectedclientid);
	} else {
		Application::GetInstance()->GetAppRegistry()->Set(L"selectedclient", selectedclientid);
	}
	Application::GetInstance()->GetAppRegistry()->Save();
	this->clientip = L"";
	this->clientname = L"";
	this->lastdir = L"~";
	ReadClient(id, clientip, clientname, lastdir);

	if (RefreshTimerStarted == true) {
		RefreshTimer_->Cancel();
		RefreshTimerStarted = false;
	}
	if (CheckConnectionTimerStarted == true) {
		CheckConnectionTimer_->Cancel();
		CheckConnectionTimerStarted = false;
	}
	if (pTransaction != null) {
		pSession->CancelTransaction(*pTransaction);
		pSession->CloseAllTransactions();
	}
	delete pTransaction;
	delete pSession;
	sendingcommand = false;
	islistening = false;
	statusdataischanged = true;

	pRequest = null;
	pSession = null;
	pTransaction = null;

	vlcoldstatus.isfullscreen = false;
	vlcoldstatus.mediaartist = L"START****";
	vlcoldstatus.mediacurtime = -1;
	vlcoldstatus.medialength = -1;
	vlcoldstatus.medianowplaying = L"START****";
	vlcoldstatus.mediatitle = L"START****...";
	vlcoldstatus.volume = -1;
	vlcoldstatus.state = "EMPTY";
	vlccurstatus.mediatitle = L"Connecting...";
	vlccurstatus.mediaoutputtitlestr = L"Connecting...";

	iswificonnected = pwifimanager.IsConnected();

	if (iswificonnected) {
		StartListening();
	}
}

Osp::Base::String PublicFunctions::urlencode_char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;

    Osp::Base::String r;
    if (dig1 == 'a') dig1 = 'A';
    if (dig1 == 'b') dig1 = 'B';
    if (dig1 == 'c') dig1 = 'C';
    if (dig1 == 'd') dig1 = 'D';
    if (dig1 == 'e') dig1 = 'E';
    if (dig1 == 'f') dig1 = 'F';
    if (dig2 == 'a') dig2 = 'A';
    if (dig2 == 'b') dig2 = 'B';
    if (dig2 == 'c') dig2 = 'C';
    if (dig2 == 'd') dig2 = 'D';
    if (dig2 == 'e') dig2 = 'E';
    if (dig2 == 'f') dig2 = 'F';
    r.Append(dig1);
    r.Append(dig2);
    return r;
}

Osp::Base::String PublicFunctions::urlencode(Osp::Base::String instr)
{

    Osp::Base::String escaped = L"";

    if (instr.GetLength() > 0) {
    Osp::Base::ByteBuffer * bb = StringUtil::StringToUtf8N(instr);
    byte byte;
    while (bb->GetRemaining() > 1) {
    	bb->GetByte(byte);
    	if ((byte >= 0x30) && (byte <= 0x39)) { //is number
    		escaped.Append((char)byte);
    	} else if ((byte >= 0x41) && (byte <= 0x5A)) { //is alpha uppercase
    		escaped.Append((char)byte);
    	} else if ((byte >= 0x61) && (byte <= 0x7A)) { //is alpha lowercase
    		escaped.Append((char)byte);
    	} else if ((byte == 0x2D) || (byte == 0x2E) || (byte == 0x5F) || (byte == 0x7E)) { //Unreserved -._~
    		escaped.Append((char)byte);
    	} else {
    		escaped.Append(L"%");
    		escaped.Append(urlencode_char2hex(byte));
    	}
    }
    delete bb;
    }

    return escaped;
}

Osp::Base::String PublicFunctions::prepareurl(Osp::Base::String url) {
	//escapebackslashes
	url.Replace(L"\\", L"\\\\");
	//addslashes
	url.Replace(L"'", L"\\'");
	//encode
	return urlencode(url);
}

Osp::Base::String PublicFunctions::DecodeXMLentities(Osp::Base::String instr) {
	if (instr.GetLength() > 0) {
		instr.Replace(L"&amp;", L"&");
		instr.Replace(L"&quot;", L"'");
		instr.Replace(L"&#39;", L"'");
		instr.Replace(L"&apos;", L"'");
		instr.Replace(L"&lt;", L"<");
		instr.Replace(L"&gt;", L">");
	}
	return instr;
}

int PublicFunctions::GetMediaFileType(Osp::Base::String fileext) {
	bool isvalidext;
	bool ismusicfile, isvideofile;
	isvalidext = false;
	Osp::Base::Utility::StringTokenizer validfileexttok(validfileext, L",");
	Osp::Base::String vaildfileext;
	while (validfileexttok.HasMoreTokens()) {
		validfileexttok.GetNextToken(vaildfileext);
		if (vaildfileext == fileext) {
			isvalidext = true;
			break;
		}
	}
	if (isvalidext) {
		ismusicfile = false;
		isvideofile = false;
		Osp::Base::Utility::StringTokenizer ismusicfiletok(validfileextmusic, L",");
		while (ismusicfiletok.HasMoreTokens()) {
			ismusicfiletok.GetNextToken(vaildfileext);
			if (vaildfileext == fileext) {
				ismusicfile = true;
				break;
			}
		}
		if (!ismusicfile) {
			Osp::Base::Utility::StringTokenizer isvideofiletok(validfileextvideo, L",");
			while (isvideofiletok.HasMoreTokens()) {
				isvideofiletok.GetNextToken(vaildfileext);
				if (vaildfileext == fileext) {
					isvideofile = true;
					break;
				}
			}
		}
		if (ismusicfile) {
			return MEDIAFILETYPE_AUDIO;
		} else if (isvideofile) {
			return MEDIAFILETYPE_VIDEO;
		} else {
			return MEDIAFILETYPE_OTHER;
		}
	} else {
		return 0;
	}
}

Osp::Base::String PublicFunctions::GetMyIp() {
	if (this->GetIP_ != null) {
		return this->GetIP_->localip;
	} else {
		return L"";
	}
}

Osp::Base::String PublicFunctions::GetTitleFromUri(Osp::Base::String uri) {
	//preveri ce je uri file://, ce ima scot
	Osp::Base::String parsedname = uri;
	int soutpos;
	int filenamepartpos;
	if (parsedname.GetLength() > 0) {
	if (parsedname.IndexOf(L":sout=#",0,soutpos) == E_SUCCESS) {
		parsedname.SubString(0,soutpos,parsedname);
		parsedname.Trim();
	}
	}
	if (parsedname.GetLength() > 0) {
	if (parsedname.StartsWith(L"file://",0)) {
		if (parsedname.LastIndexOf(L"/",parsedname.GetLength()-1,filenamepartpos) == E_SUCCESS) {
			parsedname.SubString(filenamepartpos+1,parsedname.GetLength()-filenamepartpos-1,parsedname);
			parsedname.Trim();
			parsedname = DecodeXMLentities(parsedname);
			Uri tmpuri;
			tmpuri.SetUri(parsedname);
			parsedname = tmpuri.GetPath();
			//uneascapaj string
		}
	} else if (parsedname.LastIndexOf(L"\\",parsedname.GetLength()-1,filenamepartpos) == E_SUCCESS) {
		parsedname.SubString(filenamepartpos+1,parsedname.GetLength()-filenamepartpos-1,parsedname);
		parsedname.Trim();
	}
	}
	return parsedname;
}
