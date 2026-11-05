# Third-Party Software and Licenses

This project (`cppforge`) is licensed under the MIT License. It incorporates and depends on third-party software components licensed under various open-source licenses as described below.

---

## 1. Qt 6 Framework
- **License**: GNU Lesser General Public License version 3 (LGPLv3) / GNU General Public License version 3 (GPLv3)
- **Copyright**: The Qt Company Ltd. and other contributors (https://www.qt.io)
- **Used Modules**: QtCore, QtGui, QtWidgets, QtQuick, QtQml, QtQuickControls2, QtSql, QtNetwork, QtConcurrent, QtTest
- **Dynamic Linking**: Qt libraries are dynamically linked (`.dll` on Windows, `.so` on Linux, `.dylib` on macOS). In accordance with the LGPLv3, users are entitled to replace or relink these dynamic libraries with customized or modified versions.
- **Source Code**: The source code of Qt is available at [https://code.qt.io/cgit/](https://code.qt.io/cgit/) or [https://download.qt.io](https://download.qt.io).
- **License Texts**: See `LGPL-3.0.txt` and `GPL-3.0.txt` in this directory.

---

## 2. MinGW-w64 GCC Toolchain & Binutils (Windows Distribution)
- **License**: GNU General Public License version 3 (GPLv3) / GCC Runtime Library Exception
- **Copyright**: Free Software Foundation, Inc. and MinGW-w64 contributors
- **Usage**: On Windows, the binary distribution bundles standalone binaries (`gcc.exe`, `g++.exe`, `ld.exe`, `mingw32-make.exe`) and runtime libraries for offline C++ code compilation. These binaries run as independent sub-processes (*mere aggregation*) and do not link with the `cppforge` application binary.
- **Source Code**: The source code for the MinGW-w64 toolchain and GCC packages can be obtained from the MSYS2 repository: [https://github.com/msys2/MINGW-packages](https://github.com/msys2/MINGW-packages) and the GNU project: [https://ftp.gnu.org/gnu/gcc/](https://ftp.gnu.org/gnu/gcc/).
- **License Text**: See `GPL-3.0.txt` in this directory.

---

## 3. Crow C++ Framework
- **License**: Apache License 2.0 / BSD 3-Clause License
- **Copyright**: CrowCpp Team and contributors (https://crowcpp.org)
- **Source Code**: [https://github.com/CrowCpp/Crow](https://github.com/CrowCpp/Crow)

---

## 4. Asio C++ Library
- **License**: Boost Software License 1.0 (BSL-1.0)
- **Copyright**: Christopher M. Kohlhoff (https://think-async.com/Asio/)
- **Source Code**: [https://github.com/chriskohlhoff/asio](https://github.com/chriskohlhoff/asio)

---

## 5. PostgreSQL C Client Library (`libpq`)
- **License**: PostgreSQL License (permissive, MIT/BSD-style)
- **Copyright**: The PostgreSQL Global Development Group
- **Source Code**: [https://www.postgresql.org/ftp/source/](https://www.postgresql.org/ftp/source/)
