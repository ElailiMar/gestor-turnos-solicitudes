#include <gtk/gtk.h>
#include "GestorSolicitudes.h"

#include <string>
#include <vector>
#include <ctime>
#include <iomanip>
#include <sstream>

GestorSolicitudes gestor;

GtkWidget *entradaDescripcion;
GtkWidget *selectorPrioridad;
GtkWidget *activarPlazo;
GtkWidget *entradaFechaLimite;
GtkWidget *entradaHoraLimite;
GtkListStore *modeloLista;
GtkWidget *tabla;

void mostrarMensaje(GtkMessageType tipo, const std::string& mensaje) {
    GtkWidget *dialogo = gtk_message_dialog_new(
        nullptr, GTK_DIALOG_MODAL, tipo, GTK_BUTTONS_OK,
        "%s", mensaje.c_str()
    );
    gtk_dialog_run(GTK_DIALOG(dialogo));
    gtk_widget_destroy(dialogo);
}


bool convertirFechaHoraColombia(
    const std::string& texto,
    std::string& fechaISO
) {
    std::tm tiempo = {};
    std::istringstream entrada(texto);

    entrada >> std::get_time(&tiempo, "%d/%m/%Y %H:%M");

    if (entrada.fail())
        return false;

    entrada >> std::ws;
    if (!entrada.eof())
        return false;

    // Guardamos los valores originales para detectar fechas imposibles.
    int dia = tiempo.tm_mday;
    int mes = tiempo.tm_mon;
    int anio = tiempo.tm_year;
    int hora = tiempo.tm_hour;
    int minuto = tiempo.tm_min;

    tiempo.tm_isdst = -1;

    if (std::mktime(&tiempo) == -1)
        return false;

    // mktime normaliza fechas imposibles; comprobamos que no las cambie.
    if (tiempo.tm_mday != dia ||
        tiempo.tm_mon != mes ||
        tiempo.tm_year != anio ||
        tiempo.tm_hour != hora ||
        tiempo.tm_min != minuto)
        return false;

    char buffer[32];
    std::strftime(
        buffer, sizeof(buffer), "%Y-%m-%d %H:%M", &tiempo
    );

    fechaISO = buffer;
    return true;
}

std::string fechaHoraActualMinutos() {
    std::time_t ahora = std::time(nullptr);
    std::tm* tiempo = std::localtime(&ahora);

    if (!tiempo)
        return "";

    char buffer[32];
    std::strftime(
        buffer, sizeof(buffer), "%Y-%m-%d %H:%M", tiempo
    );

    return buffer;
}

std::string formatearFechaHora(const std::string& fecha) {
    if (fecha.empty())
        return fecha;

    std::tm tiempo = {};
    std::istringstream entrada(fecha);

    entrada >> std::get_time(&tiempo, "%Y-%m-%d %H:%M:%S");

    if (entrada.fail()) {
        entrada.clear();
        entrada.str(fecha);
        tiempo = {};
        entrada >> std::get_time(&tiempo, "%Y-%m-%d %H:%M");
    }

    if (entrada.fail())
        return fecha;

    char buffer[32];
    std::strftime(
        buffer, sizeof(buffer), "%d/%m/%Y %H:%M", &tiempo
    );

    return buffer;
}

void actualizarCamposPlazo(GtkToggleButton*, gpointer) {
    bool activo = gtk_toggle_button_get_active(
        GTK_TOGGLE_BUTTON(activarPlazo)
    );

    gtk_widget_set_sensitive(entradaFechaLimite, activo);
    gtk_widget_set_sensitive(entradaHoraLimite, activo);
}

