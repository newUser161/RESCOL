from sys import argv
import mysql.connector

def abrir_conexion():    
    conn = mysql.connector.connect(
        host="185.209.230.86",
        user="ACORESCOLADMIN",
        password="passwordmuysegura123",
        database="ACO"
    )    
    return conn

def cerrar_conexion(conn):    
    conn.close()


def insertar_datos(archivo, conn, id_config):  
    cursor = conn.cursor()
    with open(archivo, 'r') as file:
        for line in file:
            datos = line.strip().split(',')
            datos_experimento = [id_config] + datos  
            query = '''INSERT INTO experimentos_ACO (configuracion_id, nombre_instancia, metodo, costo_ruta, 
                        longitud_ruta, nodo_inicio, nodo_termino, tiempo_res_us,longitud_tour,longitud_salida,cantidad_arcos,costo_recoleccion,costo_recorrer) 
                       VALUES (%s,%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)'''
            cursor.execute(query, datos_experimento)
    conn.commit()
    cursor.close()
    

def insertar_config(archivo, conn):    
    cursor = conn.cursor(buffered = True)
    with open(archivo, 'r') as file:
        for line in file:
            datos = line.strip().split(',')

            query_comprobar = '''SELECT * 
                                 FROM configuraciones_ACO 
                                 WHERE usa_matriz_salida = %s AND usa_oscilador = %s AND limite_pasada = %s AND valor_limite_pasada = %s AND
                                 beta_cero = %s AND alfa = %s AND beta = %s AND beta_salida = %s AND rho = %s AND rho_secundario = %s AND 
                                 rho_salida = %s AND tau = %s AND iteraciones_max = %s AND evaluaciones_max = %s AND
                                 umbral_inferior = %s AND cant_hormigas = %s AND epocas = %s AND semilla = %s AND umbral_superior = %s AND
                                 umbral_mejora_sin_limite = %s AND a = %s AND q0 = %s AND csi = %s AND usa_iteraciones = %s AND
                                 usa_evaluaciones = %s AND irace = %s AND silence = %s AND full_aleatorio = %s'''
            cursor.execute(query_comprobar, datos)
            resultado = cursor.fetchone()            
            if resultado:
                id_config = resultado[0]
            else :                
                query_insertar = '''INSERT INTO configuraciones_ACO (
                    usa_matriz_salida, usa_oscilador, limite_pasada, valor_limite_pasada, 
                    beta_cero, alfa, beta, beta_salida, rho, rho_secundario, 
                    rho_salida, tau, iteraciones_max, evaluaciones_max, 
                    umbral_inferior, cant_hormigas, epocas, semilla, umbral_superior, 
                    umbral_mejora_sin_limite, a, q0, csi, usa_iteraciones, 
                    usa_evaluaciones, irace, silence, full_aleatorio
                ) 
                VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, 
                        %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)'''
                cursor.execute(query_insertar, datos)
                conn.commit()
                id_config = cursor.lastrowid
    cursor.close()    
    return id_config

conn = abrir_conexion() 
id_config = insertar_config(argv[2], conn)
#id_config = insertar_config("Output\TEST-RESCOL-A1-AntSystem-20231125174246\config_csv.txt", conn)
insertar_datos(argv[1], conn,id_config)


cerrar_conexion(conn) 
