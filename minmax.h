#ifndef MM_H
#define MM_H

#include "aco.h"

class MinMax : public ACO
{
protected:

    MMArgs parametros;    
    double umbral_superior; // Umbral superior para las feromonas
    int umbral_sin_mejora_limite;  // Cantidad de iteraciones sin mejora para la actualizacion de feromonas
    int a;                                // Parámetro a, asociado a la actualizacion de feromonas


public:
    MinMax(Graph *instancia, MMArgs parametros);
    void resolver() override;
    void iterar() override;
    void inicializar_feromonas() override;
    void set_parametros(MMArgs parametros);
};

#endif