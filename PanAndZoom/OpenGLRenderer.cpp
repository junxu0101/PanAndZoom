#include "pch.h"
#include "OpenGLRenderer.h"
#include <gdiplus.h>
#include <vector>

// =============================================================================
// OpenGLRenderer Implementation
// =============================================================================

OpenGLRenderer::OpenGLRenderer()
    : m_hWnd(nullptr)
    , m_viewportWidth(0)
    , m_viewportHeight(0)
    , m_zoomFactor(1.0f)
    , m_zoomCenter(0, 0)
{
}

OpenGLRenderer::~OpenGLRenderer()
{
    Shutdown();
}

bool OpenGLRenderer::Initialize(HWND hWnd)
{
    m_hWnd = hWnd;

    if (!m_context.Create(hWnd))
        return false;

    if (!m_context.MakeCurrent())
        return false;

    SetupOpenGL();

    CheckRenderingMode();

    // Get initial viewport size
    RECT rect;
    GetClientRect(hWnd, &rect);
    OnResize(rect.right - rect.left, rect.bottom - rect.top);

    return true;
}

void OpenGLRenderer::Shutdown()
{
    m_texture.Unload();
    m_context.Destroy();
    m_hWnd = nullptr;
}

bool OpenGLRenderer::LoadImage(const CString& filePath)
{
    if (!m_context.IsValid())
        return false;

    if (!m_context.MakeCurrent())
        return false;

    return m_texture.LoadFromFile(filePath);
}

void OpenGLRenderer::UnloadImage()
{
    if (m_context.MakeCurrent())
    {
        m_texture.Unload();
    }
}

CSize OpenGLRenderer::GetImageSize() const
{
    if (!m_texture.IsValid()) return CSize(0, 0);

    int zoomedWidth = (int)(m_texture.GetWidth() * m_zoomFactor);
    int zoomedHeight = (int)(m_texture.GetHeight() * m_zoomFactor);

    return CSize(zoomedWidth, zoomedHeight);
}

bool OpenGLRenderer::HasImage() const
{
    return m_texture.IsValid();
}

void OpenGLRenderer::SetZoom(float zoomFactor)
{
    m_zoomFactor = max(GetMinZoom(), min(GetMaxZoom(), zoomFactor));
}

float OpenGLRenderer::GetZoom() const
{
    return m_zoomFactor;
}

void OpenGLRenderer::SetZoomCenter(const CPoint& center)
{
    m_zoomCenter = center;
}

CPoint OpenGLRenderer::GetZoomCenter() const
{
    return m_zoomCenter;
}

float OpenGLRenderer::GetMinZoom() const
{
	CSize imageSize = GetImageSize();

    // Calculate minimum zoom to fit image in a reasonable size (e.g., 100x100)
    float minZoomX = 100.0f / imageSize.cx;
    float minZoomY = 100.0f / imageSize.cy;
    return min(1.0f, max(0.1f, max(minZoomX, minZoomY))); // At least 10% of original but no more than the orignal size
}

float OpenGLRenderer::GetMaxZoom() const
{
    return 2.0f; // 200% as specified
}

void OpenGLRenderer::Render(CDC* pDC, const CRect& clientRect, const CPoint& scrollPos)
{
    if (!m_context.IsValid() || !m_texture.IsValid())
        return;

    if (!m_context.MakeCurrent())
        return;

    // Clear the buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Set up orthographic projection to match Windows coordinates exactly
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // This makes (0,0) top-left, Y increases downward like Windows
    glOrtho(0, m_viewportWidth, m_viewportHeight, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Simple scroll translation - just like GDI+
    glTranslatef(-scrollPos.x, -scrollPos.y, 0);

    // Enable texturing
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture.GetTextureID());

    // Render the textured quad
    RenderQuad();

    glDisable(GL_TEXTURE_2D);

    // Swap buffers
    m_context.SwapBuffers();
}

void OpenGLRenderer::OnResize(int width, int height)
{
    m_viewportWidth = width;
    m_viewportHeight = height;

    if (m_context.IsValid() && m_context.MakeCurrent())
    {
        SetupViewport();
    }
}

CString OpenGLRenderer::GetRendererName() const
{
    return _T("OpenGL (Hardware)");
}

bool OpenGLRenderer::SupportsHardwareAcceleration() const
{
    return true;
}

void OpenGLRenderer::SetupOpenGL()
{
    // Set clear color to white
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // Enable smooth shading
    glShadeModel(GL_SMOOTH);

    // Disable depth testing for 2D rendering
    glDisable(GL_DEPTH_TEST);

    // Enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    CheckGLError("SetupOpenGL");
}

void OpenGLRenderer::SetupViewport()
{
    glViewport(0, 0, m_viewportWidth, m_viewportHeight);
    CheckGLError("SetupViewport");
}

void OpenGLRenderer::RenderQuad()
{
    float width = (float)m_texture.GetWidth() * m_zoomFactor;
    float height = (float)m_texture.GetHeight() * m_zoomFactor;

    glBegin(GL_QUADS);
    // Standard texture coordinates (no flipping)
    glTexCoord2f(0.0f, 0.0f); glVertex2f(0.0f, 0.0f);      // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex2f(width, 0.0f);     // Bottom-right  
    glTexCoord2f(1.0f, 1.0f); glVertex2f(width, height);   // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex2f(0.0f, height);    // Top-left
    glEnd();
}

bool OpenGLRenderer::CheckGLError(const char* operation)
{
    GLenum error = glGetError();
    if (error != GL_NO_ERROR)
    {
        CString msg;
        msg.Format(_T("OpenGL Error in %s: 0x%x"), CString(operation), error);
        OutputDebugString(msg);
        return false;
    }
    return true;
}

