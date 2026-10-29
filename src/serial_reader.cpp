#include "serial_reader.h"
#include <sys/ioctl.h>
#include <cmath>
// Opens a serial port with the specified name and baud rate, returning the file descriptor
int openSerialPort(const char* portName, speed_t baud) {
    int fd = open(portName, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        std::cerr << "Failed to open " << portName << ": " << strerror(errno) << "\n";
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        std::cerr << "tcgetattr failed: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    cfsetispeed(&tty, baud);
    cfsetospeed(&tty, baud);

    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag &= ~PARENB;
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;
    tty.c_cflag |= CREAD | CLOCAL;

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~(ECHO | ECHOE | ISIG);

    tty.c_iflag &= ~(IXON | IXOFF | IXANY);
    tty.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL);

    tty.c_oflag &= ~OPOST;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 5;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        std::cerr << "tcsetattr failed: " << strerror(errno) << "\n";
        close(fd);
        return -1;
    }

    return fd;
}
 //Reads a single line from the serial port, terminated by a newline character
std::string readLine(int fd) { 
    std::string line;
    char c;
    while (true) {
        ssize_t n = read(fd, &c, 1);
        if (n > 0) {
            if (c == '\n') break;
            if (c != '\r') line += c;
        }
    }
    return line;
}

bool serialDataAvailable(int fd)
{
    int bytes_available = 0;

    ioctl(fd, FIONREAD, &bytes_available);

    return bytes_available > 0;
}

bool parseFields(const std::string& s, std::vector<std::string>& toks,
                        size_t expected, std::string& why) {
    toks.clear();
    std::stringstream ss(s);
    std::string tok;
    while (std::getline(ss, tok, ',')) {
        size_t a = tok.find_first_not_of(" \t\r\n");
        size_t b = tok.find_last_not_of(" \t\r\n");
        tok = (a == std::string::npos) ? "" : tok.substr(a, b - a + 1);
        try {
            size_t used = 0;
            double d = std::stod(tok, &used);
            if (used != tok.size() || !std::isfinite(d)) {
                why = "bad number in field " + std::to_string(toks.size() + 1) + ": '" + tok + "'";
                return false;
            }
        } catch (...) {
            why = "non-numeric field #" + std::to_string(toks.size() + 1) + ": '" + tok + "'";
            return false;
        }
        toks.push_back(tok);
    }
    if (toks.size() != expected) {
        why = "got " + std::to_string(toks.size()) + " fields, expected " + std::to_string(expected);
        return false;
    }
    return true;
}