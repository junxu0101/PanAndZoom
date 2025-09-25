#include "pch.h"

#include "GDIPlusRenderer.h"

ULONG_PTR GDIPlusRenderer::s_gdiplusToken = 0;
int GDIPlusRenderer::s_refCount = 0;

GDIPlusRenderer::GDIPlusRenderer()
    : m_pImage(nullptr)
    , m_imageSize(0, 0)
    , m_hWnd(nullptr)
    , m_zoomFactor(1.0f)
    , m_zoomCenter(0, 0)
{
}

GDIPlusRenderer::~GDIPlusRenderer()
{
    Shutdown();
}

bool GDIPlusRenderer::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    // Initialize GDI+ if this is the first instance
    if (s_refCount == 0)
    {
        Gdiplus::GdiplusStartupInput gdiplusStartupInput;
        Gdiplus::Status status = Gdiplus::GdiplusStartup(&s_gdiplusToken, &gdiplusStartupInput, NULL);
        if (status != Gdiplus::Ok)
            return false;
    }

    s_refCount++;
    return true;
}

void GDIPlusRenderer::Shutdown()
{
    UnloadImage();

    s_refCount--;
    if (s_refCount == 0 && s_gdiplusToken != 0)
    {
        Gdiplus::GdiplusShutdown(s_gdiplusToken);
        s_gdiplusToken = 0;
    }
}

bool GDIPlusRenderer::LoadImage(const CString& filePath)
{
    UnloadImage();

    CStringW widePathName(filePath);
    m_pImage = Gdiplus::Image::FromFile(widePathName);

    if (!m_pImage || m_pImage->GetLastStatus() != Gdiplus::Ok)
    {
        if (m_pImage)
        {
            delete m_pImage;
            m_pImage = nullptr;
        }
        return false;
    }

    m_imageSize = CSize(m_pImage->GetWidth(), m_pImage->GetHeight());
    return true;
}

void GDIPlusRenderer::UnloadImage()
{
    if (m_pImage)
    {
        delete m_pImage;
        m_pImage = nullptr;
    }
    m_imageSize = CSize(0, 0);
}

bool GDIPlusRenderer::HasImage() const
{
    return m_pImage != nullptr;
}

void GDIPlusRenderer::SetZoom(float zoomFactor)
{
    m_zoomFactor = max(GetMinZoom(), min(GetMaxZoom(), zoomFactor));
}

float GDIPlusRenderer::GetZoom() const
{
    return m_zoomFactor;
}

void GDIPlusRenderer::SetZoomCenter(const CPoint& center)
{
    m_zoomCenter = center;
}

CPoint GDIPlusRenderer::GetZoomCenter() const
{
    return m_zoomCenter;
}

float GDIPlusRenderer::GetMinZoom() const
{
    if (!m_pImage) return 1.0f;

    // Calculate minimum zoom to fit image in a reasonable size (e.g., 100x100)
    float minZoomX = 100.0f / m_imageSize.cx;
    float minZoomY = 100.0f / m_imageSize.cy;
	return min(1.0f, max(0.1f, max(minZoomX, minZoomY))); // At least 10% of original but no more than the orignal size
}

float GDIPlusRenderer::GetMaxZoom() const
{
    return 2.0f; // 200% as specified
}

CSize GDIPlusRenderer::GetImageSize() const
{
    if (!m_pImage) return CSize(0, 0);

    // Return zoomed size for scroll system
    int zoomedWidth = (int)(m_imageSize.cx * m_zoomFactor);
    int zoomedHeight = (int)(m_imageSize.cy * m_zoomFactor);
    return CSize(zoomedWidth, zoomedHeight);
}

void GDIPlusRenderer::Render(CDC* pDC, const CRect& clientRect, const CPoint& scrollPos)
{
    if (!m_pImage)
        return;

    Gdiplus::Graphics graphics(pDC->GetSafeHdc());

    // Draw at (0,0) with zoom - let CScrollView handle scroll translation
    int zoomedWidth = (int)(m_imageSize.cx * m_zoomFactor);
    int zoomedHeight = (int)(m_imageSize.cy * m_zoomFactor);

    graphics.DrawImage(m_pImage, 0, 0, zoomedWidth, zoomedHeight);
}

void GDIPlusRenderer::OnResize(int width, int height)
{
    // GDI+ doesn't need special resize handling
}

CString GDIPlusRenderer::GetRendererName() const
{
    return _T("GDI+ (Software)");
}

bool GDIPlusRenderer::SupportsHardwareAcceleration() const
{
    return false;
}