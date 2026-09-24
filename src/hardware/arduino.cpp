#include "hardware/arduino.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <array>
#include <bit>
#include <cstdint>
#include <span>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "core/config.hpp"

namespace {

// Marks the beginning of every packet.
// The Arduino can use this byte to detect where a new packet starts.
constexpr uint8_t START_BYTE = 0xAA;

// Initial value and polynomial used by the CRC16 algorithm.
// The Arduino must use the same values when validating packets.
constexpr uint16_t CRC16_INITIAL_VALUE = 0xFFFF;
constexpr uint16_t CRC16_POLYNOMIAL = 0x8408;

constexpr int BITS_PER_BYTE = 8;

// Supported serial baud rates.
constexpr int BAUD_9600 = 9600;
constexpr int BAUD_19200 = 19200;
constexpr int BAUD_38400 = 38400;
constexpr int BAUD_57600 = 57600;
constexpr int BAUD_115200 = 115200;

// Convert a normal integer baud rate into the termios constant
// required by Linux for configuring the serial port.
speed_t get_baud_rate(int baud_rate) {
    switch (baud_rate) {
        case BAUD_9600:
            return B9600;

        case BAUD_19200:
            return B19200;

        case BAUD_38400:
            return B38400;

        case BAUD_57600:
            return B57600;

        case BAUD_115200:
            return B115200;

        default:
            throw std::runtime_error("Unsupported Arduino baud rate");
    }
}

// Calculate a CRC16 checksum for the given bytes.
//
// The CRC is calculated over the entire packet BEFORE the CRC itself
// is added. The Arduino can calculate the same CRC after receiving
// the packet and compare the result to detect corrupted data.
uint16_t crc16(std::span<const uint8_t> data) {
    uint16_t crc = CRC16_INITIAL_VALUE;

    for (const auto BYTE : data) {
        crc ^= BYTE;

        // Process all 8 bits of the current byte.
        for (int bit = 0; bit < BITS_PER_BYTE; ++bit) {
            crc = ((crc & 1) != 0) ? (crc >> 1) ^ CRC16_POLYNOMIAL : (crc >> 1);
        }
    }

    return crc;
}

}  // namespace

