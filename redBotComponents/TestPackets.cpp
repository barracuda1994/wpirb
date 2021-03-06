
#include "RedBotPacket.h"
#include "ConfigurableInterface.h"
#include "DigitalInput.h"
#include "DigitalOutput.h"
#include "AnalogInput.h"
#include "RedBotSpeedController.h"
#include "RedBotEncoder.h"
#include "TestUtils.h"
#include <sstream>
#include <list>


TEST_GROUP(Packets)
{
    std::list<Packet*> myPackets;

    RedBotPacketGenerator myPacketGen;

    void teardown()
    {
        for(
                std::list<Packet*>::iterator packetIter = myPackets.begin();
                packetIter != myPackets.end();
                ++packetIter
           )
        {
            delete (*packetIter);
        }

        myPackets.clear();
    }

    Packet* readPacket(
            std::istream& inputStream
            )
    {
        return Packet::Read(inputStream, myPacketGen);
    }
};

TEST(Packets, PacketWriteTest)
{
    PingPacket packet;
    std::ostringstream outputStream;
    std::string expectedOutput = "\xFF\x01\xFF";

    outputStream << packet;

    CHECK_EQUAL(expectedOutput, outputStream.str());
}

TEST(Packets, PacketParameterWriteTest)
{
    DigitalOutputPacket packet1(13, true);
    std::ostringstream outputStream;
    std::string expectedOutput;
    std::string actualOutput;

    expectedOutput = "\xFF\x02\x0D\x02\xFF";
    packet1.write(outputStream);
    actualOutput = outputStream.str();

    CHECK_EQUAL(expectedOutput, actualOutput);

    outputStream.str("");
    DigitalOutputPacket packet2(5, false);

    expectedOutput = "\xFF\x02\x05\x01\xFF";
    packet2.write(outputStream);
    actualOutput = outputStream.str();

    CHECK_EQUAL(expectedOutput, actualOutput);
}

TEST(Packets, PacketReadTest)
{
    std::istringstream inputStream;
    inputStream.str("\xFF\x01\xFF");
    PingPacket expectedPacket;

    Packet* packet = readPacket(inputStream);

    CHECK_EQUAL(expectedPacket, *packet);

    delete packet;
}

TEST(Packets, PacketParameterReadTest)
{
    std::istringstream inputStream;
    Packet* actualPacket;
    DigitalOutputPacket expectedPacket1(7, true);
    DigitalOutputPacket expectedPacket2(4, false);

    inputStream.str("\xFF\x02\x07\x02\xFF");
    actualPacket = readPacket(inputStream);

    CHECK(actualPacket != NULL);
    CHECK_EQUAL(expectedPacket1, *actualPacket);
    CHECK(expectedPacket2 != *actualPacket);

    delete actualPacket;

    inputStream.str("\xFF\x02\x04\x01\xFF");
    actualPacket = readPacket(inputStream);

    CHECK(actualPacket != NULL);
    CHECK_EQUAL(expectedPacket2, *actualPacket);
    CHECK(expectedPacket1 != *actualPacket);

    delete actualPacket;
}

TEST(Packets, DigitalInputPacket)
{
    DigitalInputPacket dInPacket1(3);
    std::ostringstream outputStream;
    std::istringstream inputStream;

    CHECK_EQUAL(3, dInPacket1.getPin());

    dInPacket1.write(outputStream);

    BPACKET_EQUAL("\xFF\x03\x03\xFF", outputStream.str().c_str());

    DigitalInputPacket dInPacket2(12);

    CHECK_EQUAL(12, dInPacket2.getPin());

    outputStream.str("");
    dInPacket2.write(outputStream);

    STRCMP_EQUAL("\xFF\x03\x0C\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x03\x01\xFF");
    Packet* packet3 = readPacket(inputStream);

    CHECK(NULL != packet3);
    CHECK(NULL != dynamic_cast<DigitalInputPacket*>(packet3));

    DigitalInputPacket* dInPacket3 = static_cast<DigitalInputPacket*>(packet3);

    CHECK_EQUAL(1, dInPacket3->getPin());

    delete dInPacket3;
}

