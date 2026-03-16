#include "antsystem.h"
#include "aco.h"
#include <chrono>

AntSystem::AntSystem(Graph *instancia, ACOArgs parametros) : ACO(instancia, parametros)
{
    inicializar_feromonas();
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
void AntSystem::iterar()
{
    ACO::iterar();
    // evaporar feromonas
    for (size_t i = 0; i < feromonas.size(); i++)
    {
        if (feromonas[i].cantidad < umbral_inferior)
        {
            feromonas[i].cantidad = umbral_inferior;
        }
        else
        {
            feromonas[i].cantidad *= (1 - rho);
        }        
    }
    if (usarMatrizSecundaria){
        for (size_t i = 0; i < feromonas_salida.size(); i++)
        {
            if (feromonas_salida[i].cantidad < umbral_inferior)
            {
                feromonas_salida[i].cantidad = umbral_inferior;
            }
            else
            {
                feromonas_salida[i].cantidad *= (1 - rho_salida);
            }        
        }
    }

    // Actualiza las feromonas.
    for (Hormiga &hormiga : hormigas)
    {
        if (hormiga.solucion_valida){
            for (size_t i = 0; i < hormiga.arcos_visitados_tour.size(); i++)
            {
                int pasadas = hormiga.arcos_visitados_tour[i];
                if (pasadas != 0)
                    feromonas[i].cantidad += (tau / (rho * pow(hormiga.longitud_camino_final, 2) * pasadas));
            }
            if (usarMatrizSecundaria)
            {
                for (size_t i = 0; i < hormiga.arcos_visitados_salida.size(); i++)
                {
                    int pasadas = hormiga.arcos_visitados_salida[i];
                    if (pasadas != 0)
                        feromonas_salida[i].cantidad += (tau / (rho * pow(hormiga.longitud_camino_salida, 2) * pasadas));
                }
            }
        }
    }
}

/*
    Guarda la mejor solución de la iteración
    Este método guarda la mejor solución de la iteración, para luego ser utilizada en la actualización de feromonas.
*/
void AntSystem::inicializar_feromonas()
{
    feromonas.assign(grafo->arcos.size(), {tau});
    feromonas_salida.assign(grafo->arcos.size(), {tau});
    for (auto &hormiga : hormigas)
        hormiga.feromonas_locales.assign(grafo->arcos.size(), {tau});
}
