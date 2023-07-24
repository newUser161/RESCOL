#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#python3 cssolverv2.py nombre_instancia

import sys
from gurobipy import *
import numpy as np

def maximo(a, b):
        if (a>b):
            return a
        else:
            return b

#Leer parametros

if len(sys.argv) < 2:
    print('Uso:$ python3 cssolverv4.py nombre_instancia')
    exit(1)
    
file_temp = 'temp.txt'
file = open(file_temp,'w') 
with open(sys.argv[1]) as f:
    for line in f:
        if line.startswith('#') or line.startswith('\n') : #create a temp file without coments and blank lines
            continue
        else:
            file.write(line) 
file.close()

m = Model('CSSolver')

with open(file_temp) as f:
    nCTP = 0
    nA, nP, nSP, nT, nD = [int(x) for x in f.readline().split()] # read first line
    
    print ("nA:" + str(nA) + ", nP:" + str(nP) + ", nSP:" + str(nSP) + ", nT:" + str(nT) + ", nD:" + str(nD))
    preferencia = []    
    for j in range(nA):
        preferencia.append( [ int (x) for x in f.readline().split(' ') ] )
    print (preferencia)
    
    nB=0
    tsxblockxdia = []
    for l in range(nD):
        tsxblockxdia.append( [ int (x) for x in f.readline().split(' ') ] )
        nB = maximo(nB, len(tsxblockxdia[l]))
    print ("nB: "+ str(nB))
    print (tsxblockxdia)
    
    
    ## Variables
    asignacion = m.addVars(nP,nT,nSP,nD, vtype=GRB.BINARY, name='asignacion')
    insatisfaccion = m.addVars(nA,nT,nD, vtype=GRB.INTEGER, name='insatisfaccion')
    z = m.addVar(vtype=GRB.INTEGER, name='z')

    #Aca hay que revisar si no usa todos los timeslots del dia y poner en cero aquellos que no se usan.
    for l in range(nD):
        print ("Dia: "+ str(l))
        total = 0
        for t in range(len(tsxblockxdia[l])):
            total += tsxblockxdia[l][t]
            
        if total < nT:
            print ("Total: "+ str(total))
            for t in range(total, nT):
                for k in range(nSP):
                    for i in range(nP):
                        print("i, t, k, l : " + str(i) + " " + str(t) + " " + str(k) + " " + str(l))
                        m.addConstr(asignacion[i,t,k,l]==0)
        
    nRA = int(f.readline().split()[0]) #Restricciones de disponibilidad de presentadores.
    print ("nRA: "+ str(nRA))
    
    for r in range(nRA):
        values = f.readline().split(' ')
        print ("Values: " + str(values))
        print ("tipo: "+ str(values[0]))
        if values[0] == '1': #poner el id de tipo de conflicto primero.
            p=int(values[1]) #presentacion
            d=int(values[2]) #dia
            t=int(values[3]) #timeslot
            print("Restriccion 1 para presentacion: " + str(p) + ", dia: " + str(d) + ", timeslot: " + str(t))
            #Agregar las restricciones correspondientes
            for s in range(nSP):
                print("Restriccion de disponibilidad de presentadores tipo 1")
                print("Forzando presentacion: " + str(p) + ", timeslot: " + str(t) + ", sesion paralela: " + str(s) + ", dia: " + str(d))
                #m.addConstr(asignacion[p-1,t-1,s,d-1] == 0)
                m.addConstr(asignacion[p,t,s,d] == 0)
                
        if values[0] == '2': #poner el id de tipo de conflicto primero.
            p=int(values[1]) #presentacion
            d=int(values[2]) #dia
            ft=int(values[3]) #from timeslot
            tt=int(values[4]) #to timeslot
            
            #Agregar las restricciones correspondientes
            print("Restriccion 2 para presentacion: " + str(p) + ", dia: " + str(d) + ", timeslot inicio: " + str(ft) + ", timeslot fin: " + str(tt))
            
            for s in range(nSP):
                for t in range(ft,tt+1):
                    print("Restriccion de disponibilidad de presentadores tipo 2")
                    print("Forzando presentacion: " + str(p) + ", timeslot: " + str(t) + ", sesion paralela: " + str(s) + ", dia: " + str(d))
                    #m.addConstr(asignacion[p-1,t-1,s,d-1] == 0)
                    m.addConstr(asignacion[p,t,s,d] == 0)
            
            
        if values[0] == '3': #poner el id de tipo de conflicto primero.
            p=int(values[1]) #presentacion
            d=int(values[2]) #dia
            #Agregar las restricciones correspondientes
            for s in range(nSP):
                for t in range(nT):
                    print("Restriccion de disponibilidad de presentadores tipo 3")
                    print("Forzando presentacion: " + str(p) + ", timeslot: " + str(t) + ", sesion paralela: " + str(s) +", dia: " + str(d))
                    #m.addConstr(asignacion[p-1,t,s,d-1] == 0)
                    m.addConstr(asignacion[p,t,s,d] == 0)
            

    #Cambiar para bloquear entre timeslots
    nRP = int(f.readline().split()[0]) #Restricciones de conflictos entre presentaciones
    print ("nRP: "+ str(nRP))
    r = 0
    while r < nRP:
        values = f.readline().split(' ') 
        print ("Values: " + str(values))
        p1 = int(values[0])
        print ("p1: " + str(p1))
        for p2Index in range(len(values)):
            p2 = int(values[p2Index])
            if p1 != p2:
                r=r+1;
                print("Restriccion de simultaneidad de presentadores") 
                print("Forzando presentacion: " + str(p1) + " en distinto timeslot que presentacion: " + str(p2))
                for d in range(nD):
                    for j in range(nT):
                        #m.addConstr(sum(asignacion[p1-1,j,k,l] + asignacion[p2-1,j,k,l] for k in range(nSP)) <= 1)
                        m.addConstr(sum(asignacion[p1,j,k,l] + asignacion[p2,j,k,l] for k in range(nSP)) <= 1)


