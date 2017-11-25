#ifndef ROBOTDRIVE_H
#define ROBOTDRIVE_H

#include "RedBotComponent.h"
#include "RedBotPacket.h"
#include <stdint.h>

/**
 * Motor drive command class
 */
class MotorDrivePacket : public RedBotPacket
{
    public:

        /**
         * Motor enumeration
         */
        enum Motor
        {
            MOTOR_LEFT,
            MOTOR_RIGHT
        };

        /**
         * Motor direction enumeration
         */
        enum Direction
        {
            DIR_FORWARD,
            DIR_BACKWARD
        };

        /**
         * Default constructor
         */
        MotorDrivePacket();

        /**
         * Constructor given motor information
         */
        MotorDrivePacket(
                Motor       motor,      /**< Motor to drive */
                uint8_t     speed,      /**< Absolute speed to drive motor to */
                Direction   direction   /**< Direction to drive motor in */
                );

        /**
         * Constructor given motor and single floating-point drive value
         *
         * This constructor automatically translates the given value to a valid
         * speed and direction.
         */
        MotorDrivePacket(
                Motor   motor,      /**< Motor to drive */
                double  driveVal    /**< Signed floating-point drive value */
                );

        /**
         * Destructor
         */
        ~MotorDrivePacket();

        /**
         * Reads serialized binary data from input stream
         */
        void read(
                std::istream&
                );

        /**
         * Indicates if this packet is valid or not
         *
         * A Packet can be invalid if it tried to read from a malformed byte
         * stream.
         */
        bool isValid() const;

        /**
         * Equality operator
         */
        bool operator==(
                const Packet&
                ) const;

        /**
         * Indicates which motor to drive
         */
        Motor getMotor() const;

        /**
         * Provides absolute speed to drive motor to
         */
        uint8_t getSpeed() const;

        /**
         * Indicates which way the motor will be driven
         */
        Direction getDirection() const;

    private:

        /**
         * Writes binary packet contents to output stream
         */
        void writeContents(
                std::ostream&
                ) const;

        /**
         * Provides elements to include in the XML representation
         */
        void getXMLElements(
                XMLElements& elements
                ) const;

        /**
         * Motor to drive
         */
        Motor myMotor;

        /**
         * Speed to drive motor to
         */
        uint8_t mySpeed;

        /**
         * Direction to drive motor in
         */
        Direction myDirection;
};

namespace frc
{

class SpeedController;

/**
 * Handles common drive operations for a robot with two motors
 */
class DifferentialDrive : public RedBotComponent
{
    public:

        /**
         * Initializes the robot drive
         */
        DifferentialDrive(
		SpeedController& leftController,
                SpeedController& rightController
                );

        void SetExpiration(
                double expiration
                );

        /**
         * Drives the robot with the given magnitude and curve
	 *
	 * An optional argument disables squaring the magnitude and
	 * curve inputs before forwarding them to the speed
	 * controllers. Normally, this makes the robot less sensitive
	 * to input changes at low speeds.
         */
        void ArcadeDrive(
                double magnitude,
                double curve,
                bool squaredInputs = true
                );

        /**
         * Provides the next packet to send to the robot
         */
        Packet* getNextPacket();

        /**
         * Processes a packet received from the robot
         */
        bool processPacket(
                const Packet& packet
                );

    private:

        /**
         * Next packet to send to robot to drive left motor
         */
        MotorDrivePacket* myCurrentLMotorPacket;

        /**
         * Next packet to send to robot to drive right motor
         */
        MotorDrivePacket* myCurrentRMotorPacket;
};

}; /* namespace frc */

#endif /* ifndef ROBOTDRIVE_H */
