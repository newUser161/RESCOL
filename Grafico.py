import matplotlib.pyplot as plt
import pandas as pd
import seaborn as sns
from sys import argv

# Lee el archivo, asumiendo que el formato es exactamente 'Epoca: %d, Iteracion: %d, Mejor costo: %d'
df = pd.read_csv(argv[1], sep=", ", engine='python', header=None, names=['Epoca', 'Iteracion', 'Mejor costo'])

# Convierte las columnas al tipo de datos correcto y extrae los números de las cadenas
df['Epoca'] = df['Epoca'].str.extract('(\d+)').astype(int)
df['Iteracion'] = df['Iteracion'].str.extract('(\d+)').astype(int)
df['Mejor costo'] = df['Mejor costo'].str.extract('(\d+)').astype(int)

# Agrupa por 'Epoca' e 'Iteracion', tomando el mínimo 'Mejor costo' para cada grupo
df_grouped = df.groupby(['Epoca', 'Iteracion']).min().reset_index()

# Crea un color distinto para cada época
palette = sns.color_palette('hsv', df_grouped['Epoca'].nunique())

# Crea un gráfico de lineas para cada época
for epoca in df_grouped['Epoca'].unique():
    data = df_grouped[df_grouped['Epoca'] == epoca]
    
    plt.plot(data['Iteracion'], data['Mejor costo'], label=f'Epoca {epoca}', color=palette[epoca])
    

plt.xlabel('Iteraciones')
plt.ylabel('Mejor costo')
plt.title('Gráfico de convergencia')
plt.legend()
plt.savefig(argv[1] + '-Grafico.png')
plt.show()
