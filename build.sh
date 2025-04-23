#!/usr/bin/env sh

# Syllogist Build System

### Argument Default Values

BUILD=true

GUI=false

WIN=false

RUN=false

VALGRIND=false

FILE="arguments.txt"

### Arguments Checker

while [ "$#" -gt 0 ];
do
    case ${1} in
    -n|--no_build)
        BUILD=false ;;

    -w|--windows)
        WIN=true ;;

    -g|--gui)
        GUI=true ;;

    -r|--run)
        RUN=true
        if [ -n "$2" ]; then
            case $2 in
                -*) ;;
                *) 
                    FILE="$2"
                    shift
                    ;;
            esac
        fi
        ;;

    -v|--valgrind)
        VALGRIND=true
        if [ -n "$2" ]; then
            case $2 in
                -*) ;; 
                *) 
                    FILE="$2"
                    shift
                    ;;
            esac
        fi
        ;;

    *)
        echo "Invalid option: $1"
        exit 1 ;;
    esac
    shift
done

if [ "${RUN}" = true ] && [ "${VALGRIND}" = true ]
then
    echo "Error: Either --run or --valgrind can be set at a time"
    exit 1
fi

if [ "${BUILD}" = false ] && [ "${GUI}" = false ] && [ "${RUN}" = false ] && [ "${VALGRIND}" = false ] && [ "${WIN}" = false ]
then
    echo "Error: No flags set"
    exit 1
fi

### Compute

if [ "${BUILD}" = true ]
then
    echo "Building..."
    g++ -Wall -Wextra -ggdb -o build/syllogist src/*.cpp
fi

if [ "${GUI}" = true ]
then 
    echo "Building Gui..."
    javac -d build gui/SyllogistGUI.java 
fi

if [ "${WIN}" = true ]
then
    echo "Building for Windows..."
    x86_64-w64-mingw32-g++ -std=c++17 -Wall -Wextra -ggdb -o build/syllogist.exe src/*.cpp
fi

if [ "${RUN}" = true ]
then
    echo "Running on ${FILE}..."
    build/syllogist "${FILE}"
fi

if [ "${VALGRIND}" = true ]
then
    echo "Memory Check on ${FILE}..."
    valgrind build/syllogist "${FILE}"
fi