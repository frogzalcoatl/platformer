# Sahar's Platformer
## Editing on VSCode
1. **Prerequisites**
    * **C++ Compiler:** Install [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022). Instead of the "Workloads" tab, click on the "Individual components" tab at the top. Install the following four components:
        * **MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)**
        * **Windows 11 SDK** (This is compatible with windows 10 and preferred over the Windows 10 SDK)
        * **C++ CMake tools for Windows**
        * **Just-In-Time debugger**
    * **VSCode:** Install the [latest version](https://code.visualstudio.com/download).
    * **VSCode Extensions:** Install the [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools).
    * **Powershell (Min version: v7.2.24):** Run `$PSVersionTable` in Powershell to check your current version. If necessary, install the latest stable release of [PowerShell](https://github.com/PowerShell/PowerShell/releases).
2. **vcpkg (C++ Library Manager)**
    * Open a terminal and clone the vcpkg repository to `C:\vcpkg`. If you want to clone to a different location you can, but you will need to edit the project settings.json file.
    ```bash
    git clone https://github.com/microsoft/vcpkg.git C:\vcpkg
    ```
    * Run the bootstrap script to complete your installation:
    ```bash
    cd C:\vcpkg
    .\bootstrap-vcpkg.bat
    ```
    #### **For Users with a Different Vcpkg Location:**
    * In VSCode, open `.vscode/settings.json` and edit `CMAKE_TOOLCHAIN_FILE` to match your vcpkg installation.
    * For example, if your path is `D:\dev\vcpkg`, the line should be `"D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"`
3. **Inital VSCode Launch**
    * To prepare the project for both debugging and release, you must build each version once so the debugger can find the exe files.
        * Open the command palette (`Ctrl+Shift+P`) and run `CMake: Select a Varient`.
        * Choose `Debug`.
        * Then build by pressing `F7`.
        * Do the same for the `Release` varient.
4. **Subsequent Launches**
    * You will be able to swap between Debug and Release through the status bar at the bottom of the VSCode window. Pressing `F5` automatically rebuilds the necessary files and runs the exe.