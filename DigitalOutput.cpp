
#include "DigitalOutput.h"
#include "Packet.h"
#include <stdlib.h>


DigitalOutput::DigitalOutput(
        uint32_t channel
        ) :
    Component(),
    myChannel(channel),
    myCurrentPacket(NULL)
{
}

DigitalOutput::~DigitalOutput()
{
}

void
DigitalOutput::Set(
        uint32_t value
        )
{
    if (myCurrentPacket != NULL)
    {
        delete myCurrentPacket;
    }

    myCurrentPacket = new DigitalOutputPacket(
            myChannel,
            ((value == 0) ? false : true)
            );
}

Packet*
DigitalOutput::getNextPacket()
{
    Packet* packet = myCurrentPacket;
    myCurrentPacket = NULL;

    return packet;
}

bool
DigitalOutput::processPacket(
        const Packet& packet
        )
{
    return false;
}
