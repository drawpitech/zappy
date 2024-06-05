zappy_server:
	cmake -S server -B .build
	cmake --build .build -j

zappy_gui:
	cmake -S GUI -B .build
	cmake --build .build -j

zappy_ai:
	cmake -S ai -B .build
	cmake --build .build -j
