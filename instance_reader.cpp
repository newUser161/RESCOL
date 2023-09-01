#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <set>

#include "instance_reader.h"
#include "graph.h"
#include "helpers.h"

using namespace std;

// Función para leer una instancia
/* Parámetros:
    - nombre_archivo: Nombre del archivo que contiene la instancia
    - leer_restricciones: Indica si se leerán las restricciones del archivo
*/
Graph leerInstancia(const std::string &nombre_archivo, bool leer_restricciones, bool leer_coordenadas)
{
    // Inicializar el grafo
    Graph g;
    std::unordered_map<std::string, std::string> encabezado;
    std::string lineaDato;
    int IdArco = 0;

    // Intentar abrir el archivo
    std::string ruta_archivo = "Instancias/" + nombre_archivo;
    std::ifstream infile(ruta_archivo);
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
        std::set<std::pair<int, int>> arcosCreados;
        double id, origen, destino, costo_recorrido, costo_recoleccion;
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
        if (bi_or_uni == "uni"){
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
            double costo = arco->costo_recorrido ;
            g.informacion_heuristica[origen][arco] = 1.0 / costo;

            IdArco++;
            arcosCreados.insert(std::make_pair(origen, destino));
        } else 
        {
            if (arcosCreados.find(std::make_pair(origen, destino)) == arcosCreados.end() &&
            arcosCreados.find(std::make_pair(destino, origen)) == arcosCreados.end()) {

                Arco *arcoIda = new Arco;
                Arco *arcoVuelta = new Arco;

                arcoIda->id = IdArco;
                g.arcos[IdArco] = arcoIda;
                IdArco++;

                arcoVuelta->id = IdArco;
                g.arcos[IdArco] = arcoVuelta;
                IdArco++;

                arcoIda->costo_recorrido = costo_recorrido;
                arcoIda->costo_recoleccion = costo_recoleccion;

                arcoVuelta->costo_recorrido = costo_recorrido;
                arcoVuelta->costo_recoleccion = costo_recoleccion;

                arcoIda->obligatoria = true;
                arcoVuelta->obligatoria = true;

                arcoIda->origen = &g.nodos[origen];
                arcoIda->destino = &g.nodos[destino];

                arcoVuelta->origen = &g.nodos[destino];
                arcoVuelta->destino = &g.nodos[origen];
                
                arcoIda->bidireccional = true;
                arcoVuelta->bidireccional = true;

                // Conectar el arco a sus nodos
                g.nodos[origen].saliente.push_back(*arcoIda);
                g.nodos[destino].entrante.push_back(*arcoIda);

                g.nodos[origen].entrante.push_back(*arcoVuelta);
                g.nodos[destino].saliente.push_back(*arcoVuelta);

                // Actualizar la información heurística
                double costo = arcoIda->costo_recorrido ;
                g.informacion_heuristica[origen][arcoIda] = 1.0 / costo;

                costo = arcoVuelta->costo_recorrido ;
                g.informacion_heuristica[destino][arcoVuelta] = 1.0 / costo;

                arcosCreados.insert(std::make_pair(origen, destino));
                arcosCreados.insert(std::make_pair(destino, origen));
            }
        }
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
            Arco *arco = new Arco;
            arco->id = IdArco;
            arco->obligatoria = false;
            arco->origen = &g.nodos[origen];
            arco->destino = &g.nodos[destino];
            g.arcos[IdArco] = arco;

            // Conectar el arco a sus nodos
            g.nodos[origen].saliente.push_back(*arco);
            g.nodos[destino].entrante.push_back(*arco);

            // Actualizar la información heurística            
            //g.informacion_heuristica[origen][destino] = 0;
            g.informacion_heuristica[origen][arco] = 0;


            IdArco++;
        }
    }

    // Saltarse header Coordenadas
    std::getline(infile, lineaDato);

    // Leer coordenadas
    // TODO: Formato coordenadas
    // Datos dummy ⚠️
    if (leer_coordenadas){
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
        }
    } else {
        for (int i = 0; i < std::stoi(encabezado["VERTICES"]); i++)
            std::getline(infile, lineaDato);
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