// Actualiza la tabla y registra cualquier vencimiento nuevo.
void actualizarLista() {
    gestor.actualizarVencidas();
    gtk_list_store_clear(modeloLista);

    const auto& solicitudes = gestor.obtenerSolicitudes();

    for (auto it = solicitudes.rbegin();
         it != solicitudes.rend(); ++it) {
        const auto& solicitud = *it;

        GtkTreeIter fila;
        gtk_list_store_append(modeloLista, &fila);

        const char* prioridadTexto =
            solicitud.prioridad == 3 ? "Alta" :
            solicitud.prioridad == 2 ? "Media" : "Baja";

        const char* estadoTexto =
            solicitud.cancelada ? "Cancelada" :
            solicitud.atendida ? "Atendida" :
            solicitud.vencida ? "Vencida" : "Pendiente";

        std::string creacion = solicitud.fechaCreacion.empty()
            ? "No registrada" : solicitud.fechaCreacion;

        std::string limite = solicitud.fechaLimite.empty()
            ? "Sin plazo" : solicitud.fechaLimite;

        std::string atencion = solicitud.fechaAtencion.empty()
            ? "-" : solicitud.fechaAtencion;

        if (!solicitud.fechaCreacion.empty())
            creacion = formatearFechaHora(creacion);

        if (!solicitud.fechaLimite.empty())
            limite = formatearFechaHora(limite);

        if (!solicitud.fechaAtencion.empty())
            atencion = formatearFechaHora(atencion);

        gtk_list_store_set(
            modeloLista, &fila,
            0, solicitud.turno,
            1, solicitud.descripcion.c_str(),
            2, prioridadTexto,
            3, estadoTexto,
            4, creacion.c_str(),
            5, limite.c_str(),
            6, atencion.c_str(),
            -1
        );
    }
}

void registrarSolicitud(GtkButton*, gpointer) {
    const char* texto =
        gtk_entry_get_text(GTK_ENTRY(entradaDescripcion));

    std::string descripcion = texto ? texto : "";

    if (descripcion.find_first_not_of(" \t\n\r") ==
        std::string::npos) {
        mostrarMensaje(
            GTK_MESSAGE_WARNING,
            "Escribe una descripcion para la solicitud."
        );
        return;
    }

    std::string fechaLimite;

    if (gtk_toggle_button_get_active(
            GTK_TOGGLE_BUTTON(activarPlazo))) {
        const char* fechaTexto =
            gtk_entry_get_text(GTK_ENTRY(entradaFechaLimite));
        const char* horaTexto =
            gtk_entry_get_text(GTK_ENTRY(entradaHoraLimite));

        std::string fecha = fechaTexto ? fechaTexto : "";
        std::string hora = horaTexto ? horaTexto : "";

std::string fechaIntroducida = fecha + " " + hora;

if (!convertirFechaHoraColombia(fechaIntroducida, fechaLimite)) {
            mostrarMensaje(
                GTK_MESSAGE_WARNING,
                "Fecha u hora no valida. Usa DD/MM/AAAA y HH:MM."
            );
            return;
        }

        if (fechaLimite <= fechaHoraActualMinutos()) {
            mostrarMensaje(
                GTK_MESSAGE_WARNING,
                "El plazo debe ser posterior a la hora actual."
            );
            return;
        }

        fechaLimite += ":00";
    }

    int prioridad = gtk_combo_box_get_active(
        GTK_COMBO_BOX(selectorPrioridad)
    ) + 1;

    gestor.registrarSolicitud(
        descripcion, prioridad, fechaLimite
    );

    gtk_entry_set_text(GTK_ENTRY(entradaDescripcion), "");
    actualizarLista();
}

void atenderSiguiente(GtkButton*, gpointer) {
    std::string mensaje;
    bool atendida = gestor.atenderSiguiente(mensaje);

    mostrarMensaje(
        atendida ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
        mensaje
    );

    actualizarLista();
}

