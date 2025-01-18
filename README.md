# 143B-File-System
Project 1 for CS 143B class.

## Shell Commands
The following commands are implemented for this project:
- cr <name>
  - create a new file with the name <name>
  - Output: <name> created
- de <name>
  - destroy the named file <name>
  - Output: <name> destroyed
- op <name>
  - open the named file <name> for reading and writing; display an index value
  - Output: <name> opened <index>
- cl <index>
  - close the specified file <index>
  - Output: <index> closed
- rd <index> <mem> <count>
  - copy <count> bytes from open file <index> (starting from current position) to memory M (starting at location M[<mem>])
  - Output: <n> bytes read from file <index> where n is the number of characters actually read (less or equal <count>)
- wr <index> <mem> <count>
  - copy <count> bytes from memory M (starting at location M[<mem>]) to open file <index> (starting from current position)
  - Output: <n> bytes written to file <index> where n is the number of characters actually written (less or equal <count>)
- sk <index> <pos>
  - seek: set the current position of the specified file <index> to <pos>
  - Output: position is <pos>
- dr
  - directory: list the names and lengths of all files
  - Output: <file0> <len1> <file1> <len2> ... <fileN> <lenN>
- in
  - initialize the system to the original starting configuration
  - Output: system initialized
- rm <mem> <count>
  - copy <count> bytes from memory M staring with position <mem> to output device (terminal or file)
  - Output: <xx...x> where each x is a character
- wm <mem> <str>
  - copy string <str> into memory M starting with position <mem> from input device (terminal or file)
  - Output: <n> bytes written to M where n is the length of <str>h
- If any command fails, output: error
