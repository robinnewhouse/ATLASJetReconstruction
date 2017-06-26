import numpy as np
from column_definitions import *
from itertools import chain
from time import sleep

d_pt = 0.01
d_eta = 0.01
d_phi = 0.01
d_err = 0.0001


def display_side_by_side(pre_py_file, pre_c_file):
    print(pre_c_file)
    pre_py = (np.load(pre_py_file))["data"]
    pre_py = pre_py[pre_py[:,get_column_no["jet pt"]].argsort()] # sort both by pt

    pre_c = np.loadtxt(open(pre_c_file, "rb"), delimiter=",", usecols=range(51))
    # np.savez(pre_c_file+".npz", pre_c)
    # pre_c = (np.load(pre_c_file+".npz"))["arr_0"]   
    pre_c = pre_c[pre_c[:,get_column_no["jet pt"]].argsort()] # sort both by pt

    for x in range(len(pre_py)):
        vals_to_print = list(chain([0,1,3,4,5], range(20,len(pre_c[x]))))

        print("py:", end='')    
        for y in vals_to_print:
            p = pre_py[x][y]
            print("{:.5e}".format(p), end=' ')
        print()

        print("c: ", end='')    
        for y in vals_to_print:
            c = pre_c[compare_dict[x]][y]
            print("{:.5e}".format(c), end=' ')
        print()
        print("df:", end='')    
        for y in vals_to_print:
            p = pre_py[x][y]
            c = pre_c[compare_dict[x]][y]
            if p == 0: 
                rel_err = float("inf")
                if c == 0: rel_err = 0
            else: rel_err = (p - c)/p
            print("{:.5e}".format(rel_err), end=' ')
            if abs(rel_err) >= d_err:
                print("<-UNMATCHED", end=' ')
                # sleep(0.5)
        print()
        print()

    print()



def display_c(pre_c_file):
    pre_c = np.loadtxt(open(pre_c_file, "rb"), delimiter=",", usecols=range(50))
    np.savez(pre_c_file+".npz", pre_c)
    pre_c = (np.load(pre_c_file+".npz"))["arr_0"]   
    pre_c = pre_c[pre_c[:,get_column_no["jet pt"]].argsort()] # sort both by pt
    for x in range(10):
        for y in chain([0,1,3,4,5], range(20,len(pre_c[x]))):
            p = pre_c[x][y]
            print("{:.2e}".format(p), end=' ')
        print()
    print()
        
def display_py(pre_py_file):
    pre_py = (np.load(pre_py_file))["data"]
    pre_py = pre_py[pre_py[:,get_column_no["jet pt"]].argsort()] # sort both by pt
    for x in range(10):
        for y in chain([0,1,3,4,5], range(20,len(pre_py[x]))):
            p = pre_py[x][y]
            print("{:.2e}".format(p), end=' ')
        print()
    print()

def compare(pre_py_file, pre_c_file):

    # Load file
    pre_py = (np.load(pre_py_file))["data"]
    pre_c = np.loadtxt(open(pre_c_file, "rb"), delimiter=",", usecols=range(50))
    np.savez(pre_c_file+".npz", pre_c)
    pre_c = (np.load(pre_c_file+".npz"))["arr_0"]   

    # Sort by pt
    pre_c = pre_c[pre_c[:,get_column_no["jet pt"]].argsort()] # sort both by pt
    pre_py = pre_py[pre_py[:,get_column_no["jet pt"]].argsort()] # sort both by pt

    # compare number of recorded events
    print("Python array length: ",len(pre_py), "    C++ array length: ",len(pre_c))
    print()

    for x in range(len(pre_py)):
        try: compare_dict[x]
        except: print("no entry for", x)

    # for x in range(100):
    #     print(pre_py[x][get_column_no["jet pt"]], "\t\t\t\t",   pre_c[compare_dict[x]][get_column_no["jet pt"]]/1000.0)
    #     print('  ',pre_py[x][get_column_no["jet eta"]], "\t\t\t\t", pre_c[compare_dict[x]][get_column_no["jet eta"]])
    #     print('    ',pre_py[x][get_column_no["jet phi"]], "\t\t\t\t", pre_c[compare_dict[x]][get_column_no["jet phi"]])

    # compare each individual value
    for x in range(100):
        for y in chain([0,1,3,4,5], range(20,len(pre_py[x]))):
            p = pre_py[x][y]
            c = pre_c[compare_dict[x]][y]
            rel_err = (p - c)/p
            print("{:.2e}".format(rel_err), end=' ')
            if rel_err >= d_err:
                print("<-UNMATCHED", end=' ')
                # sleep(0.5)
        print()
    # print(pre_py[0][get_column_no["jet mass"]])

    # for x in range(len(pre_py)):
    #   print(pre_py[x][get_column_no["label"]])
    #   pass
    

