## Compile
```
# on graphics/assn1/src
g++ main.cpp base/object.cpp entities/player.cpp entities/canon.cpp entities/attack.cpp entities/enemy.cpp entities/bullet.cpp -o main -I. -I base -I entities -I ../../include -lGL -lGLEW -lglut
```