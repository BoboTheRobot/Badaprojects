#ifndef _FMAINFORM_H_
#define _FMAINFORM_H_

#include <FBase.h>
#include <FUi.h>
#include <FMedia.h>
#include <FSystem.h>
#include "DrawingClass.h"
#include "FormMgr.h"
#include "PublicFunctions.h"

class FMainForm :
	public Osp::Ui::Controls::Form,
	public Osp::Ui::ITouchEventListener,
	public Osp::Ui::IKeyEventListener,
	public Osp::Ui::IActionEventListener,
	public Osp::Base::Runtime::ITimerEventListener,
	public Osp::Media::IPlayerEventListener
{

// Construction
public:
	FMainForm(void);
	virtual ~FMainForm(void);
	bool Initialize(void);

// Implementation
protected:
	static const int APLAY = 1;
	static const int APAUSE = 2;
	static const int APREV = 3;
	static const int ANEXT = 4;
	static const int ASTOP = 5;
	static const int APLAYLIST = 6;
	static const int ABROWSE = 7;
	static const int AMORE = 8;
	static const int ALESS = 9;
	static const int ACLIENTS = 10;
	static const int AFULLSCREEN = 11;
	static const int ASTREAM = 13;
	static const int AHELP = 14;
	static const int ASTREAMON = 15;
	static const int ASTREAMOFF = 16;
	static const int ASTREAMFULLSCREEN = 17;
	static const int APLAYSTREAMFROMBG = 18;

	static const int BIGROUNDBTN = 1;
	static const int SMALLROUNDREDBTN = 2;
	static const int SMALLROUNDSKYBTN = 3;
	static const int BOXBTN = 4;

	static const int MOREMENI = 1;

	bool moremeniopened;

	DrawingClass * drawingclass_;

	Osp::Graphics::Canvas * displaybuffer_;
	bool displaybufferisindrawingstate;
	Osp::Graphics::Rectangle canvasredrawrect_;

	Osp::Base::Collection::IList* buttons_;
	int currpressedbtn;
	int currselslider;
	double curslidervolumevalue;
	double cursliderseekvalue;
	Osp::Graphics::Rectangle moremenipos;
	Osp::Graphics::Point oldpresspos;

	PublicFunctions * publicfunc_;

	bool streamingplaywait;
	bool streamingclearplaylistwait;
	bool streamingpausewait;
	bool streamingcurmediaaudio;
	bool isfullscreenplayer;
	Osp::Base::String streaminguri;

	Osp::Media::Player *streamplayer;
	int streamplayervol;
	Osp::Ui::Controls::OverlayPanel *streamplayerpreviewoverlay;
	Osp::Ui::Controls::Label *streamplayerbufferoverlay;
	Osp::Ui::Controls::Label *streamplayervolumeoverlay;
	Osp::Ui::Controls::Label *streamplayertitleoverlay;
	Osp::Base::Runtime::Timer *StartToPlayTimer_;
	Osp::Base::Runtime::Timer *FullScreenStreamPlayerHideControls_;
	bool streamingendoffile;

	bool isonbackground;

	int vlmstreamcom;
	Osp::Base::String vlmstreamstr;

public:
	Osp::Base::Collection::ArrayList * playliststream;
	int streamingcurrentitemindex;
	void Background();
	void Foreground();
	bool isstreaming;
	bool StreamingBrowseAddMedia(Osp::Base::String uri, bool startplay);
	void StreamingPlayListClear();
	void StreamingPlayListRemoveItem(int itemindex);
	void StreamingPlayListPlayItem(int itemindex);
	void OnTimerExpired(Osp::Base::Runtime::Timer& timer);
	void StreamPlayItemIndex(int itemindex);
	void RedrawDisplayBuffer(Osp::Graphics::Rectangle redrawrect);
	void RefreshDataUpdate();
	virtual result OnDraw();
	virtual result OnInitializing(void);
	virtual result OnTerminating(void);
	virtual void OnActionPerformed(const Osp::Ui::Control& source, int actionId);
	virtual void OnTouchDoublePressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusIn (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchFocusOut (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchLongPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchMoved (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchPressed (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	virtual void OnTouchReleased (const Osp::Ui::Control &source, const Osp::Graphics::Point &currentPosition, const Osp::Ui::TouchEventInfo &touchInfo);
	void OnKeyPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyReleased (const Control &source, Osp::Ui::KeyCode  keyCode);
	void OnKeyLongPressed (const Control &source, Osp::Ui::KeyCode  keyCode);
	virtual void OnUserEventReceivedN(RequestId requestId, Osp::Base::Collection::IList* pArgs);
	virtual void OnPlayerOpened(result r);
	virtual void OnPlayerEndOfClip(void);
	virtual void OnPlayerBuffering(int percent);
	virtual void OnPlayerErrorOccurred(Osp::Media::PlayerErrorReason r );
	virtual void OnPlayerInterrupted();
	virtual void OnPlayerReleased();
	virtual void OnPlayerSeekCompleted(result r);
};

#endif	//_FMAINFORM_H_
