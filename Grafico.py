import re
import matplotlib.pyplot as plt
import sys
import random

def rgb():
    return (random.randint(0, 255) / 255, random.randint(0, 255) / 255, random.randint(0, 255) / 255)

# Nombre del archivo desde la línea de comandos
file_name = sys.argv[1]

# Leer el archivo
with open(file_name, 'r') as file:
    lines = file.readlines()

# Inicializar las listas
epocas = []
iteraciones = []
costos = []

# Extraer los números
for line in lines:
    numbers = re.findall(r'\d+', line)
    epocas.append(int(numbers[0]))
    iteraciones.append(int(numbers[1]))
    costos.append(int(numbers[2]))

# Crear un diccionario de colores
color_dict = {}

# Asignar un color aleatorio a cada época
for epoca in set(epocas):
    color_dict[epoca] = rgb()

# Crear una lista de colores, uno para cada época
colors = [color_dict[epoca] for epoca in epocas]

# Crear un gráfico de líneas
for i in range(1, len(iteraciones)):
    if epocas[i] != epocas[i-1]:
        plt.plot(iteraciones[i-1:i+1], costos[i-1:i+1], color=colors[i-1])

# Título y etiquetas
plt.title('Grafico de Convergencia')
plt.xlabel('Iteraciones')
plt.ylabel('Mejor solucion encontrada')

# Mostrar el gráfico
plt.show()
