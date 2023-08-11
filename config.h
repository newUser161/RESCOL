#ifndef CONFIG_H
#define CONFIG_H

#include "argparse/argparse.hpp"
#include "enums.h"




struct ACOArgs : public argparse::Args {
    std::string &nombre_instancia = arg("Nombre de la instancia").set_default("Formato5x5.txt");
    //MetodoACO &metodo = kwarg("m, metodo", "Metodo a utilizar").set_default("MIN_MAX");
    float &alfa            = kwarg("alfa", "Parámetro alfa").set_default(1.0f);
    float &beta            = kwarg("beta", "Parámetro beta").set_default(2.0f);
    float &rho_secundario  = kwarg("rho-sec", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.5f);
    int &iteraciones       = kwarg("iter", "Iteraciones actuales").set_default(0);
    int &iteraciones_max   = kwarg("iter-max", "Cantidad de iteraciones máximas").set_default(1000);
    bool &debug            = flag("debug", "Flag que muestra o no información de debug como los caminos de las hormigas");
    double &umbral_inferior= kwarg("umbral-inf", "Umbral inferior para las feromonas").set_default(1.7e-100);
    int &num_hormigas      = kwarg("num-hormigas", "Número de hormigas").set_default(10);
    float &rho             = kwarg("rho", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.5f);
    int &epocas            = kwarg("epocas", "Épocas").set_default(3);
    int &epoca_actual      = kwarg("epoca-actual", "Época actual").set_default(0);
};

struct ASArgs : public ACOArgs {
    float &rho             = kwarg("rho-as", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.5f);
    float &tau             = kwarg("tau-as", "Parámetro tau, asociado a las feromonas iniciales").set_default(1.0f);
};

struct MMArgs : public ACOArgs {
    float &rho                    = kwarg("rho-mm", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.1f);
    float &tau                    = kwarg("tau-mm", "Parámetro tau, asociado a las feromonas iniciales").set_default(1.0f);
    double &umbral_superior       = kwarg("umbral-sup", "Umbral superior para las feromonas").set_default(161161.161);
    int &umbral_sin_mejora_limite = kwarg("umbral-sin-mejora", "Cantidad de iteraciones sin mejora para la actualización de feromonas").set_default(50);
    int &a                        = kwarg("a-mm", "Parámetro a, asociado a la actualización de feromonas").set_default(13);
};


struct ParametrosACOBase
{
    float alfa = 1.0;                  // Parámetro alfa
    float beta = 2.0;                  // Parámetro beta
    float rho_secundario = 0.5;        // Parámetro rho, asociado a la evaporacion de feromonas
    int iteraciones = 0;               // Iteraciones actuales
    int iteraciones_max = 1000;         // Cantidad de iteraciones maximas
    bool debug = false;                // Flag que muestra o no informacion de debug como los caminos de las hormigas
    double umbral_inferior = 1.7e-300; // Umbral inferior para las feromonas
    int num_hormigas = 10;             // Numero de hormigas
    float rho = 0.5;                   // Parámetro rho, asociado a la evaporacion de feromonas
    int epocas = 3;
    int epoca_actual = 0;
};


struct ParametrosAS : ParametrosACOBase
{
    float rho = 0.5;                   // Parámetro rho, asociado a la evaporacion de feromonas
    float tau = 1.0;                   // Parámetro tau, asociado a las feromonas iniciales
};

struct ParametrosMM : ParametrosACOBase
{
    float rho = 0.1;                   // Parámetro rho, asociado a la evaporacion de feromonas
    float tau = 1.0;                   // Parámetro tau, asociado a las feromonas iniciales
    double umbral_superior = 161161.161;   // Umbral superior para las feromonas
    int umbral_sin_mejora_limite = 50;         // Cantidad de iteraciones sin mejora para la actualizacion de feromonas
    int a = 13;                         // Parámetro a, asociado a la actualizacion de feromonas
};

#endif