void OpenGLRenderer::CheckRenderingMode()
{
    if (m_context.MakeCurrent())
    {
        const char* renderer = (const char*)glGetString(GL_RENDERER);
        const char* vendor = (const char*)glGetString(GL_VENDOR);

        CString info;
        info.Format(_T("OpenGL Renderer: %s\nVendor: %s"),
            CString(renderer), CString(vendor));

        OutputDebugString(info);
    }
}

// =============================================================================
// OpenGLContext Implementation
// =============================================================================

OpenGLRenderer::OpenGLContext::OpenGLContext()
    : m_hWnd(nullptr)
    , m_hDC(nullptr)
    , m_hRC(nullptr)
{
}

OpenGLRenderer::OpenGLContext::~OpenGLContext()
{
    Destroy();
}

bool OpenGLRenderer::OpenGLContext::Create(HWND hWnd)
{
    m_hWnd = hWnd;
    m_hDC = GetDC(hWnd);

    if (!m_hDC)
        return false;

    if (!SetupPixelFormat())
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = nullptr;
        return false;
    }

    m_hRC = wglCreateContext(m_hDC);
    if (!m_hRC)
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = nullptr;
        return false;
    }

    return true;
}

void OpenGLRenderer::OpenGLContext::Destroy()
{
    if (m_hRC)
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_hRC);
        m_hRC = nullptr;
    }

    if (m_hDC)
    {
        ReleaseDC(m_hWnd, m_hDC);
        m_hDC = nullptr;
    }

    m_hWnd = nullptr;
}

bool OpenGLRenderer::OpenGLContext::MakeCurrent()
{
    return wglMakeCurrent(m_hDC, m_hRC) == TRUE;
}

void OpenGLRenderer::OpenGLContext::SwapBuffers()
{
    ::SwapBuffers(m_hDC);
}

bool OpenGLRenderer::OpenGLContext::SetupPixelFormat()
{
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,                              // version
        PFD_DRAW_TO_WINDOW |           // support window
        PFD_SUPPORT_OPENGL |           // support OpenGL
        PFD_DOUBLEBUFFER,              // double buffered
        // REMOVED: PFD_GENERIC_FORMAT  // Remove this line!
        PFD_TYPE_RGBA,                 // RGBA type
        24,                            // 24-bit color depth
        0, 0, 0, 0, 0, 0,             // color bits ignored
        0,                            // no alpha buffer
        0,                            // shift bit ignored
        0,                            // no accumulation buffer
        0, 0, 0, 0,                   // accum bits ignored
        16,                           // 16-bit z-buffer
        0,                            // no stencil buffer
        0,                            // no auxiliary buffer
        PFD_MAIN_PLANE,               // main layer
        0,                            // reserved
        0, 0, 0                       // layer masks ignored
    };

    int pixelFormat = ChoosePixelFormat(m_hDC, &pfd);
    if (pixelFormat == 0)
        return false;

    return SetPixelFormat(m_hDC, pixelFormat, &pfd) == TRUE;
}

// =============================================================================
// Texture Implementation
// =============================================================================

OpenGLRenderer::Texture::Texture()
    : m_textureID(0)
    , m_width(0)
    , m_height(0)
{
}

OpenGLRenderer::Texture::~Texture()
{
    Unload();
}

bool OpenGLRenderer::Texture::LoadFromFile(const CString& filePath)
{
    Unload();

    // Try loading with GDI+ for better format support
    if (LoadWithGDIPlus(filePath))
        return true;

    // Fallback to bitmap loading
    return LoadBitmap(filePath);
}

void OpenGLRenderer::Texture::Unload()
{
    if (m_textureID != 0)
    {
        glDeleteTextures(1, &m_textureID);
        m_textureID = 0;
    }
    m_width = 0;
    m_height = 0;
}

bool OpenGLRenderer::Texture::LoadWithGDIPlus(const CString& filePath)
{
    // Load image using GDI+
    CStringW widePathName(filePath);
    Gdiplus::Bitmap bitmap(widePathName);

    if (bitmap.GetLastStatus() != Gdiplus::Ok)
        return false;

    m_width = bitmap.GetWidth();
    m_height = bitmap.GetHeight();

    // Lock bitmap data
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, m_width, m_height);

    if (bitmap.LockBits(&rect, Gdiplus::ImageLockModeRead,
        PixelFormat32bppARGB, &bitmapData) != Gdiplus::Ok)
        return false;

    // Generate OpenGL texture
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);

    // Upload texture data - back to BGRA for hardware OpenGL
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0,
        GL_BGRA_EXT, GL_UNSIGNED_BYTE, bitmapData.Scan0);

    // Restore linear filtering for smooth scaling
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

    bitmap.UnlockBits(&bitmapData);

    return glGetError() == GL_NO_ERROR;
}

bool OpenGLRenderer::Texture::LoadBitmap(const CString& filePath)
{
    // Simple BMP loading implementation
    // This is a fallback - you might want to expand this
    HBITMAP hBitmap = (HBITMAP)::LoadImage(nullptr, filePath, IMAGE_BITMAP,
        0, 0, LR_LOADFROMFILE);
    if (!hBitmap)
        return false;

    BITMAP bmp;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);

    m_width = bmp.bmWidth;
    m_height = bmp.bmHeight;

    // Note: This is a simplified implementation
    // For production, need to handle different bit depths and formats

    DeleteObject(hBitmap);
    return false; // Not fully implemented
}