#ifndef CONFIG_H
#define CONFIG_H

#include "argparse/argparse.hpp"
#include "enums.h"

struct ACOArgs : public argparse::Args
{
    std::string &nombre_instancia = arg("Nombre de la instancia").set_default("CasoRealChiquito.txt");
    int &metodo = kwarg("metodo", "Metodo de resolucion").set_default(0);
    float &alfa = kwarg("alfa", "Parámetro alfa").set_default(1.0f);
    float &beta = kwarg("beta", "Parámetro beta").set_default(2.0f);
    float &rho = kwarg("rho", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.5f);
    float &rho_secundario = kwarg("rho-sec", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.8f);
    float &rho_salida = kwarg("rho-salida", "Parámetro rho de salida, asociado a la evaporación de feromonas").set_default(0.1f);
    float &tau = kwarg("tau-as", "Parámetro tau, asociado a las feromonas iniciales").set_default(1.0f);
    int &iteraciones = kwarg("iter", "Iteraciones actuales").set_default(0);
    int &iteraciones_max = kwarg("iter-max", "Cantidad de iteraciones máximas").set_default(1000); // usar numero de evaluaciones o de iteraciones, pero no ambos
    int &evaluaciones = kwarg("evaluaciones", "Cantidad de evaluaciones máximas").set_default(30000);// nuevo parametro, implementar
    double &umbral_inferior = kwarg("umbral-inf", "Umbral inferior para las feromonas").set_default(1.7e-100);
    int &num_hormigas = kwarg("num-hormigas", "Número de hormigas").set_default(10);
    int &epocas = kwarg("epocas", "Épocas").set_default(3);
    int &epoca_actual = kwarg("epoca-actual", "Época actual").set_default(0);
    double &semilla = kwarg("semilla", "semilla de generacion aleatoria").set_default(1111);
    bool &debug = flag("debug", "Flag que muestra o no información de debug como los caminos de las hormigas").set_default(false);
    bool &usaMatrizSecundaria = flag("matriz-secundaria", "Flag que permite o no el uso de la matriz de salida").set_default(true);
    
};

struct ASArgs : public ACOArgs
{
};

struct MMArgs : public ACOArgs
{
    double &umbral_superior = kwarg("umbral-sup", "Umbral superior para las feromonas").set_default(161161.161);
    int &umbral_sin_mejora_limite = kwarg("umbral-sin-mejora", "Cantidad de iteraciones sin mejora para la actualización de feromonas").set_default(3);
    int &a = kwarg("a-mm", "Parámetro a, asociado a la actualización de feromonas").set_default(13);
};

struct ACSArgs : public ACOArgs
{
    double &q_0 = kwarg("q0", "Parámetro q0").set_default(0.5); // umbral de probabilidad ACS
    double &csi = kwarg("csi", "Parámetro csi").set_default(0.1);
    
    
};

struct ConfigPrograma
{
    bool leer_restricciones = false;
    bool leer_coordenadas = false;
    bool debug = false;
    bool debug_ACO = false;
    bool show_solucion = false;
    
};

#endif