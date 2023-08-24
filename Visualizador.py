from MetodoExactoGurobiPy.helpers.ConstructorRutas.ConstructorRutas import construir_grafo, construir_mapa_adyacencia, dfs, parsear_resultados_gurobi, reparar_solucion
from MetodoExactoGurobiPy.helpers.Visualizador.Visualizador import visualizar_grafo
from sys import argv

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
            ARISTAS_REQ.append((tipo, int(nodo1), int(nodo2), int(costo_recolectar), int(costo_recoger)))
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

#ARISTAS_REQ = leer_aristas_req(argv[1])
ARISTAS_REQ = leer_aristas_req("FormatoInstanciaRESCOL.txt")
camino = leer_ruta()
grafo = construir_grafo(ARISTAS_REQ)
mapa_resultados = leer_resultados()
mapa_adyacencia = construir_mapa_adyacencia(grafo, mapa_resultados)

visualizar_grafo(mapa_adyacencia, camino) # type: ignore