void cancelarSolicitud(GtkButton*, gpointer) {
    GtkTreeSelection *seleccion = gtk_tree_view_get_selection(
        GTK_TREE_VIEW(tabla)
    );

    GtkTreeModel *modelo;
    GtkTreeIter fila;

    if (!gtk_tree_selection_get_selected(
            seleccion, &modelo, &fila)) {
        mostrarMensaje(
            GTK_MESSAGE_WARNING,
            "Selecciona una solicitud de la tabla primero."
        );
        return;
    }

    int turno;
    gtk_tree_model_get(modelo, &fila, 0, &turno, -1);

    std::string descripcion;
    std::string estado;

    for (const auto& solicitud : gestor.obtenerSolicitudes()) {
        if (solicitud.turno == turno) {
            descripcion = solicitud.descripcion;
            estado = solicitud.cancelada ? "Cancelada" :
                     solicitud.atendida ? "Atendida" :
                     solicitud.vencida ? "Vencida" : "Pendiente";
            break;
        }
    }

    if (estado != "Pendiente") {
        mostrarMensaje(
            GTK_MESSAGE_WARNING,
            "Solo se pueden cancelar solicitudes pendientes."
        );
        return;
    }

    GtkWidget *confirmacion = gtk_message_dialog_new(
        nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "¿Seguro que quieres cancelar el turno #%d?\n%s",
        turno, descripcion.c_str()
    );

    gtk_window_set_title(
        GTK_WINDOW(confirmacion), "Confirmar cancelacion"
    );

    gint respuesta = gtk_dialog_run(GTK_DIALOG(confirmacion));
    gtk_widget_destroy(confirmacion);

    if (respuesta != GTK_RESPONSE_YES)
        return;

    std::string mensaje;
    bool cancelada = gestor.cancelarSolicitud(turno, mensaje);

    mostrarMensaje(
        cancelada ? GTK_MESSAGE_INFO : GTK_MESSAGE_WARNING,
        mensaje
    );

    actualizarLista();
}

