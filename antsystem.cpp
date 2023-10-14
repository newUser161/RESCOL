#include "antsystem.h"
#include "aco.h"
#include <chrono> 

AntSystem::AntSystem(Graph* instancia, ASArgs parametros) : ACO(instancia, parametros) {
    // Inicializa las feromonas.
    
    auto start = std::chrono::high_resolution_clock::now();

    inicializar_feromonas(); 

    // Detiene la medición del tiempo
    auto stop = std::chrono::high_resolution_clock::now();

    // Calcula la duración
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << "Tiempo de inicializacion: " << duration.count() << " microsegundos" << std::endl;
}

/* Resuelve el problema
    Este método resuelve el problema simplemente iterando el algoritmo hasta que se cumpla el criterio de parada.
    Algunas alternativas de mejora son:
    - Establecer un criterio de parada basado en la calidad de las soluciones, mientras menos mejor.
    - Establecer un criterio de parada basado en el tiempo de ejecución.
    - Establecer un criterio de parada basado en la cantidad de iteraciones sin mejora, de manera local, global y/o por hormiga.
    + Mas opciones en aco-book.
*/
void AntSystem::resolver() 
{
    while (iteraciones < iteraciones_max)
        {
            iterar();
            mejor_solucion = guardar_mejor_solucion_iteracion();
            limpiar();
            iteraciones++;
        }
}
void AntSystem::iterar(){
    ACO::iterar();
    // evaporar feromonas
    for (auto i = feromonas.begin(); i != feromonas.end(); i++)
    {
        i->second.cantidad *= (1 - rho);
    }

    // Actualiza las feromonas.
    for (Hormiga &hormiga : hormigas)
    {
        for (auto &par : hormiga.arcos_visitados_tour)
        {
            Arco *a = par.first;
            int pasadas = par.second;
            feromonas.at(a).cantidad += (tau / (rho * pow(hormiga.longitud_camino_tour,2) * pasadas));
        }        
        if (usarMatrizSecundaria){ 
            for (auto &par : hormiga.arcos_visitados_salida)
            {
                Arco *a = par.first;
                int pasadas = par.second;     
                if (pasadas != 0)           
                    feromonas_salida.at(a).cantidad += (tau / (rho * pow(hormiga.longitud_camino_tour,2) * pasadas));
            }
        }
        
    }
}

/*
    Guarda la mejor solución de la iteración
    Este método guarda la mejor solución de la iteración, para luego ser utilizada en la actualización de feromonas.
*/
void AntSystem::inicializar_feromonas(){
    for (auto &par : grafo->arcos)
        {
            Arco *arco = par.second;
            Feromona feromona_inicial = {arco->origen, arco->destino, tau};
            feromonas[arco] = feromona_inicial;
            feromonas_salida[arco] = feromona_inicial;
            for (auto &hormiga : hormigas)
                hormiga.feromonas_locales[arco] = feromona_inicial;
        }
}
