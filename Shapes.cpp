#include "Shapes.h"

Shapes::Shapes()
{
}

Shapes::~Shapes()
{
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pPyramidVertexBuffer) _pPyramidVertexBuffer->Release();
    if (_pPlaneVertexBuffer) _pPlaneVertexBuffer->Release();
}

void Shapes::InitCube()
{
}

void Shapes::InitPyramid()
{
}

void Shapes::InitPlane()
{
}

void Shapes::UpdateCube()
{
}

void Shapes::UpdatePyramid()
{
}

void Shapes::UpdatePlane()
{
}

void Shapes::DrawCube(ID3D11Buffer* pCubeIndexBuffer, ID3D11DeviceContext* pImmediateContext, ID3D11VertexShader* pVertexShader, ID3D11PixelShader* pPixelShader, ID3D11Buffer* pConstantBuffer, std::vector<XMFLOAT4X4> worldMatrices, int i, XMFLOAT4X4 view, XMFLOAT4X4 projection)
{
    // Set index and vertex buffer
        // Set vertex buffer
    UINT stride = sizeof(SimpleVertex);
    UINT offset = 0;
    
    pImmediateContext->IASetVertexBuffers(0, 1, &_pCubeVertexBuffer, &stride, &offset);
    pImmediateContext->IASetIndexBuffer(pCubeIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    XMMATRIX _world = XMLoadFloat4x4(&worldMatrices[i]);
    XMMATRIX _view = XMLoadFloat4x4(&view);
    XMMATRIX _projection = XMLoadFloat4x4(&projection);
    //
    // Update variables
    //
    ConstantBuffer cb;
    cb.mWorld = XMMatrixTranspose(_world);
    cb.mView = XMMatrixTranspose(_view);
    cb.mProjection = XMMatrixTranspose(_projection);

    pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);

    //
    // Renders a triangle
    //
    pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
    pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
    pImmediateContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
    pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
    pImmediateContext->DrawIndexed(36, 0, 0);
}

void Shapes::DrawPyramid()
{
    //// Set index and vertex buffer
    //    // Set vertex buffer
    //UINT stride = sizeof(SimpleVertex);
    //UINT offset = 0;

    //_pImmediateContext->IASetVertexBuffers(0, 1, &_pPyramidVertexBuffer, &stride, &offset);
    //_pImmediateContext->IASetIndexBuffer(_pPyramidIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

    //XMMATRIX world = XMLoadFloat4x4(&_worldMatrices[i]);
    //XMMATRIX view = XMLoadFloat4x4(&_view);
    //XMMATRIX projection = XMLoadFloat4x4(&_projection);
    ////
    //// Update variables
    ////
    //ConstantBuffer cb;
    //cb.mWorld = XMMatrixTranspose(world);
    //cb.mView = XMMatrixTranspose(view);
    //cb.mProjection = XMMatrixTranspose(projection);

    //_pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);

    ////
    //// Renders a triangle
    ////
    //_pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
    //_pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
    //_pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
    //_pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
    //_pImmediateContext->DrawIndexed(36, 0, 0);
}

void Shapes::DrawPlane()
{
    // Set index and vertex buffer
        // Set vertex buffer
//    UINT stride = sizeof(SimpleVertex);
//    UINT offset = 0;
//   
//    _pImmediateContext->IASetVertexBuffers(0, 1, &_pPlaneVertexBuffer, &stride, &offset);
//    _pImmediateContext->IASetIndexBuffer(_pPlaneIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
//
//    XMMATRIX world = XMLoadFloat4x4(&_worldMatrices[i]);
//    XMMATRIX view = XMLoadFloat4x4(&_view);
//    XMMATRIX projection = XMLoadFloat4x4(&_projection);
//    //
//    // Update variables
//    //
//    ConstantBuffer cb;
//    cb.mWorld = XMMatrixTranspose(world);
//    cb.mView = XMMatrixTranspose(view);
//    cb.mProjection = XMMatrixTranspose(projection);
//
//    _pImmediateContext->UpdateSubresource(_pConstantBuffer, 0, nullptr, &cb, 0, 0);
//
//    //
//    // Renders a triangle
//    //
//    _pImmediateContext->VSSetShader(_pVertexShader, nullptr, 0);
//    _pImmediateContext->VSSetConstantBuffers(0, 1, &_pConstantBuffer);
//    _pImmediateContext->PSSetConstantBuffers(0, 1, &_pConstantBuffer);
//    _pImmediateContext->PSSetShader(_pPixelShader, nullptr, 0);
//    _pImmediateContext->DrawIndexed(36, 0, 0);
}
