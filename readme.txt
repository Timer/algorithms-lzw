# General
This code is targeted for a **linx** machine, please run it on `knuth.cs.uakron.edu`.

# Build
The code can be built using `make clean && make`.

This will produce two binaries, `lzw435` and `lzw435M`.

# Execution
To execute the program, simply run `./lzw435[M] [c/e] file`.

Examples:
```
$ ./lzw435 c cases/bible.txt
```
```
$ ./lzw435 e cases/bible.txt.lzw
```

# Other
Compressed files get saved with extention `.lzw` or `.lzw2`.

Expanded files get saved with the original extension with `2` or `2M` appended, e.g. `.gif2`.
