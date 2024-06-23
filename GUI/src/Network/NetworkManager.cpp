/*
** EPITECH PROJECT, 2024
** Network
** File description:
** NetworkManager
*/

#include "Network/NetworkManager.hpp"
#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <sys/socket.h>

NetworkManager::NetworkManager(const std::string &address, int port)
    : _address(address), _socket(-1), _port(port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    inet_pton(AF_INET, _address.c_str(), &_addr.sin_addr);
}

NetworkManager::NetworkManager(int port)
    : _address("localhost"), _socket(-1), _port(port)
{
    std::memset(&_addr, 0, sizeof(_addr));
    _addr.sin_family = AF_INET;
    _addr.sin_port = htons(_port);
    inet_pton(AF_INET, _address.c_str(), &_addr.sin_addr);
}

NetworkManager::~NetworkManager()
{
    if (_socket != -1)
        close(_socket);
}

bool NetworkManager::connectToServer()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if (_socket < 0)
    {
        std::cerr << "[-]Error creating the socket" << std::endl;
        return false;
    }
    if (connect(_socket, reinterpret_cast<struct sockaddr *>(&_addr), sizeof(_addr)) < 0)
    {
        std::cerr << "[-]Error connecting to the server" << std::endl;
        close(_socket);
        return false;
    }
    return true;
}

ssize_t NetworkManager::sendData(const std::string &data) const
{
    return send(_socket, data.c_str(), data.size(), 0);
}

ssize_t NetworkManager::receiveData(char *buffer, size_t size) const
{
    return recv(_socket, buffer, size, 0);
}
