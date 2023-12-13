import shutil
import numpy as np

def copiar_instancia(archivo_original, archivo_copia):
    shutil.copy(archivo_original, archivo_copia)

def encontrar_min_max(aristas):
    pesos = [float(arista[3]) for arista in aristas]
    return min(pesos), max(pesos)

def modificar_pesos_en_copia(archivo_copia, min_peso, max_peso):
    with open(archivo_copia, 'r') as file:
        lineas = file.readlines()

    inicio_aristas_req = lineas.index("LISTA_ARISTAS_REQ : \n") + 1
    fin_aristas_req = lineas.index("LISTA_ARISTAS_NOREQ :\n")
    aristas = lineas[inicio_aristas_req:fin_aristas_req]

    # Generar nuevos pesos
    nuevos_pesos = np.random.normal((min_peso + max_peso) / 2, (max_peso - min_peso) / 6, len(aristas))

    # Modificar las aristas con los nuevos pesos
    for i, linea in enumerate(aristas):
        partes = linea.split()
        nuevo_peso = max(min_peso, min(max_peso, nuevos_pesos[i]))
        partes[3] = f"{nuevo_peso:.2f}"
        lineas[inicio_aristas_req + i] = " ".join(partes) + '\n'

    # Reescribir el archivo con las aristas modificadas
    with open(archivo_copia, 'w') as file:
        file.writelines(lineas)

def leer_instancia(archivo):
    with open(archivo, 'r') as file:
        lineas = file.readlines()
    
    inicio_aristas_req = lineas.index("LISTA_ARISTAS_REQ : \n") + 1
    fin_aristas_req = lineas.index("LISTA_ARISTAS_NOREQ :\n")

    aristas = [linea.strip().split() for linea in lineas[inicio_aristas_req:fin_aristas_req] if linea.strip()]
    return lineas[:inicio_aristas_req], aristas

# El resto del script permanece igual

corr = 1
for _ in range(1,10):
# Ruta del archivo de instancia base y nombre de la copia
    archivo_instancia_base = 'Instancias/CasoRealGrande.txt'
    archivo_instancia_copia = 'CasoRealGrande_variante'+str(corr)+'.txt'
    corr += 1

    # Copiar la instancia original
    copiar_instancia(archivo_instancia_base, archivo_instancia_copia)

    # Encontrar los pesos mínimo y máximo en la instancia original
    encabezado, aristas_base = leer_instancia(archivo_instancia_base)  # Reutilizando la función anterior
    min_peso, max_peso = encontrar_min_max(aristas_base)

    # Modificar los pesos en la copia
    modificar_pesos_en_copia(archivo_instancia_copia, min_peso, max_peso)
