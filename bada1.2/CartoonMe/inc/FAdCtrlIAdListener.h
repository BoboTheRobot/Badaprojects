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
 * @file	FAdCtrlIAdListener.h
 * @brief	This is the header file for the %IAdListener interface.
 *
 * This file contains the declarations of the %IAdListener interface.
 */


#ifndef _FADCTRL_IADLISTENER_H_
#define _FADCTRL_IADLISTENER_H_

#include "FBaseRt.h"
#include "FAdCtrlConfig.h"

namespace Osp { namespace Ads { namespace Controls { class Ad; } } }
namespace Osp { namespace Ads { namespace Controls {

/**
 * @interface	IAdListener
 * @brief		This is an interface for receiving events that occur during the ad update.
 * @since		2.0
 *
 * The %IAdListener interface handles events that occur during the ad update.
 */
class _EXPORT_ADCTRL_ IAdListener :
	public Osp::Base::Runtime::IEventListener
{

// Operations
public:
	/**
	 * This is the destructor for this interface.
	 *
	 * @since 2.0
	 */
	virtual ~IAdListener(void){}

public:
	/**
	 * Called when Ad::RequestAd() has been executed.
	 *
	 * @since		2.0
	 * @param[in]	reqId					The request ID
	 * @param[in]	source					The event source
	 * @param[in]	r						The result for the request
	 * @param[in]	errorCode				The error code
	 * @param[in]	errorMsg				The error message related to the error code
	 * @exception	E_SUCCESS				The method is successful.
	 * @exception	E_CONNECTION_FAILED		The network connection has failed.
	 * @exception	E_SERVICE_UNAVAILABLE	The server is unavailable or returns an abnormal response.
	 * @exception	E_SERVER				An error has occurred at the server. @n
	 *										The details can be obtained by checking @c errorCode and @c errorMsg.
	 */
	virtual void OnAdReceived(RequestId reqId, const Osp::Ads::Controls::Ad& source, result r, const Osp::Base::String& errorCode, const Osp::Base::String& errorMsg) = 0;

protected:
	/**
	 * @internal	This method is for internal use only. The bada platform team is not
	 *				responsible for any behavioral correctness, consistency, and
	 *				security-related issues that might arise after using this method.
	 *
	 * Following method is reserved and may change its name at any time without prior notice.
	 *
	 * @since		2.0
	 */
	virtual void IAdListener_Reserved1 (void) { }

	/**
	 * @internal	This method is for internal use only. The bada platform team is not
	 *				responsible for any behavioral correctness, consistency, and
	 *				security-related issues that might arise after using this method.
	 *
	 * Following method is reserved and may change its name at any time without prior notice.
	 *
	 * @since		2.0
	 */
	virtual void IAdListener_Reserved2 (void) { }

	/**
	 * @internal	This method is for internal use only. The bada platform team is not
	 *				responsible for any behavioral correctness, consistency, and
	 *				security-related issues that might arise after using this method.
	 *
	 * Following method is reserved and may change its name at any time without prior notice.
	 *
	 * @since		2.0
	 */
	virtual void IAdListener_Reserved3 (void) { }
};
};};};
#endif	// _FADCTRL_IADLISTENER_H_
