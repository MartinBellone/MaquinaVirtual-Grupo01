# MaquinaVirtual-Grupo01
Maquina Virtual de Fundamentos de la Arquitectura de las Computadoras

## Requerimientos Previos
<u>Sistema Operativo:</u> Windows / UNIX <br>
<u>Compilador Requerido:</u> gcc
<br>

### Pasos para ejecutar 
<ol>
<li>
 Clonar este repositorio.
</li>
<li>
 Posicionarse dentro de la carpeta del repositorio y abrir una terminal
</li>
<li>
 Si su sistema operativo es windows ejecutar:
 <br>
   <code>
   ./bin/vmx.exe "nombreDeArchivo.vmx"
   </code>
   <br>
   Si su sistema operativo es basado en unix ejectuar:
    Si su sistema operativo es windows ejecutar:<br>
   <code>
   ./bin/vmxUNIX "nombreDeArchivo.vmx"
   </code>
</li>
</ol>

### Pasos para compilar:

<u>Si su sistema operativo es basado en UNIX:</u>
<ol>
  <li>
   Descargar la ultima versión del compilador gcc junto con la herramienta MAKE. 
  </li>
  <li>
   Dirigirse a la carpeta del repositorio y ejecutar los siguientes comandos:
   <br>
   <code>make clean</code>
   <br>

   <code>make -f Makefile.unix</code>
   <br>
  </li>
  <li>
   Ejecutar siguiendo los pasos antes mencionados.
  </li>
</ol>

<u>Si su sistema operativo es Windows:</u>
<ol>
 <li>
  Instalar el paquete de herramientas de MinGW. Recordar la ubicacion de la instalacion.
 </li>
 <li>
  Dirigirse a la ubicacion de la instalacion (por defecto "C:\msys64\ucrt64\bin")
 </li>
 <li>
 Renombrar el archivo llamado "mingw32-make.exe" a "make.exe".
 </li>
 <li>
  Agregar la ubicacion actual (por defecto "C:\msys64\ucrt64\bin") a la variable PATH del sistema.
 </li>
 <li>
  Dirigirse la carpeta donde se clonó el repositorio y ejecutar los siguientes comandos:
   <br>
   <code>make clean</code>
   <br>

   <code>make</code>
   <br>
 </li>
</ol>

### <u>ATENCIÓN</u>
Los warnings que aparecen luego de ejecutar el comando make pueden ser ignorados. Los mismos hacen referencia (mayormente) a variables que no estan siendo usadas. Las mismas permanecen en el programa mantener una estructura única.
