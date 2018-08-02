/* 
    Simple example of sending an OSC message using oscpack.
*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <iostream>
#include "../../include/oscpack/osc/OscOutboundPacketStream.h"
#include "../../include/oscpack/ip/UdpSocket.h"

extern "C"
{
#include "emokit/emokit.h"
}


#define ADDRESS "127.0.0.1"
#define PORT 9997

#define OUTPUT_BUFFER_SIZE 4096
 
int main(int argc, char **argv)
{
 
	emokit_device* d;
	d = emokit_create();

	std::cout << "Current epoc devices connected " << emokit_get_count(d, EMOKIT_VID, EMOKIT_PID) << "\n";
	if(emokit_open(d, EMOKIT_VID, EMOKIT_PID, 1) != 0)
	{
		std::cout << "CANNOT CONNECT\n";
		return 1;
	}

	UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );    
	char buffer[OUTPUT_BUFFER_SIZE];
	char raw_frame[32];
	struct emokit_frame frame;
	
	std::cout << "Connected\n";
	while(1)
	{
		//std::cout << "Starting read\n";
		if(emokit_read_data(d) > 0)
		{
			struct emokit_frame c;
			c = emokit_get_next_frame(d);
			
			osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
			p << osc::BeginMessage( "/emokit/channels" )
			  << c.F3 << c.FC6 << c.P7 << c.T8 << c.F7 << c.F8 
			  << c.T7 << c.P8 << c.AF4 << c.F4 << c.AF3 << c.O2 
			  << c.O1 << c.FC5 
			  << osc::EndMessage;
			transmitSocket.Send( p.Data(), p.Size() );
			
			osc::OutboundPacketStream q( buffer, OUTPUT_BUFFER_SIZE );
			q << osc::BeginMessage( "/emokit/gyro" ) 
			  << (int)frame.gyroX << (int)frame.gyroY << osc::EndMessage;
			transmitSocket.Send( q.Data(), q.Size() );

			osc::OutboundPacketStream info( buffer, OUTPUT_BUFFER_SIZE );
			info << osc::BeginMessage( "/emokit/info" )
				<< (int)c.battery
				<< c.cq.F3 << c.cq.FC6 << c.cq.P7 << c.cq.T8 << c.cq.F7 << c.cq.F8
				<< c.cq.T7 << c.cq.P8 << c.cq.AF4 << c.cq.F4 << c.cq.AF3 << c.cq.O2 
				<< c.cq.O1 << c.cq.FC5
				<< osc::EndMessage;
			transmitSocket.Send( info.Data(), info.Size() );
			
		}
	}

	fflush(stdout);
	emokit_close(d);
	emokit_delete(d);
	return 0;
}
