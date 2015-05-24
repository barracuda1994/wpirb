
#include "DigitalInput.h"
#include "Packet.h"
#include <stdlib.h>


DigitalInput::DigitalInput(
        uint32_t channel
        ) :
    Component(),
    myChannel(channel),
    myValue(false),
    myOutgoingPacket(new DigitalInputPacket(myChannel))
{
}

DigitalInput::~DigitalInput()
{
    if (myOutgoingPacket != NULL)
    {
        delete myOutgoingPacket;
    }
}

bool
DigitalInput::Get()
{
    return myValue;
}

Packet*
DigitalInput::getNextPacket()
{
    Packet* packet = myOutgoingPacket;
    myOutgoingPacket = NULL;

    return packet;
}

bool
DigitalInput::processPacket(
        const Packet& packet
        )
{
    const DigitalValuePacket& dValPacket = static_cast<const DigitalValuePacket&>(packet);
    myValue = dValPacket.getValue();

    if (myOutgoingPacket == NULL)
    {
        myOutgoingPacket = new DigitalInputPacket(myChannel);
    }

    return true;
}
