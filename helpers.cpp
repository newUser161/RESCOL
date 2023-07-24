#include <random>
#include "graph.h"
#include <algorithm>

// Función para eliminar los espacios de una cadena
std::string eliminarEspacios(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());      
    return str;
}

// Esta función genera un número aleatorio entre min y max (ambos incluidos).
double generar_numero_aleatorio(double min, double max) {
    // El generador de números aleatorios
    static std::mt19937 generador(std::random_device{}());

    // La distribución que define el rango
    std::uniform_real_distribution<> distribucion(min, max);

    // Generar y devolver un número aleatorio
    return distribucion(generador);
}
