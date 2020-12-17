#include "Application.h"
#include "DDSTextureLoader.h"
#include <time.h>

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    PAINTSTRUCT ps;
    HDC hdc;

    switch (message)
    {
        case WM_PAINT:
            hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

Application::Application()
{
	_hInst = nullptr;
	_hWnd = nullptr;
	_driverType = D3D_DRIVER_TYPE_NULL;
	_featureLevel = D3D_FEATURE_LEVEL_11_0;
	_pd3dDevice = nullptr;
	_pImmediateContext = nullptr;
	_pSwapChain = nullptr;
	_pRenderTargetView = nullptr;
	_pVertexShader = nullptr;
	_pPixelShader = nullptr;
	_pVertexLayout = nullptr;
    _pCubeVertexBuffer = nullptr;
    _pCubeIndexBuffer = nullptr;
    _pPyramidVertexBuffer = nullptr;
    _pPyramidIndexBuffer = nullptr;
	_pConstantBuffer = nullptr;
}

Application::~Application()
{
    for (int i = 0; i < 4; i++)
    {
        delete _cam[i];
        _cam[i] = nullptr;
    }
	Cleanup();
}

//initialise application
HRESULT Application::Initialise(HINSTANCE hInstance, int nCmdShow)
{
    //iniitalise window
    if (FAILED(InitWindow(hInstance, nCmdShow)))
	{
        return E_FAIL;
	}

    RECT rc;
    GetClientRect(_hWnd, &rc);
    _WindowWidth = rc.right - rc.left;
    _WindowHeight = rc.bottom - rc.top;

    if (FAILED(InitDevice()))
    {
        Cleanup();

        return E_FAIL;
    }
    
	// Initialize the game objects
    for (int i = 0; i < 108; i++)
    {
        sphere[i].Init(XMFLOAT3(0, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(5, 5, 5), _pd3dDevice, "assets/sphere.obj");
    }
    for (int i = 0; i < 3; i++)
    {
        wall[i].Init(XMFLOAT3(10+i, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(5, 5, 5), _pd3dDevice, "assets/wall.obj");
    }
    player.Init(    XMFLOAT3(40, 0, -5.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(5, 5, 5), _pd3dDevice, "assets/rocket.obj");
    plane.Init(     XMFLOAT3(40, 5, -5.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(5, 5, 5), _pd3dDevice, "assets/plane.obj");
    spaceDuck.Init( XMFLOAT3(25, 5, 0.0f), XMFLOAT3(0, 0, 0), XMFLOAT3(.5, .5, .5), _pd3dDevice, "assets/duck.obj");
    cube.Init(      XMFLOAT3(0, 5, -5.0f),  XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), _pd3dDevice, "assets/cube.obj");
    skybox.Init(    XMFLOAT3(25, 0, 0), XMFLOAT3(0, 0, 0), XMFLOAT3(1, 1, 1), _pd3dDevice, "assets/invertedSphere.obj");
    
    //sets camera to default at 3rd person
    _currentCam = 0;

    //initialise cameras
    //3rd person camera
    _cam[0] = new Camera();
    _cam[0]->LookAt(XMFLOAT3(0.0f, 0.0f, -5.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
    _cam[0]->UpdateViewMatrix();

    //1st person camera
    _cam[1] = new Camera();
    _cam[1]->LookAt(XMFLOAT3(0.0f, 0.0f, -2.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
    _cam[1]->UpdateViewMatrix();

    //static camera
    _cam[2] = new Camera();
    _cam[2]->LookAt(XMFLOAT3(0.0f, 5.0f, -23.0f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
    _cam[2]->UpdateViewMatrix();

    //top down camera
    _cam[3] = new Camera();
    _cam[3]->LookAt(XMFLOAT3(0.0f, 23.0f, 0.00000001f), XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f));
    _cam[3]->UpdateViewMatrix();

	return S_OK;
}

//initialise shaders
HRESULT Application::InitShadersAndInputLayout()
{
	HRESULT hr;

    // Compile the vertex shader
    ID3DBlob* pVSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "VS", "vs_4_0", &pVSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the vertex shader
	hr = _pd3dDevice->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), nullptr, &_pVertexShader);

	if (FAILED(hr))
	{	
		pVSBlob->Release();
        return hr;
	}

	// Compile the pixel shader
	ID3DBlob* pPSBlob = nullptr;
    hr = CompileShaderFromFile(L"DX11 Framework.fx", "PS", "ps_4_0", &pPSBlob);

    if (FAILED(hr))
    {
        MessageBox(nullptr,
                   L"The FX file cannot be compiled.  Please run this executable from the directory that contains the FX file.", L"Error", MB_OK);
        return hr;
    }

	// Create the pixel shader
	hr = _pd3dDevice->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), nullptr, &_pPixelShader);
	pPSBlob->Release();
	pPSBlob->Release();

    if (FAILED(hr))
        return hr;

    //sets up textures for all game objects
    hr = player.AddTexture(L"assets/planets/sun_spec.dds", L"assets/planets/sun_spec.dds", hr, _pd3dDevice);
    hr = cube.AddTexture(L"assets/ChainLink.dds", L"assets/ChainLink.dds", hr, _pd3dDevice);
    for (int i = 0; i < 3; i++)
    {
        hr = wall[i].AddTexture(L"assets/ChainLink.dds", L"assets/ChainLink.dds", hr, _pd3dDevice);
    }
    hr = plane.AddTexture(L"assets/ChainLink.dds", L"assets/ChainLink.dds", hr, _pd3dDevice);
    hr = skybox.AddTexture(L"assets/spaceBackgound.dds", L"assets/ChainLink.dds", hr, _pd3dDevice);
    hr = spaceDuck.AddTexture(L"assets/duck_col.dds", L"assets/duck_col.dds", hr, _pd3dDevice);
    for (int i = 0; i < 108; i++)
    {
        if (i == 0)
        {
            hr = sphere[i].AddTexture(L"assets/planets/sun_col.dds", L"assets/planets/sun_spec.dds ", hr, _pd3dDevice);
        }
        else if (i == 1)
        {
            hr = sphere[i].AddTexture(L"assets/planets/earth_col.dds", L"assets/planets/earth_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 2)
        {
            hr = sphere[i].AddTexture(L"assets/planets/mars_col.dds", L"assets/planets/mars_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 3)
        {
            hr = sphere[i].AddTexture(L"assets/planets/gas_col.dds", L"assets/planets/gas_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 4)
        {
            hr = sphere[i].AddTexture(L"assets/planets/mars_col.dds", L"assets/planets/mars_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 5)
        {
            hr = sphere[i].AddTexture(L"assets/planets/moon_col.dds", L"assets/planets/moon_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 6)
        {
            hr = sphere[i].AddTexture(L"assets/planets/moon_col.dds", L"assets/planets/moon_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 7)
        {
            hr = sphere[i].AddTexture(L"assets/planets/moon_col.dds", L"assets/planets/moon_spec.dds", hr, _pd3dDevice);
        }
        else if (i == 8)
        {
            hr = sphere[i].AddTexture(L"assets/planets/gas_col.dds", L"assets/planets/gas_spec.dds", hr, _pd3dDevice);
        }
        else if (i > 8)
        {
            hr = sphere[i].AddTexture(L"assets/planets/moon_col.dds", L"assets/planets/moon_spec.dds", hr, _pd3dDevice);
        }
        
    }
    // Define the input layout
    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	UINT numElements = ARRAYSIZE(layout);

    // Create the input layout
	hr = _pd3dDevice->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(),
                                        pVSBlob->GetBufferSize(), &_pVertexLayout);
	pVSBlob->Release();

	if (FAILED(hr))
        return hr;

    // Set the input layout
    _pImmediateContext->IASetInputLayout(_pVertexLayout);

    //set up blend state
    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    D3D11_RENDER_TARGET_BLEND_DESC rtbd;
    ZeroMemory(&rtbd, sizeof(rtbd));

    rtbd.BlendEnable = true;
    rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
    rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
    rtbd.BlendOp = D3D11_BLEND_OP_ADD;
    rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
    rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
    rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

    blendDesc.AlphaToCoverageEnable = false;
    blendDesc.RenderTarget[0] = rtbd;

    //create blend state
    _pd3dDevice->CreateBlendState(&blendDesc, &_transparency);

    D3D11_RASTERIZER_DESC cmdesc;
    ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));

    cmdesc.FillMode = D3D11_FILL_SOLID;
    cmdesc.CullMode = D3D11_CULL_BACK;

    cmdesc.FrontCounterClockwise = true;
    hr = _pd3dDevice->CreateRasterizerState(&cmdesc, &_CCWcullMode);

	return hr;
}

//initialises vertex buffer for hard coded meshes
HRESULT Application::InitVertexBuffer()
{
	HRESULT hr;

    //set cube vertices
    SimpleVertex cubeVertices[] =
    {
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 0.0) },	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(0.0, 1.0) },  

        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0,  0.0, -1.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	

        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) },  

        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 0.0) },  	
        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(0.0, 1.0) },  	

        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0,  0.0,  1.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	

        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(-1.0,  0.0,  0.0),		XMFLOAT2(1.0, 1.0) }, 

        { XMFLOAT3(-1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 0.0) }, 	
        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(0.0, 1.0) }, 

        { XMFLOAT3(1.0,  1.0,  1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0,  1.0, -1.0),		XMFLOAT3(0.0,  1.0,  0.0),		XMFLOAT2(1.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) }, 

        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(-1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(0.0, 1.0) },  	
        { XMFLOAT3(1.0, -1.0, -1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 0.0) },  	
        { XMFLOAT3(1.0, -1.0,  1.0),		XMFLOAT3(0.0, -1.0,  0.0),		XMFLOAT2(1.0, 1.0) },	
    };

    //create buffer for vertices
    D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));
    bdCube.Usage = D3D11_USAGE_DEFAULT;
    bdCube.ByteWidth = sizeof(cubeVertices);
    bdCube.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bdCube.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA InitCubeData;
	ZeroMemory(&InitCubeData, sizeof(InitCubeData));
    InitCubeData.pSysMem = cubeVertices;

    hr = _pd3dDevice->CreateBuffer(&bdCube, &InitCubeData, &_pCubeVertexBuffer);

    if (FAILED(hr))
        return hr;

	return S_OK;
}

//initalises index buffer for hard coded meshes
HRESULT Application::InitIndexBuffer()
{
	HRESULT hr;

    //set cube indices
    WORD cubeIndices[] =
    {
        0,1,2 ,3,4,5,
        6,7,8 ,9,10,11,
        12,13,14 ,15,16,17,
        18,19,20 ,21,22,23,
        24,25,26 ,27,28,29,
        30,31,32 ,33,34,35
    };

    //create buffer for indices
	D3D11_BUFFER_DESC bdCube;
	ZeroMemory(&bdCube, sizeof(bdCube));

    bdCube.Usage = D3D11_USAGE_DEFAULT;
    bdCube.ByteWidth = sizeof(cubeIndices);
    bdCube.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bdCube.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitCubeData;
	ZeroMemory(&InitCubeData, sizeof(InitCubeData));
    InitCubeData.pSysMem = cubeIndices;
    hr = _pd3dDevice->CreateBuffer(&bdCube, &InitCubeData, &_pCubeIndexBuffer);


    if (FAILED(hr))
        return hr;

	return S_OK;
}

//initalises window to 1080x640 resolustion
HRESULT Application::InitWindow(HINSTANCE hInstance, int nCmdShow)
{
    // Register class
    WNDCLASSEX wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, (LPCTSTR)IDI_TUTORIAL1);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW );
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = L"TutorialWindowClass";
    wcex.hIconSm = LoadIcon(wcex.hInstance, (LPCTSTR)IDI_TUTORIAL1);
    if (!RegisterClassEx(&wcex))
        return E_FAIL;

    // Create window
    _hInst = hInstance;
    RECT rc = {0, 0, 1080, 640};
    AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);
    _hWnd = CreateWindow(L"TutorialWindowClass", L"Space Game", WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, CW_USEDEFAULT, rc.right - rc.left, rc.bottom - rc.top, nullptr, nullptr, hInstance,
                         nullptr);
    if (!_hWnd)
		return E_FAIL;

    ShowWindow(_hWnd, nCmdShow);

    return S_OK;
}

