# Vincenzo

A lightweight, high-performance native Windows desktop framework built with modern C++20 and the Chromium Embedded Framework (CEF). The project demonstrates an advanced Off-Screen Rendering (OSR) pipeline integrated directly with Win32 Layered Windows and custom multi-process orchestration.

---

## Technical Stack & Architecture

### Core Technologies
- **Language**: C++20 (ISO/IEC 14882:2020) with MSVC x64 native toolchain (`/std:c++20`, `/O2`).
- **Runtime Linkage**: Static CRT (`/MT`) for complete self-containment with zero external VC++ redistributable dependencies.
- **Browser Subsystem**: Chromium Embedded Framework (CEF) in headless Off-Screen Rendering (OSR) mode.
- **Display & Compositing**: Win32 GDI / GDI+ with 32-bit ARGB Device-Independent Bitmaps (DIB) surfaced via `UpdateLayeredWindow`.
- **Subprocess Engine**: Multi-process Chromium architecture isolating main coordinator from rendering and network workers.
- **Managed Interop**: Optional C# CLR helper components (`AffManaged.dll`) compiled via Roslyn (`csc`).

### Process Architecture

```
+-------------------------------------------------------------+
|                     Main Process: quicksearch.exe           |
|  - Win32 Message Loop & Window Management                   |
|  - OSR Frame Composite Buffer (32-bit ARGB DIB)             |
|  - Native IPC & Process Watchdog                            |
+------------------------------+------------------------------+
                               |
            CEF Multi-Process  |
            Subprocess Spawn   |
                               v
+-------------------------------------------------------------+
|                   Subprocess: qshelper.exe                  |
|  - Chromium Renderer Process                                |
|  - V8 JavaScript Engine Execution                           |
|  - Chromium Network & Utility Worker Pipelines              |
+-------------------------------------------------------------+
```

---

## Repository Layout

```text
.
├── portable/                  # Compiled binaries and runtime environment
│   ├── quicksearch.exe        # Main process entry executable
│   ├── qshelper.exe           # CEF subprocess utility executable
│   ├── AffHook.dll            # Native runtime support library
│   ├── AffManaged.dll         # Managed interop assembly
│   ├── libcef.dll             # Chromium Embedded Framework shared library
│   ├── icudtl.dat             # ICU internationalization data
│   ├── *.pak                  # CEF resource bundles
│   └── locales/               # Localization pak files
├── src/                       # Application source code
│   ├── main.cpp               # Main entry point, window management & OSR loop
│   ├── subproc.cpp            # CEF subprocess handler
│   ├── affhook.cpp            # Native Win32 auxiliary module
│   └── affmanaged.cs          # Managed companion module
├── sdk/                       # External SDK definitions
│   └── cef/                   # CEF C++ API headers, libraries, and resources
├── thirdparty/                # Precompiled wrapper libraries (libcef_dll_wrapper.lib)
├── build.bat                  # Primary compiler script (MSVC x64)
├── build_wrapper.bat          # CMake/Ninja script to rebuild libcef_dll_wrapper
└── README.md
```

---

## Build Instructions

### Prerequisites
- Windows 10 / 11 (x64)
- Visual Studio 2022 or VS Build Tools (x64 Native Tools command prompt)
- Windows SDK (10.0.19041.0 or higher)

### Compiling from Source

1. Clone the repository:
   ```cmd
   git clone https://github.com/sowmiyan-s/Vincenzo.git
   cd Vincenzo
   ```

2. Run the build script:
   ```cmd
   build.bat
   ```

The script configures MSVC environment variables, links against the static C++ runtime (`/MT`), compiles `quicksearch.exe`, `qshelper.exe`, and supporting modules, and outputs all binaries into `portable\`.

### Rebuilding CEF C++ Wrapper (Optional)
If modifying CEF wrapper implementations, compile via CMake and Ninja:
```cmd
build_wrapper.bat
```

---

## Technical Details

- **Headless Pipeline**: Chromium graphics pipeline runs via software rasterization (`SwiftShader`), writing raw BGRA framebuffers directly into host memory via `CefRenderHandler::OnPaint`.
- **Compositing**: Host application blends incoming CEF paint buffers onto a layered top-level Win32 window surface (`WS_EX_LAYERED`).
- **Low-level Input Routing**: Input events are captured via standard Win32 input pipelines and dispatched into the browser instance using `CefBrowserHost::SendMouseMoveEvent` and `SendKeyEvent`.
