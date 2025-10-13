# Building Radon

## Platform Support

Radon itself supports platforms that fulfill the prerequisites below, but some dependencies may not.
In that case, you're on your own. Also, some build platforms, such as macOS, may require additional steps.

## Prerequisites

- CMake
- vcpkg
- A build system

## Steps

1. Clone vcpkg and run bootstrap-vcpkg.sh or bootstrap-vcpkg.bat (depends on system)
    - Clone vcpkg using `git clone https://github.com/microsoft/vcpkg --recursive`
    - Run the bootstrap-vcpkg script in the repository  
      > For Windows, run `bootstrap-vcpkg.bat`  
      > For Unix-like systems, run `bootstrap-vcpkg.sh`
2. Create a CMakeUserPresets.json file  
    > This depends on your configuration. For a starting point, take a look at [this guide](https://learn.microsoft.com/en-us/vcpkg/get_started/get-started-vs).
3. Build
    - Run build.sh, if on Unix-like systems  
      > Usage: `build.sh {user-defined preset name}`
    - Run `cmake --preset={user-defined preset name} {other options}`

## Known Issues

- When building on macOS, you'll need command line tools installed. A Google search should help you with this.
  Additionally, you may need to install Xcode and/or Homebrew. You can install Homebrew from [here](https://brew.sh/)
- Build errors related to compilation may occur. Wait for a commit to fix this, or submit an issue to the repo.