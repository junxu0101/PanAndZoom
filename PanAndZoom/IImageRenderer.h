#pragma once

#include <memory>

class IImageRenderer
{
public:
    virtual ~IImageRenderer() = default;

    // Initialization
    virtual bool Initialize(HWND hWnd) = 0;
    virtual void Shutdown() = 0;

    // Image management
    virtual bool LoadImage(const CString& filePath) = 0;
    virtual void UnloadImage() = 0;
    virtual CSize GetImageSize() const = 0;
    virtual bool HasImage() const = 0;

    // Rendering
    virtual void Render(CDC* pDC, const CRect& clientRect, const CPoint& scrollPos) = 0;
    virtual void OnResize(int width, int height) = 0;

    // Capabilities
    virtual CString GetRendererName() const = 0;
    virtual bool SupportsHardwareAcceleration() const = 0;

    // Zoom support
    virtual void SetZoom(float zoomFactor) = 0;
    virtual float GetZoom() const = 0;
    virtual void SetZoomCenter(const CPoint& center) = 0;
    virtual CPoint GetZoomCenter() const = 0;
    virtual float GetMinZoom() const = 0;
    virtual float GetMaxZoom() const = 0;
};

// Renderer types
enum class RendererType
{
    GDIPlus,
    Direct3D,
    OpenGL
};

class RendererManager
{
public:
    // Factory methods
    static std::unique_ptr<IImageRenderer> CreateRenderer(RendererType type);
    static CString GetRendererDisplayName(RendererType type);
};
