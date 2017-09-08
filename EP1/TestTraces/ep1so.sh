#!/bin/bash
python statistics_gen.py -s 1 small smallSJF.results
python statistics_gen.py -s 2 small smallRR.results
python statistics_gen.py -s 3 small smallPRR.results
python statistics_gen.py -s 1 med medSJF.results
python statistics_gen.py -s 2 med medRR.results
python statistics_gen.py -s 3 med medPRR.results
python statistics_gen.py -s 1 long longSJF.results
python statistics_gen.py -s 2 long longRR.results
python statistics_gen.py -s 3 long longPRR.results
