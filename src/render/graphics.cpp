#include "pch.h"
#include "Graphics.h"

static GraphicsState g_state;

void Graphics::DrawMesh()
{
}

void Graphics::DrawMeshInstanced()
{
}

void Graphics::SetRenderTarget()
{
}

void Graphics::SetPipelineStateObject(Pso* pso)
{
    g_state.pso = pso;

    if (g_state.isDrawingStarted)
    {

    }
}

void Graphics::SetConstantBuffers(int index)
{
}

void Graphics::SetViewBuffer(dynamicUploadBuffer* matrix)
{
    g_state.viewMatrix = matrix;

    if (g_state.isDrawingStarted)
    {
        //Update DynamicBuffer for view and proj
    }
}

void Graphics::SetProjBuffer(dynamicUploadBuffer* matrix)
{
    g_state.viewMatrix = matrix;

    if (g_state.isDrawingStarted)
    {
        //Update DynamicBuffer for view and proj
    }
}

void Graphics::SetDepthState(bool isEnabled)
{
}

void Graphics::BeginDrawing()
{
}

void Graphics::EndDrawing()
{
}

void Graphics::ExecuteCommandList()
{
}

UINT Graphics::GetRtvDescriptorSize()
{
    return 0;
}

UINT Graphics::GetDsvDescriptorSize()
{
    return 0;
}

UINT Graphics::GetCbvSrvUavDescriptorSize()
{
    return 0;
}

ID3D12Device* Graphics::GetDevice()
{
    return nullptr;
}

IDXGIFactory4* Graphics::GetFactory()
{
    return nullptr;
}

ID3D12CommandList* Graphics::GetCommandList()
{
    return nullptr;
}

void Graphics::AddConstantBufferToCommandList()
{
}

void Graphics::FlushCommandQueue()
{
}
