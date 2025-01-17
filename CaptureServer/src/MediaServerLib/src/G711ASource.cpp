#include "G711ASource.h"
#include "RtpConnection.h"
#include <cstdio>
#include <chrono>
#if defined(__linux) || defined(__linux__) 
#include <sys/time.h>
#endif 

using namespace xop;
using namespace std;

G711ASource::G711ASource()
{
	_payload = 8;
	_mediaType = PCMA;
	_clockRate = 8000;
}

G711ASource* G711ASource::createNew()
{
	return new G711ASource();
}

G711ASource::~G711ASource()
{
	
}

string G711ASource::getMediaDescription(uint16_t port)
{
	char buf[100] = {0};
	sprintf(buf, "m=audio %hu RTP/AVP 8", port);
	
	return string(buf);
}
	
string G711ASource::getAttribute()
{
	return string("a=rtpmap:8 PCMA/8000/1");
}

bool G711ASource::handleFrame(MediaChannelId channelId, AVFrame& frame)
{
	if (frame.size > MAX_RTP_PAYLOAD_SIZE)
	{
		return false;
	}

	char *frameBuf  = frame.buffer.get();
	uint32_t frameSize = frame.size;
	
	RtpPacketPtr rtpPkt(new char[1500]);
    memcpy(rtpPkt.get()+4+RTP_HEADER_SIZE, frameBuf, frameSize);
	
	if(_sendFrameCallback)
		_sendFrameCallback(channelId, rtpPkt, frameSize+4+RTP_HEADER_SIZE, 1, frame.timestamp);
	
	return true;
}

uint32_t G711ASource::getTimeStamp()
{
	auto timePoint = chrono::time_point_cast<chrono::milliseconds>(chrono::high_resolution_clock::now());
	return (uint32_t)(timePoint.time_since_epoch().count()*8);
}

