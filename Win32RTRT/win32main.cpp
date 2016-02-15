#include <windows.h>
#include <windowsx.h>
#include <d3d11.h>
#include "src\main.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <crtdbg.h>

#define _CRTDBG_MAP_ALLOC
//#define FULLSCREEN_SMALL
#ifdef FULLSCREEN
#define SCREEN_X 1920
#define SCREEN_Y 1080
#elif defined FULLSCREEN_SMALL
#define SCREEN_X 1366
#define SCREEN_Y 768
#define FULLSCREEN
#else
#define SCREEN_X 800
#define SCREEN_Y 600
#endif

bool isRunning = false;

#define CHECK_WIN_ERROR(x,y) if(FAILED(x)) {OutputDebugStringA(y);}
#define RELEASE_DX_RESOURCE(x) x->Release(); x = NULL;

void DebugLiveObjects(ID3D11Device *device);
void SetDebugName(ID3D11DeviceChild* child, uint32 nameLength, char *name);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_CLOSE:
		{
			isRunning = false;
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

static void ProcessMessages(HWND window, Input *input)
{
	MSG message;
	while (PeekMessageA(&message, window, 0, 0, PM_REMOVE))
	{
		switch (message.message)
		{
		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			if (message.wParam == VK_ESCAPE)
			{ 
				isRunning = false;
			}
			if (message.wParam == 'R')
			{
			}
			switch (message.wParam)
			{
				case 'W':
					break;
				case 'A':
					break;
				case 'S':
					break;
				case 'D':
					break;
				case 'G':
					break;
				case 'Q':
					break;
			}						
		} break;
		case WM_MOUSEMOVE:
		{
			input->touchPosition.x = (float)GET_X_LPARAM(message.lParam);
			input->touchPosition.y = SCREEN_X - (float)GET_Y_LPARAM(message.lParam);
		} break;
		case WM_LBUTTONDOWN:
		{
			input->leftButtonDown = true;
		} break;
		case WM_LBUTTONUP:
		{
			input->leftButtonDown = false;
		} break;
		case WM_MOUSEWHEEL:
		{
			input->mouseScroll = GET_WHEEL_DELTA_WPARAM(message.wParam);

		} break;
		default:
		{
			TranslateMessage(&message);
			DispatchMessageA(&message);
		} break;
		}
	}
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	WNDCLASSEXA windowClass = {};
	windowClass.cbSize = sizeof(WNDCLASSEXA);
	windowClass.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;
	windowClass.lpfnWndProc = WndProc;
	windowClass.hInstance = hInstance;
	windowClass.hCursor = LoadCursor(0, IDC_ARROW);
	windowClass.lpszClassName = "RTRT";

	DWORD winFlags = WS_VISIBLE;
#ifdef FULLSCREEN
	winFlags |= WS_POPUP;
#else
	winFlags |= WS_OVERLAPPED;
#endif

	if (RegisterClassExA(&windowClass))
	{
		HWND window = CreateWindowExA(0, windowClass.lpszClassName, "Real Time Ray Tracer", winFlags,
			CW_USEDEFAULT, CW_USEDEFAULT, SCREEN_X, SCREEN_Y,
			NULL, NULL, hInstance, NULL);
		if (window)
		{
			isRunning = true;

			LARGE_INTEGER clockFreq;
			QueryPerformanceFrequency(&clockFreq);
			LARGE_INTEGER startTime, endTime, elapsedTicks;
			QueryPerformanceCounter(&startTime);
			srand(startTime.LowPart);

			srand(startTime.LowPart);

			ID3D11Device *device;
			ID3D11DeviceContext *context;
			IDXGISwapChain *swapChain;

			UINT flags = 0;
#ifdef DEBUG
			flags = D3D11_CREATE_DEVICE_DEBUG;
#endif
			DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
			swapChainDesc.BufferDesc.Width = (UINT)SCREEN_X;
			swapChainDesc.BufferDesc.Height = (UINT)SCREEN_Y;
			swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			swapChainDesc.SampleDesc.Count = 1;
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 1;
			swapChainDesc.OutputWindow = window;
			swapChainDesc.Windowed = true;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

			D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
			D3D_FEATURE_LEVEL supportedFeatureLevel;
			HRESULT hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, flags, &featureLevel, 1, D3D11_SDK_VERSION, 
													   &swapChainDesc, &swapChain, &device, &supportedFeatureLevel, &context);
			CHECK_WIN_ERROR(hr, "Error creating D3D11 device/context/swapChain \n");

			ID3D11Texture2D *backBuffer;
			hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void **)&backBuffer);
			CHECK_WIN_ERROR(hr, "Error getting backbuffer\n");

			ID3D11RenderTargetView *renderTargetView;
			hr = device->CreateRenderTargetView(backBuffer, NULL, &renderTargetView);
			RELEASE_DX_RESOURCE(backBuffer);
			CHECK_WIN_ERROR(hr, "Error creating Render Target\n");

			Context rayContext;
			rayContext.renderer.device = device;
			rayContext.renderer.context = context;
			rayContext.screenBuffer = (uint32 *)VirtualAlloc(0, SCREEN_X * SCREEN_Y * 4, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
			Graphics::Init(&rayContext.renderer);
			Init(&rayContext);
			rayContext.renderTarget.renderTarget = renderTargetView;
			rayContext.renderTarget.width = SCREEN_X;
			rayContext.renderTarget.height = SCREEN_Y;
			Input input = {};

			char buffer[100];
			while (isRunning)
			{
				QueryPerformanceCounter(&endTime);
				elapsedTicks.QuadPart = endTime.QuadPart - startTime.QuadPart;
				startTime.QuadPart = endTime.QuadPart;
				float dt = (float)elapsedTicks.QuadPart / clockFreq.QuadPart;
				sprintf_s(buffer, 100, "%f\n", dt);
				OutputDebugStringA(buffer);
				ProcessMessages(window, &input);
				Update(&rayContext, &input, dt);
				input.leftButtonDown = false;
				input.mouseScroll = 0;
				input.previousPosition = input.touchPosition;
				swapChain->Present(1, 0);
				QueryPerformanceCounter(&endTime);
			}
			
			Release(&rayContext);
			RELEASE_DX_RESOURCE(renderTargetView);
			RELEASE_DX_RESOURCE(swapChain);
			RELEASE_DX_RESOURCE(context);

			//DebugLiveObjects(flContext.renderer.device);
			RELEASE_DX_RESOURCE(device);
		}
	}
	_CrtDumpMemoryLeaks();
}

void DebugLiveObjects(ID3D11Device *device)
{
	ID3D11Debug *pDebug;
	HRESULT hr = device->QueryInterface(IID_PPV_ARGS(&pDebug));
	CHECK_WIN_ERROR(hr, "Error creating debug interface.\n");

	pDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);

	RELEASE_DX_RESOURCE(pDebug);
}

void SetDebugName(ID3D11DeviceChild* child, uint32 nameLength, char *name)
{
	child->SetPrivateData(WKPDID_D3DDebugObjectName, nameLength, name);
}