# Contrinbuting Using VSCode
1. **Prerequisites**
    * **C++ Compiler:** Install [Build Tools for Visual Studio 2022](https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2022). Instead of the "Workloads" tab, click on the "Individual components" tab at the top. Install the following four components:
        * **MSVC v143 - VS 2022 C++ x64/x86 build tools (Latest)**
        * **Windows 11 SDK** (This is preferred and works for both windows 10 and 11)
        * **C++ CMake tools for Windows**
        * **Just-In-Time debugger**
    * **VSCode:** Install the [latest version](https://code.visualstudio.com/download).
    * **VSCode Extensions:** Install the [C/C++ Extension Pack](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools-extension-pack) and [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools).
    * **Powershell (Min version: v7.2.24):** Run `$PSVersionTable` in Powershell to check your current version. If necessary, install the latest stable release of [PowerShell](https://github.com/PowerShell/PowerShell/releases).
2. **Ninja**
    * **Download** the latest release from their [GitHub](https://github.com/ninja-build/ninja/releases/tag/v1.12.1) (`ninja-win.zip`).
    * **Extract** and place `ninja.exe` in a permanent folder for command-line tools. I chose `C:\tools`, but you can use any folder you want. 
    * **Add the path** of this folder to the systems PATH environment variable.
        * Press the windows key, type `env`, and select **"Edit the system environment variables"**.
        * Click the **"Environment Variables..."** button.
        * In the top "User variables box, select the `Path` variable and click **"Edit..."**.
        * Click **"New"** and enter the exact path to the folder where you put `ninja.exe`. In my case this was `C:\tools`.
        * **Verify** it is working by closing all terminals (including VSCode's), then in a new terminal typing `ninja --version`. If it prints a version number you're all set.
3. **vcpkg (C++ Library Manager)**
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
4. **Subsequent VSCode Launches**
    * You can swap between Debug and Release through a dropdown menu in the "Run and Debug" tab (`Ctrl+Shift+D`). Press `F7` to rebuild the exe then `F5` to run it.