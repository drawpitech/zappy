/*
** EPITECH PROJECT, 2024
** Network
** File description:
** NetworkManager
*/

#pragma once

#include <unistd.h>
#include <netinet/in.h>
#include <string>

class NetworkManager {
public:
    NetworkManager(const std::string &address, int port);
    NetworkManager(int port);
    NetworkManager(NetworkManager &&) = delete;
    NetworkManager(const NetworkManager &) = delete;
    NetworkManager &operator=(NetworkManager &&) = delete;
    NetworkManager &operator=(const NetworkManager &) = delete;
    ~NetworkManager();

    bool connectToServer();
    [[nodiscard]] ssize_t sendData(const std::string &data) const;
    [[nodiscard]] ssize_t receiveData(char *buffer, std::size_t size) const;

private:
    std::string _address;
    int _socket;
    int _port;
    struct sockaddr_in _addr;
};
