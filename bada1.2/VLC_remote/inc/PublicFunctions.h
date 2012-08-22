/*
 * PublicFunctions.h
 *
 *  Created on: 2.4.2011
 *      Author: mrak
 */

#ifndef PUBLICFUNCTIONS_H_
#define PUBLICFUNCTIONS_H_

#include <FApp.h>
#include <FBase.h>
#include <FNet.h>
#include <FXml.h>
#include <FBaseByteBuffer.h>
#include <FSystem.h>
#include <FUi.h>
#include <FIo.h>
#include "GetIP.h"

#define REFTIMEOUT 300
#define STATUSCHECKTIMEOUT 15000
#define CLIENTSTATUSCHECKTIMEOUT 5000
#define CHECKNEXTCLIENTTIMEOUT 500

typedef struct {
	int volume;
	int medialength;
	int mediacurtime;
	Osp::Base::String mediatitle;
	Osp::Base::String mediaartist;
	Osp::Base::String medianowplaying;
	Osp::Base::String state;
	bool isfullscreen;
	Osp::Base::String medialengthstr;
	Osp::Base::String mediacurtimestr;
	Osp::Base::String mediaremainingtimestr;
	Osp::Base::String mediatimestr;
	Osp::Base::String mediaoutputtitlestr;
	float volumeproc;
	float seekproc;
} VLC_CURSTATUS;

class FileListItem : public Osp::Base::Object {
public:
	FileListItem(Osp::Base::String filename, Osp::Base::String name, Osp::Base::String fileext, bool isfolder);
	virtual ~FileListItem(void);

public:
	Osp::Base::String filename;
	Osp::Base::String name;
	Osp::Base::String fileext;
	bool isfolder;
};

class ClientListItem : public Osp::Base::Object {
public:
	ClientListItem(int id, Osp::Base::String ip, Osp::Base::String name, bool isconnected);
	virtual ~ClientListItem(void);

public:
	int id;
	Osp::Base::String ip;
	Osp::Base::String name;
	bool isconnected;
	bool isvirtual;
	bool isprotected;
	bool ischecked;
};

class PlayListItem : public Osp::Base::Object {
public:
	PlayListItem(Osp::Base::String title, Osp::Base::String artist, Osp::Base::String uri, int id, bool iscurrent);
	virtual ~PlayListItem(void);

public:
	Osp::Base::String title;
	Osp::Base::String artist;
	Osp::Base::String uri;
	int id;
	bool iscurrent;
};

class PublicFunctions:
	public Osp::Net::Http::IHttpTransactionEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Net::Wifi::IWifiManagerEventListener
{
public:
	PublicFunctions();
	virtual ~PublicFunctions();

public:

	static const RequestId EVENTREFRESHDATAUPDATE = 1;
	static const RequestId EVENTWIFICONNECTED = 2;
	static const RequestId EVENTWIFIDISCONNECTED = 3;
	static const RequestId EVENTCONNECTIONLOST = 4;
	static const RequestId EVENTREFRESHDATAUPDATEPLAYLIST = 5;
	static const RequestId EVENTREFRESHDATAUPDATEBROWSE = 6;
	static const RequestId EVENTCOMMANDRETURN = 7;

	static const int MEDIAFILETYPE_OTHER = 1;
	static const int MEDIAFILETYPE_AUDIO = 2;
	static const int MEDIAFILETYPE_VIDEO = 3;

	Osp::Base::String clientip;
	Osp::Base::String lastdir;
	Osp::Base::String clientname;
	int selectedclientid;

	Osp::Base::Runtime::Timer * RefreshTimer_;
	Osp::Base::Runtime::Timer * CheckConnectionTimer_;
	bool RefreshTimerStarted;
	bool CheckConnectionTimerStarted;

	bool iswificonnected;

	bool sendingcommand;
	int sendingcommandtype;
	bool islistening;

	bool statusdataischanged;

	VLC_CURSTATUS vlccurstatus;
	VLC_CURSTATUS vlcoldstatus;

	Osp::Base::Collection::ArrayList * playlist;

	Osp::Base::Collection::ArrayList * filelist;
	Osp::Base::String validfileextvideo;
	Osp::Base::String validfileextmusic;
	Osp::Base::String validfileext;

	int VLCAPIVER;

private:
	void OnTransactionReadyToRead(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int availableBodyLen);
	void OnTransactionAborted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, result r);
	void OnTransactionReadyToWrite(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int recommendedChunkSize);
	void OnTransactionHeaderCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, int headerLen, bool bAuthRequired);
	void OnTransactionCompleted(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction);
	void OnTransactionCertVerificationRequiredN(Osp::Net::Http::HttpSession& httpSession, Osp::Net::Http::HttpTransaction& httpTransaction, Osp::Base::String* pCert);

	Osp::Net::Http::HttpSession * pSession;
	Osp::Net::Http::HttpTransaction * pTransaction;
	Osp::Net::Http::HttpRequest * pRequest;
	Osp::Base::String currentrequesturi;

	Osp::Net::Wifi::WifiManager pwifimanager;

	int trytoregetplaylist;

	void OnWifiActivated(result r);
	void OnWifiConnected(const Osp::Base::String &ssid, result r);
	void OnWifiDeactivated (result r);
	void OnWifiDisconnected (void);
	void OnWifiRssiChanged (long rssi);
	void OnWifiScanCompletedN(const Osp::Base::Collection::IList *pWifiBssInfoList, result r);

	Osp::Base::String urlencode_char2hex( char dec );
	void ReadClient(int id, Osp::Base::String & ip, Osp::Base::String & name, Osp::Base::String & lastdir);

	GetIP * GetIP_;

public:
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);

public:
	Osp::Base::String FormateDate(int timestamp);
	Osp::Base::String GetTitleFromUri(Osp::Base::String uri);
	int GetMediaFileType(Osp::Base::String fileext);

	Osp::Base::String GetMyIp();

	void SendCommand(Osp::Base::String command, bool withoutreset);
	void SendCommand(Osp::Base::String command);
	void SendCommandVLM(Osp::Base::String command);
	void StartListening();
	void StopListening();
	void RefreshDataUpdate();
	void RefreshDataUpdateRecived();
	void LoadPlaylist();
	void ClearPlayList();
	void LoadBrowseList(Osp::Base::String dir);

	void SelectClient(int id);
	void SetCurClientLastDir(Osp::Base::String lastdir);
	Osp::Base::String DecodeXMLentities(Osp::Base::String instr);
	Osp::Base::String urlencode(Osp::Base::String instr);
	Osp::Base::String prepareurl(Osp::Base::String url);
};

#endif /* PUBLICFUNCTIONS_H_ */
