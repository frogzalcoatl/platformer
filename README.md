# Sahar's Platformer
This project uses Visual Studio 2022 and vcpkg for C++ package management via a vcpkg.json manifest file. If you want to test what I've completed so far using Visual Studio 2022, follow the instructions below.

## Prerequisites
1. **Git:** install from https://git-scm.com/downloads
2. **Visual Studio 2022:**
    * Install from https://visualstudio.microsoft.com/downloads/
    * Include the **"Desktop development with C++"** workload
    * Make sure **vcpkg package manager** is selected under "individual components" or within the C++ workload during installation.

## Setup
1. **Clone the repository**
```bash
git clone https://github.com/frogzalcoatl/platformer
cd platformer
```
2. **Open in Visual Studio 2022**
    * Open the solution file (.sln) in Visual Studio 2022.
3. **Automatic Dependency Installation**
    * Visual Studio will automatically detect vcpkg.json and install the required dependencies. This process might take some time during the first project open or build. You can monitor progress in Visual Studio's Output window.
4. **Build Project**
    * Once dependencies are installed, build the solution **(Build > Build Solution)**

## Project Configuration
This project is setup in Visual Studio to:

* **Use Vcpkg Manifest:** Yes
* **App-locally deploy DLLs:** Yes

This means dependencies listed in vcpkg.json will be automatically managed, and DLLs from those dependencies will be copied to your build output directory.