#ifndef INPUT_H
#define INPUT_H

#include "RedBotComponent.h"
#include <stdint.h>
#include <stdlib.h>

/**
 * Generic input class
 *
 * This class encapsulates an data input source on the robot. It sends RequestType
 * packets to request the latest value detected on the source which are returned
 * via ResponseType packets. If the last-sent request was dropped for whatever
 * reason, another request will be automatically sent after a number of cycles
 * have passed without any response.
 */
template <class RequestType, class ResponseType, class ValueType>
class Input : public RedBotComponent
{
    public:

  Input();

        /**
         * Destructor
         */
        virtual ~Input();

        /**
         * Gets the current value from the input channel
         */
        ValueType Get() const;

    protected:

        /**
         * Provides the next available packet to send
         *
         * This function returns a new request packet to send if a response
         * from the robot was just received or if a number of cycles have
         * passed without any response since the last request.
         */
        RequestType* getNextPacketIfTimedOut();

        /**
         * Examines and processes a data packet if needed
         *
         * \return True if this packet contains signal value for this input,
         * false otherwise
         */
        bool processDataPacket(
                const Packet& packet
                );

  virtual RequestType* createRequest() = 0;

  virtual bool checkResponse(const ResponseType* packet);

    private:

        /**
         * Number of cycles to wait for reply packet before calling time-out
         */
        const static unsigned int TIMEOUT_THRESH = 5;

        /**
         * Last value read from pin
         */
        ValueType myValue;

        /**
         * Next packet to send to robot
         */
        RequestType* myOutgoingPacket;

        /**
         * Counter value used to detect time-outs for packet replies
         */
        unsigned int myTimeoutCounter;
};

/**
 * Pin input class
 */
template <class RequestType, class ResponseType, class ValueType>
class PinInput : public Input<RequestType, ResponseType, ValueType>
{
public:

  /**
   * Constructor given channel
   */
  PinInput(
	uint32_t channel
	);

  virtual ~PinInput(){}

protected:

  /**
   * Channel to read input signal from
   */
  const uint32_t myChannel;

private:

  RequestType* createRequest();

  bool checkResponse(const ResponseType* packet);
};

// Template implementations

template <class RequestType, class ResponseType, class ValueType>
Input<RequestType, ResponseType, ValueType>::Input() :
    RedBotComponent(),
    myValue(0),
    myOutgoingPacket(NULL),
    myTimeoutCounter(TIMEOUT_THRESH+1)
{
}

template <class RequestType, class ResponseType, class ValueType>
Input<RequestType, ResponseType, ValueType>::~Input()
{
    if (myOutgoingPacket != NULL)
    {
        delete myOutgoingPacket;
    }
}

template <class RequestType, class ResponseType, class ValueType>
ValueType
Input<RequestType, ResponseType, ValueType>::Get() const
{
    return myValue;
}

template <class RequestType, class ResponseType, class ValueType>
RequestType*
Input<RequestType, ResponseType, ValueType>::getNextPacketIfTimedOut()
{
    RequestType* packet = NULL;

    if (myOutgoingPacket == NULL)
    {
        if (myTimeoutCounter > TIMEOUT_THRESH)
        {
	  myOutgoingPacket = createRequest();
            myTimeoutCounter = 0;
        }
        else
        {
            ++myTimeoutCounter;
        }
    }

    packet = myOutgoingPacket;
    myOutgoingPacket = NULL;

    return packet;
}

template <class RequestType, class ResponseType, class ValueType>
bool
Input<RequestType, ResponseType, ValueType>::processDataPacket(
        const Packet& packet
        )
{
    const ResponseType* responsePacket = dynamic_cast<const ResponseType*>(&packet);
    if (responsePacket == NULL)
    {
        return false;
    }

    if (checkResponse(responsePacket) == false)
    {
        return false;
    }

    myValue = responsePacket->getValue();
    myTimeoutCounter = 0;

    if (myOutgoingPacket == NULL)
    {
      myOutgoingPacket = createRequest();
    }

    return true;
}

template <class RequestType, class ResponseType, class ValueType>
bool
Input<RequestType, ResponseType, ValueType>::checkResponse(const ResponseType* packet)
{
  return true;
}


template <class RequestType, class ResponseType, class ValueType>
PinInput<RequestType, ResponseType, ValueType>::PinInput(uint32_t channel) : 
  myChannel(channel)
{
}

template <class RequestType, class ResponseType, class ValueType>
RequestType*
PinInput<RequestType, ResponseType, ValueType>::createRequest()
{
  return new RequestType(myChannel);
}

template <class RequestType, class ResponseType, class ValueType>
bool
PinInput<RequestType, ResponseType, ValueType>::checkResponse(const ResponseType* packet)
{
  return (packet->getPin() == myChannel);
}

#endif /* ifndef INPUT_H */
