#include "pch.h"
#include "IImageRenderer.h"
#include "GDIPlusRenderer.h"
#include "OpenGLRenderer.h"

std::unique_ptr<IImageRenderer> RendererManager::CreateRenderer(RendererType type) {

    std::unique_ptr<OpenGLRenderer> openGLRendererPtr = nullptr;

    switch (type)
    {
    case RendererType::GDIPlus:
        return std::make_unique<GDIPlusRenderer>();

    //case RendererType::Direct3D:
	// Not implemented in this example
    //    if (IsDirectXAvailable())
    //    {
    //        // return std::make_unique<Direct3DRenderer>();
    //    }
    //    // Fallback to GDI+ if DirectX not available
    //    return std::make_unique<GDIPlusRenderer>();

    case RendererType::OpenGL:
         return std::make_unique<OpenGLRenderer>();

    default:
        return std::make_unique<GDIPlusRenderer>();
    }
}

CString RendererManager::GetRendererDisplayName(RendererType type)
{
    switch (type)
    {
    case RendererType::GDIPlus:
        return _T("GDI+ (Software)");
    case RendererType::Direct3D:
        return _T("Direct3D (Hardware)");
    case RendererType::OpenGL:
        return _T("OpenGL (Hardware)");
    default:
        return _T("Unknown");
    }
}