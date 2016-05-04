#include "SocketClient.h"
#include <MSWSock.h>

char *szOperation[] = 
{
	"ACCEPT_POSTED",
	"SEND_POSTED",
	"RECV_POSTED",
	"CONNECT_POSTED",
	"DISCONNECT_POSTED",
};
LPFN_CONNECTEX		CSocketClient::m_pfnConnectEx = NULL;