//gets shader from HLSL file
HRESULT Application::CompileShaderFromFile(WCHAR* szFileName, LPCSTR szEntryPoint, LPCSTR szShaderModel, ID3DBlob** ppBlobOut)
{
    HRESULT hr = S_OK;

    DWORD dwShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined(DEBUG) || defined(_DEBUG)
    // Set the D3DCOMPILE_DEBUG flag to embed debug information in the shaders.
    // Setting this flag improves the shader debugging experience, but still allows 
    // the shaders to be optimized and to run exactly the way they will run in 
    // the release configuration of this program.
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif

    ID3DBlob* pErrorBlob;
    hr = D3DCompileFromFile(szFileName, nullptr, nullptr, szEntryPoint, szShaderModel, 
        dwShaderFlags, 0, ppBlobOut, &pErrorBlob);

    if (FAILED(hr))
    {
        if (pErrorBlob != nullptr)
            OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

        if (pErrorBlob) pErrorBlob->Release();

        return hr;
    }

    if (pErrorBlob) pErrorBlob->Release();

    return S_OK;
}

//initialises device data
HRESULT Application::InitDevice()
{
    D3D11_TEXTURE2D_DESC depthStencilDesc;

    depthStencilDesc.Width = _WindowWidth;
    depthStencilDesc.Height = _WindowHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;


    HRESULT hr = S_OK;

    UINT createDeviceFlags = 0;

#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    D3D_DRIVER_TYPE driverTypes[] =
    {
        D3D_DRIVER_TYPE_HARDWARE,
        D3D_DRIVER_TYPE_WARP,
        D3D_DRIVER_TYPE_REFERENCE,
    };

    UINT numDriverTypes = ARRAYSIZE(driverTypes);

    D3D_FEATURE_LEVEL featureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
    };

	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 1;
    sd.BufferDesc.Width = _WindowWidth;
    sd.BufferDesc.Height = _WindowHeight;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = _hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;

    for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
    {
        _driverType = driverTypes[driverTypeIndex];
        hr = D3D11CreateDeviceAndSwapChain(nullptr, _driverType, nullptr, createDeviceFlags, featureLevels, numFeatureLevels,
                                           D3D11_SDK_VERSION, &sd, &_pSwapChain, &_pd3dDevice, &_featureLevel, &_pImmediateContext);
        if (SUCCEEDED(hr))
            break;
    }
    
    if (FAILED(hr))
        return hr;

    // Create a render target view
    ID3D11Texture2D* pBackBuffer = nullptr;
    hr = _pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);

    if (FAILED(hr))
        return hr;

    hr = _pd3dDevice->CreateRenderTargetView(pBackBuffer, nullptr, &_pRenderTargetView);
    pBackBuffer->Release();

    if (FAILED(hr))
        return hr;

    _pd3dDevice->CreateTexture2D(&depthStencilDesc, nullptr, &_depthStencilBuffer);
    _pd3dDevice->CreateDepthStencilView(_depthStencilBuffer, nullptr, &_depthStencilView);


    _pImmediateContext->OMSetRenderTargets(1, &_pRenderTargetView, _depthStencilView);

    // Setup the viewport
    D3D11_VIEWPORT vp;
    vp.Width = (FLOAT)_WindowWidth;
    vp.Height = (FLOAT)_WindowHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0;
    vp.TopLeftY = 0;
    _pImmediateContext->RSSetViewports(1, &vp);

    //initialise shaders
	InitShadersAndInputLayout();

    //initialise vertex and indices buffers
	InitVertexBuffer();
	InitIndexBuffer();

    // Set primitive topology
    _pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Create the constant buffer
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ConstantBuffer);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
    hr = _pd3dDevice->CreateBuffer(&bd, nullptr, &_pConstantBuffer);

    //sets render state
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));

    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    hr = _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);

    

    if (FAILED(hr))
        return hr;

    return S_OK;
}

