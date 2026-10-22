# Gestor de Turnos y Solicitudes

## Descripción

Aplicación de escritorio desarrollada en C++ con GTK 3 para registrar y gestionar solicitudes según su nivel de prioridad.

Permite organizar las solicitudes, consultar su estado y registrar eventos relacionados con su gestión. Los datos se guardan en un archivo local para conservar la información entre ejecuciones.

## Funcionalidades

- Registrar solicitudes con diferentes niveles de prioridad.
- Consultar las solicitudes registradas y su estado.
- Atender y cancelar solicitudes.
- Confirmar la cancelación antes de realizarla.
- Establecer fechas y horas límite para las solicitudes.
- Identificar solicitudes vencidas.
- Consultar el historial de eventos.
- Conservar los datos entre ejecuciones mediante almacenamiento local.

## Tecnologías utilizadas

- C++
- GTK 3
- Biblioteca estándar de C++
- Sistema de compilación mediante g++

## Estructuras de datos

El proyecto utiliza las siguientes estructuras:

- **Arreglo dinámico (`std::vector`):** almacena las solicitudes registradas.
- **Cola de prioridad (`std::priority_queue`):** organiza las solicitudes según su prioridad.
- **Pila (`std::stack`):** almacena eventos del historial.

## Requisitos

- Un sistema Linux.
- Un compilador compatible con C++17, como g++.
- GTK 3 y sus archivos de desarrollo.
- `pkg-config`.

En Fedora, si faltan las herramientas necesarias, se pueden instalar con:

```bash
sudo dnf install gcc-c++ gtk3-devel pkgconf-pkg-config
```

## Compilación

Desde la carpeta del proyecto, ejecutar:

```bash
g++ -std=c++17 -Wall -Wextra main.cpp GestorSolicitudes.cpp -o gestor-turnos $(pkg-config --cflags --libs gtk+-3.0)
```

## Ejecución

Después de compilar, ejecutar:

```bash
./gestor-turnos
```

## Almacenamiento de datos

El archivo `datos_gestor.txt` almacena los datos persistentes de la aplicación, incluidas las solicitudes y la información del historial.

No se debe eliminar este archivo si se desea conservar los datos guardados.

## Archivos principales

- `main.cpp`: interfaz gráfica y manejo de las interacciones del usuario.
- `Solicitud.h`: definición de la estructura que representa una solicitud.
- `GestorSolicitudes.h`: declaración de la clase encargada de gestionar las solicitudes.
- `GestorSolicitudes.cpp`: implementación de la gestión de solicitudes y sus operaciones.
- `datos_gestor.txt`: archivo de almacenamiento persistente generado por la aplicación.

## Uso de inteligencia artificial

Porcentaje de código generado con ayuda de inteligencia artificial: 100 %.

