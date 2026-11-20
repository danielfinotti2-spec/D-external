#pragma once
#include <Includes/Includes.hpp>

namespace Custom
{
	namespace WaterMark
	{
		// Desenha o logo da marca recortado num circulo, a partir do canto superior esquerdo.
		inline void LogoCircle( ImDrawList* DrawList , ImVec2 Pos , float Diameter , ImU32 BorderCol , ImU32 GlowCol )
		{
			if ( !g_Variables.BrandLogo )
				return;

			const float Radius = Diameter * 0.5f;
			const ImVec2 Center = ImVec2( Pos.x + Radius , Pos.y + Radius );

			// Glow atras
			DrawList->AddCircleFilled( Center , Radius + 1.5f , GlowCol , 64 );

			// Fundo do circulo (cobre PNG com transparencia)
			DrawList->AddCircleFilled( Center , Radius , IM_COL32( 12 , 12 , 14 , 255 ) , 64 );

			// Recorte real do ImGui
			DrawList->PushClipRect( ImVec2( Pos.x , Pos.y ) , ImVec2( Pos.x + Diameter , Pos.y + Diameter ) , true );
			DrawList->AddImage( ( ImTextureID ) g_Variables.BrandLogo , ImVec2( Pos.x , Pos.y ) , ImVec2( Pos.x + Diameter , Pos.y + Diameter ) , ImVec2( 0.f , 0.f ) , ImVec2( 1.f , 1.f ) );
			DrawList->PopClipRect( );

			DrawList->AddCircle( Center , Radius , BorderCol , 64 , 1.5f );
		}

		inline void Render( )
		{
			ImDrawList * DrawList = ImGui::GetBackgroundDrawList( );

			const char* Line1 = Brand::WatermarkName;
			const char* Line2 = Brand::WatermarkTag;

			ImGui::PushFont( g_Variables.m_FontNormal );
			const ImVec2 S1 = ImGui::CalcTextSize( Line1 );
			ImGui::PopFont( );
			ImGui::PushFont( g_Variables.m_FontSmaller );
			const ImVec2 S2 = ImGui::CalcTextSize( Line2 );
			ImGui::PopFont( );

			const float LogoBox = Brand::WatermarkLogoBox;
			const float Gap = Brand::WatermarkGap;
			const float Pad = Brand::WatermarkPadding;

			const float TextW = ImMax( S1.x , S2.x );
			const float TextH = S1.y + 2.f + S2.y;
			const float BoxH = ImMax( Brand::WatermarkHeight , TextH + Pad * 0.5f );

			const float TotalW = Pad + LogoBox + Gap + TextW + Pad + 10.f;

			const ImVec2 Size( TotalW , BoxH );
			const ImVec2 Pos( g_Variables.g_vGameWindowSize.x - Size.x - Brand::WatermarkMarginX , Brand::WatermarkMarginY );

			// Fundo
			DrawList->AddRectFilled( Pos , Pos + Size , ImGui::GetColorU32( g_Col.ChildCol ) , 6.f );
			DrawList->AddRect( Pos , Pos + Size , ImGui::GetColorU32( g_Col.BorderCol ) , 6.f );

			// Barra de acento a esquerda
			DrawList->AddRectFilled( Pos , Pos + ImVec2( 2.f , Size.y ) , ImGui::GetColorU32( g_Col.Base ) , 6.f , ImDrawFlags_RoundCornersLeft );

			// Logo
			const ImVec2 LogoPos( Pos.x + Pad , Pos.y + ( Size.y - LogoBox ) * 0.5f );
			LogoCircle( DrawList , LogoPos , LogoBox , ImGui::GetColorU32( g_Col.BorderCol ) , ImGui::GetColorU32( ImVec4( g_Col.Base.x , g_Col.Base.y , g_Col.Base.z , 0.35f ) ) );

			// Texto
			const float TextX = LogoPos.x + LogoBox + Gap;
			const float TextY = Pos.y + ( Size.y - TextH ) * 0.5f;

			ImGui::PushFont( g_Variables.m_FontNormal );
			DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ImVec2( TextX + 1.f , TextY + 1.f ) , IM_COL32( 0 , 0 , 0 , 140 ) , Line1 );
			ImGui::PopFont( );

			DrawList->AddText( g_Variables.m_FontNormal , g_Variables.m_FontNormal->FontSize , ImVec2( TextX , TextY ) , ImGui::GetColorU32( g_Col.Base2 ) , Line1 );

			DrawList->AddText( g_Variables.m_FontSmaller , g_Variables.m_FontSmaller->FontSize , ImVec2( TextX , TextY + S1.y + 2.f ) , ImGui::GetColorU32( g_Col.SecundaryText ) , Line2 );
		}
	}

	namespace ArrayList
	{
		inline void Render( )
		{

		}
	}

}
