#pragma once
#include "IImageRenderer.h"
#include <windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

class OpenGLRenderer : public IImageRenderer
{
public:
    OpenGLRenderer();
    virtual ~OpenGLRenderer();

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

    void CheckRenderingMode();

private:
    // OpenGL context management
    class OpenGLContext
    {
    public:
        OpenGLContext();
        ~OpenGLContext();

        bool Create(HWND hWnd);
        void Destroy();
        bool MakeCurrent();
        void SwapBuffers();
        bool IsValid() const { return m_hRC != nullptr; }

    private:
        HWND m_hWnd;
        HDC m_hDC;
        HGLRC m_hRC;

        bool SetupPixelFormat();
    };

    // Texture management
    class Texture
    {
    public:
        Texture();
        ~Texture();

        bool LoadFromFile(const CString& filePath);
        void Unload();
        bool IsValid() const { return m_textureID != 0; }

        GLuint GetTextureID() const { return m_textureID; }
        int GetWidth() const { return m_width; }
        int GetHeight() const { return m_height; }

    private:
        GLuint m_textureID;
        int m_width;
        int m_height;

        bool LoadBitmap(const CString& filePath);
        bool LoadWithGDIPlus(const CString& filePath);
    };

    // Renderer state
    OpenGLContext m_context;
    Texture m_texture;
    HWND m_hWnd;
    int m_viewportWidth;
    int m_viewportHeight;

    // Zoom functionality
    float m_zoomFactor;
    CPoint m_zoomCenter;

    // Helper methods
    void SetupOpenGL();
    void SetupViewport();
    void RenderQuad();
    bool CheckGLError(const char* operation);
};