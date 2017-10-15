import math
import numpy as np
import scipy.stats as st
import matplotlib.pyplot as plt

def get_CI(v, opt=False):
    if opt:
        ci = st.t.interval(0.95, len(v) - 1, loc=np.mean(v), scale=st.sem(v))
        arr = [0, 0]
        arr[0] = np.mean(v) - ci[0]
        arr[1] =ci[1] - np.mean(v)
        return arr
    return st.t.interval(0.95, len(v) - 1, loc=np.mean(v), scale=st.sem(v))

def plot_bar(vals, xlabel ,ylabel, xticklabels, title):
    colors = ["skyblue", "chartreuse", "gold"]
    ind = np.array([0, 2, 4]) # the x locations for the groups
    width = 0.35        # the width of the bars
    # Here comes the lambda again...
    get_err = lambda x, y, z : ((x[0],y[0],z[0]), (x[1],y[1],z[1]))
    ci_0, ci_1, ci_2 = get_CI(vals[0], True), get_CI(vals[1], True), get_CI(vals[2], True)
    ci_3, ci_4, ci_5 = get_CI(vals[3], True), get_CI(vals[4], True), get_CI(vals[5], True)
    ci_6, ci_7, ci_8 = get_CI(vals[6], True), get_CI(vals[7], True), get_CI(vals[8], True)
    yerr_bar1 = get_err(ci_0, ci_1, ci_2)
    yerr_bar2 = get_err(ci_3, ci_4, ci_5)
    yerr_bar3 = get_err(ci_6, ci_7, ci_8)
    fig, ax = plt.subplots(figsize=(9, 7))
    time_bar1 = ax.bar(ind - width/2, (np.mean(vals[0]), np.mean(vals[1]), np.mean(vals[2])),
                       width, yerr=yerr_bar1,  color=colors[0], capsize=5, label='Poucos')
    time_bar2 = ax.bar(ind + width/2, (np.mean(vals[3]), np.mean(vals[4]), np.mean(vals[5])),
                       width, yerr=yerr_bar2, color=colors[1],capsize=5, label='Médio')
    time_bar3 = ax.bar(ind + 1.5*width, (np.mean(vals[6]), np.mean(vals[7]), np.mean(vals[8])),
                       width, yerr=yerr_bar3, color=colors[2],capsize=5, label='Muitos')
    ax.set_title(title, size=18)
    ax.set_xlabel(xlabel, size=16)
    ax.set_ylabel(ylabel, color='b', size=16)
    ax.tick_params('y', colors='b')
    ax.set_xticks(ind + width / 2)
    ax.set_xticklabels(xticklabels)
    plt.legend(shadow=True, fancybox=True, title="Quantidade de ciclistas")
    plt.show()

# OBS: 'sb' stands for short bikers, 'sl' for short laps, etc.
# TIME ARRAYS
# short bikers
sb_sl = np.array([6.41, 5.87, 5.35, 5.27, 5.55, 5.54, 5.53, 5.78, 5.79, 5.94,
                 5.72, 5.88, 5.38, 5.82, 5.56, 5.83, 5.64, 5.80, 5.59, 5.73,
                 5.44, 5.35, 5.72, 5.58, 5.64, 5.66, 5.69, 5.34, 5.67, 5.66])



sb_ml = np.array([10.45, 10.10, 10.20, 9.82, 9.85, 9.60, 9.90, 9.98, 10.17,
                 9.74, 10.01, 9.76, 10.16, 9.66, 9.62, 9.75, 10.10, 10.08,
                 9.72, 10.22, 9.70, 10.32, 9.57, 9.78, 10.10, 9.93, 9.74,
                 10.19, 9.80, 9.89])
sb_ll = np.array([21.84, 21.62, 19.34, 21.59, 18.78, 20.82, 18.53, 20.86,
                 22.49, 23.87, 21.74, 23.39, 24.18, 24.17, 25.35, 19.78,
                 25.49, 23.52, 19.05, 19.03, 21.76, 19.29, 18.05, 18.98,
                 19.93, 21.55, 21.55, 20.04, 19.27, 19.20])

# med bikers
mb_sl = np.array([22.25, 23.28, 21.96, 21.82, 22.60, 24.14, 23.02, 22.66,
                 22.73, 21.16, 24.22, 22.73, 22.10, 22.97, 21.68, 23.20,
                 22.32, 21.87, 22.06, 22.95, 22.21, 22.15, 22.05, 22.62,
                 23.01, 22.72, 22.94, 22.26, 23.02, 21.76])
mb_ml = np.array([39.91, 39.38, 40.08, 39.54, 37.94, 42.94, 39.09, 44.77,
                 45.20, 45.90, 44.85, 44.90, 48.60, 41.09, 47.90, 45.26,
                 48.80, 60.77, 61.61, 60.39, 61.58, 61.24, 55.71, 63.49,
                 59.86, 51.89, 53.52, 53.04, 56.10, 51.64])
mb_ll = np.array([76.46, 75.92, 74.28, 73.66, 75.07, 74.37, 73.83, 76.65,
                 74.31, 75.87, 73.97, 75.12, 74.82, 75.38, 76.35, 76.14,
                 73.80, 75.34, 74.77, 81.13, 75.14, 75.30, 76.19, 80.49,
                 75.89, 74.67, 74.40, 77.38, 75.59, 74.88])

# long bikers
lb_sl = np.array([54.41, 56.32, 55.89, 52.90, 51.32, 53.67, 56.20, 51.61,
                 52.81, 65.24, 49.22, 47.78, 51.14, 49.95, 49.97, 51.62,
                 51.39, 50.49, 50.67, 50.67, 51.31, 51.86, 49.36, 50.99,
                 49.48, 48.96, 49.63, 49.94, 51.66, 49.97])
