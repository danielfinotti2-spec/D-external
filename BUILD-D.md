# D — build e operacao

Base: Squid-Ext (Fivem-External.sln), rebrandeada para `D`.
Binario: `bin\build\Fivem-Loader.exe` — 6.383.616 bytes, PE32+ x64, subsystem GUI, SHA256 `9DF009D85F585C0EBCF7693B510DE8C850145D8C28CFB60A5731F232BC389E0C`.

## 1. Toolchain exata

| Item | Valor | Verificacao |
|---|---|---|
| Visual Studio | 18 Community | `C:\Program Files\Microsoft Visual Studio\18\Community` |
| MSBuild | `MSBuild\Current\Bin\MSBuild.exe` | presente |
| Toolset | v143, MSVC 14.51.36231 | `VC\Tools\MSVC\14.51.36231` |
| Windows SDK | 10.0 | `WindowsTargetPlatformVersion` |
| Language | `stdcpplatest` / `stdc17` | Release x64 |
| Config | `Release\|x64` | unica valida |

`src.vcxproj` declara `ConfigurationType=Application` com `DllMain`. O entry point e `WinMain` (`EntryPointSymbol` vazio). Saida e EXE, nao DLL.

## 2. Build por linha de comando

```powershell
Set-Location "C:\Users\danie\Downloads\Nova pasta (4)\Squid-Ext\Squid-Ext"
$msb = "C:\Program Files\Microsoft Visual Studio\18\Community\MSBuild\Current\Bin\MSBuild.exe"
& $msb "Fivem-External.sln" /t:Rebuild /p:Configuration=Release /p:Platform=x64 /m /v:minimal /nologo
```

Saida: `bin\build\Fivem-Loader.exe`. Intermediate: `bin\trash`.

## 3. Rebranding — ponto unico

`src\Ext\Includes\Brand.hpp` controla tudo.

```cpp
inline constexpr const char* Name          = "D";
inline constexpr const char* Tag           = "D";
inline constexpr const char* Version       = "1.0.0";
inline constexpr const char* WindowClass   = "D";
inline constexpr const char* ConfigFile    = "C:\\Windows\\SysWOW64\\slmgr\\0416\\slmgrs.ini";
inline constexpr const char* BinaryName    = "D";
inline constexpr int AccentR = 214, AccentG = 40, AccentB = 40, AccentA = 255;
```

Alterar `Name`/`Tag`/`Version` altera watermark, painel de perfil e `g_Variables.id`. Nenhum outro arquivo guarda string de marca.

## 4. Ponto de entrada

`src\Ext\Includes\ImGui\BrandLogo.hpp` — PNG 500x500 RGBA reencodado, array `inline unsigned char brand_logo[120082]`.

Regenerar a partir de outro PNG:

```powershell
Set-Location "C:\Users\danie\Downloads\Nova pasta (4)\Squid-Ext\Squid-Ext\src"
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass -Force
.\tools\gen_brand_logo.ps1 -In "C:\caminho\logo.png" -Out "Ext\Includes\ImGui\BrandLogo.hpp"
```

`-MaxDim 512` (default), `-BytesPerLine 12`. O script nunca faz upscale e preserva alfa.

## 5. Pontos de patch aplicados

| Arquivo | Mudanca |
|---|---|
| `src\Ext\Includes\Brand.hpp` | novo — fonte unica de identidade |
| `src\Ext\Includes\ImGui\BrandLogo.hpp` | novo — asset da marca |
| `src\Ext\Includes\Includes.hpp:20-21` | `#include <Includes/Brand.hpp>` |
| `src\Ext\Globals.hpp:12` | `cColors::Base` e `ButtonHovered` puxam `Brand::Accent*` |
| `src\Ext\Globals.hpp:49-50` | `g_Variables.id` / `.version` inicializados de `Brand::` |
| `src\Ext\Globals.hpp:97` | `ID3D11ShaderResourceView* BrandLogo` |
| `src\Src\Gui\Overlay\Overlay.cpp:6` | include do `BrandLogo.hpp` |
| `src\Src\Gui\Overlay\Overlay.cpp:222` | `D3DX11CreateShaderResourceViewFromMemory(..., brand_logo, ...)` |
| `src\Ext\Includes\CustomWidgets\WaterMarks.hpp` | watermark reescrito: barra de acento, logo circular, `Name` + `Version` |
| `src\Ext\Includes\CustomWidgets\Custom.hpp:217-218` | `ProfileBar` cai para `Brand::Name` / `Brand::Tag` |
| `src\Src\Core\Config.hpp:20` | `General::WaterMark = true` (default ligado) |
| `src\Src\Gui\Pages\Settings.hpp:57` | checkbox `Watermark` |
| `src\Src\Core\Threads\ConfigThread.hpp:15` | `configPath = Brand::ConfigFile` |
| `src\src.vcxproj:349,352` | headers novos registrados |

