<h1 align="center">ModZero</h1>
<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-20-blue?style=flat-square&logo=c%2B%2B" alt="C++20">
  <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="License">
  <img src="https://img.shields.io/badge/protocol-Modbus-orange?style=flat-square" alt="Modbus">
</p>
<p align="center"><em>ModBus Exploration Tool — Industrial Control Systems Reconnaissance & Assessment</em></p>

---

ModZero is a **Modbus TCP exploration and security assessment tool** designed for industrial control systems (ICS/SCADA) security researchers and penetration testers. It connects to Programmable Logic Controllers (PLCs) and performs device identification, slave discovery, and memory dumping across all standard Modbus data types.

## Features

- **Device Reconnaissance** — Identify PLC vendor, product code, firmware revision, and more via Modbus FC 43 / MEI Type 0x0E (Read Device Identification)
- **Slave ID Brute-Forcing** — Scan slave IDs 1–247 to discover active devices behind Modbus gateways
- **Memory Dumping** — Read and display values from all four Modbus data areas:
  - Coils (digital outputs)
  - Discrete Inputs (digital inputs)
  - Input Registers (analog inputs)
  - Holding Registers (analog outputs)
- **Intelligent Boundary Detection** — Fallback from chunk reads to single-address probing when hitting memory limits, with contiguous range coalescing for compact output

## Getting Started

### Prerequisites

- **g++** with C++20 support
- **libmodbus** development library
- **TCLAP** (Templatized C++ Command Line Parser) — header-only

#### Install Dependencies (Ubuntu)

```bash
sudo apt install g++ libmodbus-dev libtclap-dev
```

### Build

```bash
git clone https://github.com/Try-H4ck-M3/modzero.git
cd modzero
./build.sh
```

The compiled binary will be at `bin/modzero`.

## Usage

```text
modzero -i <target_ip> [options] [action(s)]
```

### Arguments

| Argument | Description |
|---|---|
| `-i <ip>` | Target PLC IP address **(required)** |
| `-p <port>` | Modbus TCP port (default: `502`) |
| `-s <id>` | Modbus unit/slave ID (default: `1`) |
| `-t <ms>` | Connection timeout in milliseconds (default: `500`) |
| `-d <ms>` | Delay between Modbus packets in milliseconds (default: `100`) |
| `-b` | Brute-force slave IDs 1–247 |
| `-r` | Device identification (reconnaissance) |
| `-a` | Dump all memory types |
| `-c` | Dump coil values |
| `-n` | Dump discrete input values |
| `-e` | Dump input register values |
| `-g` | Dump holding register values |
| `-h` | Display help |

### Examples

**Basic device reconnaissance:**

```bash
./bin/modzero -i 192.168.1.100 -r
```

**Brute-force slave IDs:**

```bash
./bin/modzero -i 192.168.1.100 -b
```

**Dump all memory from a specific slave:**

```bash
./bin/modzero -i 192.168.1.100 -s 10 -a
```

## Project Structure

```bash
modzero/
├── build.sh              # Build script
├── bin/
│   └── modzero           # Compiled binary
└── src/
    ├── main.cpp          # Entry point and CLI orchestration
    ├── includes.h        # Central includes
    ├── logger.h/cpp      # Color-coded logging
    ├── recon.h/cpp       # Device identification & slave brute-force
    ├── read.h/cpp        # Memory dump functions
    ├── write.h/cpp       # Write stubs (not yet implemented)
    └── utils.h/cpp       # Hex formatting & ASCII art banner
```

## Roadmap

- [X] Device reconnaissance via Modbus FC 43 / MEI 0x0E
- [X] Slave ID brute-force (1–247)
- [X] Memory dumping — coils, discrete inputs, input registers, holding registers
- [X] Intelligent boundary detection with range coalescing
- [X] Color-coded timestamped logging
- [X] ASCII art banner
- [ ] Write functionality (coil/register write operations)
- [ ] Configurable address ranges
- [ ] Machine-readable output formats (JSON, CSV)
- [ ] CMake build system
- [ ] Perhaps Modbus RTU (serial) support?

## Disclaimer

This tool is intended for **authorized security assessments and research purposes only**. Unauthorized access to any computer or ICS is illegal. The author is not responsible for any misuse or damage caused by this tool. Use responsibly and only on systems you own or have explicit permission to test.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
