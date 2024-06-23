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
    ~ProtocolHandler();

    void run();

private:
    NetworkManager &_networkManager;
    void handleCommand(const std::string &command);
};
