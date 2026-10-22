# Uso de estructuras de datos
## Gestor de Turnos y Solicitudes

### 1. Introducción

El proyecto utiliza tres estructuras de datos lineales de la biblioteca estándar de C++:

1. Arreglo dinámico (`std::vector`).
2. Cola de prioridad (`std::priority_queue`).
3. Pila (`std::stack`).

Cada estructura cumple una función diferente dentro de la aplicación.

## 2. Arreglo dinámico: std::vector

### ¿Qué es?

Un arreglo dinámico es una estructura que permite almacenar varios elementos y cambiar su tamaño durante la ejecución.

En C++, `std::vector` implementa esta estructura.

### ¿Dónde se utiliza?

En `GestorSolicitudes.h`:

```cpp
std::vector<Solicitud> solicitudes;
```

También se utiliza para almacenar temporalmente elementos durante algunas operaciones de guardado y consulta del historial.

### ¿Para qué sirve en el proyecto?

El vector `solicitudes` conserva los registros completos de las solicitudes. Cada registro contiene información como el número de turno, la descripción, la prioridad, el estado y las fechas correspondientes.

Cuando se registra una solicitud, se utiliza `push_back()` para añadirla al vector.

### ¿Por qué se eligió?

Porque la cantidad de solicitudes puede variar durante la ejecución. No es necesario establecer de antemano una cantidad máxima fija de registros.

Además, permite recorrer las solicitudes para mostrarlas en la interfaz, consultar sus datos y actualizar sus estados.

## 3. Cola de prioridad: std::priority_queue

### ¿Qué es?

Una cola de prioridad es una estructura que permite extraer primero el elemento que tiene mayor prioridad, en lugar de atender los elementos únicamente por su posición de llegada.

### ¿Dónde se utiliza?

En `GestorSolicitudes.h`:

```cpp
std::priority_queue<
    EntradaPrioridad,
    std::vector<EntradaPrioridad>,
    CompararPrioridad
> pendientes;
```

La estructura almacena entradas que contienen tres datos:

- `prioridad`: nivel de prioridad de la solicitud.
- `orden`: orden utilizado para resolver los empates.
- `turno`: número que identifica la solicitud.

La clase `CompararPrioridad` define cómo se ordenan estas entradas.

### ¿Para qué sirve en el proyecto?

Organiza las solicitudes que están pendientes de atención. La aplicación consulta la entrada que encabeza la cola mediante `top()` y la retira mediante `pop()` cuando corresponde procesarla.

Las solicitudes de prioridad Alta se sitúan antes que las de prioridad Media, y estas antes que las de prioridad Baja.

Cuando dos solicitudes tienen la misma prioridad, el comparador utiliza el orden de registro para que la más antigua tenga preferencia.

Las solicitudes canceladas o vencidas no deben tratarse como solicitudes atendibles.

### ¿Por qué se eligió?

Porque permite seleccionar la siguiente solicitud según su prioridad sin tener que buscar manualmente cuál debe atenderse en cada operación.

## 4. Pila: std::stack

### ¿Qué es?

Una pila es una estructura de datos que sigue el principio LIFO, siglas de *Last In, First Out*: el último elemento que entra es el primero que sale.

Sus operaciones principales son:

- `push()`: añade un elemento.
- `top()`: consulta el elemento que está en la cima.
- `pop()`: retira el elemento de la cima.

### ¿Dónde se utiliza?

En `GestorSolicitudes.h`:

```cpp
std::stack<std::string> historial;
```

### ¿Para qué sirve en el proyecto?

La pila conserva los mensajes de los eventos y las operaciones relacionadas con las solicitudes.

Cuando ocurre una operación que debe registrarse en el historial, se añade una cadena de texto a la pila mediante `push()`.

Para consultar o guardar el historial, el programa utiliza copias de la pila y retira sus elementos con `top()` y `pop()`. Esto permite recorrer los registros sin vaciar la pila original que mantiene el gestor.

### ¿Por qué se eligió?

Porque una pila permite añadir y consultar eventos mediante operaciones sencillas, siguiendo el principio LIFO.

En este proyecto se utiliza para conservar los eventos del historial. No se emplea como mecanismo para deshacer operaciones.

## 5. ¿Por qué se utilizan las tres estructuras?

Las tres estructuras tienen responsabilidades diferentes:

| Estructura | Responsabilidad |
|---|---|
| `std::vector<Solicitud>` | Conservar los registros completos de las solicitudes. |
| `std::priority_queue` | Organizar las solicitudes pendientes según su prioridad. |
| `std::stack<std::string>` | Conservar los eventos del historial. |

Utilizar las tres permite separar el almacenamiento general de las solicitudes, la selección de la siguiente solicitud atendible y el registro de los eventos.

## 6. Persistencia de los datos

Las estructuras anteriores se utilizan durante la ejecución del programa. Para conservar los datos después de cerrar la aplicación, el gestor los guarda en el archivo `datos_gestor.txt`.

Al iniciar nuevamente la aplicación, los datos guardados se cargan para recuperar las solicitudes y el historial.

Por tanto, las estructuras de datos organizan la información en memoria, mientras que el archivo permite conservarla entre ejecuciones.

## 7. Conclusión

El proyecto aplica tres estructuras de datos lineales con propósitos distintos: el arreglo dinámico almacena las solicitudes, la cola de prioridad determina el orden de atención y la pila mantiene los eventos del historial.

La combinación de estas estructuras permite organizar las operaciones principales del Gestor de Turnos y Solicitudes.
