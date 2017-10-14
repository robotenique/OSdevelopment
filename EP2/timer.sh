#!/bin/bash
# Important: Install the time package with apt install time!!
# THIS IS NOT THE DEFAULT 'time' command!
fixed_size=300
short_laps=20
med_laps=40
long_laps=80
short_bikers=40
med_bikers=1
long_bikers=1
let "med_bikers = $fixed_size / 2"
let "long_bikers = $fixed_size + 37"
printf "road_size = $fixed_size
short_laps = $short_laps
med_laps = $med_laps
long_laps = $long_laps
short_bikers = $short_bikers
med_bikers = $med_bikers
long_bikers = $long_bikers \n"

echo "Time values are written in the stdout, and memoryusage is written in the output file."
#################################################################
#                            SHORT_LAPS                         #
#################################################################


echo "---> SHORT_LAPS" && echo "SHORT_LAPS" >> output

echo "     ----> SHORT_BIKERS" && echo "SHORT_BIKER" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $short_bikers $short_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $short_bikers $short_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> MED_BIKERS" && echo "MED_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $med_bikers $short_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $med_bikers $short_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> LONG_BIKERS" && echo "LONG_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $long_bikers $short_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $long_bikers $short_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)

done

#################################################################
#                            MED_LAPS                           #
#################################################################
echo "---> MED_LAPS" && echo "MED_LAPS" >> output

echo "     ----> SHORT_BIKERS" && echo "SHORT_BIKER" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $short_bikers $med_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $short_bikers $med_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> MED_BIKERS" && echo "MED_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $med_bikers $med_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $med_bikers $med_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> LONG_BIKERS" && echo "LONG_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $long_bikers $med_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $long_bikers $med_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)

done

#################################################################
#                            LONG_LAPS                          #
#################################################################

echo "---> LONG_LAPS" && echo "LONG_LAPS" >> output

echo "     ----> SHORT_BIKERS" && echo "SHORT_BIKER" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $short_bikers $long_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $short_bikers $long_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> MED_BIKERS" && echo "MED_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $med_bikers $long_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $med_bikers $long_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)
done

echo "     ----> LONG_BIKERS" && echo "LONG_BIKERS" >> output
for i in {1..30};
do
    $(/usr/bin/time -f "%e" ./bikeSim $fixed_size $long_bikers $long_laps > /dev/null)
    $(./memusage.sh --progname="bikeSim" ./bikeSim $fixed_size $long_bikers $long_laps > /dev/null 2> coisa.out && head -n2 coisa.out >> output)

done
