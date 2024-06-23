/*
** EPITECH PROJECT, 2024
** Network
** File description:
** ProtocolManager
*/

#include "Network/ProtocolHandler.hpp"

ProtocolHandler::ProtocolHandler(NetworkManager &networkManager)
    : _networkManager(networkManager)
{
}

void ProtocolHandler::run()
{
    char buffer[4096];
    for (;;)
    {
        ssize_t bytesRead = _networkManager.receiveData(buffer, sizeof(buffer) - 1);
        if (bytesRead > 0)
        {
            buffer[bytesRead] = '\0';
            std::string command(buffer);
            handleCommand(command);
        }
    }
}

void ProtocolHandler::handleCommand(const std::string &command)
{
    (void)command;
}
