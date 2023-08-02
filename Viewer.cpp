#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <conio.h>

#include "graph.h"
#include "aco.h"
#include "instance_reader.h"
#include "helpers.h"

using namespace std;

/*
    Implementacion de la metaheuristica ACO para el problema de recoleccion de residuos.
    Autor: Alonso M. Robledo Álamos.
    Fecha: 2023-07-23
    Correo electrónico: alonsorobledo@protonmail.com | alonso.robledo.a@mail.pucv.cl

    Este programa implementa una metaheuristica tipo Ant Colony Optimization (ACO) para resolver un problema de ruteo de arcos (ARP)
    en el contexto de la recoleccion de residuos domiciliarios.
    Esto en el contexto de mi proyecto de tesis para optar al grado de magister en ingenieria informatica.
*/

// Funcion principal
/* Parámetros:
    - nombre_archivo: Nombre del archivo que contiene la instancia
    - iteraciones: Número de iteraciones que realizará el algoritmo
    - hormigas: Número de hormigas que se utilizarán
    - leer_restricciones: Indica si se leerán las restricciones del archivo
    - carga_auto: Indica si se cargará una instancia automática o manual
    - debug: Indica si se mostrarán mensajes de debug
*/
int main(int argc, char *argv[])
{

    // Parámetros y configuración por defecto
    string nombre_archivo = "Formato5x5.txt";
    int iteraciones = 5000;
    int hormigas = 10;
   
    
    bool carga_auto = true;
    bool leer_restricciones = false;
    bool leer_coordenadas = false;
    bool debug = true;
    bool debug_ACO = false;

    Graph grafo;

    if (carga_auto)
    {
        grafo = leerInstancia(nombre_archivo, leer_restricciones, leer_coordenadas);
    }
    else
    {
        if (argc != 11)
        {
            cout << "Uso: " << argv[0] << " <nombre_Instancia> <#iteraciones> <#hormigas> <alfa> <beta> <rho> <tau> <carga_auto> <leer_restricciones> <debug>" << endl;
            return 1;
        }

        // Carga los parametros pasados por consola
        nombre_archivo = argv[1];
        iteraciones = atoi(argv[2]);
        hormigas = atoi(argv[3]);
       
        //q0 = atof(argv[8]);
        
        carga_auto = atoi(argv[8]);
        leer_restricciones = atoi(argv[9]);
        leer_coordenadas = atoi(argv[10]);

        // Intenta abrir el archivo
        ifstream archivo(nombre_archivo);
        if (!archivo.is_open())
        {
            cout << "No se pudo abrir el archivo ❌" << nombre_archivo << endl;
            return 1;
        }
        grafo = leerInstancia(nombre_archivo, leer_restricciones, leer_coordenadas);
    }

    (grafo.vector_arcos.empty()) ? cout << "Error al leer la instancia ❌" << endl : cout << "Instancia leída correctamente ✔️" << endl;
    if (debug)
    {
        cout << endl;
        for (int i = 0; i < 161; i++) cout << "-";
        cout << endl;
        cout << "Datos de debug 🪲: " << endl;
        cout << endl;
        cout << "Nombre de la instancia: " << grafo.metadatos.encabezado["NOMBRE"] << endl;
        cout << "Comentario: " << grafo.metadatos.encabezado["COMENTARIO"] << endl;
        cout << "Número de nodos: " << grafo.metadatos.encabezado["VERTICES"] << endl;
        cout << "Número de arcos obligatorios: " << grafo.metadatos.encabezado["ARISTAS_REQ"] << endl;
        (grafo.metadatos.encabezado["ARISTAS_NOREQ"] != "0") ? cout << "Número de arcos opcionales: " << grafo.metadatos.encabezado["ARISTAS_NOREQ"] << endl : cout << "No hay arcos opcionales" << endl;
        cout << "Número de nodos iniciales: " << grafo.metadatos.encabezado["NODOS_INICIALES"] << endl;
        (grafo.metadatos.encabezado["NODOS_TERMINO"] != "0") ? cout << "Número de nodos terminales: " << grafo.metadatos.encabezado["NODOS_TERMINO"] << endl : cout << "Todos los nodos pueden ser terminales" << endl;
        cout << "Número de nodos terminales: " << grafo.metadatos.encabezado["NODOS_TERMINO"] << endl;
        (grafo.metadatos.encabezado["RESTRICCIONES"] != "0") ? cout << "Número de restricciones: " << grafo.metadatos.encabezado["RESTRICCIONES"] << endl : cout << "No hay restricciones" << endl;
        cout << endl;
        cout << "Datos: " << endl;
        cout << "Nodos iniciales: " << endl;
        for (auto &nodo : grafo.metadatos.nodos_iniciales)
        {
            cout << nodo.id << " " << endl;
        }
        cout << endl;
        cout << "Nodos terminales: " << endl;
        for (auto &nodo : grafo.metadatos.nodos_termino)
        {
            cout << nodo.id << " ";
        }
        cout << endl;
        cout << "Arcos:" << endl;
        cout << "nodo_origen nodo_destino costo_recorrido costo_recoleccion obligatorio(1|0) bidireccional(1|0)" << endl;
        for (auto &arco : grafo.vector_arcos)
        {
            cout << arco->origen->id << " " << arco->destino->id << " " << arco->costo_recorrido << " " << arco->costo_recoleccion << " " << arco->obligatoria << " " << arco->bidireccional << endl;
        }
        cout << endl;
        cout << "Nodos:" << endl;
        cout << "id posX posY grado" << endl;
        for (auto &par : grafo.nodos)
        {
            cout << par.second.id << " " << par.second.x << " " << par.second.y << " " << par.second.grado << endl;
        }
        for (int i = 0; i < 161; i++)
            cout << "-";
        cout << endl;
    }
    cout << endl;

    // Crea una instancia del algoritmo y la resuelve, el tercer parametro indica si se muestran mensajes de debug
    ACO aco = ACO(&grafo, hormigas, debug_ACO);    
    aco.resolver(iteraciones);

    // Muestra la solución
    bool show_solucion = true;
    aco.mostrar_solucion(show_solucion);

    cout << "Programa finalizado correctamente" << endl;
    for (int i = 0; i < hormigas; i++)
        cout << "🐜 ";
    cout << endl;
    cout << endl;
    cout << "Presione cualquier tecla para continuar..." << endl;    
    getch();
    return 0;
}