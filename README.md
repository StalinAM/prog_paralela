## 1) Descargar MinGW64

[MinGW64](https://github.com/brechtsanders/winlibs_mingw/releases/download/15.2.0posix-14.0.0-ucrt-r7/winlibs-x86_64-posix-seh-gcc-15.2.0-mingw-w64ucrt-14.0.0-r7.7z)

## 2) Clonar y bootstrappear `vcpkg`

En una terminal de PowerShell o Git Bash, dentro de una carpeta de su preferencia, ejecute:

```powershell
cd $PWD
git clone https://github.com/microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
```

Esto generará `vcpkg.exe` en la carpeta `vcpkg`.

Si es necesario, agregue MinGW64 al `PATH`:

```
set PATH=c:/tools/mingw64/bin;%PATH%
```

---

## 3) Instalar `fmt` y `SFML` con `vcpkg`

Usaremos el triplete MinGW dinámico `x64-mingw-dynamic`, que crea bibliotecas vinculadas dinámicamente. Desde la carpeta `vcpkg` ejecute:

```cmd
vcpkg install fmt:x64-mingw-dynamic
vcpkg install sfml:x64-mingw-dynamic
```

Si desea ver los tripletes disponibles, ejecute:

```powershell
.\vcpkg.exe list
.\vcpkg.exe help triplets
```

---
