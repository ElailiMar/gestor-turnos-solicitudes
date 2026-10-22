# Manual de usuario
## Gestor de Turnos y Solicitudes

### 1. Introducción

El Gestor de Turnos y Solicitudes es una aplicación de escritorio que permite registrar solicitudes, asignarles niveles de prioridad, atenderlas, cancelarlas y consultar el historial de eventos.

La aplicación también permite establecer fechas y horas límite para las solicitudes y conserva los datos entre ejecuciones.

### 2. Requisitos

Para ejecutar la aplicación se necesita:
- Un sistema Linux.
- El ejecutable `gestor-turnos`, previamente compilado.
- El archivo `datos_gestor.txt` para conservar los datos guardados, cuando ya exista.

### 3. Iniciar la aplicación

Abre una terminal en la carpeta del proyecto y ejecuta:

```bash
./gestor-turnos
```

Se abrirá la ventana principal con los campos de registro, los botones de operación y la tabla de solicitudes.

### 4. Registrar una solicitud

1. Escribe una descripción en el campo «Describe la solicitud...».
2. Selecciona la prioridad: Baja, Media o Alta. La prioridad inicial es Media.
3. Si la solicitud tiene un plazo, activa la casilla «Esta solicitud tiene fecha y hora límite».
4. Introduce la fecha en formato DD/MM/AAAA, por ejemplo, `22/10/2026`.
5. Introduce la hora en formato de 24 horas HH:MM, por ejemplo, `14:30`.
6. Pulsa «Registrar solicitud».

Si la fecha u hora no es válida, o el plazo no es posterior al momento actual, la aplicación mostrará un aviso y no registrará la solicitud con esos datos.

Si la solicitud no tiene fecha límite, deja desactivada la casilla correspondiente.

### 5. Consultar las solicitudes

La tabla principal muestra las solicitudes registradas y contiene estas columnas:

- **Turno:** número asignado a la solicitud.
- **Descripción:** texto que identifica la solicitud.
- **Prioridad:** nivel asignado (Baja, Media o Alta).
- **Estado:** situación actual de la solicitud, como Pendiente, Atendida, Cancelada o Vencida.
- **Creada:** fecha y hora de registro.
- **Fecha límite:** plazo establecido, o «Sin plazo» si no se indicó uno.
- **Atendida:** fecha y hora de atención, cuando corresponda.

Las solicitudes más recientes aparecen primero. Para cancelar una solicitud, selecciónala en la tabla.

### 6. Atender una solicitud

Pulsa «Atender siguiente» para solicitar la atención de la siguiente solicitud elegible según la prioridad.

Las solicitudes canceladas o vencidas no deben atenderse. Las solicitudes que no tengan fecha límite pueden atenderse mientras permanezcan pendientes.

### 7. Cancelar una solicitud

1. Selecciona la solicitud que deseas cancelar en la tabla.
2. Pulsa «Cancelar solicitud seleccionada».
3. Lee el mensaje de confirmación.
4. Confirma la operación si realmente deseas cancelarla.

La confirmación permite evitar cancelaciones accidentales.

### 8. Consultar el historial

Pulsa «Ver historial» para consultar los eventos registrados por la aplicación, como las operaciones realizadas sobre las solicitudes.

### 9. Fechas límite y vencimientos

Las fechas deben escribirse en formato DD/MM/AAAA y las horas en formato de 24 horas HH:MM.

La aplicación valida que las fechas existan en el calendario y que el plazo sea posterior al momento actual. Una solicitud pendiente cuyo plazo haya vencido se identifica como Vencida.

### 10. Almacenamiento de los datos

La aplicación utiliza el archivo `datos_gestor.txt` para guardar información persistente.

Para conservar los datos entre ejecuciones, no elimines ni sobrescribas este archivo. Si lo borras, puedes perder las solicitudes y el historial almacenados.

### 11. Cerrar la aplicación

Cierra la ventana principal cuando termines de utilizar el programa. Los datos guardados en el archivo persistente pueden recuperarse al iniciar la aplicación nuevamente.
