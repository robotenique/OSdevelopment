#!/bin/bash
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 4 small smallSJF.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 5 small smallRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 small smallPRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 small smallPRRSIGMOD.results SIGMOIDPLZ
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 4 med medSJF.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 5 med medRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 med medPRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 med medPRRSIGMOD.results SIGMOIDPLZ
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 4 long longSJF.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 5 long longRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 long longPRR.results
/home/bcc/robotenique/miniconda3/bin/python3.6 statistics_gen.py 6 long longPRRSIGMOD.results SIGMOIDPLZ
