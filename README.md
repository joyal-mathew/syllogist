# Syllogist

An automated proof generator for propositional logic.

## Background

Developed by Anthony Fabius, Joyal Mathew, and Charlie Chen as a final project for Computability and Logic at Rensselaer Polytechnic Institute in Spring 2025. Syllogist is a way to take formal propositional logic arguments, access their validity and construct a natural deduction fitch style proof. Syllogist also offers direct integration with other [Bramhub](https://github.com/Bram-Hub) projects [Aris](https://github.com/Bram-Hub/aris) and [Willow](https://github.com/Bram-Hub/Willow).

## Install

### Dependencies

GCC for compiling and running Syllogist

Java 11+ for compiling and running the GUI. Java 8.1 may also work but hasn't been tested.

### Build

To build Syllogist run

```bash
$ build.sh
# Builds Syllogist
```

To build Syllogist Gui run

```bash
$ build.sh --gui
# Builds Syllogist & Syllogist Gui
```

## Usage

### End User

For end users download the package containing the latest version of syllogist here (we havent made it yet...). There is a gui you can use to interact with the program or read the [Developers](#developers) section.

Input any formal logic argument into the input textbox and Syllogist will construct either a proof or counter proof for it. An example of an input would be

```text
A>B
A
B
```

See [examples/](/examples/) for other proofs.

The results of the argument are then put in [outputs/](/output/). If the argument is valid an Aris Proof as well as a plain text fitch proof is generated. If the argument is invalid a Willow file is generated. Regardless the truth tree to test the validity is also generated as a dot file.

Here is the table of characters to logic symbols for reference.

| Character | Logic Symbol |
|:---------:|:------------:|
|     ~     |       ¬      |
|     &     |       ∧      |
|     \|    |       ∨      |
|     >     |       →      |
|     =     |       ↔      |

### Developers

To make the process simple we've included a few flags to make building and testing Syllogist easier. To build and run Syllogist with an argument file run the following command.

```bash
$ build.sh --run arguments.txt
# Builds Syllogist and runs it on arguments.txt
```

We also have flags for not compiling (useful for repeated testing) and valgrind as `--no_build` and `--valgrind` respectively. For more information read [build.sh](build.sh).

## Contributing

PRs accepted.

## License

[MIT](LICENSE)
