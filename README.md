# Sahar's Platformer
## Editing on VSCode
1. **C++ Compiler**
    * Download [Build Tools for Visual Studio 2022](https://visualstudiomicrosoft.com/downloads/#build-tools-for-visual-studio-2022)
    * You must select **"Desktop development with C++"** during installation. This will install the compiler and all the necessary libraries.
2. **VSCode**
    * Download and install the latest version of **VSCode** if you don't have it installed already.
3. **VSCode Extensions**
    * Launch VSCode and install the following extensions:
        * [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack)
        * [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)
4. **Powershell**
    * Vcpkg requires Powershell v7.2.24 at minimum. Run `$PSVersionTable` in Powershell to check your current version.
    * If necessary, download and install the latest stable release of [PowerShell](https://github.com/PowerShell/PowerShell/releases).
5. **vcpkg (C++ Library Manager)**
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
    * For example, f your path is `D:\dev\vcpkg`, the line should be `"D:/dev/vcpkg/scripts/buildsystems/vcpkg.cmake"`
6. **First Launch**
    * To prepare the project for both debugging and release, you must build each version once. This is required so the debugger can find the exe files.
        * Open the command palette `` 
    