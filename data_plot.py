import matplotlib.pyplot as plt
import numpy as np
import json

thermocouple_count = 20

with open('data.txt', 'r') as datafile:
    data = datafile.read()
    runs = data.split("==========")
    for run in runs:
        if run == "":
            break
        header, body = run.split("=====")
        body = body.replace("NAN","\"NAN\"")
        header = json.loads(header)
        print(body)
        body = json.loads(body)
        p_data = []
        p_data.append([])
        p_data.append([])
        d_data = []
        w_data = []
        tc_data = []
        for i in range(0, thermocouple_count):
            tc_data.append([])
        # print(json.dumps(body, sort_keys=True, indent=4, separators=(',', ': ')))
        for read in body.values():
            counter = 0
            for tc in read["TC"]:
                if tc != "NAN":
                    tc_data[counter].append(tc)
                else:
                    tc_data[counter].append(0)
                counter += 1
            p_data[0].append(read["P"][0])
            p_data[1].append(read["P"][1])
            d_data.append(read["D"])
            w_data.append(read["W"])
        t = np.arange(0,(2.5*len(p_data)), 2.5)
        # print(t)
        # print(p_data)
        # print(tc_data)
        f, axarr = plt.subplots(4, sharex=True)
        axarr[0].plot(t, p_data, 'ro', t, p_data, 'k')
        axarr[0].set_title("Pressure")
        axarr[0].set_ylabel("Pressure (Pa)")
        axarr[0].grid(True)
        axarr[1].plot(t, d_data, 'go', t, d_data, 'k')
        axarr[1].set_title("Distance")
        axarr[1].set_ylabel("Distance (cm)")
        axarr[1].grid(True)
        axarr[2].plot(t, w_data, 'bo', t, w_data, 'k')
        axarr[2].set_title("Weight")
        axarr[2].set_ylabel("Weight (lbs)")
        axarr[2].grid(True)
        counter = 1
        axarr[3].set_title("Temperature")
        for series in tc_data:
            axarr[3].plot(t, series, label = "Temp {0} (C)".format(counter))
            counter += 1
        plt.xlabel("Time (s)")
        axarr[3].set_ylabel("Temperature (°C)")
        axarr[3].legend(bbox_to_anchor=(1.01, 3), loc=2, borderaxespad=0.)
        plt.grid(True)
        plt.subplots_adjust(right=0.75, left=0.07)

        plt.suptitle("Reactor Data - Single Run")
        plt.show()
            
        
