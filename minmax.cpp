#include "minmax.h"
#include "aco.h"
#include <iostream>


MinMax::MinMax(Graph* instancia, MMArgs parametros) : ACO(instancia, parametros) {
    // Inicializa las feromonas.
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
    Hormiga mejor_solucion_local;
    while (iteraciones < iteraciones_max)
        {
            iterar();
            Hormiga mejor_solucion_anterior = mejor_solucion;
            mejor_solucion = guardar_mejor_solucion_iteracion();
            if (mejor_solucion_anterior.costo_camino == mejor_solucion.costo_camino && mejor_solucion_anterior.longitud_camino == mejor_solucion.longitud_camino) {                
                iteraciones_sin_mejora++;
                if (iteraciones_sin_mejora == umbral_sin_mejora_limite) {                    
                    inicializar_feromonas();
                    iteraciones_sin_mejora = 0;
                }
            } else {
                umbral_superior = tau/(rho * mejor_solucion.longitud_camino);
                umbral_inferior = umbral_superior / a;
                iteraciones_sin_mejora = 0;                
            }
            limpiar();
            iteraciones++;
        }
};

void MinMax::iterar(){
    ACO::iterar();
    
    // evaporar feromonas
    for (auto i = feromonas.begin(); i != feromonas.end(); i++)
    {
        i->second.cantidad *= (1 - rho);
    }
    
    // Actualiza las feromonas.
    for (auto &par : mejor_solucion.arcosVisitados)
    {
        Arco *a = par.first;
        int pasadas = par.second;        
        feromonas.at(a).cantidad += (tau / (rho * mejor_solucion.longitud_camino * pasadas));
        if (feromonas.at(a).cantidad > umbral_superior)
            feromonas.at(a).cantidad = umbral_superior;
    }
}

/*
    Guarda la mejor solución de la iteración
    Este método guarda la mejor solución de la iteración, para luego ser utilizada en la actualización de feromonas.
*/
void MinMax::inicializar_feromonas(){
    umbral_inferior = umbral_superior / a;
    for (auto &par : grafo->arcos)
        {
            Arco *arco = par.second;
            Feromona feromona_inicial = {arco->origen, arco->destino, umbral_superior};
            feromonas[arco] = feromona_inicial;
            for (auto &hormiga : hormigas)
                hormiga.feromonas_locales[arco] = feromona_inicial;
        }
}
