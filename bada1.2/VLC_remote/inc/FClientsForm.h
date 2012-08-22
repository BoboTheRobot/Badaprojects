#ifndef _FClientsForm_H_
#define _FClientsForm_H_

#include <FBase.h>
#include <FUi.h>
#include <FNet.h>
#include <FXml.h>
#include <FBaseByteBuffer.h>
#include <FMedia.h>
#include <FSystem.h>
#include <FIo.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "PublicFunctions.h"

class FClientsForm :
	public Osp::Ui::Controls::Form,
	public Osp::Net::Http::IHttpTransactionEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Net::Wifi::IWifiManagerEventListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Ui::IItemEventListener,
	public Osp::Ui::ITouchEventListener
{

// Construction
public:
	FClientsForm(bool isfromstart);
	virtual ~FClientsForm(void);
	bool Initialize();
	result OnInitializing(void);
	result OnTerminating(void);

// Implementation
protected:
	static const int AREMOVE = 1;
	static const int AADD = 2;
	static const int AEDIT = 3;
	static const int ACONNECT = 4;
	static const int ASAVE = 5;
	static const int ACANCEL = 6;
	static const int ABACK = 7;
	static const int ACONNECTRESPONSEOFF = 8;
	static const int ACONNECTRESPONSEON = 9;
	static const int AHELP = 10;
	static const int ACANCELCHECK = 11;
	static const int ACONNECTRESPONSEOFFFORBIDDEN = 12;
	static const int ACONNECTNEW = 13;

	static const int BOXBTN = 4;
	Osp::Base::Runtime::Timer * CheckConnectionTimer_;
	bool CheckConnectionTimerStarted;
	Osp::Base::Runtime::Timer * CheckNextClientTimer_;
	bool CheckNextClientTimerStarted;
	Osp::Base::Runtime::Timer * CheckConnectionProgressTimer_;

	Osp::Base::Runtime::Timer * CheckForMyIpTimer_;
	bool CheckForMyIpTimerStarted;

	DrawingClass * drawingclass_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	bool sendingcommand;

	Osp::Base::Collection::ArrayList * clientlist;
	int currcheckclientindex;
	long long currcheckclienttime;

	Osp::Ui::Controls::List * UList_;
	Osp::Graphics::Bitmap * connectedicon;
	Osp::Graphics::Bitmap * disconnectedicon;
	Osp::Graphics::Bitmap * protectedicon;
	Osp::Graphics::Bitmap * newconnectedicon;
	Osp::Graphics::Bitmap * newprotectedicon;

	Osp::Ui::Controls::ContextMenu * UListContextmenu_;
	int curselitemid;

	Osp::Ui::Controls::Popup* addeditpopup_;
	Osp::Ui::Controls::Popup* connectingpopup_;

	bool connectinginprogress;

	bool isfromstart;
	bool startupchecklast;

	bool ischeckinginprogress;

	PublicFunctions * publicfunc_;

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

	Osp::Net::Wifi::WifiManager pwifimanager;

	void OnWifiActivated(result r);
	void OnWifiConnected(const Osp::Base::String &ssid, result r);
	void OnWifiDeactivated (result r);
	void OnWifiDisconnected (void);
	void OnWifiRssiChanged (long rssi);
	void OnWifiScanCompletedN(const Osp::Base::Collection::IList *pWifiBssInfoList, result r);

// Generated call-back functions
public:
	void FillPotentialClients();

	void SaveClient(int id, Osp::Base::String ip, Osp::Base::String name);
	void DeleteClient(int id);

	void CheckingClients();
	void CheckingClientsStop();
	void ResetChekingClients(int fromindex);

	void StartUpCheck();

	void FillListItems();
	void PingClient(Osp::Base::String ip);
	void PingClientResponse(bool status, bool isforbidden);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
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
};

#endif
