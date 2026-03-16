#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <set>
#include <memory>

#include "instance_reader.h"
#include "graph.h"
#include "helpers.h"

using namespace std;

Graph leerInstancia(const std::string &nombre_archivo, bool leer_restricciones, bool leer_coordenadas, bool irace)
{
    Graph g;
    std::unordered_map<std::string, std::string> encabezado;
    std::string lineaDato;
    std::string ruta_archivo;
    std::set<std::pair<int, int>> arcosCreados;
    std::set<std::pair<int, int>> arcosCreadosOpcionales;
    int IdArco = 0;
    int id_secundario = 1;

    if (irace){
        ruta_archivo = nombre_archivo;
    } else {
        ruta_archivo = "Instancias/" + nombre_archivo;
    }    
    std::ifstream infile(ruta_archivo);
    if (!infile)
    {
        std::cerr << "No se pudo abrir el archivo: " << nombre_archivo << std::endl;
        return Graph();
    }

    for (int i = 0; i < 8; i++)
    {
        std::string linea;
        std::getline(infile, linea);
        size_t pos = linea.find(':');
        std::string clave = eliminarEspacios(linea.substr(0, pos));
        std::string valor = eliminarEspacios(linea.substr(pos + 1));
        encabezado[clave] = valor;
    }    
    g.metadatos.encabezado = encabezado;

    std::getline(infile, lineaDato);

    for (int i = 0; i < std::stoi(encabezado["ARISTAS_REQ"]); i++)
    {
        id_secundario = 1;
        std::getline(infile, lineaDato);
        std::istringstream arcStream(lineaDato);
        std::string bi_or_uni;
        int id, origen, destino;
        double costo_recorrido, costo_recoleccion;
        if (!(arcStream >> bi_or_uni >> origen >> destino >> costo_recorrido >> costo_recoleccion))
        {
            std::cerr << "Error en el formato del archivo" << std::endl;
            return Graph();
        }

        Nodo* p_origen = g.get_or_create_nodo(origen);
        Nodo* p_destino = g.get_or_create_nodo(destino);

        if (bi_or_uni == "uni" )
        {
            auto arco_ptr = std::make_unique<Arco>();
            Arco *arco = arco_ptr.get();
            arco->id = IdArco;
            arco->costo_recorrido = costo_recorrido;
            arco->costo_recoleccion = costo_recoleccion;
            arco->origen = p_origen;
            arco->destino = p_destino;
            arco->bidireccional = false;
            g.arcos.push_back(std::move(arco_ptr));

            p_origen->saliente.push_back(arco);
            p_destino->entrante.push_back(arco);

            auto subnodo_saliente_origen = std::make_unique<Subnodo>();
            subnodo_saliente_origen->tipo = 2; // 2: saliente
            p_origen->subnodos.push_back(std::move(subnodo_saliente_origen));

            auto subnodo_entrante_destino = std::make_unique<Subnodo>();
            subnodo_entrante_destino->tipo = 1; // 1: entrante
            p_destino->subnodos.push_back(std::move(subnodo_entrante_destino));

            IdArco++;
        }
        else // arco bidireccional
        {
            if (arcosCreados.find(std::make_pair(origen, destino)) == arcosCreados.end() &&
                arcosCreados.find(std::make_pair(destino, origen)) == arcosCreados.end())
            {
                auto arcoIda_ptr = std::make_unique<Arco>();
                Arco *arcoIda = arcoIda_ptr.get();
                arcoIda->id = IdArco++;
                
                auto arcoVuelta_ptr = std::make_unique<Arco>();
                Arco *arcoVuelta = arcoVuelta_ptr.get();
                arcoVuelta->id = IdArco++;
                
                g.arcos.push_back(std::move(arcoIda_ptr));
                g.arcos.push_back(std::move(arcoVuelta_ptr));

                arcoIda->costo_recorrido = costo_recorrido;
                arcoIda->costo_recoleccion = costo_recoleccion;

                arcoVuelta->costo_recorrido = costo_recorrido;
                arcoVuelta->costo_recoleccion = costo_recoleccion;

                arcoIda->origen = p_origen;
                arcoIda->destino = p_destino;

                arcoVuelta->origen = p_destino;
                arcoVuelta->destino = p_origen;

                arcoIda->bidireccional = true;
                arcoVuelta->bidireccional = true;

                arcoIda->arco_reciproco = arcoVuelta;
                arcoVuelta->arco_reciproco = arcoIda;

                p_origen->saliente.push_back(arcoIda);
                p_destino->entrante.push_back(arcoIda);

                p_origen->entrante.push_back(arcoVuelta);
                p_destino->saliente.push_back(arcoVuelta);

                auto subnodo_entrante_origen = std::make_unique<Subnodo>();
                auto subnodo_saliente_origen = std::make_unique<Subnodo>();

                subnodo_entrante_origen->id_secundario = id_secundario;
                subnodo_saliente_origen->id_secundario = id_secundario;
                id_secundario++;

                subnodo_entrante_origen->tipo = 1; // 1: entrante
                subnodo_saliente_origen->tipo = 2; // 2: saliente

                subnodo_entrante_origen->nodo_reciproco = subnodo_saliente_origen.get();
                subnodo_saliente_origen->nodo_reciproco = subnodo_entrante_origen.get();

                p_origen->subnodos.push_back(std::move(subnodo_entrante_origen));
                p_origen->subnodos.push_back(std::move(subnodo_saliente_origen));

                auto subnodo_entrante_destino = std::make_unique<Subnodo>();
                auto subnodo_saliente_destino = std::make_unique<Subnodo>();

                subnodo_entrante_destino->id_secundario = id_secundario;
                subnodo_saliente_destino->id_secundario = id_secundario;
                id_secundario++;

                subnodo_entrante_destino->tipo = 1; // 1: entrante
                subnodo_saliente_destino->tipo = 2; // 2: saliente

                subnodo_entrante_destino->nodo_reciproco = subnodo_saliente_destino.get();
                subnodo_saliente_destino->nodo_reciproco = subnodo_entrante_destino.get();

                p_destino->subnodos.push_back(std::move(subnodo_entrante_destino));
                p_destino->subnodos.push_back(std::move(subnodo_saliente_destino));

                arcosCreados.insert(std::make_pair(origen, destino));
                arcosCreados.insert(std::make_pair(destino, origen));
            }
        }
    }

    std::getline(infile, lineaDato);

    if (encabezado["ARISTAS_NOREQ"] != "0")
    {
        for (int i = 0; i < std::stoi(encabezado["ARISTAS_NOREQ"]); i++)
        {
            std::getline(infile, lineaDato);
            std::istringstream arcStream(lineaDato);
            std::string bi_or_uni;
            int origen, destino;
            if (!(arcStream >> bi_or_uni >>origen >> destino))
            {
                std::cerr << "Error en el formato del archivo" << std::endl;
                return Graph();
            }

            Nodo* p_origen = g.get_or_create_nodo(origen);
            Nodo* p_destino = g.get_or_create_nodo(destino);

            if (bi_or_uni == "uni" )
            {
                auto arco_ptr = std::make_unique<Arco>();
                Arco *arco = arco_ptr.get();
                arco->id = IdArco++;
                arco->obligatoria = false;
                arco->bidireccional = false;
                arco->origen = p_origen;
                arco->destino = p_destino;
                
                g.arcos.push_back(std::move(arco_ptr));
                p_origen->saliente.push_back(arco);
                p_destino->entrante.push_back(arco);
            }
            else
            {
                if (arcosCreadosOpcionales.find(std::make_pair(origen, destino)) == arcosCreadosOpcionales.end() &&
                    arcosCreadosOpcionales.find(std::make_pair(destino, origen)) == arcosCreadosOpcionales.end())
                {
                    auto arcoIda_ptr = std::make_unique<Arco>();
                    Arco *arcoIda = arcoIda_ptr.get();
                    arcoIda->id = IdArco++;

                    auto arcoVuelta_ptr = std::make_unique<Arco>();
                    Arco *arcoVuelta = arcoVuelta_ptr.get();
                    arcoVuelta->id = IdArco++;

                    arcoIda->origen = p_origen;
                    arcoIda->destino = p_destino;

                    arcoVuelta->origen = p_destino;
                    arcoVuelta->destino = p_origen;

                    arcoIda->obligatoria = false;
                    arcoVuelta->obligatoria = false;

                    arcoIda->bidireccional = true;
                    arcoVuelta->bidireccional = true;

                    arcoIda->arco_reciproco = arcoVuelta;
                    arcoVuelta->arco_reciproco = arcoIda;

                    p_origen->saliente.push_back(arcoIda);
                    p_destino->entrante.push_back(arcoIda);

                    p_origen->entrante.push_back(arcoVuelta);
                    p_destino->saliente.push_back(arcoVuelta);

                    g.arcos.push_back(std::move(arcoIda_ptr));
                    g.arcos.push_back(std::move(arcoVuelta_ptr));

                    auto subnodo_entrante_origen = std::make_unique<Subnodo>();
                    auto subnodo_saliente_origen = std::make_unique<Subnodo>();

                    subnodo_entrante_origen->id_secundario = id_secundario;
                    subnodo_saliente_origen->id_secundario = id_secundario;
                    id_secundario++;

                    subnodo_entrante_origen->tipo = 1; // 1: entrante
                    subnodo_saliente_origen->tipo = 2; // 2: saliente

                    subnodo_entrante_origen->nodo_reciproco = subnodo_saliente_origen.get();
                    subnodo_saliente_origen->nodo_reciproco = subnodo_entrante_origen.get();

                    p_origen->subnodos.push_back(std::move(subnodo_entrante_origen));
                    p_origen->subnodos.push_back(std::move(subnodo_saliente_origen));

                    auto subnodo_entrante_destino = std::make_unique<Subnodo>();
                    auto subnodo_saliente_destino = std::make_unique<Subnodo>();

                    subnodo_entrante_destino->id_secundario = id_secundario;
                    subnodo_saliente_destino->id_secundario = id_secundario;
                    id_secundario++;

                    subnodo_entrante_destino->tipo = 1; // 1: entrante
                    subnodo_saliente_destino->tipo = 2; // 2: saliente

                    subnodo_entrante_destino->nodo_reciproco = subnodo_saliente_destino.get();
                    subnodo_saliente_destino->nodo_reciproco = subnodo_entrante_destino.get();

                    p_destino->subnodos.push_back(std::move(subnodo_entrante_destino));
                    p_destino->subnodos.push_back(std::move(subnodo_saliente_destino));

                    arcosCreadosOpcionales.insert(std::make_pair(origen, destino));
                    arcosCreadosOpcionales.insert(std::make_pair(destino, origen));
                }
            }
        }
    }

    std::getline(infile, lineaDato);

    if (leer_coordenadas)
    {
        for (int i = 0; i < std::stoi(encabezado["VERTICES"]); i++)
        {
            std::getline(infile, lineaDato);
            std::istringstream arcStream(lineaDato);
            int id;
            double x, y;
            if (!(arcStream >> x >> y))
            {
                std::cerr << "Error en el formato del archivo" << std::endl;
                return Graph();
            }
        }
    }

    std::getline(infile, lineaDato);

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

    std::getline(infile, lineaDato);

    for (int i = 0; i < std::stoi(encabezado["NODOS_INICIALES"]); i++)
    {
        std::getline(infile, lineaDato);
        std::istringstream arcStream(lineaDato);
        int nodo;
        if (!(arcStream >> nodo))
        {
            std::cerr << "Error en el formato del archivo" << std::endl;
            return Graph();
        }
        g.metadatos.nodos_iniciales.push_back(g.get_or_create_nodo(nodo));
    }

    std::getline(infile, lineaDato);

    if (encabezado["NODOS_TERMINO"] != "0")
    {
        for (int i = 0; i < std::stoi(encabezado["NODOS_TERMINO"]); i++)
        {
            std::getline(infile, lineaDato);
            std::istringstream arcStream(lineaDato);
            int nodo;
            if (!(arcStream >> nodo))
            {
                std::cerr << "Error en el formato del archivo" << std::endl;
                return Graph();
            }
            g.metadatos.nodos_termino.push_back(g.get_or_create_nodo(nodo));
        }
    }
    else
    {
        for (int i = 0; i < std::stoi(encabezado["VERTICES"]); i++)
        {
            g.metadatos.nodos_termino.push_back(g.get_or_create_nodo(i + 1));
        }
    }

    infile.close();

    for (auto &par : g.nodos)
    {
        for (auto &par2 : par->subnodos)
        {
            if (par2->tipo == 1) // 1:entrante
            {
                for (auto &par3 : par->subnodos)
                {
                    if ((par3->tipo == 2) && (par3->id_secundario != par2->id_secundario)) // 2:saliente
                    {
                        auto arco_ptr = std::make_unique<Arco>();
                        Arco *arco = arco_ptr.get();
                        arco->id = IdArco++;

                        par2->saliente.push_back(arco);
                        par3->entrante.push_back(arco);

                        g.arcos.push_back(std::move(arco_ptr));
                    }
                }
            }
        }
    }

    g.init_heuristica(g.nodos.size(), g.arcos.size());
    for (const auto& arco : g.arcos) {
        if (arco->costo_recorrido > 0) {
            double heur = 1.0 / arco->costo_recorrido;
            if (arco->origen) {
                g.informacion_heuristica[arco->origen->index][arco->id] = heur;
            }
        }
    }

    return g;
}