TEST(Packets, AnalogInputPacket)
{
    AnalogInputPacket aInPacket1(4);
    std::ostringstream outputStream;
    std::istringstream inputStream;

    CHECK_EQUAL(RedBotPacket::TYPE_AINPUT, aInPacket1.getType());
    CHECK_EQUAL(4, aInPacket1.getPin());

    aInPacket1.write(outputStream);

    BPACKET_EQUAL("\xFF\x04\x05\xFF", outputStream.str().c_str());

    AnalogInputPacket aInPacket2(11);

    CHECK_EQUAL(11, aInPacket2.getPin());

    outputStream.str("");
    aInPacket2.write(outputStream);

    BPACKET_EQUAL("\xFF\x04\x0C\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x04\x03\xFF");
    Packet* packet3 = readPacket(inputStream);

    CHECK(NULL != packet3);

    AnalogInputPacket* aInPacket3 = static_cast<AnalogInputPacket*>(packet3);

    CHECK_EQUAL(RedBotPacket::TYPE_AINPUT, aInPacket3->getType());
    CHECK_EQUAL(2, aInPacket3->getPin());

    delete aInPacket3;
}

TEST(Packets, PinConfigPacket)
{
    std::ostringstream outputStream;
    std::istringstream inputStream;
    PinConfigPacket configPacket1(4, PinConfigPacket::DIR_INPUT);

    CHECK_EQUAL(RedBotPacket::TYPE_PINCONFIG, configPacket1.getType());
    CHECK_EQUAL(4, configPacket1.getPin());
    CHECK_EQUAL(PinConfigPacket::DIR_INPUT, configPacket1.getDirection());

    configPacket1.write(outputStream);

    STRCMP_EQUAL("\xFF\x05\x04\x02\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x05\x06\x01\xFF");
    Packet* packet2 = readPacket(inputStream);

    CHECK(NULL != packet2);
    CHECK(NULL != dynamic_cast<PinConfigPacket*>(packet2));

    PinConfigPacket* configPacket2 = static_cast<PinConfigPacket*>(packet2);

    CHECK_EQUAL(6, configPacket2->getPin());
    CHECK_EQUAL(PinConfigPacket::DIR_OUTPUT, configPacket2->getDirection());

    delete packet2;
}

TEST(Packets, MotorDrivePacket)
{
    std::ostringstream outputStream;
    std::istringstream inputStream;

    MotorDrivePacket mDrivePacket1(
            MotorDrivePacket::MOTOR_RIGHT,
            255,
            MotorDrivePacket::DIR_FORWARD
            );
    mDrivePacket1.write(outputStream);

    STRCMP_EQUAL("\xFF\x06\x01\x01\x10\x10\xFF", outputStream.str().c_str());

    outputStream.str("");
    MotorDrivePacket mDrivePacket2(
            MotorDrivePacket::MOTOR_LEFT,
            128,
            MotorDrivePacket::DIR_BACKWARD
            );
    mDrivePacket2.write(outputStream);

    STRCMP_EQUAL("\xFF\x06\x02\x02\x09\x01\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x06\x02\x01\x01\x01\xFF");
    Packet* packet3 = readPacket(inputStream);

    CHECK(NULL != packet3);
    CHECK(NULL != dynamic_cast<MotorDrivePacket*>(packet3));

    MotorDrivePacket* mDrivePacket3 = static_cast<MotorDrivePacket*>(packet3);

    CHECK_EQUAL(MotorDrivePacket::MOTOR_LEFT, mDrivePacket3->getMotor());
    CHECK_EQUAL(0, mDrivePacket3->getSpeed());
    CHECK_EQUAL(MotorDrivePacket::DIR_FORWARD, mDrivePacket3->getDirection());

    delete mDrivePacket3;
}

