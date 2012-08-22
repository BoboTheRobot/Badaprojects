//
// Copyright (c) 2011 Samsung Electronics Co., Ltd.
// All rights reserved.
 
// This software contains confidential and proprietary information 
// of Samsung Electronics Co. Ltd.
// The user of this software agrees not to disclose, disseminate or copy such 
// Confidential Information and shall use the software only in accordance with 
// the terms of the license agreement the user entered into with Samsung.
//
/**
 * @file	FAdCtrlAd.h
 * @brief	This is the header file for the %Ad class.
 *
 * This file contains the declarations of the %Ad class, which is used to embed the advertisement functionality in an application.
 */


#ifndef _FADCTRL_AD_H_
#define _FADCTRL_AD_H_

#include "FAdCtrlIAdListener.h"
#include "FUi.h"

struct MRestClient;

namespace Osp { namespace Ads { namespace Controls {

/**
 * @class	Ad
 * @brief	This class provides methods to embed ads in an application.
 * @since	2.0
 *
 * This class provides methods to embed ads in an application.
 * An ad can be embedded in an application by adding the %Ad control in almost the same way as any other
 * UI control (Ui::Controls).
 * 
 * For more information on the class features, see <a href="../com.osp.cppprogramming.help/html/dev_guide/ads/ads_namespace.htm">Ads Guide</a>.
 *
 * 
 * @see		Osp::Ads::Controls::IAdListener
 *
 * The following example demonstrates how to use the %Ad class.
 *
 * @code
 * /////////////////////////////////////
 * //AdExample.h
 *
 * #include <FAds.h>
 *
 * class AdExample : public Osp::Ads::Controls::IAdListener
 * {
 * public:
 *	result CreateControl(Osp::Ui::Controls::Form* pParentForm);
 *	void RefreshAd(void);
 *	void OnAdReceived(RequestId reqId, const Osp::Ads::Controls::Ad& source, result r, const Osp::Base::String& errorCode, const Osp::Base::String& errorMsg);
 *
 * private:
 *	Osp::Ads::Controls::Ad* __pAd;
 * };
 *
 * /////////////////////////////////////
 * //AdExample.cpp
 *
 * #include "AdExample.h"
 *
 * using namespace Osp::Ads::Controls;
 * using namespace Osp::Base;
 * using namespace Osp::Base::Collection;
 * using namespace Osp::Ui::Controls;
 *
 *
 * // Add Ad control to parent container
 * result
 * AdExample::CreateControl(Form* pParentForm)
 * {
 *	result r = E_SUCCESS;
 *
 *	__pAd = new Ad();
 *	r = __pAd->Construct(Osp::Graphics::Rectangle(0, 0, 480, 80), L"2011000001_001", this);
 *	__pAd->SetTextColor(Osp::Graphics::Color(255, 255, 255));
 *	__pAd->SetBackgroundColor(Osp::Graphics::Color(0, 0, 0));
 *
 *	// Set keywords
 *	ArrayList* pKeywordsList = new ArrayList;
 *	pKeywordsList->Construct();
 *	pKeywordsList->Add(*(new String(L"bada")));
 *	pKeywordsList->Add(*(new String(L"Samsung")));
 *	__pAd->SetKeywords(pKeywordsList);
 *	delete pKeywordsList;
 *
 *	// Set the extra information
 *	HashMap* pExtraInfoList = new HashMap;
 *	pExtraInfoList->Construct();
 *	pExtraInfoList->Add(*(new String(L"gender")), *(new String(L"male")));
 *	pExtraInfoList->Add(*(new String(L"age")), *(new String(L"30")));
 *	pExtraInfoList->Add(*(new String(L"interests")), *(new String(L"car,soccer")));
 *	pExtraInfoList->Add(*(new String(L"location")), *(new String(L"37.5,127.5,10.5")));
 *	__pAd->SetExtraInfo(pExtraInfoList);
 *	delete pExtraInfoList;
 *
 *	r = pParentForm->AddControl(*__pAd);
 *
 *	return r;
 * }
 *
 * // Update Ad control with a new ad
 * void
 * AdExample::RefreshAd(void)
 * {
 *	RequestId reqId;
 *	__pAd->RequestAd(reqId);
 * }
 *
 * void
 * AdExample::OnAdReceived(RequestId reqId, const Osp::Ads::Controls::Ad& source, result r, const Osp::Base::String& errorCode, const Osp::Base::String& errorMsg)
 * {
 *	AppLog("[Received Ad] (reqId=%d, r=%s, errorCode=%S, errorMsg=%S)", reqId, GetErrorMessage(r), errorCode.GetPointer(), errorMsg.GetPointer());
 *	// Do something
 * }
 *
 *	@endcode
 *
 */

class _EXPORT_ADCTRL_ Ad :
	public Osp::Ui::Container
{
	DECL_CLASSTYPE(Ad);
// Lifecycle
public:

	/**
	 * This is the default constructor for this class.
	 *
	 * @since		2.0
	 * @remarks		After creating an instance of this class, the Construct() method must be called explicitly to initialize this instance.
	 * @see			Construct()
	 */
	Ad(void);

	/**
	 * This is the destructor for this class.
	 *
	 * @since		2.0
	 */
	virtual ~Ad(void);

	/**
	 * Initializes this instance of %Ad with the specified parameters.
	 *
	 * @since		2.0
	 * @privlevel	NORMAL
	 * @privgroup	AD_SERVICE
	 *
	 * @return		An error code
	 * @param[in]	rect				The location and size of the %Ad control as a Rectangle instance @n
	 *									The minimum size of this control is 480 x 80 on a WVGA screen, @n
	 *									or 240 x 40 on a WQVGA screen.
	 * @param[in]	inventoryId			The inventory ID of the %Ad control
	 * @param[in]	pListener			The listener for the ad notifications
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @exception	E_INVALID_STATE		This instance has already been constructed.
	 * @exception	E_INVALID_ARG		The specified input parameter is invalid.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 * @exception	E_PRIVILEGE_DENIED	The application does not have the privilege to call this method.
	 *
	 * @remarks		The @c inventoryId parameter contains the string value that uniquely identifies the Ad control. @n
	 *				You must first sign up on the bada developer site and configure the application's ad inventory settings in order to get this ID.
	 */
	result Construct(const Osp::Graphics::Rectangle& rect, const Osp::Base::String& inventoryId, const Osp::Ads::Controls::IAdListener* pListener = null);

public:

	/**
	 * Requests an ad update for this control.
	 *
	 * @since		2.0
	 * @privlevel	NORMAL
	 * @privgroup	AD_SERVICE
	 *
	 * @return		An error code
	 * @param[out]	reqId				The request ID
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_OPERATION_FAILED	The previous ad request or ad click event is not finished. @n
	 *									The interval between the ad requests is less than the minimum interval specified by the platform.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 * @exception	E_PRIVILEGE_DENIED	The application does not have the privilege to call this method.
	 *
	 * @remarks		After calling this method, the result can be checked using the IAdListener::OnAdReceived() method.
	 * @remarks		The subsequent ad request is unavailable for the minimum interval.
	 * @see			IAdListener::OnAdReceived(), GetMinAdRequestInterval()
	 */
	result RequestAd(RequestId& reqId);

	/**
	 * Sets the default image for the portrait mode when there is no ad available.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	imagePath			The local file path of the portrait image source
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_DATA		The specified input instance has invalid data.
	 * @exception	E_FILE_NOT_FOUND	The specified file cannot be found or accessed.
	 * @exception	E_ILLEGAL_ACCESS	The specified image is protected with DRM.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 * @exception	E_SYSTEM			A system error has occurred.
	 *
	 * @remarks		The image should perfectly fit into the Ad control, otherwise the image is automatically resized to the Ad control's size.
	 * @see			SetDefaultLandscapeImage()
	 */
	result SetDefaultPortraitImage(const Osp::Base::String& imagePath);

	/**
	 * Sets the default image for the landscape mode when there is no ad available.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	imagePath			The local file path of the landscape image source
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_DATA		The specified input instance has invalid data.
	 * @exception	E_FILE_NOT_FOUND	The specified file cannot be found or accessed.
	 * @exception	E_ILLEGAL_ACCESS	The specified image is protected with DRM.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 * @exception	E_SYSTEM			A system error has occurred.
	 *
	 * @remarks		The image should perfectly fit into the Ad control, otherwise the image is automatically resized to the Ad control's size.
	 * @see			SetDefaultPortraitImage()
	 */
	result SetDefaultLandscapeImage(const Osp::Base::String& imagePath);

	/**
	 * Gets the background color of this control.
	 *
	 * @since		2.0
	 * @return		The background color
	 * @see			SetBackgroundColor()
	 */
	Osp::Graphics::Color GetBackgroundColor(void) const;

	/**
	 * Sets the background color of this control.
	 *
	 * @since		2.0
	 * @param[in]	color		The background color
	 *
	 * @remarks		Setting the alpha value for the Color instance is not recommended.
	 * @see			GetBackgroundColor()
	 */
	void SetBackgroundColor(const Osp::Graphics::Color& color);

	/**
	 * Gets the text color of this control.
	 *
	 * @since		2.0
	 * @return		The text color
	 * @see			SetTextColor()
	 */
	Osp::Graphics::Color GetTextColor(void) const;

	/**
	 * Sets the text color of this control.
	 *
	 * @since		2.0
	 * @param[in]	color		The text color
	 *
	 * @remarks		Setting the alpha value for the Color instance is not recommended.
	 * @see			GetTextColor()
	 */
	void SetTextColor(const Osp::Graphics::Color& color);

	/**
	 * Sets the size of this control.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	width				The new width
	 * @param[in]	height				The new height
	 * @exception	E_SUCCESS 			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The specified input parameter is invalid.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @see			@ref Osp::Ui::Control::SetSize()
	 */
	result SetSize(int width, int height);

	/**
	 * Sets the size of this control.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	size 				The new width and height
	 * @exception	E_SUCCESS 			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The specified input parameter is invalid.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @see			@ref Osp::Ui::Control::SetSize()
	 */
	result SetSize(const Osp::Graphics::Dimension &  size);

	/**
	 * Sets the position and size of this control.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	x					The new X coordinate
	 * @param[in]	y					The new Y coordinate
	 * @param[in]	width				The new width
	 * @param[in]	height				The new height
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The specified input parameter is invalid.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @see			SetSize()
	 * @see			@ref Osp::Ui::Control::SetBounds()
	 */
	result SetBounds(int x, int y, int width, int height);

	/**
	 * Sets the position and size of this control.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	rect				The new bounds of this control
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The specified input parameter is invalid.
	 * @exception	E_SYSTEM			A system error has occurred.
	 * @see			SetSize()
	 * @see			@ref Osp::Ui::Control::SetBounds()
	 */
	result SetBounds(const Osp::Graphics::Rectangle& rect);

	/**
	 * Sets the keywords that describe the specific ads.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	pKeywords			A list of keywords of type String @n
	 * 									The parameter can contain @c null if the list is to be removed.
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The @c pKeywords parameter contains an element of an incompatible class type. @n
	 *									The @c pKeywords parameter contains more than four strings. @n
	 *									A keyword contains more than 20 characters.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 *
	 * @remarks		Only String objects are allowed for the elements of the keywords list.
	 *				The RequestAd() method does not send the keywords list to the server if any of its elements has an incompatible class type.
	 * @see			GetKeywordsN()
	 */
	result SetKeywords(const Osp::Base::Collection::IList* pKeywords);

	/**
	 * Gets the keywords list that describes the specific ads.
	 *
	 * @since		2.0
	 * @return		A pointer to an IList that contains the String objects, @n
	 * 				else @c null if an exception occurs or if there is no keyword specified by the application
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 *
	 * @remarks		The specific error code can be accessed using the GetLastResult() method.
	 * @see			SetKeywords()
	 */
	Osp::Base::Collection::IList* GetKeywordsN(void) const;

	/**
	 * Sets the extra information for the requesting ads.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @param[in]	pExtraInfo			The <a href="../com.osp.cppprogramming.help/html/dev_guide/ads/ad_targeting.htm">hash map of the additional information</a> for the requesting ads @n
	 *									The parameter can contain @c null if the hash map is to be removed.
	 *
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_INVALID_ARG		The @c pExtraInfo parameter contains an element of an incompatible class type.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 *
	 * @remarks		Only String objects are allowed for the <key, value> pair of the hash map.
	 *				The RequestAd() method does not send the extra information list to the server if any of its elements has an incompatible class type.
	 * @see			GetExtraInfoN()
	 */
	result SetExtraInfo(const Osp::Base::Collection::IMap* pExtraInfo);

	/**
	 * Gets the <a href="../com.osp.cppprogramming.help/html/dev_guide/ads/ad_targeting.htm">hash map of the additional information</a> for the requesting ads.
	 *
	 * @since		2.0
	 * @return		A pointer to IMap, @n
	 *				else @c null if an exception occurs or if there is no extra information specified by the application
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_INVALID_STATE		This instance has not been constructed as yet.
	 * @exception	E_OUT_OF_MEMORY		The memory is insufficient.
	 *
	 * @remarks		The specific error code can be accessed using the GetLastResult() method.
	 * @see			SetExtraInfo()
	 */
    Osp::Base::Collection::IMap* GetExtraInfoN(void) const;

	/**
	 * Gets the minimum ad request interval.
	 *
	 * @since		2.0
	 * @return		The minimum interval in seconds
	 * @see			RequestAd()
	 */
	static int GetMinAdRequestInterval(void);

protected:
	/**
	 * @internal
	 * This is internal. If used in an application, the application can get rejected during the certification process.
	 * Frees the resources allocated by this control.
	 *
	 * @since		2.0
	 * @return		An error code
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_SYSTEM			The method has failed to free the resource possessed by this instance.
	 */
	virtual result Dispose(void);

	/**
	 * @internal
	 * This is internal. If used in an application, the application can get rejected during the certification process.
	 * Called when this control needs to draw itself..
	 *
	 * @since		2.0
	 * @return		An error code
	 * @exception	E_SUCCESS			The method is successful.
	 * @exception	E_SYSTEM			A system error has occurred.
	 */
	virtual result OnDraw(void);

private:
	result __LoadResources(void);
	void __Realign(void);

private:
	bool										__waiting;
	bool										__loading;
	bool										__impression;
	int											__screenMagnification;
	int											__state;
	long long									__requestTime;

	Osp::Base::String							__inventoryId;
	int											__inventoryHeight;
	int											__inventoryWidth;
	Osp::Base::Collection::ArrayList*			__pKeywordsList;
	Osp::Base::Collection::HashMap*				__pExtraInfoList;

	Osp::Base::String							__bannerReqID;
	int											__action;

	int											__interval;
	Osp::Base::Runtime::Timer*					__pTimer;
	Osp::Base::Runtime::ITimerEventListener*	__pTimerListener;

	Osp::Base::Collection::ArrayList*			__pAnimationFrameList;
	Osp::Ui::IAnimationEventListener*			__pAnimationListener;
	Osp::Ui::ITouchEventListener*				__pTouchListener;
	const Osp::Ads::Controls::IAdListener*		__pIAdListener;
	MRestClient*								__pRestClient;
	void*										__pMAdInfo;

	Osp::Ui::Controls::Label*					__pBanner;
	Osp::Ui::Controls::Label*					__pButton;
	Osp::Ui::Controls::Label*					__pBannerText;
	Osp::Ui::Controls::Label*					__pBannerError;
	Osp::Ui::Controls::Animation*				__pAnimation;

	Osp::Graphics::Bitmap*						__pBitmapWWW;
	Osp::Graphics::Bitmap*						__pBitmapWWWPressed;
	Osp::Graphics::Bitmap*						__pBitmapCall;
	Osp::Graphics::Bitmap*						__pBitmapCallPressed;
	Osp::Graphics::Bitmap*						__pBitmapStore;
	Osp::Graphics::Bitmap*						__pBitmapStorePressed;
	Osp::Graphics::Bitmap*						__pBitmapTextBGPortrait;
	Osp::Graphics::Bitmap*						__pBitmapTextBGLandscape;
	Osp::Graphics::Bitmap*						__pBitmapBannerErrorPortrait;
	Osp::Graphics::Bitmap*						__pBitmapBannerErrorLandscape;

private:
	friend class __AdTouchEventListener;
	friend class __AdCtrlAccessor;
	friend class __AdUtil;
	friend class __TimerListener;
	friend class __HttpRequestHelper;

private:
	friend class AdEx;
	class AdEx* __pAdEx;
	
protected:
	friend class AdImpl;

	/**
	 * @internal
	 * This is internal. If used in an application, the application can get rejected during the certification process.
	 * Gets the implementation instance of this control
	 *
	 * @since		2.0
	 */
	class AdImpl* GetImpl(void) const;
};

};};};
#endif


