#include "GestorSolicitudes.h"

#include <fstream>
#include <iomanip>
#include <algorithm>
#include <sstream>
#include <ctime>

// Devuelve la fecha y hora local del sistema.
static std::string fechaHoraActual() {
    std::time_t ahora = std::time(nullptr);
    std::tm* tiempo = std::localtime(&ahora);

    if (!tiempo)
        return "";

    char buffer[32];
    std::strftime(
        buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tiempo
    );

    return buffer;
}

GestorSolicitudes::GestorSolicitudes() {
    cargarDatos();
    actualizarVencidas();
    reconstruirCola();
}

// Reconstruye la cola con solicitudes todavía atendibles.
void GestorSolicitudes::reconstruirCola() {
    pendientes = decltype(pendientes){};

    for (const auto& solicitud : solicitudes) {
        if (!solicitud.atendida &&
            !solicitud.cancelada &&
            !solicitud.vencida) {
            pendientes.push({
                solicitud.prioridad,
                solicitud.turno,
                solicitud.turno
            });
        }
    }
}

// Carga tanto el formato anterior como el formato con fechas.
void GestorSolicitudes::cargarDatos() {
    std::ifstream archivo("datos_gestor.txt");
    std::string linea;

    if (!archivo.is_open())
        return;

    while (std::getline(archivo, linea)) {
        if (linea.empty())
            continue;

        std::istringstream entrada(linea);
        char tipo;

        if (!(entrada >> tipo))
            continue;

        if (tipo == 'S') {
            Solicitud solicitud{};

            if (!(entrada
                  >> solicitud.turno
                  >> solicitud.prioridad
                  >> solicitud.atendida
                  >> solicitud.cancelada
                  >> std::quoted(solicitud.descripcion))) {
                continue;
            }

            // Los campos siguientes no existían en archivos antiguos.
            entrada >> std::quoted(solicitud.fechaCreacion);
            entrada >> std::quoted(solicitud.fechaLimite);
            entrada >> std::quoted(solicitud.fechaAtencion);
            entrada >> solicitud.vencida;

            solicitudes.push_back(solicitud);

            siguienteTurno = std::max(
                siguienteTurno,
                solicitud.turno + 1
            );
        } else if (tipo == 'H') {
            std::string operacion;

            if (entrada >> std::quoted(operacion))
                historial.push(operacion);
        }
    }
}

// Guarda todas las solicitudes y el historial.
bool GestorSolicitudes::guardarDatos() const {
    std::ofstream archivo("datos_gestor.txt");

    if (!archivo.is_open())
        return false;

    for (const auto& solicitud : solicitudes) {
        archivo << "S "
                << solicitud.turno << ' '
                << solicitud.prioridad << ' '
                << solicitud.atendida << ' '
                << solicitud.cancelada << ' '
                << std::quoted(solicitud.descripcion) << ' '
                << std::quoted(solicitud.fechaCreacion) << ' '
                << std::quoted(solicitud.fechaLimite) << ' '
                << std::quoted(solicitud.fechaAtencion) << ' '
                << solicitud.vencida << '\n';
    }

    // Escribe el historial desde el más antiguo hasta el más reciente.
    std::stack<std::string> copia = historial;
    std::vector<std::string> operaciones;

    while (!copia.empty()) {
        operaciones.push_back(copia.top());
        copia.pop();
    }

    for (auto it = operaciones.rbegin();
         it != operaciones.rend(); ++it) {
        archivo << "H " << std::quoted(*it) << '\n';
    }

    return static_cast<bool>(archivo);
}

// Registra una solicitud con plazo opcional.
void GestorSolicitudes::registrarSolicitud(
    const std::string& descripcion,
    int prioridad,
    const std::string& fechaLimite
) {
    Solicitud nueva;

    nueva.turno = siguienteTurno++;
    nueva.descripcion = descripcion;
    nueva.prioridad = prioridad;
    nueva.fechaCreacion = fechaHoraActual();
    nueva.fechaLimite = fechaLimite;

    solicitudes.push_back(nueva);

    pendientes.push({
        prioridad,
        nueva.turno,
        nueva.turno
    });

    std::string operacion =
        "[" + nueva.fechaCreacion + "] Registrada solicitud #"
        + std::to_string(nueva.turno) + ": " + descripcion;

    if (!fechaLimite.empty())
        operacion += " (plazo: " + fechaLimite + ")";

    historial.push(operacion);
    guardarDatos();
}

