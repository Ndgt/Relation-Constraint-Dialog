[README - 日本語](/README-jp.md) | [DeepWiki](https://deepwiki.com/Ndgt/Relation-Constraint-Dialog)

# Relation Constraint Dialog

This MotionBuilder plugin provides a convenient search dialog for quickly creating objects in the Relation Constraint.

<p align = "center">
<img src="docs/images-readme/demo.gif"><br>
</p>

<br>

## Platform
- **MotionBuilder: 2020 ~ 2026**

    **Note**:  Use with MotionBuilder 2020 is not recommended (See [Notification about Building plugin with MotionBuilder 2020](#notification-about-building-plugin-with-motionbuilder-2020) for details).

- **OS: Windows**

<br>

## Intended Usage & Limitations
> [!CAUTION]
> This plugin is implemented based on the internal specifications of MotionBuilder, which are not officially supported by the MotionBuilder SDK. It has been developed by leveraging the Qt libraries, based on the assumption that the MotionBuilder UI is built on Qt. However, these internal specifications are subject to change without notice in future updates. Please use this plugin at your own risk.

It is **not recommended** for use in **production environments**, such as:

- Live performances or real-time operation

Instead, please use this plugin only for:

- Offline editing or pre-setup tasks

**For safety, please remove installed plugin from the `plugins` folder in your production environment**.


<br>

## Installation

1. Download the latest release from the [Releases](https://github.com/Ndgt/Relation-Constraint-Dialog/releases) page

2. Extract the downloaded archive

3. Identify the product version of your software

   (e.g., use `...-MB2026.dll` if you're using MotionBuilder 2026)

4. Copy the corresponding `.dll` file into the default plugin folder:
   `MotionBuilder <version>/bin/x64/plugins`

    (**Note**: You may need administrator privileges to copy files to the `plugins` folder.)

<br>

## Documentation

See the DeepWiki page for detailed documentation: [DeepWiki](https://deepwiki.com/Ndgt/Relation-Constraint-Dialog)

You can build the documentation locally using [Doxygen](https://www.doxygen.nl/) if you have it installed. Run `doxygen` in the [docs/Doxygen](/docs/Doxygen/) folder, and refer to the generated `index.html` in the `html` folder.


<br>

## Usage

- **Tab** – Open search dialog / Toggle "Find Option"

    (**Note**: This control only works if **the mouse pointer is hovering over the relation view**.)

    <p align = "center">
    <img src="docs/images-readme/usage_show.gif" width=80%><br>
    </p>

- **Up/Down** – Navigate the suggest list

    <p align = "center">
    <img src="docs/images-readme/usage_select.gif" width=80%><br>
    </p>

- **Enter / Click** – Confirm selection and create object

    <p align = "center">
    <img src="docs/images-readme/usage_finalize.gif" width=80%><br>
    </p>

- **Esc / Click outside** – Cancel and close dialog

<br>

> [!TIP]
> **Press A while hovering over the relation view** if the relation object is not created at the expected position (at the mouse cursor). <br>
> Since this plugin monitors mouse interaction with the relation view to determine where to create the object, the calculation will result in an incorrect position if the view is "frozen" (i.e., dragged with the mouse but the view was not moved).

<br>

## Development

### Requirements
1. **Visual Studio Build Tools & "Desktop development with C++" workload**

    - MSVC v143 - VS 2022 C++ x64/x86 build tool : MotionBuilder 2024 ~
    - MSVC v142 - VS 2019 C++ x64/x86 build tool : MotionBuilder 2022, 2023
    - MSVC v141 - VS 2017 C++ x64/x86 build tool : MotionBuilder 2020

<br>

2. **Qt - qtbase(Required)**, **qttools**(Optional)

    This plugin depends on **qtbase** (specifically QtCore, QtGui, and QtWidgets modules), and utilizes the **qttools** module to design the UI using [Qt Widgets Designer](https://doc.qt.io/qt-6/qtdesigner-manual.html).
    - Qt 6.5.3:  MotionBuilder 2025, 2026
    - Qt 5.15.2 : MotionBuilder 2022 ~ 2024
    - Qt 5.12.5 : MotionBuilder 2020


    **Perl** is required for configuring the Qt source. Install with the following command:

    ```
    winget install StrawberryPerl.StrawberryPerl
    ```

<br>

3. **Detours**

    This project uses [Microsoft Detours](https://github.com/microsoft/Detours) to hook OpenGL functions.
    
    Set environment variable with `vcvarsall.bat` (see [Building Qt from Git](#building-qt-from-git) step 1), then clone and build Detours:

    ```
    git clone https://github.com/microsoft/Detours.git
    cd Detours/src
    git checkout v4.0.1
    nmake
    ```
    Header files will be located in `Detours/include`, and the library files will be in `Detours/lib.X64`. 

<br>


### Recommended Build Configurations

Click version number to see the official resources.

- Qt Source

    | Qt Version  | Visual Studio Version |
    |-------------|-----------------------|
    | Qt [6.5.3](https://doc.qt.io/qt-6.5/supported-platforms.html) | Visual Studio 2019 or 2022| 
    |  Qt [5.15.2](https://doc-snapshots.qt.io/qt5-5.15/supported-platforms.html) | Visual Studio 2019 |
    | Qt [5.12.5](https://wiki.qt.io/Qt_5.12_Tools_and_Versions) | Visual Studio 2017 | 

<br>

- MotionBuilder Plugin

    | MotionBuilder Version | Visual Studio Version |
    |-----------------------|-----------------------|
    | [2024](https://help.autodesk.com/view/MOBPRO/2024/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2025](https://help.autodesk.com/view/MOBPRO/2025/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2026](https://help.autodesk.com/view/MOBPRO/2026/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C) | Visual Studio 2022 |
    | [2022](https://help.autodesk.com/view/MOBPRO/2022/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C), [2023](https://help.autodesk.com/view/MOBPRO/2023/ENU/?guid=GUID-0C6F6BA3-E9A2-40D0-87AE-F4F8CF20A66C)| Visual Studio 2019 |
    | [2020](https://help.autodesk.com/view/MOBPRO/2020/ENU/?guid=__files_GUID_0C6F6BA3_E9A2_40D0_87AE_F4F8CF20A66C_htm) | Visual Studio 2017 |

<br>

### Building Qt from Git
1. Open a terminal and set the Visual Studio environment variables

    Use **`vcvarsall.bat`**, which is installed by default at  
    `C:/Program Files (x86)/Microsoft Visual Studio/2022/BuildTools/VC/Auxiliary/Build`.

    ```
    path/to/vcvarsall.bat x64 [-vcvars_ver=<version>]
    ```

    Use the option `-vcvars_ver=14.29` for VS2019, or `-vcvars_ver=14.16` for VS2017.  
    See the [Microsoft documentation](https://learn.microsoft.com/en-us/cpp/build/building-on-the-command-line?view=msvc-170#vcvarsall) for more details.


<br>

2. Get Source and checkout to the target tag (e.g. `v5.15.2`)

    ```
    git clone https://github.com/qt/qt5.git
    cd qt5
    git checkout v<version>  # Replace <version> with your target version
    git submodule update --init --recursive qtbase qttools
    ```

    **Note**: There is no "branch" `5.12.5` in the Qt/qt5 repository.

<br>

3. Configure Source

    ```
    cd ..
    mkdir build
    cd build
    ../qt5/configure.bat -opensource -confirm-license -release -nomake examples -nomake tests
    ```

<br>

4. Build and Install

    ```
    nmake
    nmake install
    ```

    In case Qt 6.5.3:
    ```
    cmake --build . --parallel
    cmake --install .
    ```

    By default, the built Qt source will be installed to: `C:/Qt/Qt-<version>`

<br>

### Customizing UI
Open `src/SearchDialog/SearchDialog.ui` with Qt Widgets Designer.

```
cd src/SearchDialog
path/to/Qt/bin/designer.exe SearchDialog.ui
```

<p align = "center">
<img src="docs/images-readme/qt_widgets_designer.png"><br>
</p>
<br>

After you've finished editing the UI, use **uic** (User Interface Compiler) to convert the `.ui` file to the header file `.h`.

```
path/to/Qt/bin/uic.exe SearchDialog.ui -o ui_SearchDialog.h
```

> [!Note]
> This process is not necessary, as the `AUTOUIC` CMake property automatically generates the header files from `.ui` files during the build process. Run this command only if you need it for features like code completion in your IDE.

<br>

### Building Plugin
1. Open the terminal **as an administrator**.

    See [Building Qt from Git](#building-qt-from-git) step 1 to set the Visual Studio environment variables.

<br>

2. Clone this repository

    ```
    git clone https://github.com/Ndgt/Relation-Constraint-Dialog.git
    cd Relation-Constraint-Dialog/src
    ```

<br>

3. Edit the user-specific variables in `CMakeLists.txt` according to your environment

    ```CMake
    # === Environment-specific user configuration ===
    set(PRODUCT_VERSION 2026)
    set(MOBU_ROOT "C:/Program Files/Autodesk/MotionBuilder ${PRODUCT_VERSION}")
    set(QT_SOURCE_SEARCH_PATH "C:/Qt/6.5.3/msvc2019_64")
    set(DETOURS_ROOT "C:/Detours")
    ```

<br>

4. Build plugin

    ```
    cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release
    cmake --build build
    ```

    The built plugin `.dll` file will be copied to the default folder `MotionBuilder <version>/bin/x64/plugins`.

<br>

### Notification about Building plugin with MotionBuilder 2020

If you use Visual Studio 2017 to build the plugin for MotionBuilder 2020, you may encounter the following warning `C4946` due to the limitations of casting:

```
error C4946: reinterpret_cast used between related classes : 'QMapNodeBase' and 'QMapNode<Key, T>'
```

To avoid this warning, the [released plugin](https://github.com/Ndgt/Relation-Constraint-Dialog/releases) for MotionBuilder 2020 **is built with Visual Studio 2019 instead**.

But note that the development environment for C++ in the MotionBuilder 2020 SDK is officially specified as Visual Studio 2017 only. Therefore, the plugin for MotionBuilder 2020 built with Visual Studio 2019 may not work correctly due to compatibility issues with the SDK.

<br>
<br>

## Dependencies

- This project uses the Qt framework (Community Edition) via dynamic linking. 
    
    The Qt components used in this project are licensed under the GNU Lesser General Public License version 3 (LGPLv3). The LGPLv3 license text is included in this repository ([LICENSES/Qt/lgpl-3.0.txt](/LICENSES/Qt/lgpl-3.0.txt)). You can also view the official license text [here](https://www.gnu.org/licenses/lgpl-3.0.en.html).

    For more information about Qt licensing, visit the [Qt Company licensing page](https://www.qt.io/qt-licensing).

<br>

- This project also uses [Microsoft Detours](https://github.com/microsoft/Detours).

     Detours is licensed under the MIT License. The MIT license text is included in this repository ([LICENSES/Detours/MIT.txt](/LICENSES/Detours/LICENSE.txt))

<br>

## License

This project is licensed under the BSD 3-Clause License.
See the [LICENSE](/LICENSE) file for full details.

Please note that while this project is BSD-licensed, the use of Qt is subject to LGPLv3 and Microsoft Detours is subject to the MIT License. Redistribution of the binary must comply with the terms of these licenses.