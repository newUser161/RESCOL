#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>

#include "instance_reader.h"
#include "graph.h"
#include "helpers.h"

using namespace std;

// Función para leer una instancia
/* Parámetros:
    - nombre_archivo: Nombre del archivo que contiene la instancia
    - leer_restricciones: Indica si se leerán las restricciones del archivo
*/
Graph leerInstancia(const std::string &nombre_archivo, bool leer_restricciones)
{
    // Inicializar el grafo
    Graph g;
    std::unordered_map<std::string, std::string> encabezado;
    std::string lineaDato;
    int IdArco = 0;

    // Intentar abrir el archivo
    std::ifstream infile(nombre_archivo);
    if (!infile)
    {
        std::cerr << "No se pudo abrir el archivo: " << nombre_archivo << std::endl;
        return Graph();
    }

    // Leer la información general del archivo
    for (int i = 0; i < 2; i++)
    {
        std::string linea;
        std::getline(infile, linea);
        size_t pos = linea.find(':');
        std::string clave = eliminarEspacios(linea.substr(0, pos));
        std::string valor = eliminarEspacios(linea.substr(pos + 1));
        encabezado[clave] = valor;
    }
    for (int i = 0; i < 6; i++)
    {
        std::string linea;
        std::getline(infile, linea);
        size_t pos = linea.find(':');
        std::string clave = eliminarEspacios(linea.substr(0, pos));
        std::string valor = eliminarEspacios(linea.substr(pos + 1));
        encabezado[clave] = valor;
    }
    g.metadatos.encabezado = encabezado;

    // Saltarse header aristas obligatorias
    std::getline(infile, lineaDato);

    // Leer aristas obligatorias
    for (int i = 0; i < std::stoi(encabezado["ARISTAS_REQ"]); i++)
    {
        std::getline(infile, lineaDato);
        std::istringstream arcStream(lineaDato);
        std::string bi_or_uni;
        int id, origen, destino, costo_recorrido, costo_recoleccion;
        if (!(arcStream >> bi_or_uni >> origen >> destino >> costo_recorrido >> costo_recoleccion))
        {
            // Error de formato
            std::cerr << "Error en el formato del archivo" << std::endl;
            return Graph();
        }

        // Crear los nodos si no existen
        if (g.nodos.count(origen) == 0)
        {
            Nodo nodo_origen;
            nodo_origen.id = origen;
            g.nodos[origen] = nodo_origen;
        }
        if (g.nodos.count(destino) == 0)
        {
            Nodo nodo_destino;
            nodo_destino.id = destino;
            g.nodos[destino] = nodo_destino;
        }

        // Crear el arco
        Arco *arco = new Arco;
        arco->id = IdArco;
        arco->costo_recorrido = costo_recorrido;
        arco->costo_recoleccion = costo_recoleccion;
        arco->obligatoria = true;
        arco->origen = &g.nodos[origen];
        arco->destino = &g.nodos[destino];
        arco->bidireccional = (bi_or_uni == "bi");
        g.arcos[IdArco] = arco;

        // Conectar el arco a sus nodos
        g.nodos[origen].saliente.push_back(*arco);
        g.nodos[destino].entrante.push_back(*arco);

        // Actualizar la información heurística
        double costo = arco->costo_recorrido + arco->costo_recoleccion;
        g.informacion_heuristica[origen][destino] = 1.0 / costo;

        IdArco++;
    }
    // Saltarse header aristas opcionales
    std::getline(infile, lineaDato);

    // Leer aristas opcionales
    if (encabezado["ARISTAS_NOREQ"] != "0")
    {
        for (int i = 0; i < std::stoi(encabezado["ARISTAS_NOREQ"]); i++)
        {
            std::getline(infile, lineaDato);
            std::istringstream arcStream(lineaDato);
            int origen, destino;
            if (!(arcStream >> origen >> destino))
            {
                // Error de formato
                std::cerr << "Error en el formato del archivo" << std::endl;
                return Graph();
            }
            // Crear los nodos si no existen
            if (g.nodos.count(origen) == 0)
            {
                Nodo nodo_origen;
                nodo_origen.id = origen;
                g.nodos[origen] = nodo_origen;
            }
            if (g.nodos.count(destino) == 0)
            {
                Nodo nodo_destino;
                nodo_destino.id = destino;
                g.nodos[destino] = nodo_destino;
            }

            // Crear el arco opcional
            Arco *arco;
            arco->id = IdArco;
            arco->obligatoria = false;
            arco->origen = &g.nodos[origen];
            arco->destino = &g.nodos[destino];
            g.arcos[IdArco] = arco;

            // Conectar el arco a sus nodos
            g.nodos[origen].saliente.push_back(*arco);
            g.nodos[destino].entrante.push_back(*arco);

            IdArco++;
        }
    }

    // Saltarse header Coordenadas
    std::getline(infile, lineaDato);

    // Leer coordenadas
    // TODO: Formato coordenadas
    // Datos dummy ⚠️
    int idNodo = 1;
    for (int i = 0; i < std::stoi(encabezado["VERTICES"]); i++)
    {
        std::getline(infile, lineaDato);
        std::istringstream arcStream(lineaDato);
        int id;
        double x, y;
        if (!(arcStream >> x >> y))
        {
            // Error de formato
            std::cerr << "Error en el formato del archivo" << std::endl;
            return Graph();
        }
        // Actualizar las coordenadas del nodo
        g.nodos[idNodo].x = x;
        g.nodos[idNodo].y = y;

        idNodo++;
    }

    // Saltarse header Restricciones
    std::getline(infile, lineaDato);

    // Leer restricciones
    // TODO: Evaluar si son necesarias las restricciones
    if (encabezado["RESTRICCIONES"] != "0")
    {
        if (leer_restricciones)
        {
            for (int i = 0; i < std::stoi(encabezado["RESTRICCIONES"]); i++)
            {
                std::getline(infile, lineaDato);
                std::istringstream arcStream(lineaDato);
                int id, nodo1, nodo2, nodo3;
                if (!(arcStream >> nodo1 >> nodo2 >> nodo3))
                {
                    // Error de formato
                    std::cerr << "Error en el formato del archivo" << std::endl;
                    return Graph();
                }
            }
        }
        else
        {
            for (int i = 0; i < std::stoi(encabezado["RESTRICCIONES"]); i++)
            {
                std::getline(infile, lineaDato);
            }
        }
    }

    // Saltarse header Depositos o nodos iniciales
    std::getline(infile, lineaDato);

    // Leer depositos o nodos iniciales
    for (int i = 0; i < std::stoi(encabezado["NODOS_INICIALES"]); i++)
    {
        std::getline(infile, lineaDato);
        std::istringstream arcStream(lineaDato);
        int id, nodo;
        if (!(arcStream >> nodo))
        {
            // Error de formato
            std::cerr << "Error en el formato del archivo" << std::endl;
            return Graph();
        }
        g.metadatos.nodos_iniciales.push_back(g.nodos[nodo]);
    }

    // Saltarse header Terminal
    std::getline(infile, lineaDato);

    // Leer terminales
    if (encabezado["NODOS_TERMINO"] != "0")
    {
        for (int i = 0; i < std::stoi(encabezado["NODOS_TERMINO"]); i++)
        {
            std::getline(infile, lineaDato);
            std::istringstream arcStream(lineaDato);
            int id, nodo;
            if (!(arcStream >> nodo))
            {
                // Error de formato
                std::cerr << "Error en el formato del archivo" << std::endl;
                return Graph();
            }
            g.metadatos.nodos_termino.push_back(g.nodos[nodo]);
        }
    }
    else
    {
        for (int i = 0; i < std::stoi(encabezado["VERTICES"]); i++)
        {
            g.metadatos.nodos_termino.push_back(g.nodos[i + 1]);
        }
    }
    // Crear vector de arcos
    for (const auto &par : g.arcos)
    {
        g.vector_arcos.push_back(par.second);
    }

    // Cerrar el archivo
    infile.close();

    return g;
}
