#ifndef CONFIG_H
#define CONFIG_H

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