TEST(Packets, EncoderInputPacket)
{
  EncoderInputPacket leftEncInPacket(false);
  std::ostringstream outputStream;
  leftEncInPacket.write(outputStream);

  BPACKET_EQUAL("\xFF\x07\x01\xFF", outputStream.str().c_str());

  EncoderInputPacket rightEncInPacket(true);
  outputStream.str("");
  rightEncInPacket.write(outputStream);

  BPACKET_EQUAL("\xFF\x07\x02\xFF", outputStream.str().c_str());

  std::istringstream inputStream;
  inputStream.str("\xFF\x07\x01\xFF");
  Packet* packet1 = readPacket(inputStream);

  CHECK(NULL != packet1);
  CHECK(NULL != dynamic_cast<EncoderInputPacket*>(packet1));

  EncoderInputPacket* readLeftEncInPacket = dynamic_cast<EncoderInputPacket*>(packet1);

  CHECK_FALSE(readLeftEncInPacket->isRight());

  inputStream.str("\xFF\x07\x02\xFF");
  Packet* packet2 = readPacket(inputStream);

  CHECK(NULL != packet2);
  CHECK(NULL != dynamic_cast<EncoderInputPacket*>(packet2));

  EncoderInputPacket* readRightEncInPacket = dynamic_cast<EncoderInputPacket*>(packet2);

  CHECK(readRightEncInPacket->isRight());

  delete packet1;
  delete packet2;
}

TEST(Packets, EncoderCountPacket)
{
  EncoderCountPacket leftEncCountPacket1(false, 5);
  std::ostringstream outputStream;
  leftEncCountPacket1.write(outputStream);

  BPACKET_EQUAL("\xFF\x85\x02\x01\x01\x01\x01\x06\xFF", outputStream.str().c_str());

  EncoderCountPacket rightEncCountPacket1(true, 5);
  outputStream.str("");
  rightEncCountPacket1.write(outputStream);

  BPACKET_EQUAL("\xFF\x85\x01\x01\x01\x01\x01\x06\xFF", outputStream.str().c_str());

  EncoderCountPacket leftEncCountPacket2(false, 228);
  outputStream.str("");
  leftEncCountPacket2.write(outputStream);

  BPACKET_EQUAL("\xFF\x85\x02\x01\x01\x01\x0F\x05\xFF", outputStream.str().c_str());

  EncoderCountPacket rightEncCountPacket2(true, -30);
  outputStream.str("");
  rightEncCountPacket2.write(outputStream);

  BPACKET_EQUAL("\xFF\x85\x01\x80\x80\x80\x7F\x03\xFF", outputStream.str().c_str());

  std::istringstream inputStream;
  inputStream.str("\xFF\x85\x01\x01\x01\x01\x01\x07\xFF");
  Packet* packet1 = readPacket(inputStream);

  CHECK(NULL != packet1);
  CHECK(NULL != dynamic_cast<EncoderCountPacket*>(packet1));

  EncoderCountPacket* rightEncCountInPacket = dynamic_cast<EncoderCountPacket*>(packet1);

  CHECK(rightEncCountInPacket->isRight());
  CHECK_EQUAL(6, rightEncCountInPacket->getCount());

  inputStream.str("\xFF\x85\x02\x01\x01\x01\x02\x05\xFF");
  Packet* packet2 = readPacket(inputStream);

  CHECK(NULL != packet2);
  CHECK(NULL != dynamic_cast<EncoderCountPacket*>(packet2));

  EncoderCountPacket* leftEncCountInPacket = dynamic_cast<EncoderCountPacket*>(packet2);

  CHECK_FALSE(leftEncCountInPacket->isRight());
  CHECK_EQUAL(20, leftEncCountInPacket->getCount());

  inputStream.str("\xFF\x85\x01\x80\x80\x80\x7C\x0A\xFF");
  Packet* packet3 = readPacket(inputStream);

  CHECK(NULL != packet3);
  CHECK(NULL != dynamic_cast<EncoderCountPacket*>(packet3));

  rightEncCountInPacket = dynamic_cast<EncoderCountPacket*>(packet3);

  CHECK_TRUE(rightEncCountInPacket->isRight());
  CHECK_EQUAL(-71, rightEncCountInPacket->getCount());

  delete packet1;
  delete packet2;
  delete packet3;
}

