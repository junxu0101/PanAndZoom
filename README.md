# PanAndZoom

PanAndZoom is a Windows desktop application that demonstrates smooth image panning and zooming using both hardware-accelerated OpenGL and GDI+ rendering. The project is written in C++14 and is designed for use with Visual Studio 2022.

## Features

- **Image Loading:** Supports loading images from disk using GDI+ for broad format compatibility.
- **Hardware Acceleration:** Utilizes OpenGL for fast, smooth rendering and scaling.
- **Zoom & Pan:** Interactive zooming (10%–200%) and panning of images.
- **Renderer Switching:** Easily switch between OpenGL and GDI+ rendering modes.
- **Viewport Management:** Handles window resizing and maintains correct image aspect ratio.
- **Error Handling:** Outputs debug information for OpenGL errors and renderer details.

## Getting Started

### Prerequisites

- Visual Studio 2022
- Windows 10 or later
- C++14 compatible compiler
- [GDI+](https://learn.microsoft.com/en-us/windows/win32/gdiplus/-gdiplus-gdi-start) (included with Windows)

### Building

1. Open `PanAndZoom.sln` in Visual Studio 2022.
2. Build the solution (`Ctrl+Shift+B`).
3. Run the application (`F5`).

### Usage

1. Launch the application.
2. Use the menu or toolbar to open an image file.
3. Zoom in/out by dragging the image (press and hold mouse's left button then drag the mouse).
4. Pan using mouse wheel or scroll bars.
5. Switch between OpenGL and GDI+ renderers via the settings on menu.

## Project Structure

- `PanAndZoom.cpp` / `PanAndZoomView.cpp` / `PanAndZoomDoc.cpp`: Main application, document, and view logic.
- `OpenGLRenderer.cpp` / `OpenGLRenderer.h`: Hardware-accelerated image rendering using OpenGL.
- `GDIPlusRenderer.cpp` / `GDIPlusRenderer.h`: Software rendering using GDI+.
- `RendererManager.cpp`: Manages renderer selection and switching.
- `MainFrm.cpp`, `ChildFrm.cpp`: Window and frame management.
- `res/`: Application resources (icons, bitmaps).

## Technical Notes

- OpenGL context and texture management are handled in `OpenGLRenderer`.
- GDI+ is used for image loading to support formats like PNG, JPEG, BMP, etc.
- The minimum zoom ensures images are always visible (at least 10% of original size).
