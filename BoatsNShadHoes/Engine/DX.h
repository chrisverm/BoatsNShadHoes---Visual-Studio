#ifndef DX_H
#define DX_H

#include "Windows.h"
#include <string>
#include <sstream>
#include <d3d11.h>
#include <assert.h>
#include "Globals.h"
#include "dxerr.h"
#include "GameTimer.h"

#include "CameraManager.h"
#include "GameStateManager.h"
#include "InputManager.h"

class DX
{
public:
	static void Release();

	static bool Initialize(int = 0);
	static void OnResize();
	static void PreUpdate();
	static void PostUpdate();
	static LRESULT MsgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	float AspectRatio() const;

private:
	static bool InitMainWindow(int = 0);
	static bool InitDirect3D();

	static void CalculateFrameStats();

protected:
	static HINSTANCE hAppInst;
	static HWND hMainWnd;

	static bool calcFrameStates;
	static bool gamePaused;
	static bool minimized;
	static bool maximized;
	static bool resizing;

	static GameTimer timer;
	static UINT msaa4xQuality;
	static ID3D11Device* device;
	static ID3D11DeviceContext* deviceContext;
	static IDXGISwapChain* swapChain;
	static ID3D11Texture2D* depthStencilBuffer;
	static ID3D11RenderTargetView* renderTargetView;
	static ID3D11DepthStencilView* depthStencilView;
	static D3D11_VIEWPORT viewport;
	static D3D_DRIVER_TYPE driverType;
	static D3D_FEATURE_LEVEL featureLevel;

	static std::wstring windowCaption;
	static int windowWidth;
	static int windowHeight;
	static bool enable4xMsaa;

	// double pointer so when a state changes this 
	// doesn't still point at the old state
	static GameState** currentState;

};

#endif