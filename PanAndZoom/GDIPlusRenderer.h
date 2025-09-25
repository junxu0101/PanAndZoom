#pragma once
#include "IImageRenderer.h"
#include <gdiplus.h>

class GDIPlusRenderer : public IImageRenderer
{
public:
    GDIPlusRenderer();
    virtual ~GDIPlusRenderer();

    // IImageRenderer implementation
    bool Initialize(HWND hWnd) override;
    void Shutdown() override;

    bool LoadImage(const CString& filePath) override;
    void UnloadImage() override;
    CSize GetImageSize() const override;
    bool HasImage() const override;

    void Render(CDC* pDC, const CRect& clientRect, const CPoint& scrollPos) override;
    void OnResize(int width, int height) override;

    CString GetRendererName() const override;
    bool SupportsHardwareAcceleration() const override;

	// Zoom support
    void SetZoom(float zoomFactor) override;
    float GetZoom() const override;
    void SetZoomCenter(const CPoint& center) override;
    CPoint GetZoomCenter() const override;
    float GetMinZoom() const override;
	float GetMaxZoom() const override;

private:
    Gdiplus::Image* m_pImage;
    CSize m_imageSize;
    HWND m_hWnd;

    // Zoom functionality
    float m_zoomFactor;
    CPoint m_zoomCenter;

    static ULONG_PTR s_gdiplusToken;
    static int s_refCount;
};