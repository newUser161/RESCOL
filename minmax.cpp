#include "minmax.h"
#include "aco.h"
#include <iostream>
#include <chrono>
#include <ctime>

MinMax::MinMax(Graph *instancia, ACOArgs parametros) : ACO(instancia, parametros)
{
    set_parametros(parametros);
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
void MinMax::resolver()
{
    int iteraciones_sin_mejora = 0;
    while (iteraciones < iteraciones_max)
    {        
        iterar();
        Hormiga mejor_solucion_anterior = mejor_solucion;
        mejor_solucion = guardar_mejor_solucion_iteracion();

        if (mejor_solucion_anterior.costo_camino == mejor_solucion.costo_camino && mejor_solucion_anterior.longitud_camino_final == mejor_solucion.longitud_camino_final)
        {
            iteraciones_sin_mejora++;
            if (iteraciones_sin_mejora == umbral_sin_mejora_limite)
            {
                inicializar_feromonas();
                iteraciones_sin_mejora = 0;
            }
        }
        else
        {
            umbral_superior = tau / (rho * mejor_solucion.longitud_camino_final);
            umbral_inferior = umbral_superior / a;
            iteraciones_sin_mejora = 0;
        }
        limpiar();
        iteraciones++;
    }
};

void MinMax::iterar()
{
    ACO::iterar();

    // evaporar feromonas
    for (size_t i = 0; i < feromonas.size(); i++)
    {
        feromonas[i].cantidad *= (1 - rho);
        if (feromonas[i].cantidad < umbral_inferior)
            feromonas[i].cantidad = umbral_inferior;
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
    for (size_t i = 0; i < mejor_solucion.arcos_visitados_tour.size(); i++)
    {
        int pasadas = mejor_solucion.arcos_visitados_tour[i];
        if (pasadas != 0)
            feromonas[i].cantidad += (tau / (rho * mejor_solucion.longitud_camino_final * pasadas));
        if (feromonas[i].cantidad > umbral_superior)
            feromonas[i].cantidad = umbral_superior;
    }
    if (usarMatrizSecundaria)
    {   
        for (size_t i = 0; i < mejor_solucion.arcos_visitados_salida.size(); i++)
        {
            int pasadas = mejor_solucion.arcos_visitados_salida[i];
            if (pasadas != 0)
                feromonas_salida[i].cantidad += (tau / (rho * mejor_solucion.longitud_camino_salida * pasadas));
            if (feromonas_salida[i].cantidad > umbral_superior)
                feromonas_salida[i].cantidad = umbral_superior;
        }
    }
}

/*
    Guarda la mejor solución de la iteración
    Este método guarda la mejor solución de la iteración, para luego ser utilizada en la actualización de feromonas.
*/
void MinMax::inicializar_feromonas()
{
    umbral_inferior = umbral_superior_inicial / a;
    umbral_superior = umbral_superior_inicial;
    feromonas.assign(grafo->arcos.size(), {umbral_superior_inicial});
    feromonas_salida.assign(grafo->arcos.size(), {umbral_superior_inicial});
    for (auto &hormiga : hormigas)
        hormiga.feromonas_locales.assign(grafo->arcos.size(), {umbral_superior_inicial});
}

void MinMax::set_parametros(ACOArgs parametros)
{
    umbral_superior = parametros.umbral_superior;
    umbral_superior_inicial = parametros.umbral_superior;
    umbral_sin_mejora_limite = parametros.umbral_sin_mejora_limite;
    a = parametros.a;
}