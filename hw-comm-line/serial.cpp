#include "hardware/serial.hpp"

#include <fcntl.h>
#include <termios.h>
#include <unistd.h>

#include <iostream>
#include <stdexcept>
#include <string>

#include "config.hpp"

void send_command(const std::string& component_label, float value) {
    const std::string port = arduino_port();

    speed_t baud_rate;

    switch (arduino_baud_rate()) {
        case 9600:
            baud_rate = B9600;
            break;

        case 19200:
            baud_rate = B19200;
            break;

        case 38400:
            baud_rate = B38400;
            break;

        case 57600:
            baud_rate = B57600;
            break;

        case 115200:
            baud_rate = B115200;
            break;

        default:
            throw std::runtime_error("Unsupported Arduino baud rate");
    }

    int serial = open(port.c_str(), O_WRONLY | O_NOCTTY);

    if (serial == -1) {
        std::cerr << "Failed to open " << port << '\n';
        return;
    }

    termios tty{};

    if (tcgetattr(serial, &tty) != 0) {
        std::cerr << "Failed to get serial configuration\n";
        close(serial);
        return;
    }

    cfsetospeed(&tty, baud_rate);
    cfsetispeed(&tty, baud_rate);

    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CSIZE;
    tty.c_cflag |= CS8;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_oflag &= ~OPOST;

    if (tcsetattr(serial, TCSANOW, &tty) != 0) {
        std::cerr << "Failed to configure serial port\n";
        close(serial);
        return;
    }

    const std::string message = component_label + " " + std::to_string(value) + '\n';

    const ssize_t bytes_written = write(serial, message.c_str(), message.size());

    if (bytes_written == -1) {
        std::cerr << "Failed to send command\n";
    }

    close(serial);
}