TEST(Packets, EncoderClearPacket)
{
  EncoderClearPacket leftClearPacket1(false);
  std::ostringstream outputStream;
  leftClearPacket1.write(outputStream);

  BPACKET_EQUAL("\xFF\x08\x02\xFF", outputStream.str().c_str());

  EncoderClearPacket rightClearPacket1(true);
  outputStream.str("");
  rightClearPacket1.write(outputStream);

  BPACKET_EQUAL("\xFF\x08\x01\xFF", outputStream.str().c_str());

  std::istringstream inputStream;
  inputStream.str("\xFF\x08\x01\xFF");
  Packet* packet1 = readPacket(inputStream);

  CHECK(NULL != packet1);
  CHECK(NULL != dynamic_cast<EncoderClearPacket*>(packet1));

  EncoderClearPacket* clearPacket1 = dynamic_cast<EncoderClearPacket*>(packet1);

  CHECK(clearPacket1->isRight());

  inputStream.str("\xFF\x08\x02\xFF");
  Packet* packet2 = readPacket(inputStream);

  CHECK(NULL != packet2);
  CHECK(NULL != dynamic_cast<EncoderClearPacket*>(packet2));

  EncoderClearPacket* clearPacket2 = dynamic_cast<EncoderClearPacket*>(packet2);

  CHECK_FALSE(clearPacket2->isRight());

  delete packet1;
  delete packet2;
}

TEST(Packets, DigitalValuePacket)
{
    DigitalValuePacket dValPacket1(2, true);

    CHECK_EQUAL(RedBotPacket::TYPE_DVALUE, dValPacket1.getType());
    CHECK_EQUAL(2, dValPacket1.getPin());
    CHECK_EQUAL(true, dValPacket1.getValue());

    std::ostringstream outputStream;
    outputStream << dValPacket1;

    STRCMP_EQUAL("\xFF\x81\x02\x02\xFF", outputStream.str().c_str());

    std::istringstream inputStream;
    inputStream.str("\xFF\x81\x0A\x01\xFF");

    Packet* packet2 = readPacket(inputStream);
    myPackets.push_back(packet2);

    CHECK(packet2 != NULL);
    CHECK(NULL != dynamic_cast<DigitalValuePacket*>(packet2));

    DigitalValuePacket* dValPacket2 = static_cast<DigitalValuePacket*>(packet2);

    CHECK_EQUAL(10, dValPacket2->getPin());
    CHECK_EQUAL(false, dValPacket2->getValue());

    inputStream.str("\xFF\x81\x0F\x02\xFF");

    Packet* packet3 = readPacket(inputStream);
    myPackets.push_back(packet3);

    CHECK(packet3 != NULL);
    CHECK(NULL != dynamic_cast<DigitalValuePacket*>(packet2));

    DigitalValuePacket* dValPacket3 = static_cast<DigitalValuePacket*>(packet3);

    CHECK_EQUAL(15, dValPacket3->getPin());
    CHECK_EQUAL(true, dValPacket3->getValue());
}