## 6. Geometria do watermark

```
Pos.x = GameWindowSize.x - Size.x - 12
Pos.y = 12
Size.x = 12 (pad) + 22 (logo) + 8 (gap) + max(S1.x, S2.x) + 12 + 10
Size.y = max(34, S1.y + 2 + S2.y + 6)
```

Camadas em ordem: `AddRectFilled` fundo `g_Col.ChildCol` → `AddRect` borda `g_Col.BorderCol` → barra 2px `g_Col.Base` (`#D62828`) → glow `AddCircleFilled(r+1.5)` → fundo do circulo `#0C0C0E` → `AddImage` recortado por `PushClipRect` → `AddCircle` borda → `AddText` com sombra 1px.

Fonte linha 1: `m_FontNormal` (18px). Linha 2: `m_FontSmaller` (14px), cor `g_Col.SecundaryText`.

Toggle: Settings → Globals → `Watermark`, ou `General.WaterMark` no config (`C:\Windows\SysWOW64\slmgr\0416\slmgrs.ini`, XOR key `@189Khkzmhakdolajkdj2!098394083276632uiuj9pu90hlka2JO`).

## 7. Ciclo de operacao

1. Iniciar FiveM ate o menu principal. Build alvo: 3258 (`SavedBuildNumber` em `%LOCALAPPDATA%\FiveM\FiveM.app\CitizenFX.ini`).
2. Executar `Fivem-Loader.exe` como Administrador — `UACExecutionLevel=RequireAdministrator` no vcxproj.
3. O loader procura `grcWindow` (`FindWindowA`), resolve o PID por regex `FiveM_b\d{4}_GTAProcess\.exe`, abre com `PROCESS_ALL_ACCESS`.
4. Overlay criado com `WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT`, `SetWindowDisplayAffinity(WDA_EXCLUDEFROMCAPTURE)`.
5. Tecla de menu: `VK_INSERT` (`Config.hpp:30`, `MenuKey`).

## 8. Verificacao do binario

```powershell
$exe = "bin\build\Fivem-Loader.exe"
$b = [System.IO.File]::ReadAllBytes($exe)
"machine 0x{0:X4}" -f [BitConverter]::ToUInt16($b, [BitConverter]::ToInt32($b,0x3C) + 4)   # 0x8664
"subsystem $([BitConverter]::ToUInt16($b, [BitConverter]::ToInt32($b,0x3C) + 0x5C))"        # 2 = GUI
(Get-FileHash $exe -Algorithm SHA256).Hash
```

Esperado: `0x8664`, `2`, `9DF009D85F585C0EBCF7693B510DE8C850145D8C28CFB60A5731F232BC389E0C`.

## 9. Carga de driver

`BattleFive` (Settings → Globals → `EmacLab Support`) desvia a leitura para o driver. `Memory.cpp:347` baixa `https://files.catbox.moe/td4bn6.sys` para `C:\Windows\4ware.sys`, registra `sc create 4ware type= kernel start= auto` e `sc start 4ware`. Sem `BattleFive`, usa `OpenProcess(PROCESS_ALL_ACCESS)`.

## 10. Escrita de config

`ConfigThread.hpp` grava em `C:\Windows\SysWOW64\slmgr\0416\slmgrs.ini`. O diretorio precisa existir antes do primeiro save:

```powershell
New-Item -ItemType Directory -Force "C:\Windows\SysWOW64\slmgr\0416" | Out-Null
```
