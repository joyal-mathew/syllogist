#!/usr/bin/env sh

### Argument Default Values

BUILD=true

RUN=false

VALGRIND=false

FILE="arguments.txt"

### Arguments Checker

while [ "$#" -gt 0 ];
do
    case ${1} in
    -n|--no_build)
        BUILD=false ;;

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
    echo "Running on ${FILE}..."
    build/syllogist "${FILE}"
fi

if [ "${VALGRIND}" = true ]
then
    echo "Memory Check on ${FILE}..."
    valgrind build/syllogist "${FILE}"
fi