/*
** EPITECH PROJECT, 2024
** Network
** File description:
** ProtocolManager
*/

#include "Network/ProtocolHandler.hpp"
#include <iostream>
#include <array>
#include <stdexcept>
#include <sys/select.h>

ProtocolHandler::ProtocolHandler(NetworkManager &networkManager)
    : _networkManager(networkManager)
{
}

bool ProtocolHandler::waitForData() const
{
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(_networkManager.getSocket(), &readfds);
    timeval timeout { .tv_sec = 0, .tv_usec = 0 }; // Adjust the timeout as needed

    int activity = select(_networkManager.getSocket() + 1, &readfds, nullptr, nullptr, &timeout);
    if (activity == -1) {
        perror("select");
        return false;
    }

    return (activity > 0 && FD_ISSET(_networkManager.getSocket(), &readfds));
}

bool ProtocolHandler::readDataFromServer(std::string &outData)
{
    if (waitForData())
    {
        std::array<char, 1024000> readBuffer = {0};
        std::size_t readSize{};

        while (readSize < 1024000)
        {
            ssize_t bytesRead = _networkManager.receiveData(readBuffer.data() + readSize, 1024000 - readSize);
            if (bytesRead <= 0)
            {
                if (bytesRead == 0)
                {
                    std::cerr << "[-]Server closed the connection?" << std::endl;
                } else {
                    throw std::runtime_error("[-]Read failed.");
                }
            }
            readSize += bytesRead;
            if (readBuffer[readSize - 1] == '\n')
            {
                break;
            }
        }
        _buffer.append(readBuffer.data(), readSize);
        outData = _buffer;
        _buffer.clear();
        return true;
    }
    return false;
}
