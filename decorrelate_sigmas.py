#!/usr/bin/python3.6
# -*-coding:Utf-8 -*

import os
import sys
import pandas as pd
import numpy as np
from math import *

pd.set_option('display.max_rows', None)

def triple_coincidence(sigmas,energies):
    sig1 = sigmas.iloc[0]
    sig2 = sigmas.iloc[1]
    sig3 = sigmas.iloc[2]
    comb = (sig2-sig3+sig1)/2.

    en1 = sqrt(1./energies.iloc[0])
    en2 = sqrt(1./energies.iloc[1])
    en3 = sqrt(1./energies.iloc[2])

    true_sigmas = [sqrt(comb/en1**2),sqrt((sig1-comb)/en2**2),sqrt((sig3-sig1+comb)/en3**2)]

    return true_sigmas

# def triple_coincidence(sigmas,energies):
#     A = (sigmas.iloc[0])**2
#     B = (sigmas.iloc[1])**2
#     C = (sigmas.iloc[2])**2

#     a = energies.iloc[0]
#     b = energies.iloc[1]
#     c = energies.iloc[2]

#     x = sqrt((a/2)*(A+B-C))
#     y = sqrt((b/2)*(A-B+C))
#     z = sqrt((c/2)*(-A+B+C))

#     true_sigmas = [x,y,z]

#     return true_sigmas

try:
    SigmaFile = np.loadtxt(fname="sigma_files/%s/sigma_%s_%s.txt" % tuple(sys.argv[1:4]))
    EnergyFile = np.loadtxt(fname="energy_files/%s/energy_%s_%s.txt" % tuple(sys.argv[1:4]))

    if SigmaFile.size: #garantit que le fichier ait une taille non nulle
        if SigmaFile.ndim>1: #garantit qu'il y ait plus d'une ligne dans le fichier (sinon ça fait planter EnergyFile[:,3])

            df = pd.DataFrame({
                'col': SigmaFile[:,0],
                'row': SigmaFile[:,1],
                'counts': SigmaFile[:,2],
                'sigma': SigmaFile[:,3],
                'energy': EnergyFile[:,3],
            })

            df['col'] = df['col'].astype(int)
            df['row'] = df['row'].astype(int)
            df['counts'] = df['counts'].astype(int)

            sorted_data = df.sort_values(by=['counts'], ascending=False)

            for i in range(0,df['col'].size,3):
                if (i+2)<df['col'].size: #garantit que le groupe ait bien trois entrées

                    tuple_first_counts = (sys.argv[1],sorted_data['col'].iloc[i],sorted_data['row'].iloc[i])

                    SigmaFile2 = np.loadtxt(fname="sigma_files/%s/sigma_%i_%i.txt" % tuple_first_counts)
                    EnergyFile2 = np.loadtxt(fname="energy_files/%s/energy_%i_%i.txt" % tuple_first_counts)

                    df2 = pd.DataFrame({
                        'col': SigmaFile2[:,0],
                        'row': SigmaFile2[:,1],
                        'counts': SigmaFile2[:,2],
                        'sigma': SigmaFile2[:,3],
                        'energy': EnergyFile2[:,3],
                    })

                    mask_second_counts = df2[(df2.col == sorted_data['col'].iloc[i+1]) & (df2.row == sorted_data['row'].iloc[i+1])]

                    if mask_second_counts.empty==False:

                        first = pd.DataFrame({
                            'col': [sorted_data['col'].iloc[i],sorted_data['col'].iloc[i+1],mask_second_counts['col'].iloc[0]],
                            'row': [sorted_data['row'].iloc[i],sorted_data['row'].iloc[i+1],mask_second_counts['row'].iloc[0]],
                            'sigma': [sorted_data['sigma'].iloc[i],sorted_data['sigma'].iloc[i+1],mask_second_counts['sigma'].iloc[0]],
                            'energy': [sorted_data['energy'].iloc[i],sorted_data['energy'].iloc[i+1],mask_second_counts['energy'].iloc[0]]
                        })

                        true_sigmas = triple_coincidence(first['sigma'],first['energy'])

                        x0 = np.array([
                            (sorted_data['col'].iloc[i],sorted_data['row'].iloc[i],true_sigmas[0]),
                            (sorted_data['col'].iloc[i+1],sorted_data['row'].iloc[i+1],true_sigmas[1]),
                            (sorted_data['col'].iloc[i+2],sorted_data['row'].iloc[i+2],true_sigmas[2])
                        ])


                        with open('true_sigmas.txt', mode='a') as file_object:
                            print(x0, file=file_object)




        # tuple_first_counts = (sys.argv[1],sorted_data['col'].iloc[0],sorted_data['row'].iloc[0])


        # SigmaFile2 = np.loadtxt(fname="sigma_files/%s/sigma_%i_%i.txt" % tuple_first_counts)
        # EnergyFile2 = np.loadtxt(fname="energy_files/%s/energy_%i_%i.txt" % tuple_first_counts)

        # df2 = pd.DataFrame({
        #     'col': SigmaFile2[:,0],
        #     'row': SigmaFile2[:,1],
        #     'counts': SigmaFile2[:,2],
        #     'sigma': SigmaFile2[:,3],
        #     'energy': EnergyFile2[:,3],
        # })


        # mask_second_counts = df2[(df2.col == sorted_data['col'].iloc[1]) & (df2.row == sorted_data['row'].iloc[1])]

        # first = pd.DataFrame({
        #     'col': [sorted_data['col'].iloc[0],sorted_data['col'].iloc[1],mask_second_counts['col'].iloc[0]],
        #     'row': [sorted_data['row'].iloc[0],sorted_data['row'].iloc[1],mask_second_counts['row'].iloc[0]],
        #     'sigma': [sorted_data['sigma'].iloc[0],sorted_data['sigma'].iloc[1],mask_second_counts['sigma'].iloc[0]],
        #     'energy': [sorted_data['energy'].iloc[0],sorted_data['energy'].iloc[1],mask_second_counts['energy'].iloc[0]]
        # })

        # true_sigmas = triple_coincidence(first['sigma'],first['energy'])

        # x0 = np.array([
        #     (sorted_data['col'].iloc[0],sorted_data['row'].iloc[0],true_sigmas[0]),
        #     (sorted_data['col'].iloc[1],sorted_data['row'].iloc[1],true_sigmas[1]),
        #     (sorted_data['col'].iloc[2],sorted_data['row'].iloc[2],true_sigmas[2])
        # ])

        # with open('true_sigmas.txt', mode='a') as file_object:
        #     print(x0, file=file_object)

    else:
        print("File is Empty!!!")

except IOError:
    print("File does not exist!!!")
