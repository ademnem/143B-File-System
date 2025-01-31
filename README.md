# 143B-File-System
Project 1 for CS 143B class.

## Requirements
- at least cmake 3.20.0
- g++

## How to Run (FOR GRADERS)
```{bash}
bash compile.sh
./build/main < FS-input-1.txt > output.txt
```


## How to Run
Build cmake depending on the OS
```{bash}
cmake --preset linux
cmake --prest windows
```

To build the binary, enter:
```{bash}
cmake --build build --target main
```

To execute the shell, enter:
```{bash}
./build/main
```

In order to test against the released results:
```{bash}
./build/main < FS-input-1.txt > output.txt
```
