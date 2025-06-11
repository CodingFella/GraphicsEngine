# Graphi.c
A graphics engine. Yipee!

## Demos
requires python!!
```bash
zig build serve-tri
zig build serve-cube
zig build serve-rubix
```

## Testing
Testing is done by comparing ppm files to a reference to test for breaking
changes. Its does not validate correctness just consistency.
```bash
zig build test
```