//cleans up variables
void Application::Cleanup()
{
    if (_pImmediateContext) _pImmediateContext->ClearState();

    if (_pConstantBuffer) _pConstantBuffer->Release();
    if (_pCubeVertexBuffer) _pCubeVertexBuffer->Release();
    if (_pCubeIndexBuffer) _pCubeIndexBuffer->Release();
    if (_pVertexLayout) _pVertexLayout->Release();
    if (_pVertexShader) _pVertexShader->Release();
    if (_pPixelShader) _pPixelShader->Release();
    if (_pRenderTargetView) _pRenderTargetView->Release();
    if (_pSwapChain) _pSwapChain->Release();
    if (_pImmediateContext) _pImmediateContext->Release();
    if (_pd3dDevice) _pd3dDevice->Release();

    if (_depthStencilView) _depthStencilView->Release();
    if (_depthStencilBuffer) _depthStencilBuffer->Release();
    _transparency->Release();
    _CCWcullMode->Release();
   // if (_wireFrame) _wireFrame->Release();
   // if (_solid) _solid->Release();

}

//updates game
void Application::Update()
{
    static float t = 0.0f;
    bool speedMultipler = false;
    float speed = 0.3f;

    //gets time passed between frames
    if (_driverType == D3D_DRIVER_TYPE_REFERENCE)
    {
        t += (float) XM_PI * 0.0125f;
    }
    else
    {
        static DWORD dwTimeStart = 0;
        DWORD dwTimeCur = GetTickCount();

        if (dwTimeStart == 0)
            dwTimeStart = dwTimeCur;

        t = (dwTimeCur - dwTimeStart) / 1000.0f;
    }

    //if player pressed shift set sprinting to true
    if (GetKeyState(SHIFT_PRESSED) & 0x8000)
    {
        speedMultipler = true;
    }

    
    //switches between the different cameras
    //3rd person
    if (GetKeyState('1') & 0x8000)
    {
        _currentCam = 0;
    }
    //1st person
    else if (GetKeyState('2') & 0x8000)
    {
        _currentCam = 1;
    }
    //static view
    else if (GetKeyState('3') & 0x8000)
    {
        _currentCam = 2;
    }
    //top down
    else if (GetKeyState('4') & 0x8000)
    {
        _currentCam = 3;
    }
    //toggles blending mode
    else if (GetKeyState('B') & 0x8000)
    {
        _blendActive = !_blendActive;
    }

    //sets speed modifier if player is sprinting
    if (speedMultipler == true)
    {
        speed = 0.6f;
    }
    else
    {
        speed = 0.03f;
    }

    //move player/cams forwards
    if (GetKeyState('W') & 0x8000)
    {
        _cam[0]->Walk(speed);
        _cam[1]->Walk(speed);
        _cam[3]->Fly(-speed);
        
    }
    //move player/cams backwards
    else if (GetKeyState('S') & 0x8000)
    {
        _cam[0]->Walk(-speed);
        _cam[1]->Walk(-speed);
        _cam[3]->Fly(speed);
    }
    //move player/cams left
    if (GetKeyState('A') & 0x8000)
    {
        _cam[0]->Strafe(-speed);
        _cam[1]->Strafe(-speed);
        _cam[3]->Strafe(speed);
    }
    //move player/cams right
    else if (GetKeyState('D') & 0x8000)
    {
        _cam[0]->Strafe(speed);
        _cam[1]->Strafe(speed);
        _cam[3]->Strafe(-speed);
        
    }
    //rotate player/cams left
    else if (GetKeyState('Q') & 0x8000)
    {
        _cam[0]->RotateY(-.02f);
        _cam[1]->RotateY(-.02f);
        _cam[3]->RotateY(-.02f);
        player.RotatePlayer(0, -0.02f, 0);
    }
    //rotate player/cams right
    else if (GetKeyState('E') & 0x8000)
    {
        _cam[0]->RotateY(.02f);
        _cam[1]->RotateY(.02f);
        _cam[3]->RotateY(.02f);
        player.RotatePlayer(0, 0.02f, 0);

    }

    //updates view based on which camera is active
    _cam[_currentCam]->UpdateViewMatrix();

    //sets up time speed
    _gTime = t / 30;

    //update player
    player.SetScale(.05, .05, .05);
    player.SetRotation(0, player.GetPlayerRotation().y, 0, 0, _cam[0]->GetLook().y, 0);
    player.SetPosition(0, -.2, 2.5, _cam[0]->GetPosition().x, _cam[0]->GetPosition().y, _cam[0]->GetPosition().z);
    player.Update();

    //update cube
    cube.SetRotation(_gTime, _gTime, _gTime, 0, 0, 0);
    cube.SetPosition(0, 0, -5, 0, 0, 0);
    cube.Update();

    //update skybox
    skybox.SetScale(50, 50, 50);
    skybox.SetRotation(0, 0, 0, 0, 0, 0);
    skybox.SetPosition(0, 0, 0, 0, 0, 0);
    skybox.Update();

    //update duck
    spaceDuck.SetRotation(0, 0, 0, 0, 0, 0);
    spaceDuck.SetPosition(5, 0, 5, 0, 0, 0);
    spaceDuck.Update();

    for (int i = 0; i < 3; i++)
    {
        wall[i].SetScale(2, 2, 2);
        wall[i].SetRotation(0, 0, 0, 0, 0, 0);
        wall[i].SetPosition(10 + i, 0, 0, 0, 0, 0);
        wall[i].Update();
    }
    plane.SetScale(2, 2, 2);
    plane.SetRotation(0, 0, 0, 0, 0, 0);
    plane.SetPosition(10, -1.5f, 0, 0, 0, 0);
    plane.Update();

    for (int i = 0; i < 108; i++)
    {
        if (i == 0)
        {
            //sun
            sphere[i].SetScale(2, 2, 2);
            sphere[i].SetRotation(0, _gTime, 0, 0, 0, 0);
            sphere[i].SetPosition(0, 0, 0, 0, 0, 0);
            sphere[i].Update();
        }
        else if (i == 1)
        {
            //earth
            sphere[i].SetScale(.65, .65, .65);
            sphere[i].SetRotation(0, _gTime, 180, 0, _gTime, 0);
            sphere[i].SetPosition(0, 0, 0, 0, 0, 9);
            sphere[i].Update();
        }
        //other planets
        else if (i == 8)
        {
            sphere[i].SetScale(.8, .8, .8);
            sphere[i].SetRotation(0, _gTime / 3, 0, 0, _gTime / 3, 0);
            sphere[i].SetPosition(0, 0, 0, 0, 0, 22);
            sphere[i].Update();
        }
        else if (i == 2)
        {
            sphere[i].SetScale(.4, .4, .4);
            sphere[i].SetRotation(0, _gTime * 2, 0, 0, _gTime * 1.5, 0);
            sphere[i].SetPosition(0, 0, 0, 0, 0, 6);
            sphere[i].Update();
        }
        else if (i == 3)
        {
            sphere[i].SetScale(1, 1, 1);
            sphere[i].SetRotation(0, _gTime/2, 0, 0, _gTime / 2, 0);
            sphere[i].SetPosition(0, 0, 0, 0, 0, 16);
            sphere[i].Update();
        }
        //moons
        else if (i == 4)
        {
            sphere[i].SetScale(.2, .2, .2);
            sphere[i].SetRotation(0, _gTime*3, 0, 0, _gTime * 1.5, 0);
            sphere[i].SetPosition(0, 0, 2, 0, 0, 6);
            sphere[i].Update();
        }
        else if (i == 5)
        {
            sphere[i].SetScale(.25, .25, .25);
            sphere[i].SetRotation(0, _gTime*3, 0, 0, _gTime, 0);
            sphere[i].SetPosition(0, 0, 2, 0, 0, 9);
            sphere[i].Update();
        }
        else if (i == 6)
        {
            sphere[i].SetScale(.4, .4, .4);
            sphere[i].SetRotation(0, _gTime*1.5, 0, 0, _gTime/2, 0);
            sphere[i].SetPosition(0, 0, 4, 0, 0, 16);
            sphere[i].Update();
        }
        else if (i == 7)
        {
            sphere[i].SetScale(.1, .1, .1);
            sphere[i].SetRotation(0, _gTime*5, 0, 0, _gTime/2, 0);
            sphere[i].SetPosition(0, 0, 2.5, 0, 0, 16);
            sphere[i].Update();
        }
        
        else
        {
            float rotationOffset = rand() % 100;
            float localTranformOffet = (rand() % 30) / 10;
            
            sphere[i].SetScale(.05, .05, .05);
            sphere[i].SetRotation(0, _gTime + (i/2), 0, 0, _gTime/3, 0);
            sphere[i].SetPosition(0, 0, 3 , 0, 0, 22);
            sphere[i].Update();
        }
    }
}

