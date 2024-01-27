Formato instancia RESCOL

El objetivo de este documento es mostrar la composición de una instancia válida para el algoritmo Ant Colony Optimization. Al final del documento se adjunta una de las instancias que fue utilizada a modo de prueba.

Todas las instancias, sin importar su tamaño, se componen de 7 secciones: Información de instancia: la primera sección incluye información general de la instancia como el nombre de ésta, un comentario, las cantidades de nodos/vértices, aristas, restricciones, nodos iniciales y terminales: NOMBRE : Renca Chiquito COMENTARIO : Caso real 1 VERTICES : 16 ARISTAS_REQ : 16 ARISTAS_NOREQ : 0 RESTRICCIONES : 0 NODOS_INICIALES : 1 NODOS_TERMINO : 1 En este caso la instancia “Renca Chiquito” tiene 16 nodos y 16 arcos, no tiene aristas opcionales ni restricciones. Solo tiene un nodo de inicio y uno de término. Actualmente en ninguna instancia se están usando las aristas opcionales ni restricciones, por lo que se recomienda usar 0 en ambas líneas.

Lista de aristas requeridas u obligatorias: En esta sección, luego del encabezado, cada línea representa un arco de la instancia. Las líneas están en el siguiente formato: bidireccionalidad:uni|bi nodo1:int nodo2:int pesoPasada:real pesoRecoleccion:real uni 32709 32619 43.81 0

Lista de aristas no requerida u opcionales: No se usa en este momento ya que todas las aristas del grafo deben ser recorridas y las aristas opcionales se crean de manera interna dentro de los subnodos de un nodo para modelar el sentido de las calles, por lo que no se necesitan aristas opcionales explícitas entre nodos. De usarse este mecanismo, se seguiría el mismo formato de las aristas obligatorias menos los pesos, quedando: bidireccionalidad:uni|bi nodo1:int nodo2:int

Lista de coordenadas: No se ha implementado un sistema de coordenadas para los nodos, por lo que esta sección no se considera, pero si se mantiene en el caso de que se implemente en una próxima versión. Se recomienda dejar solo el encabezado de esta sección.

Restricciones: Las restricciones pueden ser leídas en la instancia, pero aún no se ha implementado su uso. El formato es nodo1:int nodo2:int nodo3:int 34 79 45 El formato se interpreta de la siguiente manera; se prohíbe transitar desde el nodo 34 hasta el 45 pasando por el 79. Este formato podría cambiar de ser necesario un control mas fino de las restricciones.

Nodos iniciales: Es el conjunto de nodos que pueden ser escogidos como punto de inicio del recorrido. Cada línea es un nodo: nodo1:int 944

Nodos terminales: Similar a los nodos de inicio. Representan el punto de termino de un recorrido: nodo1:int 588

Ejemplo de instancia completo:

NOMBRE : Renca Chiquito

COMENTARIO : Caso real 1

VERTICES : 16

ARISTAS_REQ : 16

ARISTAS_NOREQ : 0

RESTRICCIONES : 0 

NODOS_INICIALES : 1 

NODOS_TERMINO : 1 

LISTA_ARISTAS_REQ : 

uni 32709 32619 43.81 0 

uni 32619 32518 49.62 0 

bi 32519 32518 59.97 0 

bi 32460 32490 39.91 0 

bi 32490 32501 37.90 0 

bi 32501 32519 38.21 0 

bi 32657 32490 82.85 0 

bi 32658 32657 14.72 0 

bi 32657 51553 14.04 0 

bi 32676 32519 83.02 0 

bi 32677 32676 15.44 0

bi 32676 51556 13.63 0 

bi 32700 32460 111.69 0 

bi 32700 32719 62.41 0 

bi 32718 32501 110.52 0

bi 32719 32718 15.54 0 

LISTA_ARISTAS_NOREQ :

CORDENADAS_NODOS :

RESTRICCIONES : 

NODOS_INICIALES: 32709 

NODOS_TERMINO: 32677 

Otro ejemplo:

NOMBRE : TEST-RESCOL-A1 

COMENTARIO : Ejemplo de formato

VERTICES : 25 

ARISTAS_REQ : 40

ARISTAS_NOREQ : 0

RESTRICCIONES : 3 

NODOS_INICIALES : 3 

NODOS_TERMINO : 2 

LISTA_ARISTAS_REQ : 

bi 1 2 4 1 

bi 2 3 2 1 

bi 4 3 10 1 

bi 5 4 8 1 

uni 1 6 9 1 

uni 7 2 3 1 

uni 3 8 2 1 

uni 9 4 1 1 

uni 5 10 8 1

bi 7 6 2 1

bi 8 7 2 1 

bi 9 8 7 1 

bi 10 9 8 1

uni 6 11 9 1

uni 12 7 2 1

uni 8 13 3 1

uni 14 9 9 1

uni 10 15 8 1 

bi 12 11 8 1

bi 13 12 4 1

bi 13 14 2 1

bi 15 14 8 1

uni 11 16 4 1

uni 17 12 3 1

uni 13 18 8 1

uni 19 14 9 1

uni 15 20 3 1

bi 17 16 8 1 

bi 18 17 4 1 

bi 19 18 3 1

bi 20 19 2 1

uni 16 21 8 1 

uni 22 17 4 1 

uni 18 23 8 1 

uni 24 19 9 1 

uni 20 25 1 1 

bi 22 21 8 1 

bi 23 22 3 1 

bi 24 23 11 1

bi 25 24 8 1 

LISTA_ARISTAS_NOREQ : 

CORDENADAS_NODOS :

RESTRICCIONES : 

1 3 5

5 3 1 

4 5 3

1 3 2 

NODOS_INICIALES: 
6 

3

5

NODOS_TERMINO:

1 

25
