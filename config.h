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
    bool &crear_directorios_solucion = flag("crear-directorios-solucion", "permite crear directorios para las soluciones");


    // Parametros del oscilador
    float &inc_alfa = kwarg("inc-alfa", "Incremento de alfa").set_default(0.1);
    float &min_alfa = kwarg("min-alfa", "Valor minimo de alfa").set_default(1);
    float &max_alfa = kwarg("max-alfa", "Valor maximo de alfa").set_default(5);
    float &inc_beta = kwarg("inc-beta", "Incremento de beta").set_default(0.1);
    float &min_beta = kwarg("min-beta", "Valor minimo de beta").set_default(1);
    float &max_beta = kwarg("max-beta", "Valor maximo de beta").set_default(0.5);
    float &inc_rho = kwarg("inc-rho", "Incremento de rho").set_default(0.1);
    float &min_rho = kwarg("min-rho", "Valor minimo de rho").set_default(0.1);
    float &max_rho = kwarg("max-rho", "Valor maximo de rho").set_default(0.5);
    float &inc_rho_secundario = kwarg("inc-rho-sec", "Incremento de rho secundario").set_default(0.1);
    float &min_rho_secundario = kwarg("min-rho-sec", "Valor minimo de rho secundario").set_default(0.1);
    float &max_rho_secundario = kwarg("max-rho-sec", "Valor maximo de rho secundario").set_default(0.5);
    //float &inc_rho_salida = kwarg("inc-rho-salida", "Incremento de rho salida").set_default(0.1); No usar oscilador en mecanismos de salida
    //float &min_rho_salida = kwarg("min-rho-salida", "Valor minimo de rho salida").set_default(0.1);
    //float &max_rho_salida = kwarg("max-rho-salida", "Valor maximo de rho salida").set_default(0.5);


    
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