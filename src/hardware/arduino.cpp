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
#include <vector>

#include "core/config.hpp"

namespace {

// ============================================================
// Packet format
//
// +------------+-----------+-----------+-------------+---------+
// | Start      | Packet ID | Component | Float Value | CRC     |
// | 1 byte     | 2 bytes   | 1 byte    | 4 bytes     | 2 bytes |
// +------------+-----------+-----------+-------------+---------+
//
// Total packet size = 10 bytes
//
// CRC is calculated over:
//
//     Start + Packet ID + Component + Float Value
//
// The CRC itself is not included in the calculation.
// ============================================================

// Marks the beginning of every packet.
// The Arduino uses this byte to find the start of a packet.
constexpr uint8_t START_BYTE = 0xAA;

// Initial value and polynomial used by the CRC16 algorithm.
// The Arduino must use the same values.
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
// required by Linux.
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

// Calculate CRC16 for the supplied bytes.
uint16_t crc16(std::span<const uint8_t> data) {
    uint16_t crc = CRC16_INITIAL_VALUE;

    for (const auto BYTE : data) {
        crc ^= BYTE;

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
//
//     send_command("shoulder", 120.0f);
//
// The component name is used to look up its numeric ID from
// the configuration file.
//
// For example:
//
//     "shoulder" -> ID 2
//
// The actual packet sent to the Arduino contains only the ID.
//
// Packet:
//
//     [START][PACKET ID][COMPONENT ID][FLOAT][CRC]

void send_command(const std::string& component_label, float value) {
    // Give every packet a unique ID.
    //
    // First call  -> 0
    // Second call -> 1
    // Third call  -> 2
    // ...
    static uint16_t packet_id = 0;

    // Look up the component ID from config.toml.
    //
    // Example:
    //
    //     "shoulder" -> 2
    //
    const uint8_t COMPONENT_ID = config::component_id(component_label);

    // Read the Arduino serial configuration.
    const auto BAUD_RATE = get_baud_rate(config::arduino_baud_rate());
    const std::string PORT = config::arduino_port();

    // Open the serial port for writing.
    const int SERIAL = open(PORT.c_str(), O_WRONLY | O_NOCTTY);

    if (SERIAL == -1) {
        throw std::runtime_error("Failed to open " + PORT);
    }

    // Linux serial-port configuration.
    termios tty{};

    if (tcgetattr(SERIAL, &tty) != 0) {
        close(SERIAL);
        throw std::runtime_error("Failed to get serial configuration");
    }

    // Configure baud rate.
    cfsetospeed(&tty, BAUD_RATE);
    cfsetispeed(&tty, BAUD_RATE);

    // Configure 8N1:
    //
    //     8 data bits
    //     No parity
    //     1 stop bit
    //
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;

    // Enable receiving and ignore modem-control signals.
    tty.c_cflag |= CREAD | CLOCAL;

    // Disable terminal processing.
    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);

    // Disable software flow control.
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);

    // Disable output processing.
    tty.c_oflag &= ~OPOST;

    // Apply serial configuration.
    if (tcsetattr(SERIAL, TCSANOW, &tty) != 0) {
        close(SERIAL);
        throw std::runtime_error("Failed to configure serial port");
    }

    // ========================================================
    // Build packet
    // ========================================================

    // Packet layout:
    //
    //     START       = 1 byte
    //     PACKET ID   = 2 bytes
    //     COMPONENT   = 1 byte
    //     FLOAT       = 4 bytes
    //     CRC         = 2 bytes
    //
    // Total = 10 bytes
    constexpr size_t PACKET_SIZE =
        sizeof(START_BYTE) + sizeof(packet_id) + sizeof(COMPONENT_ID) + sizeof(float) + sizeof(uint16_t);

    std::vector<uint8_t> packet;
    packet.reserve(PACKET_SIZE);

    // --------------------------------------------------------
    // 1. Start byte
    // --------------------------------------------------------
    packet.push_back(START_BYTE);

    // --------------------------------------------------------
    // 2. Packet ID
    //
    // Send uint16_t in little-endian order:
    //
    //     Low byte
    //     High byte
    // --------------------------------------------------------
    packet.push_back(static_cast<uint8_t>(packet_id));
    packet.push_back(static_cast<uint8_t>(packet_id >> BITS_PER_BYTE));

    // --------------------------------------------------------
    // 3. Component ID
    //
    // The name is NOT sent.
    //
    // Example:
    //
    //     "shoulder" -> 2
    //
    // Only the value 2 is transmitted.
    // --------------------------------------------------------
    packet.push_back(COMPONENT_ID);

    // --------------------------------------------------------
    // 4. Float value
    //
    // Convert the IEEE-754 float into four raw bytes.
    // --------------------------------------------------------
    const auto VALUE_BYTES = std::bit_cast<std::array<uint8_t, sizeof(float)>>(value);

    packet.insert(packet.end(), VALUE_BYTES.begin(), VALUE_BYTES.end());

    // --------------------------------------------------------
    // 5. CRC16
    //
    // Calculate the CRC over:
    //
    //     START
    //     PACKET ID
    //     COMPONENT ID
    //     FLOAT
    //
    // The CRC itself is not included.
    // --------------------------------------------------------
    const auto CRC = crc16(packet);

    // Store CRC in little-endian order.
    packet.push_back(static_cast<uint8_t>(CRC));
    packet.push_back(static_cast<uint8_t>(CRC >> BITS_PER_BYTE));

    // ========================================================
    // Send packet
    // ========================================================

    const auto BYTES_WRITTEN = write(SERIAL, packet.data(), packet.size());

    // Make sure the complete packet was transmitted.
    if (std::cmp_not_equal(BYTES_WRITTEN, packet.size())) {
        close(SERIAL);
        throw std::runtime_error("Failed to send complete command");
    }

    // Increment packet ID only after successful transmission.
    ++packet_id;

    // Close serial port.
    close(SERIAL);
}

}  // namespace arduino
