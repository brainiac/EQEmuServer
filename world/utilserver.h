#ifndef UtilServer_H__
#define UtilServer_H__

#include "../common/types.h"
#include "../common/EmuTCPConnection.h"
#include "../common/servertalk.h"

class UtilServerConnection
{
public:
	UtilServerConnection();

	void SetConnection(EmuTCPConnection* inStream);
	bool Process();
	bool SendPacket(ServerPacket* pack);
	void Disconnect() { if(Stream) Stream->Disconnect(); }

private:
	inline uint32 GetIP() const { return Stream ? Stream->GetrIP() : 0; }
	EmuTCPConnection* Stream;
	bool authenticated;
};

#endif // UtilServer_H__