TEST(Packets, AnalogValuePacket)
{
    std::ostringstream outputStream;
    std::istringstream inputStream;

    AnalogValuePacket aValPacket1(4, 45);

    CHECK_EQUAL(RedBotPacket::TYPE_AVALUE, aValPacket1.getType());
    CHECK_EQUAL(4, aValPacket1.getPin());
    CHECK_EQUAL(45, aValPacket1.getValue());

    outputStream << aValPacket1;

    BPACKET_EQUAL("\xFF\x83\x05\x02\x0E\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x83\x0A\x02\x15\xFF");

    Packet* packet2 = readPacket(inputStream);
    myPackets.push_back(packet2);

    CHECK(packet2 != NULL);
    CHECK(NULL != dynamic_cast<AnalogValuePacket*>(packet2));

    AnalogValuePacket* aValPacket2 = static_cast<AnalogValuePacket*>(packet2);

    CHECK_EQUAL(9, aValPacket2->getPin());
    CHECK_EQUAL(52, aValPacket2->getValue());

    AnalogValuePacket aValPacket3(7, 165);

    CHECK_EQUAL(7, aValPacket3.getPin());
    CHECK_EQUAL(165, aValPacket3.getValue());

    outputStream.str("");
    outputStream << aValPacket3;

    BPACKET_EQUAL("\xFF\x83\x08\x06\x06\xFF", outputStream.str().c_str());
}

TEST(Packets, PinConfigInfoPacket)
{
    std::ostringstream outputStream;
    std::istringstream inputStream;

    PinConfigInfoPacket confInfoPacket1(5, RedBotPacket::DIR_INPUT);

    CHECK_EQUAL(RedBotPacket::TYPE_PINCONFIGINFO, confInfoPacket1.getType());
    CHECK_EQUAL(5, confInfoPacket1.getPin());
    CHECK_EQUAL(RedBotPacket::DIR_INPUT, confInfoPacket1.getDirection());

    outputStream.str("");
    outputStream << confInfoPacket1;

    BPACKET_EQUAL("\xFF\x84\x05\x02\xFF", outputStream.str().c_str());

    PinConfigInfoPacket confInfoPacket2(8, RedBotPacket::DIR_OUTPUT);

    CHECK_EQUAL(8, confInfoPacket2.getPin());
    CHECK_EQUAL(RedBotPacket::DIR_OUTPUT, confInfoPacket2.getDirection());

    outputStream.str("");
    outputStream << confInfoPacket2;

    BPACKET_EQUAL("\xFF\x84\x08\x01\xFF", outputStream.str().c_str());

    inputStream.str("\xFF\x84\x03\x01\xFF");
    Packet* packet3 = readPacket(inputStream);

    CHECK(NULL != packet3);
    CHECK(NULL != dynamic_cast<PinConfigInfoPacket*>(packet3));

    PinConfigInfoPacket* confInfoPacket3 = static_cast<PinConfigInfoPacket*>(packet3);

    CHECK_EQUAL(3, confInfoPacket3->getPin());
    CHECK_EQUAL(RedBotPacket::DIR_OUTPUT, confInfoPacket3->getDirection());

    delete packet3;
}

TEST(Packets, AcknowledgePacket)
{
    AcknowledgePacket ackPacket1;
    std::stringstream packetStream;

    packetStream << ackPacket1;

    STRCMP_EQUAL("\xFF\x82\xFF", packetStream.str().c_str());

    Packet* packet2 = readPacket(packetStream);

    CHECK(NULL != packet2);
    CHECK(NULL != dynamic_cast<AcknowledgePacket*>(packet2));

    delete packet2;
}