lb_ml = np.array([121.43, 110.99, 103.36, 118.99, 99.38, 104.51, 139.66,
                 132.74, 134.32, 117.22, 116.20, 137.39, 129.18, 114.79,
                 122.88, 116.80, 128.65, 118.53, 132.00, 109.62, 129.56,
                 96.17, 98.42, 94.96, 95.64, 95.50, 99.52, 99.18, 99.97, 116.86])
lb_ll = np.array([185.52, 189.21, 181.71, 182.42, 190.21, 185.29, 183.01,
                 188.69, 180.90, 184.58, 182.44, 184.41, 191.69, 185.71,
                 181.15, 184.28, 176.89, 185.68, 185.60, 185.82, 181.39,
                 179.15, 180.67, 180.47, 181.90, 180.06, 181.62, 186.66, 176.64, 185.52])

# -------------  MEMORY -----------
# short bikers
sb_sl_m = np.array([30101066, 29190090, 30269162, 30683450, 31568706, 30522458,
                    30839370, 33504362, 31087082, 32362106, 26230306, 30701498,
                    26181146, 30369530, 33768866, 27905306, 27435594, 26771050,
                    29443178, 26280266, 30691274, 30492354, 27559562, 28337954,
                    31873418, 29470218, 27251354, 29231834, 27621418, 28365722])

sb_ml_m = np.array([50135498, 48517146, 52183826, 50630938, 53061738, 43556954,
                    47700962, 45959106, 58750514, 47506850, 50516394, 55245842,
                    52310554, 49619178, 59394594, 53320226, 48602738, 55289570,
                    46346370, 52738034, 49970706, 51495282, 58277786, 50560274,
                    45966386, 53194226, 55403138, 51859610, 48147562, 51712954])

sb_ll_m = np.array([96573586, 99456482, 91175522, 93058658, 90256570, 95969474,
                    101651218,  95373786, 100545370,  88827218, 85131746,
                    96981970, 90957554, 92205082, 89986082, 97316266, 95173626,
                    101769586,  100454442,  92031370, 87550450, 92192298,
                    89955754, 95715234, 100504226,  92503106, 100573002,
                    90480394, 100401466,  93128178])
# med bikers
mb_sl_m = np.array([155872398, 145773846, 151413846, 145749646, 140622390,
                    153581150, 152880606, 143391318, 144202166, 140197118,
                    138343142, 145701366, 153144294, 143391862, 143591518,
                    148462934, 144124430, 154041014, 146395710, 144811622,
                    143524590, 152016078, 138950014, 146681054, 151983694,
                    144232566, 143052182, 137779110, 146450550, 148314470])

mb_ml_m = np.array([253051894, 268298854, 269642814, 266055734, 279242606,
                    280164966, 259253430, 271278342, 250832710, 267446062,
                    274077894, 281426838, 266359182, 292416782, 272542446,
                    260677270, 288964614, 275053486, 250257974, 280916694,
                    280932822, 258023286, 277528998, 276795646, 268273878,
                    281285318, 274596022, 282840990, 265219854, 263650342])

mb_ll_m = np.array([512197534, 517055062, 512838894, 475525998, 499283894,
                    476038790, 492121398, 465961118, 507050758, 471249830,
                    494719254, 503459278, 489441046, 514200046, 482064038,
                    496318958, 488560694, 474486542, 465468942, 480035910,
                    493635734, 498220486, 503423598, 485262894, 552716782,
                    504123998, 518899718, 497526358, 497440350, 482459486])

# long bikers
lb_sl_m = np.array([465325252, 507550924, 489906628, 487978620, 534376324,
                    499905964, 474847668, 512859100, 541977916, 522315988,
                    526869772, 513761556, 525648268, 520166516, 496216268,
                    502017444, 501956316, 498767804, 501470132, 504584196,
                    462899404, 499675252, 575820916, 506342812, 516829612,
                    484511268, 480785876, 522258596, 510325292, 521464852])

lb_ml_m = np.array([1009822900, 1081705612, 966473580, 1021051956, 1004604908,
                    992138060, 1033460428, 979758676, 964413164, 1010052396,
                    955554652, 1038888404, 958603068, 957789564, 1008279252,
                    1022611988, 970532828, 945636580, 977889788, 1015895348,
                    1007353204, 963736404, 1046831268, 1005147516, 1021931564,
                    1000965316, 1014360828, 1040066660, 1011717692, 99947034])

lb_ll_m = np.array([1887097532, 1912069916, 1908078508, 1920715292, 1837070180,
                    1910005212, 1989845100, 1969521068, 1860664548, 2017546076,
                    1943858652, 1924751508, 1945520204, 1910534380, 1897171396,
                    1888090892, 1892131876, 1924238684, 1936242796, 1878202596,
                    1891515308, 1941582468, 1789471444, 1891732804, 1770616908,
                    1951294860, 1845939636, 1954368100, 1905951884, 1940183548])

#print("TIME USAGE, Short biker | short_laps: ", get_CI(sb_sl))
# Time usage
plot_bar((sb_sl, sb_ml, sb_ll, mb_sl, mb_ml, mb_ll, lb_sl, lb_ml, lb_ll),
        "Quantidade de voltas", "Tempo (s)", ('Baixa', 'Média', 'Alta'),
         "Uso de tempo")

# Memory usage
plot_bar((sb_sl_m/1e6, sb_ml_m/1e6, sb_ll_m/1e6, mb_sl_m/1e6, mb_ml_m/1e6, mb_ll_m/1e6, lb_sl_m/1e6, lb_ml_m/1e6, lb_ll_m/1e6),
        "Quantidade de voltas", "Memória (MB)", ('Baixa', 'Média', 'Alta'),
         "Uso de memória")
