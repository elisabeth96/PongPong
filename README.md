[![Build Status](https://github.com/elisabeth96/PongPong/workflows/Build/badge.svg)](https://github.com/eli96/PongPong/actions?query=workflow%3ABuild)
# PongPong  
PongPong is a Pong clone. At the moment it is a bit buggy but you can play endlessly in 2-player-mode.

## Controls
The left player uses the keys 'a' and 'd' to move the paddle up and down. The right player uses the left and right arrow keys.
## Installation
To build PongPong you need to clone the repository and its submodules.  
```
git clone --recurse-submodules https://github.com/elisabeth96/PongPong.git
```
Then you can build it with CMake.  
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j
``` 

