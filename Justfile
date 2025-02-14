build:
    g++ -Wall -Wextra -ggdb -o build/syllogist src/*.cpp

run:
    build/syllogist arguments.txt

mem-check:
    valgrind build/syllogist arguments.txt