//draws objects
void Application::Draw()
{
    // Clear the back buffer
    float ClearColor[4] = {0.0f, 0.125f, 0.3f, 1.0f}; // red,green,blue,alpha
    _pImmediateContext->ClearRenderTargetView(_pRenderTargetView, ClearColor);
    
    //draw game objects
    player.Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    cube.Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    skybox.Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    spaceDuck.Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    plane.Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    
    //draw walls
    for (int i = 0; i < 3; i++) {
        this->wall[i].Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    }
    //draw spheres
    for (int i = 0; i < 108; i++)
    {
        //blend mode active
        if (_blendActive == true)
        {
            float blendFactor[] = { 1.0f, 0.75f, 0.75f, 1.0f };
            _pImmediateContext->OMSetBlendState(_transparency, blendFactor, 0xffffffff);
        }
        
        sphere[i].Draw(_cam[_currentCam], _pImmediateContext, _pConstantBuffer, _pVertexShader, _pPixelShader, _pSamplerLinear);
    }

    _pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
    _pImmediateContext->ClearDepthStencilView(_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

    // Present our back buffer to our front buffer
    _pSwapChain->Present(0, 0);

}

//initialises wire frame mode
HRESULT Application::InitWireframeView()
{
    D3D11_RASTERIZER_DESC wfdesc;
    ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
    wfdesc.FillMode = D3D11_FILL_WIREFRAME;
    wfdesc.CullMode = D3D11_CULL_NONE;
    return _pd3dDevice->CreateRasterizerState(&wfdesc, &_wireFrame);
}

//initialises solid mode
HRESULT Application::InitSolidView()
{
    D3D11_RASTERIZER_DESC sodesc;
    ZeroMemory(&sodesc, sizeof(D3D11_RASTERIZER_DESC));
    sodesc.FillMode = D3D11_FILL_SOLID;
    sodesc.CullMode = D3D11_CULL_BACK;
    return _pd3dDevice->CreateRasterizerState(&sodesc, &_solid);
}