void verHistorial(GtkButton*, gpointer) {
    std::vector<std::string> historial =
        gestor.obtenerHistorial();

    std::string texto;

    for (const auto& operacion : historial)
        texto += operacion + "\n";

    if (texto.empty())
        texto = "Todavia no hay operaciones registradas.";

    GtkWidget *dialogo = gtk_message_dialog_new(
        nullptr, GTK_DIALOG_MODAL, GTK_MESSAGE_INFO,
        GTK_BUTTONS_OK, "%s", texto.c_str()
    );

    gtk_window_set_title(
        GTK_WINDOW(dialogo), "Historial de operaciones"
    );

    gtk_dialog_run(GTK_DIALOG(dialogo));
    gtk_widget_destroy(dialogo);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *ventana = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(
        GTK_WINDOW(ventana), "Gestor de Turnos y Solicitudes"
    );
    gtk_window_set_default_size(GTK_WINDOW(ventana), 1100, 600);
    gtk_window_set_position(GTK_WINDOW(ventana), GTK_WIN_POS_CENTER);

    g_signal_connect(
        ventana, "destroy", G_CALLBACK(gtk_main_quit), nullptr
    );

    GtkWidget *contenedor = gtk_box_new(
        GTK_ORIENTATION_VERTICAL, 8
    );
    gtk_container_set_border_width(GTK_CONTAINER(contenedor), 12);

    GtkWidget *titulo = gtk_label_new(
        "Gestor de Turnos y Solicitudes"
    );
    gtk_box_pack_start(
        GTK_BOX(contenedor), titulo, FALSE, FALSE, 3
    );

    entradaDescripcion = gtk_entry_new();
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(entradaDescripcion), "Describe la solicitud..."
    );
    gtk_box_pack_start(
        GTK_BOX(contenedor), entradaDescripcion, FALSE, FALSE, 0
    );

    selectorPrioridad = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(selectorPrioridad), "Baja"
    );
    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(selectorPrioridad), "Media"
    );
    gtk_combo_box_text_append_text(
        GTK_COMBO_BOX_TEXT(selectorPrioridad), "Alta"
    );
    gtk_combo_box_set_active(
        GTK_COMBO_BOX(selectorPrioridad), 1
    );
    gtk_box_pack_start(
        GTK_BOX(contenedor), selectorPrioridad, FALSE, FALSE, 0
    );

    activarPlazo = gtk_check_button_new_with_label(
        "Esta solicitud tiene fecha y hora limite"
    );
    gtk_box_pack_start(
        GTK_BOX(contenedor), activarPlazo, FALSE, FALSE, 0
    );

    GtkWidget *filaFecha = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8);

    entradaFechaLimite = gtk_entry_new();
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(entradaFechaLimite), "Fecha: DD/MM/AAAA"
    );
    gtk_widget_set_sensitive(entradaFechaLimite, FALSE);
    gtk_box_pack_start(
        GTK_BOX(filaFecha), entradaFechaLimite, TRUE, TRUE, 0
    );

    entradaHoraLimite = gtk_entry_new();
    gtk_entry_set_placeholder_text(
        GTK_ENTRY(entradaHoraLimite), "Hora: HH:MM"
    );
    gtk_widget_set_sensitive(entradaHoraLimite, FALSE);
    gtk_box_pack_start(
        GTK_BOX(filaFecha), entradaHoraLimite, TRUE, TRUE, 0
    );

    gtk_box_pack_start(
        GTK_BOX(contenedor), filaFecha, FALSE, FALSE, 0
    );

    g_signal_connect(
        activarPlazo, "toggled",
        G_CALLBACK(actualizarCamposPlazo), nullptr
    );

    GtkWidget *filaBotones = gtk_box_new(
        GTK_ORIENTATION_HORIZONTAL, 8
    );

    GtkWidget *botonRegistrar =
        gtk_button_new_with_label("Registrar solicitud");
    GtkWidget *botonAtender =
        gtk_button_new_with_label("Atender siguiente");
    GtkWidget *botonCancelar =
        gtk_button_new_with_label("Cancelar solicitud seleccionada");
    GtkWidget *botonHistorial =
        gtk_button_new_with_label("Ver historial");

    g_signal_connect(
        botonRegistrar, "clicked",
        G_CALLBACK(registrarSolicitud), nullptr
    );
    g_signal_connect(
        botonAtender, "clicked",
        G_CALLBACK(atenderSiguiente), nullptr
    );
    g_signal_connect(
        botonCancelar, "clicked",
        G_CALLBACK(cancelarSolicitud), nullptr
    );
    g_signal_connect(
        botonHistorial, "clicked",
        G_CALLBACK(verHistorial), nullptr
    );

    gtk_box_pack_start(
        GTK_BOX(filaBotones), botonRegistrar, FALSE, FALSE, 0
    );
    gtk_box_pack_start(
        GTK_BOX(filaBotones), botonAtender, FALSE, FALSE, 0
    );
    gtk_box_pack_start(
        GTK_BOX(filaBotones), botonCancelar, FALSE, FALSE, 0
    );
    gtk_box_pack_start(
        GTK_BOX(filaBotones), botonHistorial, FALSE, FALSE, 0
    );
    gtk_box_pack_start(
        GTK_BOX(contenedor), filaBotones, FALSE, FALSE, 0
    );

    // Columnas: turno, descripción, prioridad, estado y fechas.
    modeloLista = gtk_list_store_new(
        7,
        G_TYPE_INT,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING,
        G_TYPE_STRING
    );

    tabla = gtk_tree_view_new_with_model(
        GTK_TREE_MODEL(modeloLista)
    );

    gtk_tree_selection_set_mode(
        gtk_tree_view_get_selection(GTK_TREE_VIEW(tabla)),
        GTK_SELECTION_SINGLE
    );

    const char* nombresColumnas[] = {
        "Turno", "Descripcion", "Prioridad", "Estado",
        "Creada", "Fecha limite", "Atendida"
    };

    for (int i = 0; i < 7; ++i) {
        GtkCellRenderer *renderizador =
            gtk_cell_renderer_text_new();

        GtkTreeViewColumn *columna =
            gtk_tree_view_column_new_with_attributes(
                nombresColumnas[i], renderizador,
                "text", i, nullptr
            );

        gtk_tree_view_append_column(
            GTK_TREE_VIEW(tabla), columna
        );
    }

    GtkWidget *desplazamiento =
        gtk_scrolled_window_new(nullptr, nullptr);
    gtk_widget_set_vexpand(desplazamiento, TRUE);
    gtk_container_add(GTK_CONTAINER(desplazamiento), tabla);

    gtk_box_pack_start(
        GTK_BOX(contenedor), desplazamiento, TRUE, TRUE, 0
    );
    gtk_container_add(GTK_CONTAINER(ventana), contenedor);

    actualizarLista();
    gtk_widget_show_all(ventana);
    gtk_main();

    g_object_unref(modeloLista);
    return 0;
}