# pre_py = (np.load("testrun_short_10_pt_min_max_scale_pt_shift_prim_and_rotate_prim_and_flip_eta_phiflip_everything.npz"))["data"]

# # pre_c = np.loadtxt(open("jet_data.csv", "rb"), delimiter=",", usecols=range(50))
# # np.savez("jet_data.npz", pre_c)
# pre_c = (np.load("jet_data.npz"))["arr_0"]

# pre_c = pre_c[pre_c[:,get_column_no["jet pt"]].argsort()] # sort both by pt
# pre_py = pre_py[pre_py[:,get_column_no["jet pt"]].argsort()] # sort both by pt

# convert to GeV
# pre_c[:,get_column_no["jet pt"]] = pre_c[:,get_column_no["jet pt"]] / 1000

# np.savetxt("pre_py_data.csv", pre_py, delimiter=",")
# np.savetxt("pre_c_data.csv", pre_c, delimiter=",")


# print("Python jet pt: \t\t\t","C++ jet pt: ")

# for x in range(len(pre_py)):

def get_compare_dict(pre_py_file, pre_c_file):
    # print(pre_c_file)
    # pre_py = (np.load(pre_py_file))["data"]
    # pre_py = pre_py[pre_py[:,get_column_no["jet pt"]].argsort()] # sort both by pt

    # pre_c = np.loadtxt(open(pre_c_file, "rb"), delimiter=",", usecols=range(51))
    # # np.savez(pre_c_file+".npz", pre_c)
    # # pre_c = (np.load(pre_c_file+".npz"))["arr_0"]   
    # pre_c = pre_c[pre_c[:,get_column_no["jet pt"]].argsort()] # sort both by pt

    # compare_dict = {}
    
    # for x in range(len(pre_py)):
    # # for x in range(100): # for testing
    #     # print()
    #     for y in range(len(pre_c)):
    #     # for y in range(1000): # for testing
    #         if (abs(pre_py[x][get_column_no["jet pt"]] - pre_c[y][get_column_no["jet pt"]])) <= d_pt:
    #             if(abs(pre_py[x][get_column_no["jet eta"]] - pre_c[y][get_column_no["jet eta"]] )) <= d_pt:
    #                 if (abs(pre_py[x][get_column_no["jet phi"]] - pre_c[y][get_column_no["jet phi"]])) <= d_pt:
    #                     # print("match")
    #                     compare_dict[x] = y
    #                     break
    # # print(compare_dict)

    # np.save('event_map.npy', compare_dict)
    return np.load('event_map.npy').item()

if __name__ == '__main__':
    compare_dict = get_compare_dict("npz_w_pred.npz", "jet_data_scored.csv")
    # display_py("testrun_short_10_no_scale_pt_no_scale_eta_phiflip_everything.npz")    
    # print()
    # display_py("testrun_short_10_pt_min_max_scale_pt_shift_prim_eta_phiflip_everything.npz")
    # print()
    # print()
    # display_c("jet_data_scaled_shifted.csv")
    # display_side_by_side("testrun_short_10_no_scale_pt_no_scale_eta_phiflip_everything.npz", "jet_data_untransformed.csv")
    # display_side_by_side("testrun_short_10_pt_min_max_scale_pt_no_scale_eta_phiflip_everything.npz", "jet_data_scaled.csv")
    # display_side_by_side("testrun_short_10_pt_min_max_scale_pt_shift_prim_eta_phiflip_everything.npz", "jet_data_scaled_shifted.csv")
    # display_side_by_side("testrun_short_10_pt_min_max_scale_pt_shift_prim_and_rotate_prim_eta_phiflip_everything.npz", "jet_data_scaled_shifted_rotated.csv")
    # display_side_by_side("testrun_short_10_pt_min_max_scale_pt_shift_prim_and_rotate_prim_and_flip_eta_phiflip_everything.npz", "jet_data_scaled_shifted_rotated_flipped.csv")
    # display_side_by_side("testrun_short_10_pt_min_max_scale_pt_shift_prim_and_rotate_prim_and_flip_eta_phiflip_everything.npz", "jet_data_final.csv")
    display_side_by_side("npz_w_pred.npz", "jet_data_scored.csv")
    # print(compare_dict)

