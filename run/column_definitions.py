# define a dictionary for keeping track of what is where
global column_no_to_name


get_column_name = {0: 'weight',
                   1: 'label',
                   2:  'jet mass',
                   3:  'jet pt',
                   4:  'jet eta',
                   5:  'jet phi',
                   6:  'Tau32_wta',
                   7:  'Split23',
                   8:  'NPV',
                   9:  'C2',
                   10: 'D2',
                   11:  'subjet start',
                   12:  'b eta',
                   13:  'b phi',
                   14:  'W_1 pt',
                   15:  'W_1 eta',
                   16:  'W_1 phi',
                   17:  'W_2 pt',
                   18:  'W_2 eta',
                   19:  'W_2 phi',
                   20:  'constit start pt',
		       21:  '1 const eta',
		       22:  '1 const phi',
                   23:  '2 const pt',
		       24:  '2 const eta',
                   25:  '2 const phi',
		       26:  '3 const pt',
                   27:  '3 const eta',
                   28:  '4 const phi'
			}

get_column_no = dict((reversed(item) for item in get_column_name.items()))
