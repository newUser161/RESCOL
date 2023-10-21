#include <random>

std::mt19937 generador;
void inicializar_generador(int semilla)
{
    generador.seed(semilla);
}
