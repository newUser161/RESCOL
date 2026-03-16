#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include "graph.h"


void bfs(int nodoInicial, std::vector<std::vector<double>> &adyacencia) {
    std::queue<int> cola;
    std::unordered_map<int, bool> visitado;

    // Marcar todos los nodos como no visitados al inicio
    for (size_t i = 0; i < adyacencia.size(); i++) {
        visitado[i] = false;
    }

    // Marcar el nodo inicial como visitado y encolarlo
    visitado[nodoInicial] = true;
    cola.push(nodoInicial);

    while (!cola.empty()) {
        int nodoActual = cola.front();
        cola.pop();

        // Procesar los vecinos del nodo actual
        for (size_t i = 0; i < adyacencia[nodoActual].size(); i++) {
            if (adyacencia[nodoActual][i] > 0.0) {
                int idVecino = i; // Assuming the index corresponds to node ID in some way, this test function seems broken anyway since adyacencia[arco_id] doesn't map directly to destination id without Arco info

                if (!visitado[idVecino]) {
                    visitado[idVecino] = true;
                    cola.push(idVecino);
                }
            }
        }
    }
    /*
    int contador = 0;
    for (auto &par : adyacencia) {
        if (visitado[par.first] == false){
            std::cout << "El nodo " << par.first << " no fue visitado" << std::endl;
            contador++;
        }
    }
    cout << "cantidad de nodos no visitados: " << contador << endl;
    contador = 0;
    for (auto &par : adyacencia) {
        if (visitado[par.first] != false){
            std::cout << "El nodo " << par.first << " fue visitado" << std::endl;
            contador++;
        }
    }
    cout << "cantidad de nodos visitados: " << contador << endl;
    */
}


    
