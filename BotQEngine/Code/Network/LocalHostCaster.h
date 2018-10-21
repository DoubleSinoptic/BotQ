#pragma once

#include "../Core.h"
#include "tinysocket.h"

class SGE_EXPORT LocalHostCaster
{
	ts::socket sock;
	bool binded = false;
	void bind_to_uncast();
	DynamicArray<ts::ip_end_point> pairs;
public:
	struct CasterHeaderData
	{
		uint16_t port;
		uint32_t loby_total;
		uint32_t loby_aver;
		int8_t server_name[100];
	};

	LocalHostCaster();

	void Broadcast(int port, const String& name, int lobyMax, int lobyAver);

	void Clear();

	bool Uncast(ts::ip_address& addr, CasterHeaderData& f);
};