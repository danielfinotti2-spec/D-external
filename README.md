# D

External para FiveM build 3258. Base Squid-Ext, rebrandeada para `D`.

> Preview de UI roda standalone, sem FiveM. O alvo real exige o jogo em execucao.

## Conteudo

| Alvo | Saida | Requer FiveM |
|---|---|---|
| `src\src.vcxproj` | `bin\build\Fivem-Loader.exe` | sim |
| `src\preview.vcxproj` | `bin\build\D-Preview.exe` | nao |

O alvo `preview` desenha o menu completo — abas, widgets, watermark — numa janela 1280x720 propria. Sem leitura de memoria, sem driver, sem processo alvo. Serve para iterar em UI, branding e layout.

## Toolchain

| Item | Valor |
|---|---|
| Visual Studio | 18 Community |
| MSBuild | `MSBuild\Current\Bin\MSBuild.exe` |
| Toolset | v143 / MSVC 14.51.36231 |
| Windows SDK | 10.0 |
| Language | `stdcpplatest` / `stdc17` |

## Build

```powershell
$msb = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"

# loader (exige FiveM)
& $msb "src\src.vcxproj" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /v:minimal

# preview (standalone)
& $msb "src\preview.vcxproj" /t:Build /p:Configuration=Release /p:Platform=x64 /v:minimal
```

Nao use `/m` em nenhum dos dois. O alvo `preview` tem `MultiProcessorCompilation=false` de proposito: com `/MP` os workers `cl.exe` penduram no handshake de named pipe em ambientes com sandbox restritivo.

## Branding

Ponto unico: `src\Ext\Includes\Brand.hpp`.

```cpp
inline constexpr const char* Name    = "D";
inline constexpr const char* Tag     = "D";
inline constexpr const char* Version = "1.0.0";
inline constexpr int AccentR = 214, AccentG = 40, AccentB = 40, AccentA = 255;
```

Muda watermark, painel de perfil, `g_Variables.id` e a paleta (`cColors::Base`, `ButtonHovered`, `SecundaryFeaturesText`). Nenhum outro arquivo guarda string de marca.

Logo: `src\Ext\Includes\ImGui\BrandLogo.hpp` — PNG reencodado como array C.

```powershell
Set-Location src
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force
.\tools\gen_brand_logo.ps1 -In "C:\caminho\logo.png" -Out "Ext\Includes\ImGui\BrandLogo.hpp"
```

Preserva alfa, nunca faz upscale, `-MaxDim 512` por default.

## Estrutura

```
src/
  main.cpp              alvo loader: espera grcWindow, abre o PID do jogo
  main_preview.cpp      alvo preview: janela propria, sem alvo
  src.vcxproj           loader
  preview.vcxproj       preview
  Ext/
    Includes/Brand.hpp          identidade
    Includes/Includes.hpp       include raiz
    Globals.hpp                 g_Variables, g_Col, g_MenuInfo
    Includes/ImGui/             imgui + freetype + fontes + imagens
    Includes/CustomWidgets/     widgets do menu (Custom, WaterMarks, Notify, Preview)
    Security/                   xorstr, AntiCrack
    Driver/Driver.hpp           comunicacao com driver
  Src/
    Core/Core.hpp               SetupOffsets, resolucao de assinaturas
    Core/Config.hpp             arvore de config
    Core/Offsets.hpp            struct de offsets
    Core/SDK/                   Memory, SDK, GameClasses
    Core/Features/              Aimbot, ESP, SilentAim, Triggerbot, MagicBullets
    Core/Threads/               EntityList, VehicleList, ConfigThread
    Gui/Overlay/                D3D11 + ImGui
    Gui/Pages/                  paginas do menu
```

## Operacao

1. FiveM build 3258 aberto ate o menu principal.
2. `New-Item -ItemType Directory -Force "C:\Windows\SysWOW64\slmgr\0416"` — o config grava la.
3. `Fivem-Loader.exe` como Administrador.
4. `INSERT` abre o menu.

Detalhes completos, incluindo mapa de patches e verificacao do binario, em [`BUILD-D.md`](BUILD-D.md).