#Cada presentación debe realizarse en un timeslot en una sesión paralela en un dia.
for i in range(nP):
    m.addConstr(sum(sum(sum(asignacion[i,j,k,l] for j in range(nT)) for k in range(nSP)) for l in range (nD)) == 1)
    
#A lo mas una presentación por timeslot por sesión paralela por dia.
for j in range(nT):
    for k in range(nSP):
        for l in range(nD):
            m.addConstr(sum(asignacion[i,j,k,l] for i in range(nP)) <= 1)
    
#Contabilizar la cantidad maxima de sesiones preferidas con tope de timeslots por asistente
for a in range(nA):
    for l in range(nD):
        for j in range(nT):
            m.addConstr(sum(sum(asignacion[i,j,k,l] * preferencia[a][i] for i in range(nP)) for k in range(nSP)) - 1 <= insatisfaccion[a,j,l])

# Minimizar la cantidad maxima de insatisfacciones.
#for a in range(nA):
    #m.addConstr(sum(sum(insatisfaccion[a,j,l] for j in range(nT)) for l in range(nD)) <= z)

#Minimizar la insatisfaccion maxima (suma de todas las insatisfacciones de cada persona)    
m.addConstr(sum(sum(sum(insatisfaccion[a,j,l] for j in range(nT)) for l in range(nD)) for a in range(nA)) <= z)
    
m.setObjective(z, GRB.MINIMIZE)
m.optimize()
m.write('csSolver.lp')

print("\t \t \t ") 
for l in range (nD):
    print("\t \t Day " + str(l), end = '')
print ('')
print("Block \t TS", end = '')
for l in range (nD):
    for k in range (nSP):
        print("\t PS " + str(k), end = '')
print ('')
for j in range (nT):
    print ("\t" + str(j), end = '')
    for l in range (nD):
        for k in range (nSP):
            for i in range(nP):
                if asignacion[i,j,k,l].X == 1: 
                    print ("\t" + str(i), end = '')
        
    print ('')
print ('')


print("z: " + str(z.X))
    
#for a in range(nA):
    #print ("Persona: " + str(a)) 
    #total=0
    #for l in range(nD):
        #for j in range(nT):
            #if insatisfaccion[a,j,l].X > 0: 
                #print ("D:" + str(l) + ",T:" + str(j) + ": " + str(insatisfaccion[a,j,l].X))
                #total += insatisfaccion[a,j,l].X
    #print ("TOTAL:" + str(total))
    
