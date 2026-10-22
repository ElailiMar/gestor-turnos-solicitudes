#ifndef SOLICITUD_H
#define SOLICITUD_H

#include <string>

// Representa una solicitud y sus fechas importantes.
struct Solicitud {
    int turno = 0;
    std::string descripcion;
    int prioridad = 2;

    bool atendida = false;
    bool cancelada = false;
    bool vencida = false;

    std::string fechaCreacion;
    std::string fechaLimite;
    std::string fechaAtencion;
};

#endif
