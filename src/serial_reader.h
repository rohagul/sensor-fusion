#ifndef SERIAL_READER_H
#define SERIAL_READER_H

#include <termios.h>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <sstream>

int openSerialPort(const char* portName, speed_t baud);
std::string readLine(int fd);
bool serialDataAvailable(int fd);
bool parseFields(const std::string& s, std::vector<std::string>& toks,
                        size_t expected, std::string& why);
#endif // SERIAL_READER_H