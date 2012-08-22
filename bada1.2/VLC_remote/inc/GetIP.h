/*
 * GetIP.h
 *
 *  Created on: 3.4.2011
 *      Author: mrak
 */

#ifndef GETIP_H_
#define GETIP_H_

#include <FApp.h>
#include <FBase.h>
#include <FNet.h>

class GetIP
	:public Osp::Net::INetConnectionEventListener
 {
public:
	GetIP();
	virtual ~GetIP();

private:
	Osp::Net::NetConnection* __pNetConnection;

public:
	Osp::Base::String localip;
	virtual void  OnNetConnectionResumed (Osp::Net::NetConnection &netConnection);
	virtual void  OnNetConnectionStarted (Osp::Net::NetConnection &netConnection, result r);
	virtual void  OnNetConnectionStopped (Osp::Net::NetConnection &netConnection, result r);
	virtual void  OnNetConnectionSuspended (Osp::Net::NetConnection &netConnection);
};

#endif /* GETIP_H_ */
