##
## EPITECH PROJECT, 2024
## zappy
## File description:
## Makefile
##

CMAKEFLAGS ?= -DCMAKE_BUILD_TYPE=Release
CMAKEFLAGS += -DCMAKE_INSTALL_PREFIX=./ -DCMAKE_INSTALL_BINDIR=./

DEFAULT_GOAL := all

.PHONY: all
.NOTPARALLEL: re # Because the pc is gonna die if we dont
all: zappy_server zappy_gui zappy_ai

.PHONY: zappy_server
zappy_server:
	cmake -S server -B .build/server ${CMAKEFLAGS}
	cmake --build .build/server --target install ${MAKEFLAGS}

.PHONY: zappy_gui
zappy_gui:
	cmake -S GUI -B .build/gui ${CMAKEFLAGS}
	cmake --build .build/gui --target install ${MAKEFLAGS}

# TODO
.PHONY: zappy_ai
zappy_ai:
	cmake -S ai -B .build/ai ${CMAKEFLAGS}
	cmake --build .build/ai --target install ${MAKEFLAGS}

.PHONY: clean
clean:
	rm -rf .build lib64 include

.PHONY: re
.NOTPARALLEL: re
re: clean all
