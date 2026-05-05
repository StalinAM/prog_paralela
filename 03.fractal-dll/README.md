## Comandos

Comandos utiles para inspeccionar la DLL con GNU objdump. Ejecuta los ejemplos desde la carpeta `Debug`.

### Informacion general del binario

Muestra el formato, cabeceras y metadatos principales de la DLL.

```
objdump -p libfractal-dll.dll
```

Ejemplo de salida:

```
libfractal-dll.dll:     file format pei-x86-64

Characteristics 0x2026
        executable
        line numbers stripped
        large address aware
        DLL

Time/Date               Tue May  5 13:50:50 2026
Magic                   020b    (PE32+)
MajorLinkerVersion      2
MinorLinkerVersion      46
SizeOfCode              0000000000002e00
SizeOfInitializedData   0000000000002600
SizeOfUninitializedData 0000000000000200
AddressOfEntryPoint     00000000000012ef
BaseOfCode              0000000000001000
ImageBase               00000003aab50000
SectionAlignment        00001000
FileAlignment           00000200
```

### Buscar un simbolo exportado

Filtra la salida para ver si el simbolo `julia_simd` esta presente.

```
objdump -p libfractal-dll.dll | findstr "julia_simd"
```

Ejemplo de salida:

```
        [   0] +base[   1]  0000 _Z10julia_simdddddjjiPj
```

### Ver dependencias del modulo

Lista las DLL requeridas por el binario.

```
objdump -p libfractal-dll.dll | findstr "DLL"
```

Ejemplo de salida:

```
        DLL
 vma:            Hint    Time      Forward  DLL       First
        DLL Name: libgcc_s_seh-1.dll
        DLL Name: libstdc++-6.dll
        DLL Name: KERNEL32.dll
        DLL Name: api-ms-win-crt-heap-l1-1-0.dll
        DLL Name: api-ms-win-crt-private-l1-1-0.dll
        DLL Name: api-ms-win-crt-runtime-l1-1-0.dll
        DLL Name: api-ms-win-crt-stdio-l1-1-0.dll
        DLL Name: api-ms-win-crt-string-l1-1-0.dll
```
