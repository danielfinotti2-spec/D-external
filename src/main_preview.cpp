#pragma once

// main_preview.cpp - alvo standalone de UI.
// Desenha o menu D sem FiveM, sem leitura de memoria, sem driver.
// Nao substitui o alvo "src" (Fivem-Loader.exe).

#include <Gui/Overlay/Overlay.hpp>
#include <Includes/Includes.hpp>
#include <Includes/Utils.hpp>
#include <Gui/gui.hpp>
#include <windows.h>
#include <dwmapi.h>
#include <tchar.h>
#include <vector>
#include <regex>
#include <Includes/CustomWidgets/Custom.hpp>
#include <Includes/CustomWidgets/WaterMarks.hpp>
#include <Includes/CustomWidgets/Notify.hpp>

#include <Includes/ImGui/BrandLogo.hpp>
#include <Includes/ImGui/Images.hpp>
#include <Includes/ImGui/Fonts.hpp>
#include <Includes/ImGui/Files/imgui_freetype.h>

#include <Gui/Pages/Combat.hpp>
#include <Gui/Pages/Visuals.hpp>
#include <Gui/Pages/Local.hpp>
#include <Gui/Pages/World.hpp>
#include <Gui/Pages/Exploits.hpp>
#include <Gui/Pages/Settings.hpp>

#include <d3dx11.h>
#include <d3d11.h>
#include <D3DX11tex.h>

#pragma comment( lib , "d3d11.lib" )
#pragma comment( lib , "d3dx11.lib" )
#pragma comment( lib , "d3dx9.lib" )
#pragma comment( lib , "d3d9.lib" )
#pragma comment( lib , "dxgi.lib" )
#pragma comment( lib , "d3dcompiler.lib" )
#pragma comment( lib , "winmm.lib" )

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

namespace Preview
{
	inline ID3D11Device* g_Device = nullptr;
	inline ID3D11DeviceContext* g_Context = nullptr;
	inline IDXGISwapChain* g_Swap = nullptr;
	inline ID3D11RenderTargetView* g_RTV = nullptr;
	inline HWND g_Hwnd = nullptr;
	inline WNDCLASSEXW g_Wc = { };

	// Paginas disponiveis no preview (Login omitido - exige backend)
	enum PreviewPage { P_Combat = 0 , P_Visuals , P_Local , P_World , P_Exploits , P_Settings };
	inline int g_CurrentTab = P_Combat;
	inline int g_PrevTab = P_Combat;
	inline float g_TabAlpha = 1.f;
	inline bool g_Pressed = false;

	inline void CreateRTV( )
	{
		ID3D11Texture2D* BackBuffer = nullptr;
		g_Swap->GetBuffer( 0 , IID_PPV_ARGS( &BackBuffer ) );
		g_Device->CreateRenderTargetView( BackBuffer , nullptr , &g_RTV );
		if ( BackBuffer ) BackBuffer->Release( );
	}

	inline void CleanupRTV( )
	{
		if ( g_RTV ) { g_RTV->Release( ); g_RTV = nullptr; }
	}

