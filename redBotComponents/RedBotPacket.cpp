
#include "RedBotPacket.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "RedBotSpeedController.h"
#include "RedBotEncoder.h"
#include <sstream>
#include <vector>
#include <math.h>


RedBotPacket::RedBotPacket(
        Type        type,
        const char* typeName,
        BinaryID    binID
        ) :
    myType(type),
    myTypeName(typeName),
    myBinaryID(binID)
{
}

void
RedBotPacket::write(
        std::ostream& outputStream
        ) const
{
    outputStream
        << BINARY_BOUND
        << (unsigned char)myBinaryID;

    writeContents(outputStream);

    outputStream << BINARY_BOUND;
}

void
RedBotPacket::writeXML(
        std::ostream& outputStream
        ) const
{
    XMLElements elements;
    getXMLElements(elements);

    outputStream
        << "<packet>"
        << "<type>" << myTypeName << "</type>"
        << elements
        << "</packet>";
}

RedBotPacket::Type
RedBotPacket::getType() const
{
    return myType;
}

bool
RedBotPacket::isAcknowledge() const
{
    return (getType() == TYPE_ACK);
}

bool
RedBotPacket::operator!=(
        const Packet&   packet
        ) const
{
    return !(*this == packet);
}

RedBotPacket::operator std::string() const
{
    std::ostringstream stringStream;
    write(stringStream);
    return stringStream.str();
}

Packet*
RedBotPacketGenerator::createPacket(
        unsigned char type
        )
{
    switch (type)
    {
        case RedBotPacket::BID_PING:          return createPingPacket(); break;
        case RedBotPacket::BID_PINCONFIG:     return new PinConfigPacket(); break;
        case RedBotPacket::BID_PINCONFIGINFO: return new PinConfigInfoPacket(); break;
        case RedBotPacket::BID_DINPUT:        return new DigitalInputPacket(); break;
        case RedBotPacket::BID_DOUTPUT:       return new DigitalOutputPacket(); break;
        case RedBotPacket::BID_DVALUE:        return new DigitalValuePacket(); break;
        case RedBotPacket::BID_AINPUT:        return new AnalogInputPacket(); break;
        case RedBotPacket::BID_AVALUE:        return new AnalogValuePacket(); break;
        case RedBotPacket::BID_MDRIVE:        return new MotorDrivePacket(); break;
        case RedBotPacket::BID_ENCINPUT:      return new EncoderInputPacket(); break;
        case RedBotPacket::BID_ENCCOUNT:      return new EncoderCountPacket(); break;
        case RedBotPacket::BID_ENCCLEAR:      return new EncoderClearPacket(); break;
        case RedBotPacket::BID_ACK:           return new AcknowledgePacket(); break;
        default: return NULL; break;
    };

    return NULL;
}

Packet*
RedBotPacketGenerator::createPingPacket()
{
    return new PingPacket();
}


PingPacket::PingPacket() :
    RedBotPacket(TYPE_PING, "PING", BID_PING),
    myIsValid(true)
{
}

void
PingPacket::writeContents(
        std::ostream& outputStream
        ) const
{
}

void
PingPacket::getXMLElements(
        XMLElements& elements
        ) const
{
}

void
PingPacket::read(
        std::istream& inputStream
        )
{
    unsigned char trailer;

    inputStream >> trailer;
    if (trailer == 0xFF)
    {
        myIsValid = true;
    }
    else
    {
        myIsValid = false;
    }
}

bool
PingPacket::isValid() const
{
    return myIsValid;
}

bool
PingPacket::operator==(
        const Packet&   packet
        ) const
{
    if (NULL != dynamic_cast<const PingPacket*>(&packet))
    {
        return true;
    }
    else
    {
        return false;
    }
}


AcknowledgePacket::AcknowledgePacket() :
    RedBotPacket(TYPE_ACK, "ACK", BID_ACK),
    myIsValid(true)
{
}

void
AcknowledgePacket::writeContents(
        std::ostream& outputStream
        ) const
{
}

void
AcknowledgePacket::getXMLElements(
        XMLElements& elements
        ) const
{
}

void
AcknowledgePacket::read(
        std::istream& inputStream
        )
{
    unsigned char trailer;

    inputStream >> trailer;
    if (trailer == 0xFF)
    {
        myIsValid = true;
    }
    else
    {
        myIsValid = false;
    }
}

bool
AcknowledgePacket::isValid() const
{
    return myIsValid;
}

bool
AcknowledgePacket::operator==(
        const Packet& packet
        ) const
{
    if (packet.isAcknowledge() == true)
    {
        return true;
    }
    else
    {
        return false;
    }
}

