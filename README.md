# 🖥️ MaquinaVirtual-Grupo01
**Maquina Virtual de Fundamentos de la Arquitectura de las Computadoras**

![OS](https://img.shields.io/badge/OS-Windows%20%7C%20UNIX-blue?logo=windows&logoColor=white)
![Compiler](https://img.shields.io/badge/Compiler-gcc-orange?logo=gnu)
![Status](https://img.shields.io/badge/Project-Active-success)

---

## 📋 Requerimientos Previos
- **Sistema Operativo:** Windows / UNIX  
- **Compilador Requerido:** gcc  

---

## ▶️ Pasos para ejecutar

1. Clonar este repositorio.  
2. Posicionarse dentro de la carpeta del repositorio y abrir una terminal.
3. Colocar el archivo con extensión .vmx a ejecutar en la misma carpeta (por ejemplo "nombreDeArchivo.vmx").
4. Ejecutar según el sistema operativo:  

   - **💻 Windows**  
     ```bash
     ./bin/vmx.exe "nombreDeArchivo.vmx"
     ```

   - **🐧 UNIX**  
     ```bash
     ./bin/vmxUNIX "nombreDeArchivo.vmx"
     ```

---

## ⚙️ Pasos para compilar

### 🐧 En UNIX
1. Descargar la última versión del compilador **gcc** junto con la herramienta **make**.  
2. Dirigirse a la carpeta del repositorio y ejecutar los siguientes comandos:  
   ```bash
   make clean
   make -f Makefile.unix
### 💻 En Windows
1. Instalar el paquete de herramientas de **MinGW**. Recordar la ubicación de la instalación.  
2. Dirigirse a la ubicación de la instalación (por defecto `C:\msys64\ucrt64\bin`).  
3. Renombrar el archivo llamado `mingw32-make.exe` a `make.exe`.  
4. Agregar la ubicación actual (por defecto `C:\msys64\ucrt64\bin`) a la variable **PATH** del sistema.  
5. Dirigirse a la carpeta donde se clonó el repositorio y ejecutar los siguientes comandos:  
   ```bash
   make clean
   make
