# Contributing to Flipper Pineapple Desktop

Thank you for your interest in contributing to the Flipper Pineapple Desktop project!

## Development Setup

```bash
# Clone the repository
git clone https://github.com/bad-antics/flipper-pineapple-desktop.git
cd flipper-pineapple-desktop

# Create build directory
mkdir build
cd build

# Configure CMake
cmake ..

# Build the project
cmake --build . --config Release
```

## Build Requirements
- CMake 3.10+
- GCC or Clang compiler
- Windows MinGW (for Windows builds)
- Standard C library

## Code Standards
- Follow K&R C style conventions
- Use meaningful variable and function names
- Comment complex logic
- Keep functions focused and modular
- Test on Windows, macOS, and Linux

## Pull Request Process

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test the build on your platform
5. Commit with clear messages
6. Push to your fork
7. Open a Pull Request

## Reporting Issues

Please include:
- Platform and compiler version
- Build error messages
- Steps to reproduce runtime issues
- Expected vs actual behavior

Thank you for contributing!