TEST(Packets, DigitalOutputPacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    DigitalOutputPacket dOutPacket1(7, true);

    dOutPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DOUTPUT</type>"
        "<pin>7</pin>"
        "<value>1</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    DigitalOutputPacket dOutPacket2(8, false);

    packetStream.str("");
    dOutPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DOUTPUT</type>"
        "<pin>8</pin>"
        "<value>0</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, DigitalInputPacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    DigitalInputPacket dInPacket1(13);

    dInPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DINPUT</type>"
        "<pin>13</pin>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    DigitalInputPacket dInPacket2(4);

    packetStream.str("");
    dInPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DINPUT</type>"
        "<pin>4</pin>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, AnalogInputPacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    AnalogInputPacket aInPacket1(3);

    aInPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>AINPUT</type>"
        "<pin>3</pin>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    AnalogInputPacket aInPacket2(7);

    packetStream.str("");
    aInPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>AINPUT</type>"
        "<pin>7</pin>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, PinConfigPacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;

    PinConfigPacket configPacket1(
            3,
            PinConfigPacket::DIR_OUTPUT
            );
    configPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>PINCONFIG</type>"
        "<pin>3</pin>"
        "<direction>OUTPUT</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    packetStream.str("");

    PinConfigPacket configPacket2(
            10,
            PinConfigPacket::DIR_INPUT
            );
    configPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>PINCONFIG</type>"
        "<pin>10</pin>"
        "<direction>INPUT</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, MotorDrivePacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;

    MotorDrivePacket mDrivePacket1(
            MotorDrivePacket::MOTOR_RIGHT,
            255,
            MotorDrivePacket::DIR_BACKWARD
            );
    mDrivePacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>MDRIVE</type>"
        "<motor>right</motor>"
        "<speed>255</speed>"
        "<direction>backward</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    packetStream.str("");
    MotorDrivePacket mDrivePacket2(
            MotorDrivePacket::MOTOR_LEFT,
            34,
            MotorDrivePacket::DIR_FORWARD
            );
    mDrivePacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>MDRIVE</type>"
        "<motor>left</motor>"
        "<speed>34</speed>"
        "<direction>forward</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, EncoderInputPacketXML)
{
  std::ostringstream packetStream;

  EncoderInputPacket rightEncInPacket(true);
  rightEncInPacket.writeXML(packetStream);

  std::string expectedOutput =
    "<packet>"
    "<type>ENCINPUT</type>"
    "<motor>right</motor>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());

  EncoderInputPacket leftEncInPacket(false);
  packetStream.str("");
  leftEncInPacket.writeXML(packetStream);

  expectedOutput =
    "<packet>"
    "<type>ENCINPUT</type>"
    "<motor>left</motor>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, EncoderClearPacketXML)
{
  std::ostringstream packetStream;

  EncoderClearPacket rightEncClearPacket(true);
  rightEncClearPacket.writeXML(packetStream);

  std::string expectedOutput =
    "<packet>"
    "<type>ENCCLEAR</type>"
    "<motor>right</motor>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());

  EncoderClearPacket leftEncClearPacket(false);
  packetStream.str("");
  leftEncClearPacket.writeXML(packetStream);

  expectedOutput =
    "<packet>"
    "<type>ENCCLEAR</type>"
    "<motor>left</motor>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, DigitalValuePacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    DigitalValuePacket dValPacket1(10, true);

    dValPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DVALUE</type>"
        "<pin>10</pin>"
        "<value>1</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    DigitalValuePacket dValPacket2(6, false);

    packetStream.str("");
    dValPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>DVALUE</type>"
        "<pin>6</pin>"
        "<value>0</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, AnalogValuePacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    AnalogValuePacket aValPacket1(4, 87);

    aValPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>AVALUE</type>"
        "<pin>4</pin>"
        "<value>87</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    AnalogValuePacket aValPacket2(8, 243);

    packetStream.str("");
    aValPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>AVALUE</type>"
        "<pin>8</pin>"
        "<value>243</value>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, PinConfigInfoPacketXML)
{
    std::ostringstream packetStream;
    std::string expectedOutput;
    PinConfigInfoPacket confInfoPacket1(5, RedBotPacket::DIR_INPUT);
    confInfoPacket1.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>PINCONFIGINFO</type>"
        "<pin>5</pin>"
        "<direction>INPUT</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());

    PinConfigInfoPacket confInfoPacket2(6, RedBotPacket::DIR_OUTPUT);
    packetStream.str("");
    confInfoPacket2.writeXML(packetStream);

    expectedOutput =
        "<packet>"
        "<type>PINCONFIGINFO</type>"
        "<pin>6</pin>"
        "<direction>OUTPUT</direction>"
        "</packet>";
    CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, EncoderCountPacketXML)
{
  std::ostringstream packetStream;
  std::string expectedOutput;
  EncoderCountPacket countPacket1(true, 4);
  countPacket1.writeXML(packetStream);

  expectedOutput =
    "<packet>"
    "<type>ENCCOUNT</type>"
    "<motor>right</motor>"
    "<count>4</count>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());

  EncoderCountPacket countPacket2(false, -3);
  packetStream.str("");
  countPacket2.writeXML(packetStream);

  expectedOutput =
    "<packet>"
    "<type>ENCCOUNT</type>"
    "<motor>left</motor>"
    "<count>-3</count>"
    "</packet>";
  CHECK_EQUAL(expectedOutput, packetStream.str());
}

TEST(Packets, InvalidReadPacket)
{
    std::istringstream inputStream;
    std::ostringstream outputStream;

    DigitalValuePacket dValPacket;

    outputStream << dValPacket;

    BPACKET_EQUAL("\xFF\x81\xFF", outputStream.str().c_str());

    inputStream.str("\x0A\xFF");
    inputStream >> dValPacket;

    CHECK_FALSE(dValPacket.isValid());

    outputStream.str("");
    outputStream << dValPacket;
    std::string dValPacketData = outputStream.str();

    BPACKET_EQUAL("\xFF\x81\x0A\xFF", dValPacketData.c_str());
}


TEST_GROUP(RedBotPacketGenerator)
{
    RedBotPacketGenerator myPacketGen;

    std::list<Packet*> myPackets;

    void teardown()
    {
        for(
                std::list<Packet*>::const_iterator packetIter = myPackets.begin();
                packetIter != myPackets.end();
                ++packetIter
           )
        {
            delete (*packetIter);
        }
    }

    Packet* createPacket(
            char type
            )
    {
        Packet* packet = myPacketGen.createPacket(type);
        myPackets.push_back(packet);
        return packet;
    }
};

#define CHECK_PACKETGEN(bid, type) (CHECK(NULL != dynamic_cast<type*>(createPacket(bid))));

TEST(RedBotPacketGenerator, InvalidType)
{
    Packet* packet = createPacket(0);

    CHECK_EQUAL((Packet*)NULL, packet);
}

TEST(RedBotPacketGenerator, Ping)
{
    CHECK_PACKETGEN(RedBotPacket::BID_PING, PingPacket);
}

TEST(RedBotPacketGenerator, Acknowledge)
{
    CHECK_PACKETGEN(RedBotPacket::BID_ACK, AcknowledgePacket);
}

TEST(RedBotPacketGenerator, PinConfig)
{
    CHECK_PACKETGEN(RedBotPacket::BID_PINCONFIG, PinConfigPacket);
}

TEST(RedBotPacketGenerator, PinConfigInfo)
{
    CHECK_PACKETGEN(RedBotPacket::BID_PINCONFIGINFO, PinConfigInfoPacket);
}

TEST(RedBotPacketGenerator, DigitalInput)
{
    CHECK_PACKETGEN(RedBotPacket::BID_DINPUT, DigitalInputPacket);
}

TEST(RedBotPacketGenerator, DigitalOutput)
{
    CHECK_PACKETGEN(RedBotPacket::BID_DOUTPUT, DigitalOutputPacket);
}

TEST(RedBotPacketGenerator, DigitalValue)
{
    CHECK_PACKETGEN(RedBotPacket::BID_DVALUE, DigitalValuePacket);
}

TEST(RedBotPacketGenerator, AnalogInput)
{
    CHECK_PACKETGEN(RedBotPacket::BID_AINPUT, AnalogInputPacket);
}

TEST(RedBotPacketGenerator, AnalogValue)
{
    CHECK_PACKETGEN(RedBotPacket::BID_AVALUE, AnalogValuePacket);
}

TEST(RedBotPacketGenerator, MotorDrive)
{
    CHECK_PACKETGEN(RedBotPacket::BID_MDRIVE, MotorDrivePacket);
}
