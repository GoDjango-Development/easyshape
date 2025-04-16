# EasyShape

**EasyShape** is a lightweight Linux tool that uses `tc` (Traffic Control) to
shape bandwidth between devices by specifying traffic limits per IP. It's ideal
for managing network performance in local environments or testing scenarios.

## Features

- Bandwidth shaping using Linux `tc`
- IP-to-IP traffic limit configuration
- Simple configuration file syntax
- Can be embedded as a library in custom C applications (excluding `main.c`)

## Usage

### 1. Build

```bash
make clean
make
```

This will compile the tool into the `release/` folder.

### 2. Run

```bash
release/eshp conf/example.conf
```

This runs EasyShape using the example configuration file.

### 3. Integrate as a Library

If you want to embed EasyShape into your own C application, simply remove or
exclude `main.c` and include the other source files in your build system.

## Configuration

The configuration file defines all IP devices and their bandwidth limits. See
the example file for the full syntax and options:

```
conf/example.conf
```

## Dependencies

- Linux operating system
- `tc` (usually comes with `iproute2` package)
- GCC or compatible C compiler

## License

MIT License

