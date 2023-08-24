#ifndef AS_H
#define AS_H

#include "aco.h"


class AntSystem : public ACO
{
protected:

public:
    AntSystem(Graph *instancia, ASArgs parametros);
    //AntSystem(Graph *instancia, ParametrosAS parametros2);
    void resolver() override;
    void iterar() override;
    void inicializar_feromonas() override;


};

#endif