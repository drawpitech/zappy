/*
** EPITECH PROJECT, 2024
** zappy
** File description:
** CubeMappingPass
*/

#pragma once

class CubeMappingPass {
    public:
        CubeMappingPass();
        ~CubeMappingPass();

        CubeMappingPass(const CubeMappingPass &cpy) = delete;
        CubeMappingPass &operator=(const CubeMappingPass &src) = delete;

        CubeMappingPass(CubeMappingPass &&mv) = delete;
        CubeMappingPass &operator=(CubeMappingPass &&mv) = delete;

    protected:
    private:
};
