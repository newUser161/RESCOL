
from sys import argv
import networkx as nx
import matplotlib.pyplot as plt
import matplotlib.cm as cm
import numpy as np

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

def visualizar_grafo(mapa_adyacencia, camino, nombre_archivo):
    G = nx.DiGraph()  # Crear un grafo dirigido
    
    # Añadir arcos al grafo
    for nodo, adyacentes in mapa_adyacencia.items():
        for adyacente in adyacentes:
            G.add_edge(nodo, adyacente[0])

    # Calcular la posición de los nodos para visualizar
    pos = nx.kamada_kawai_layout(G)
    nx.draw(G, pos, with_labels=True)

    # Dibujar todos los nodos y arcos
    nx.draw_networkx_nodes(G, pos, node_color='blue')
    nx.draw_networkx_edges(G, pos, edge_color='gray')

    # Calcular los colores para los arcos del camino
    cmap = cm.get_cmap('cool')  # Puedes cambiar a cualquier otro mapa de colores
    colores = [cmap(i) for i in np.linspace(0, 1, len(camino)-1)]
    edge_labels = {(camino[i], camino[i+1]): str(i+1) for i in range(len(camino)-1)}
    
    # Dibujar los arcos del camino con colores gradientes
    for i in range(len(camino)-1):
        nx.draw_networkx_edges(G, pos,
                            edgelist=[(camino[i], camino[i+1])],
                            edge_color=colores[i]) # type: ignore
        #nx.draw_networkx_edge_labels(G, pos, edge_labels=edge_labels)
    edge_labels = {}
    for i in range(len(camino)-1):
        edge = (camino[i], camino[i+1])
        if edge in edge_labels:
            edge_labels[edge].append(str(i+1))
        else:
            edge_labels[edge] = [str(i+1)]

# Calcular la posición de las etiquetas a lo largo del arco y dibujarlas
    for edge in edge_labels:
        x = (pos[edge[0]][0] + pos[edge[1]][0]) / 2
        y = (pos[edge[0]][1] + pos[edge[1]][1]) / 2
        for i, label in enumerate(edge_labels[edge]):
            offset = 0.03 * i  # Ajustar la offset como sea necesario
            label_pos = (x + offset, y + offset)
            plt.text(label_pos[0], label_pos[1], label, color='black')

    plt.savefig(nombre_archivo + '-GraficoMapa.png')
    plt.show()  # Mostrar la visualización

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

def construir_mapa_adyacencia(grafo, mapa_resultados):
    mapa_adyacencia = {}
    for id, nodo in grafo.nodos.items():
        mapa_adyacencia[nodo.id] = []
        for arco in nodo.saliente:            
            cantidad = mapa_resultados[(nodo.id, arco.destino.id)]
            if cantidad > 0:
                mapa_adyacencia[nodo.id].append([arco.destino.id,cantidad])
    return mapa_adyacencia

def leer_aristas_req(ruta):
    encabezado = {}
    ARISTAS_REQ     = []  
    with open("Instancias/"+ruta, 'r') as archivo:
        # Leer las primeras 8 líneas (clave: valor)
        for _ in range(8):
            linea = archivo.readline().strip()
            clave, valor = linea.split(':')
            encabezado[clave.strip()] = valor.strip()

        # Leer las líneas de aristas requeridas
        archivo.readline() # leer encabezado de seccion
        for _ in range(int(encabezado['ARISTAS_REQ'])):
            linea = archivo.readline().strip()
            tipo, nodo1, nodo2, costo_recolectar, costo_recoger = linea.split()            
            ARISTAS_REQ.append((tipo, int(nodo1), int(nodo2), float(costo_recolectar), float(costo_recoger)))
    return ARISTAS_REQ

def leer_ruta():
    ruta = []
    with open("Temp/camino.txt", 'r') as archivo:
        for linea in archivo.readlines():
            ruta.append(int(linea.strip()))
    return ruta

def leer_resultados():
    mapa = {}
    with open("Temp/mapa_resultados.txt", 'r') as archivo:
        for linea in archivo.readlines():
            nodos, valor = linea.strip().split(':')
            mapa[eval(nodos)] = int(valor)
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

ARISTAS_REQ = leer_aristas_req(argv[1])
#ARISTAS_REQ = leer_aristas_req("FormatoInstanciaRESCOL.txt")
camino = leer_ruta()
grafo = construir_grafo(ARISTAS_REQ)
mapa_resultados = leer_resultados()
mapa_adyacencia = construir_mapa_adyacencia(grafo, mapa_resultados)

nombre_archivo = argv[2][:-4]
visualizar_grafo(mapa_adyacencia, camino, nombre_archivo) # type: ignore

