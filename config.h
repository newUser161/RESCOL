#ifndef CONFIG_H
#define CONFIG_H

#include "argparse/argparse.hpp"
#include "enums.h"

struct ACOArgs : public argparse::Args
{    
    std::string &nombre_instancia = arg("Nombre de la instancia").set_default("CasoRealChiquito.txt");
    int &metodo = kwarg("metodo", "Metodo de resolucion").set_default(1);
    float &alfa = kwarg("alfa", "Parámetro alfa").set_default(1.0);
    float &beta = kwarg("beta", "Parámetro beta").set_default(2.0);
    float &beta_salida = kwarg("beta-salida", "Parámetro beta de salida").set_default(2.0);
    float &rho = kwarg("rho", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.5);
    float &rho_secundario = kwarg("rho-sec", "Parámetro rho, asociado a la evaporación de feromonas").set_default(0.8);
    float &rho_salida = kwarg("rho-salida", "Parámetro rho de salida, asociado a la evaporación de feromonas").set_default(0.8);
    float &tau = kwarg("tau-as", "Parámetro tau, asociado a las feromonas iniciales").set_default(1.0);
    int &iteraciones_max = kwarg("iter-max", "Cantidad de iteraciones máximas").set_default(1000); 
    int &evaluaciones_maximas = kwarg("eval-max", "Cantidad de evaluaciones máximas").set_default(500);
    double &umbral_inferior = kwarg("umbral-inf", "Umbral inferior para las feromonas").set_default(0.001);
    int &num_hormigas = kwarg("num-hormigas", "Número de hormigas").set_default(10);
    int &epocas = kwarg("epocas", "Épocas").set_default(3);
    double &semilla = kwarg("semilla", "semilla de generacion aleatoria").set_default(1188444438);
    bool &usaMatrizSecundaria = flag("matriz-secundaria", "Flag que permite o no el uso de la matriz de salida");

    // MinMax
    double &umbral_superior = kwarg("umbral-sup", "Umbral superior para las feromonas").set_default(161161.161);
    int &umbral_sin_mejora_limite = kwarg("umbral-sin-mejora", "Cantidad de iteraciones sin mejora para la actualización de feromonas").set_default(10);
    int &a = kwarg("a-mm", "Parámetro a, asociado a la actualización de feromonas").set_default(13);

    // ACS
    double &q_0 = kwarg("q0", "Parámetro q0").set_default(0.389); // umbral de probabilidad ACS
    double &csi = kwarg("csi", "Parámetro csi").set_default(0.1);

    // Otros
    bool &conectar_bd = flag("bdconn", "Flag que permite o no subir los resultados a la BD");
    bool &beta_0 = flag("beta0", "Flag que fuerza el valor de beta a 0 en el recorrido");
    int &oscilador = kwarg("oscilador", "Parametro que permite o no el uso del oscilador de parametros, 0 es desactivado, 1 es stepper, 2 es caotico").set_default(0);
    bool &usar_iteraciones = flag("usar-iteraciones", "usa iteraciones en vez de evaluaciones, no se pueden usar ambas");
    bool &usar_evaluaciones = flag("usar-evaluaciones", "usa evaluaciones en vez de iteraciones, no se pueden usar ambas");
    bool &limitador = flag("usar-limitador", "usa el limitador de pasadas");
    int &valor_limitador = kwarg("valor-limitador", "cantidad limite de veces que se puede pasar por un arco").set_default(4);
    bool &irace = flag("irace", "usa el formato de llamada e instancias de irace");
    bool &silence = flag("silence", "output minimo");
    bool &full_aleatorio = flag("random-seed", "semilla aleatoria");
    
};


struct ConfigPrograma
{
    bool leer_restricciones = false;
    bool leer_coordenadas = false;
    bool debug = false;
    bool debug_ACO = false;
    bool show_solucion = false;    
    std::string show_grafico = "False";
    
};

#endif