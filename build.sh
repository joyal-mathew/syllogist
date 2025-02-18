#!/usr/bin/env sh

### Argument Default Values

BUILD=true

RUN=false

VALGRIND=false

### Arguments Checker

while [ "$#" -gt 0 ];
do
    case ${1} in
    -n|--no_build)
        BUILD=false ;;

    -r|--run)
        RUN=true ;;

    -v|--valgrind)
        VALGRIND=true ;;

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

if [ "${BUILD}" = false ] && [ "${RUN}" = false ] && [ "${VALGRIND}" = false ]
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

if [ "${RUN}" = true ]
then
    echo "Running..."
    build/syllogist arguments.txt
fi

if [ "${VALGRIND}" = true ]
then
    echo "Memory Check..."
    valgrind build/syllogist arguments.txt
fi