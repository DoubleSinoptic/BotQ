#include "LocalHostCaster.h"

void LocalHostCaster::bind_to_uncast()
{
	for (int i = 0; i < 5; i++)
	{
		try
		{
			ts::ip_end_point sr(ts::ip_address_any, 53531 + i);
			sock.bind(sr);
			return;
		}
		catch (std::exception&)
		{
		}
	}

}

LocalHostCaster::LocalHostCaster() :
	sock(ts::protocol_type::udp)
{
	sock.set_noblocking(true);
	sock.set_broadcast(true);
}

void LocalHostCaster::Broadcast(int port, const String & name, int lobyMax, int lobyAver)
{
	CasterHeaderData hfr;
	hfr.loby_aver = lobyAver;
	hfr.port = port;
	name.CopyTo((char*)hfr.server_name);
	hfr.loby_total = lobyMax;
	for (int i = 0; i < 5; i++) {
		ts::ip_end_point point(ts::ip_address_broadcast, 53531 + i);
		sock.send_to_some((const void*)&hfr, sizeof(hfr), point);
	}
}

void LocalHostCaster::Clear()
{
	pairs.Clear();
}

bool LocalHostCaster::Uncast(ts::ip_address & addr, CasterHeaderData & f)
{
	if (!binded)
	{
		binded = true;
		bind_to_uncast();
	}
	ts::ip_end_point p;
	CasterHeaderData hdr;

	int code = sock.receive_from_some((void*)&hdr, sizeof(hdr), p);
	if (code != sizeof(hdr))
		return false;
	if (pairs.IndexOf(p) == -1)
	{
		pairs.Add(p);
		f = hdr;
		addr = p.get_v4_address();
		return true;
	}
	return false;
}