const std::vector<Solicitud>&
GestorSolicitudes::obtenerSolicitudes() const {
    return solicitudes;
}

// Marca como vencidas las solicitudes que superaron su plazo.
// Se ejecuta al iniciar, al refrescar la tabla y antes de atender.
void GestorSolicitudes::actualizarVencidas() {
    const std::string ahora = fechaHoraActual();
    bool huboCambios = false;

    for (auto& solicitud : solicitudes) {
        if (!solicitud.atendida &&
            !solicitud.cancelada &&
            !solicitud.vencida &&
            !solicitud.fechaLimite.empty() &&
            ahora >= solicitud.fechaLimite) {

            solicitud.vencida = true;
            huboCambios = true;

            historial.push(
                "[" + ahora + "] Vencio el plazo del turno #"
                + std::to_string(solicitud.turno) + ": "
                + solicitud.descripcion
            );
        }
    }

    if (huboCambios) {
        reconstruirCola();
        guardarDatos();
    }
}

// Atiende la solicitud de mayor prioridad que no esté vencida.
bool GestorSolicitudes::atenderSiguiente(std::string& mensaje) {
    actualizarVencidas();

    if (pendientes.empty()) {
        mensaje = "No hay solicitudes pendientes que puedan atenderse.";
        return false;
    }

    EntradaPrioridad siguiente = pendientes.top();
    pendientes.pop();

    for (auto& solicitud : solicitudes) {
        if (solicitud.turno == siguiente.turno &&
            !solicitud.atendida &&
            !solicitud.cancelada &&
            !solicitud.vencida) {

            // Comprueba otra vez el plazo justo antes de atender.
            actualizarVencidas();

            if (solicitud.vencida) {
                mensaje = "El plazo de esta solicitud ya vencio.";
                reconstruirCola();
                return false;
            }

            solicitud.atendida = true;
            solicitud.fechaAtencion = fechaHoraActual();

            mensaje = "Se atendio el turno #"
                + std::to_string(solicitud.turno)
                + ": " + solicitud.descripcion
                + "\nFecha y hora: " + solicitud.fechaAtencion;

            historial.push(
                "[" + solicitud.fechaAtencion + "] " + mensaje
            );

            reconstruirCola();
            guardarDatos();
            return true;
        }
    }

    mensaje = "No se encontro una solicitud atendible.";
    reconstruirCola();
    return false;
}

// Cancela una solicitud pendiente que no esté vencida.
bool GestorSolicitudes::cancelarSolicitud(
    int turno,
    std::string& mensaje
) {
    actualizarVencidas();

    for (auto& solicitud : solicitudes) {
        if (solicitud.turno == turno) {
            if (solicitud.atendida) {
                mensaje = "No se puede cancelar: ya fue atendida.";
                return false;
            }

            if (solicitud.cancelada) {
                mensaje = "La solicitud ya estaba cancelada.";
                return false;
            }

            if (solicitud.vencida) {
                mensaje = "No se puede cancelar: la solicitud ya vencio.";
                return false;
            }

            solicitud.cancelada = true;

            mensaje = "Se cancelo el turno #"
                + std::to_string(solicitud.turno)
                + ": " + solicitud.descripcion;

            historial.push(
                "[" + fechaHoraActual() + "] " + mensaje
            );

            reconstruirCola();
            guardarDatos();
            return true;
        }
    }

    mensaje = "No se encontro el turno indicado.";
    return false;
}

// Devuelve las operaciones de la más reciente a la más antigua.
std::vector<std::string>
GestorSolicitudes::obtenerHistorial() const {
    std::vector<std::string> resultado;
    std::stack<std::string> copia = historial;

    while (!copia.empty()) {
        resultado.push_back(copia.top());
        copia.pop();
    }

    return resultado;
}
