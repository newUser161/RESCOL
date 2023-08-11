#ifndef MM_H
#define MM_H

#include "aco.h"

class MinMax : public ACO
{
protected:

/*
    MMArgs parametros;    
    float &rho = parametros.rho;                          // Parámetro rho, asociado a la evaporacion de feromonas
    double &umbral_superior = parametros.umbral_superior; // Umbral superior para las feromonas
    float &tau = parametros.tau;                          // Parámetro tau, asociado a las feromonas iniciales
    int &umbral_sin_mejora_limite = parametros.umbral_sin_mejora_limite;  // Cantidad de iteraciones sin mejora para la actualizacion de feromonas
    int &a = parametros.a;                                // Parámetro a, asociado a la actualizacion de feromonas
*/
    ParametrosMM parametros2;
    float &rho = parametros2.rho;                          // Parámetro rho, asociado a la evaporacion de feromonas
    double &umbral_superior = parametros2.umbral_superior; // Umbral superior para las feromonas
    float &tau = parametros2.tau;                          // Parámetro tau, asociado a las feromonas iniciales
    int &umbral_sin_mejora_limite = parametros2.umbral_sin_mejora_limite;  // Cantidad de iteraciones sin mejora para la actualizacion de feromonas
    int &a = parametros2.a; 



public:
    MinMax(Graph *instancia, MMArgs parametros);
    MinMax(Graph *instancia, ParametrosMM parametros2);
    void resolver() override;
    void iterar() override;
    void inicializar_feromonas() override;
};

#endif