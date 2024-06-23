/*
** EPITECH PROJECT, 2024
** Network
** File description:
** ProtocolProject
*/

#pragma once

#include "NetworkManager.hpp"

class ProtocolHandler {
public:
    ProtocolHandler(NetworkManager &networkManager);
    ProtocolHandler(ProtocolHandler &&) = delete;
    ProtocolHandler(const ProtocolHandler &) = delete;
    ProtocolHandler &operator=(ProtocolHandler &&) = delete;
    ProtocolHandler &operator=(const ProtocolHandler &) = delete;
    ~ProtocolHandler() = default;

    bool readDataFromServer(std::string &outData);

private:
    NetworkManager &_networkManager;
    std::string _buffer;
    [[nodiscard]] bool waitForData() const;
};
