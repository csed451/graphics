
## Compile Command
```bash
# on graphics/assn1/src
g++ main.cpp base/object.cpp entities/player.cpp entities/canon.cpp entities/attack.cpp entities/enemy.cpp entities/bullet.cpp \
    -o main -I. -I base -I entities -I ../../include -lGL -lGLEW -lglut

# Using WildCard
g++ main.cpp base/*.cpp entities/*.cpp -o main \
    -I. -Ibase -Ientities -I../../include \
    -lGL -lGLEW -lglut
```

- ``-I`` (Uppercase): Include Path

- ``-L`` (Uppercase): Library Path

- ``-l`` (Lowercase): Link Library