namespace arduino {

// Build and send a command to the Arduino.
//
// Example:
//     send_command("servo", 120.0f);
//
// The resulting packet has this format:
//
// +------------+-----------+------------+----------------+-------------+---------+
// | Start      | Packet ID | Label Len  | Component      | Float Value | CRC     |
// | 1 byte     | 2 bytes   | 1 byte     | N bytes        | 4 bytes     | 2 bytes |
// +------------+-----------+------------+----------------+-------------+---------+
//
// For example:
//     send_command("servo", 120.0f)
//
// produces:
//
//     0xAA
//     packet ID
//     0x05                  <- length of "servo"
//     's' 'e' 'r' 'v' 'o'   <- component label
//     120.0 as 4 bytes
//     CRC16

void send_command(const std::string& component_label, float value) {
    // Give every packet a unique ID.
    //
    //     First call  -> packet ID 0
    //     Second call -> packet ID 1
    //     Third call  -> packet ID 2
    //     ...
    static uint16_t packet_id = 0;

    // Read the Arduino serial configuration from the application config.
    const auto BAUD_RATE = get_baud_rate(config::arduino_baud_rate());
    const std::string PORT = config::arduino_port();

    // Open the serial port for writing.
    //
    // For example:
    //
    //     /dev/ttyUSB0 or any port defined in ~/.config/cdsl/config.toml
    //
    // O_WRONLY  -> open for writing
    // O_NOCTTY  -> don't make the Arduino serial device the controlling terminal
    const int SERIAL = open(PORT.c_str(), O_WRONLY | O_NOCTTY);

    if (SERIAL == -1) {
        throw std::runtime_error("Failed to open " + PORT);
    }

    // termios stores the Linux serial-port configuration.
    termios tty{};

    if (tcgetattr(SERIAL, &tty) != 0) {
        close(SERIAL);
        throw std::runtime_error("Failed to get serial configuration");
    }

    // Configure the baud rate for both input and output.
    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    // Configure the serial connection as 8N1:
    //
    //     8 data bits
    //     No parity
    //     1 stop bit
    //
    tty.c_cflag &= ~PARENB;  // Disable parity
    tty.c_cflag &= ~CSTOPB;  // Use one stop bit
    tty.c_cflag &= ~CSIZE;   // Clear existing data-bit setting
    tty.c_cflag |= CS8;      // Use 8 data bits

    // Enable receiving and ignore modem-control signals.
    tty.c_cflag |= CREAD | CLOCAL;

    // Disable terminal processing.
    //
    // We are sending raw binary data, not normal terminal text.
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Disable software flow control.
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Disable output processing.
    tty.c_oflag &= ~OPOST;

    // Apply the serial configuration immediately.
    if (tcsetattr(SERIAL, TCSANOW, &tty) != 0) {
        close(SERIAL);
        throw std::runtime_error("Failed to configure serial port");
    }

    // Calculate the fixed-size parts of the packet.
    //
    // Header:
    //     START_BYTE -> 1 byte
    //     packet_id  -> 2 bytes
    //     label size -> 1 byte
    //
    // The component label itself is variable-sized.
    constexpr size_t HEADER_SIZE = sizeof(START_BYTE) + sizeof(packet_id) + sizeof(uint8_t);

    // A float is normally 4 bytes on the target platforms.
    constexpr size_t VALUE_SIZE = sizeof(float);

    // CRC16 occupies 2 bytes.
    constexpr size_t CRC_SIZE = sizeof(uint16_t);

    // Calculate the total expected packet size so that the vector
    // can reserve enough memory before we start adding bytes.
    //
    // reserve() does NOT change packet.size().
    // It only preallocates memory to avoid reallocations.
    const size_t PACKET_SIZE = HEADER_SIZE + component_label.size() + VALUE_SIZE + CRC_SIZE;

    // The packet is simply a dynamically sized array of bytes.
    std::vector<uint8_t> packet;
    packet.reserve(PACKET_SIZE);

    // 1. Start byte
    // The Arduino can look for 0xAA to identify the beginning of
    // a new packet.
    packet.push_back(START_BYTE);

    // 2. Packet ID
    // packet_id is a uint16_t, so it requires 2 bytes.
    //
    // We explicitly send it in little-endian order:
    //
    //     Low byte
    //     High byte
    //
    packet.push_back(static_cast<uint8_t>(packet_id));
    packet.push_back(static_cast<uint8_t>(packet_id >> BITS_PER_BYTE));

    // 3. Component label length
    // The Arduino needs to know how many bytes belong to the label.
    //
    // For:
    //
    //     "servo"
    //
    // the length is:
    //
    //     5
    //
    packet.push_back(static_cast<uint8_t>(component_label.size()));

    // 4. Component label
    // Add the actual characters to the binary packet.
    //
    // "servo" becomes:
    //
    //     73 65 72 76 6F
    //
    // in hexadecimal ASCII.
    packet.insert(packet.end(), component_label.begin(), component_label.end());

    // 5. Float value
    // The float cannot be sent directly as a character.
    //
    // Instead, reinterpret its 4-byte binary representation as
    // four uint8_t values.
    //
    // For example:
    //
    //     120.0f
    //
    // becomes four raw bytes representing the IEEE-754 float.
    const auto VALUE_BYTES = std::bit_cast<std::array<uint8_t, sizeof(float)>>(value);

    // Append those four bytes to the packet.
    packet.insert(packet.end(), VALUE_BYTES.begin(), VALUE_BYTES.end());

    // 6. CRC16
    // Calculate the checksum over everything added so far:
    //
    //     START BYTE
    //     + PACKET ID
    //     + LABEL LENGTH
    //     + LABEL
    //     + FLOAT VALUE
    //
    // The CRC itself is NOT included in this calculation.
    const auto CRC = crc16(packet);

    // Store the CRC as two bytes in little-endian order.
    packet.push_back(static_cast<uint8_t>(CRC));
    packet.push_back(static_cast<uint8_t>(CRC >> BITS_PER_BYTE));

    // At this point the complete packet exists in memory:
    //
    //     [START][ID][LABEL LEN][LABEL][FLOAT][CRC]
    //
    // packet.data() points to the raw bytes.
    // packet.size() tells us how many bytes need to be transmitted.

    // 7. Send packet to Arduino
    const auto BYTES_WRITTEN = write(SERIAL, packet.data(), packet.size());

    // Make sure the complete packet was written.
    //
    // If write() sends fewer bytes than expected, the Arduino would
    // receive an incomplete packet, so treat that as an error.
    if (std::cmp_not_equal(BYTES_WRITTEN, packet.size())) {
        close(SERIAL);
        throw std::runtime_error("Failed to send complete command");
    }

    // Only increment the packet ID after successfully sending
    // the complete packet.
    ++packet_id;

    // Close the serial port after the packet has been sent.
    close(SERIAL);
}

}  // namespace arduino
