import random
class Nodo:
    def __init__(self, id):
        self.id = id
        self.saliente = []
        self.entrante = []

class Arco:
    def __init__(self, id, costo_recorrido, costo_recoleccion, obligatoria, origen, destino, bidireccional):
        self.id = id
        self.costo_recorrido = costo_recorrido
        self.costo_recoleccion = costo_recoleccion
        self.obligatoria = obligatoria
        self.origen = origen
        self.destino = destino
        self.bidireccional = bidireccional

class Grafo:
    def __init__(self):
        self.nodos = {}
        self.arcos = {}

def construir_grafo(arcos):
    grafo = Grafo()
    IdArco = 0
    for arco in arcos:
        bi_or_uni, origen, destino, costo_recorrido, costo_recoleccion = arco

        if origen not in grafo.nodos:
            grafo.nodos[origen] = Nodo(origen)

        if destino not in grafo.nodos:
            grafo.nodos[destino] = Nodo(destino)

        arco = Arco(IdArco, costo_recorrido, costo_recoleccion, True, grafo.nodos[origen], grafo.nodos[destino], bi_or_uni == "bi")

        grafo.arcos[IdArco] = arco

        grafo.nodos[origen].saliente.append(arco)
        grafo.nodos[destino].entrante.append(arco)

        IdArco += 1

    return grafo



def parsear_resultados_gurobi(resultados_gurobi):
    mapa = {}
    for resultado in resultados_gurobi:
        # Extraemos los nodos del resultado
        nodos = resultado[0][2:-1].split(',')
        nodo_inicial = int(nodos[0])
        nodo_final = int(nodos[1])

        # Extraemos la cantidad de veces que se recorre el arco
        y = int(resultado[1])

        # Usamos la tupla (nodo_inicial, nodo_final) como clave en el mapa
        mapa[(nodo_inicial, nodo_final)] = y
    return mapa





def construir_mapa_adyacencia(grafo, mapa_resultados):
    mapa_adyacencia = {}
    for id, nodo in grafo.nodos.items():
        mapa_adyacencia[nodo.id] = []
        for arco in nodo.saliente:            
            cantidad = mapa_resultados[(nodo.id, arco.destino.id)]
            if cantidad > 0:
                mapa_adyacencia[nodo.id].append([arco.destino.id,cantidad])
    return mapa_adyacencia
    

def dfs(limite_paso_salida, mapa_adyacencia, nodo_inicial, nodo_final, camino=None): 
    if camino is None:
        camino = [nodo_inicial]

    if (nodo_inicial == nodo_final and limite_paso_salida == 0):
        return camino
    contador = 0
    max_iteraciones = 1000
    while mapa_adyacencia[nodo_inicial] and contador < max_iteraciones:
        contador += 1        
        adyacente, recorrido = random.choice(mapa_adyacencia[nodo_inicial])
        if adyacente == nodo_final: 
            limite_paso_salida -= 1
        camino.append(adyacente)
        recorrido -= 1
        if recorrido == 0:
            mapa_adyacencia[nodo_inicial] = mapa_adyacencia[nodo_inicial][1:]
        ruta_resultante = dfs(limite_paso_salida, mapa_adyacencia, adyacente ,nodo_final, camino)
        if ruta_resultante:
            return ruta_resultante
    camino.pop()
    return None
    





 