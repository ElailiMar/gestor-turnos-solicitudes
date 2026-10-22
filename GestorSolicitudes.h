#ifndef GESTOR_SOLICITUDES_H
#define GESTOR_SOLICITUDES_H

#include "Solicitud.h"
#include <string>
#include <vector>
#include <queue>
#include <stack>

class GestorSolicitudes {
private:
    struct EntradaPrioridad {
        int prioridad;
        int orden;
        int turno;
    };

    struct CompararPrioridad {
        bool operator()(const EntradaPrioridad& a,
                        const EntradaPrioridad& b) const {
            if (a.prioridad != b.prioridad)
                return a.prioridad < b.prioridad;

            return a.orden > b.orden;
        }
    };

    // Arreglo dinámico: almacena todas las solicitudes.
    std::vector<Solicitud> solicitudes;

    // Cola de prioridad: organiza las solicitudes atendibles.
    std::priority_queue<
        EntradaPrioridad,
        std::vector<EntradaPrioridad>,
        CompararPrioridad
    > pendientes;

    // Pila: conserva el historial de operaciones.
    std::stack<std::string> historial;

    int siguienteTurno = 1;

    void cargarDatos();
    bool guardarDatos() const;
    void reconstruirCola();

public:
    GestorSolicitudes();

    void registrarSolicitud(
        const std::string& descripcion,
        int prioridad,
        const std::string& fechaLimite
    );

    const std::vector<Solicitud>& obtenerSolicitudes() const;

    bool atenderSiguiente(std::string& mensaje);
    bool cancelarSolicitud(int turno, std::string& mensaje);

    // Detecta y registra los plazos que ya vencieron.
    void actualizarVencidas();

    std::vector<std::string> obtenerHistorial() const;
};

#endif
