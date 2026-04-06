# Graphical-Engine
A lightweight graphical engine built to demonstrate the fundamental mathematics of 3D-to-2D projection. This project bypasses high-level graphics APIs (OpenGL/DirectX) to implement the rendering pipeline manually in C++.

# Features
- Custom Rendering Pipeline: Implements manual Parallel and Perspective projection transformations.
- Mathematical Engine: Handles 3D transformations including arbitrary axis rotation, scaling about a point, and translation. 
- Cross-Language Architecture: Utilizes a C++ backend for performance-critical matrix math, interfaced with a Python/Tkinter frontend via ctypes.
- Dynamic Clipping & Viewport Mapping: Real-time calculation of view volumes, viewport scaling, and line clipping logic.

# Technical Stack
- Backend: C++20 (Matrix mathematics, Projection logic).
- Frontend: Python 3.12 (Tkinter GUI, Event handling).
- Interface: ctypes (FFI - Foreign Function Interface).

# Build and Installation
Prerequisites:
- Python 11+
- C++ Compiler (GCC/MinGW, Clang, or MSVC)
- Dependencies: pip install numpy
  
1. ### Compile the C++ Engine
   To generate the shared library, run the following command in your terminal:  
   Windows
   ```
   g++ -shared -o engine.dll src/renderer.cpp src/engine_api.cpp src/math.cpp -Iinclude -std=c++20
   ```
   Linux/macOS
   ```
   g++ -shared -fPIC -o engine.so src/renderer.cpp src/engine_api.cpp src/math.cpp -Iinclude -std=c++20
   ```
3. ### Run the Application
   Ensure the compiled .dll or .so file is in the root directory, then run:
   python app.py

# Project Structure
- src/renderer.cpp: Core logic for projections and clipping.
- src/math.cpp : Helper functions for matrix and vector multiplication
- app.py: Tkinter-based GUI and C++ library interfacing.
- sample/: Contains sample .txt files for cameras and 3D object vertices.

# Project Roadmap
### Phase 1: Graphical Core
- Transformations: Rotation, Scaling, and Translation
- Perspectives: Parallel and Projection
- Clipping
- Resolution <-- I am here
- Ray Tracing
### Phase 2: Physics Integration
- TBD
### Phase 3: Logic Engine
- TBD
