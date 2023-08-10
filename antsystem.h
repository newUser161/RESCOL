#ifndef AS_H
#define AS_H

#include "aco.h"


class AntSystem : public ACO
{
protected:
    ASArgs parametros;
    float &rho = parametros.rho;                          // Parámetro rho, asociado a la evaporacion de feromonas
    float &tau = parametros.tau;                          // Parámetro tau, asociado a las feromonas iniciales


public:
    AntSystem(Graph *instancia, ASArgs parametros);
    void resolver() override;
    void iterar() override;
    void inicializar_feromonas() override;


};

#endif