	inline LRESULT CALLBACK WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam )
	{
		if ( ImGui_ImplWin32_WndProcHandler( hWnd , msg , wParam , lParam ) )
			return true;

		switch ( msg ) {
		case WM_SIZE:
			if ( g_Device != nullptr && wParam != SIZE_MINIMIZED ) {
				CleanupRTV( );
				g_Swap->ResizeBuffers( 0 , ( UINT ) LOWORD( lParam ) , ( UINT ) HIWORD( lParam ) , DXGI_FORMAT_UNKNOWN , 0 );
				CreateRTV( );
			}
			return 0;
		case WM_SYSCOMMAND:
			if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage( 0 );
			return 0;
		}
		return ::DefWindowProcW( hWnd , msg , wParam , lParam );
	}

	inline bool CreateDevice( HWND hWnd )
	{
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory( &sd , sizeof( sd ) );
		sd.BufferCount = 2;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.Windowed = TRUE;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		D3D_FEATURE_LEVEL Level;
		const D3D_FEATURE_LEVEL Levels [ 2 ] = { D3D_FEATURE_LEVEL_11_0 , D3D_FEATURE_LEVEL_10_0 };
		if ( D3D11CreateDeviceAndSwapChain( nullptr , D3D_DRIVER_TYPE_HARDWARE , nullptr , 0 , Levels , 2 ,
			D3D11_SDK_VERSION , &sd , &g_Swap , &g_Device , &Level , &g_Context ) != S_OK )
			return false;

		CreateRTV( );
		return true;
	}

	inline void Fonts( )
	{
		ImGuiIO& io = ImGui::GetIO( );
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.IniFilename = nullptr;

		ImFontConfig Cfg;
		Cfg.FontBuilderFlags = ImGuiFreeTypeBuilderFlags_ForceAutoHint | ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags_LoadColor;
		Cfg.FontDataOwnedByAtlas = false;

		static const ImWchar Ranges [ ] = { 0x0020 , 0x00FF , 0x0400 , 0x044F , 0 };

		g_Variables.m_FontBig = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 20.f , &Cfg , Ranges );
		g_Variables.m_FontBigSmall = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 18.f , &Cfg , Ranges );
		g_Variables.m_FontNormal = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 18.f , &Cfg , Ranges );
		g_Variables.m_FontSecundary = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 16.f , &Cfg , Ranges );
		g_Variables.m_FontSmaller = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 14.f , &Cfg , Ranges );
		g_Variables.m_ChangeLogFont = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 12.f , &Cfg , Ranges );
		g_Variables.m_DrawFont = io.Fonts->AddFontFromMemoryTTF( Compact , sizeof( Compact ) , 12.f , &Cfg , Ranges );
		g_Variables.m_Expand = io.Fonts->AddFontFromMemoryTTF( expand_binary , sizeof( expand_binary ) , 11.f , &Cfg , Ranges );

		static const ImWchar FaRanges [ ] = { ICON_MIN_FA , ICON_MAX_FA , 0 };
		ImFontConfig FaCfg;
		FaCfg.PixelSnapH = true;
		FaCfg.GlyphMinAdvanceX = 19;
		g_Variables.FontAwesomeSolid = io.Fonts->AddFontFromMemoryCompressedTTF( FontAwesome6Solid_compressed_data , sizeof( FontAwesome6Solid_compressed_size ) , 19 , &FaCfg , FaRanges );
		g_Variables.FontAwesomeSolidSmall = io.Fonts->AddFontFromMemoryCompressedTTF( FontAwesome6Solid_compressed_data , sizeof( FontAwesome6Solid_compressed_size ) , 16 , &FaCfg , FaRanges );
		g_Variables.FontAwesomeRegular = io.Fonts->AddFontFromMemoryCompressedTTF( FontAwesome6Regular_compressed_data , sizeof( FontAwesome6Regular_compressed_size ) , 19 , &FaCfg , FaRanges );
	}

	inline void Style( )
	{
		ImGui::StyleColorsDark( );
		ImGuiStyle* style = &ImGui::GetStyle( );
		style->WindowPadding = ImVec2( 0 , 0 );
		style->WindowBorderSize = 0.f;
		style->ItemSpacing = ImVec2( 12 , 12 );
		style->ScrollbarSize = 4.f;
	}

	inline void LoadBrand( )
	{
		D3DX11_IMAGE_LOAD_INFO Info;
		ID3DX11ThreadPump* Pump = nullptr;
		D3DX11CreateShaderResourceViewFromMemory( g_Device , brand_logo , sizeof( brand_logo ) , &Info , Pump , &g_Variables.BrandLogo , 0 );
		D3DX11CreateShaderResourceViewFromMemory( g_Device , logo_binary , sizeof( logo_binary ) , &Info , Pump , &g_Variables.Logo , 0 );
		D3DX11CreateShaderResourceViewFromMemory( g_Device , decor_background , sizeof( decor_background ) , &Info , Pump , &g_Variables.decor_background , 0 );
	}

	inline void RenderMenu( )
	{
		// Estado de login forcado: preview nao tem backend.
		g_MenuInfo.IsLogged = true;
		g_MenuInfo.IsOpen = true;
		g_MenuInfo.MenuSize = ImVec2( 1100 , 570 );

		ImGui::SetNextWindowSize( g_MenuInfo.MenuSize );
		ImGui::SetNextWindowPos( ImVec2( 40 , 40 ) );

		ImGui::Begin( " " , nullptr , Gui::ImGuiWindowFlags );
		ImGui::PushFont( g_Variables.m_FontNormal );
		{
			Custom::DrawBackground( true );

			// Sidebar de abas
			ImGui::SetCursorPos( ImVec2( 25 , 100 ) );
			ImGui::BeginGroup( );
			ImGui::PushFont( g_Variables.m_FontSecundary );
			{
				struct TabDef { const char* Label; const char* Icon; int Id; };
				const TabDef Tabs [ ] = {
					{ "Combat" , ICON_FA_LOCATION_CROSSHAIRS , P_Combat },
					{ "Visuals" , ICON_FA_EYE_SLASH , P_Visuals },
					{ "Local" , ICON_FA_GLOBE , P_Local },
					{ "World" , ICON_FA_E , P_World },
					{ "Exploits" , ICON_FA_USER_SECRET , P_Exploits },
					{ "Settings" , ICON_FA_GEAR , P_Settings },
				};
				for ( int i = 0; i < 6; ++i ) {
					if ( ImGui::CircleTab( Tabs [ i ].Label , Tabs [ i ].Icon , g_CurrentTab == Tabs [ i ].Id , &g_Pressed ) )
						g_CurrentTab = Tabs [ i ].Id;
					if ( i < 5 ) ImGui::Spacing( );
				}
			}
			ImGui::PopFont( );
			ImGui::EndGroup( );

			g_TabAlpha = ImClamp( g_TabAlpha + ( 5.f * ImGui::GetIO( ).DeltaTime * ( g_CurrentTab == g_PrevTab ? 1.f : -1.f ) ) , 0.f , 1.f );
			if ( g_TabAlpha == 0.f )
				g_PrevTab = g_CurrentTab;

			ImGui::PushStyleVar( ImGuiStyleVar_Alpha , g_TabAlpha );

			ImGui::SetCursorPos( ImVec2( 176 , 16 ) );
			ImGui::BeginGroup( );
			switch ( g_PrevTab ) {
			case P_Combat:   Combat::Render( );   break;
			case P_Visuals:  Visuals::Render( );  break;
			case P_Local:    Local::Render( );    break;
			case P_World:    World::Render( );    break;
			case P_Exploits: Exploits::Render( ); break;
			case P_Settings: Settings::Render( ); break;
			}
			ImGui::EndGroup( );

			ImGui::PopStyleVar( );

			// Watermark + notificacoes por cima
			NotifyManager::Render( );
			if ( g_Config.General->WaterMark )
				Custom::WaterMark::Render( );
		}
		ImGui::PopFont( );
		ImGui::End( );
	}

	inline int Run( )
	{
		// Painel de preview 1280x720
		const int W = 1280 , H = 720;

		g_Wc = { sizeof( g_Wc ) , CS_CLASSDC , WndProc , 0L , 0L , GetModuleHandle( nullptr ) , nullptr , nullptr , nullptr , nullptr , L"DPREVIEW" , nullptr };
		::RegisterClassExW( &g_Wc );

		g_Hwnd = ::CreateWindowW( g_Wc.lpszClassName , L"D - preview" , WS_OVERLAPPEDWINDOW , 80 , 80 , W , H , nullptr , nullptr , g_Wc.hInstance , nullptr );
		if ( !g_Hwnd ) {
			::UnregisterClassW( g_Wc.lpszClassName , g_Wc.hInstance );
			return 1;
		}

		if ( !CreateDevice( g_Hwnd ) ) {
			if ( g_Swap ) g_Swap->Release( );
			if ( g_Context ) g_Context->Release( );
			if ( g_Device ) g_Device->Release( );
			::DestroyWindow( g_Hwnd );
			::UnregisterClassW( g_Wc.lpszClassName , g_Wc.hInstance );
			return 1;
		}

		::ShowWindow( g_Hwnd , SW_SHOWDEFAULT );
		::UpdateWindow( g_Hwnd );

		ImGui::CreateContext( );
		Style( );
		Fonts( );
		ImGui_ImplWin32_Init( g_Hwnd );
		ImGui_ImplDX11_Init( g_Device , g_Context );

		// Dados de exemplo, so para a UI ter o que desenhar
		g_Variables.UserName = Brand::Name;
		g_Variables.Role = Brand::Tag;
		g_Variables.g_vGameWindowSize = ImVec2( ( float ) W , ( float ) H );

		LoadBrand( );

		std::thread( [ ] { NotifyManager::Send( xorstr( "Preview mode - sem alvo" ) , 5000 ); } ).detach( );

		MSG msg = { };
		bool Running = true;
		while ( Running ) {
			while ( ::PeekMessage( &msg , nullptr , 0U , 0U , PM_REMOVE ) ) {
				::TranslateMessage( &msg );
				::DispatchMessage( &msg );
				if ( msg.message == WM_QUIT )
					Running = false;
			}
			if ( !Running )
				break;

			ImGui_ImplDX11_NewFrame( );
			ImGui_ImplWin32_NewFrame( );
			ImGui::NewFrame( );
			{
				ImGui::GetBackgroundDrawList( )->AddRectFilled( ImVec2( 0 , 0 ) , ImVec2( ( float ) W , ( float ) H ) , IM_COL32( 8 , 8 , 10 , 255 ) );
				RenderMenu( );
			}
			ImGui::EndFrame( );

			const float Clear [ 4 ] = { 0.f , 0.f , 0.f , 1.f };
			g_Context->OMSetRenderTargets( 1U , &g_RTV , nullptr );
			g_Context->ClearRenderTargetView( g_RTV , Clear );
			ImGui::Render( );
			ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );
			g_Swap->Present( 1 , 0U );
		}

		ImGui_ImplDX11_Shutdown( );
		ImGui_ImplWin32_Shutdown( );
		ImGui::DestroyContext( );
		CleanupRTV( );
		if ( g_Swap ) g_Swap->Release( );
		if ( g_Context ) g_Context->Release( );
		if ( g_Device ) g_Device->Release( );
		::DestroyWindow( g_Hwnd );
		::UnregisterClassW( g_Wc.lpszClassName , g_Wc.hInstance );
		return 0;
	}
}

int APIENTRY WinMain( HINSTANCE , HINSTANCE , PSTR , int )
{
	return Preview::Run( );
}
