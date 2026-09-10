#include "Precompiled.h"
#include "NetworkElement.h"

using namespace VEngine;
using namespace VEngine::Network;

VEngine::Network::NetworkElement::NetworkElement(short prot) : mPort(port)
{

}

VEngine::Network::NetworkElement::~NetworkElement()
{
	StopNetWork();
}

void VEngine::Network::NetworkElement::Initialize(HWND handle, const char* serverAddress)
{
	return mInitialized;
}

void VEngine::Network::NetworkElement::Terminate()
{
}

void VEngine::Network::NetworkElement::ReceiveMsg()
{
}

void VEngine::Network::NetworkElement::SendMsg(const char* msg, int length)
{
}

bool VEngine::Network::NetworkElement::IsInitialized() const
{
	return false;
}

SOCKET VEngine::Network::NetworkElement::GetSocket() const
{
	return SOCKET();
}

unsigned short VEngine::Network::NetworkElement::GetPort() const
{
	return 0;
}

const char* VEngine::Network::NetworkElement::GetData() const
{
	return nullptr;
}

int VEngine::Network::NetworkElement::GetDataLength() const
{
	return 0;
}

int VEngine::Network::NetworkElement::GetLastError() const
{
	return 0;
}

void VEngine::Network::NetworkElement::ResetMsg()
{
}

bool VEngine::Network::NetworkElement::StartNetwork()
{
	return false;
}

void VEngine::Network::NetworkElement::StopNetWork()
{
}

bool VEngine::Network::NetworkElement::ConfigureSocketForMessages(HWND handle)
{
	return false;
}
