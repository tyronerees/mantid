from ReductionWrapper import *

class MDReductionWrapper(ReductionWrapper):
    """ A similar class to ReductionWrapper, but for reduction followed by accumulation
        to a merged MD file
    """ 
    def __init__(self, instrumentName, web_var = None):
        super(MDReductionWrapper, self).__init__(instrumentName, web_var = None)

    # Create a file with the filename and path, with a '.lock' extension, to indicate file in use,
    # unless this already exists, in which case do nothing and return False
    def lock_obtained(self, fname):
        if not os.path.isfile(fname + '.lock'):
            lock_file = open(fname + '.lock', 'w')
            lock_file.close()
            return True
        else:
            return False

    # Remove lock file, exception will be thrown if this does not exist
    def release_lock(self, fname):
        os.remove(fname + '.lock')

    # The table below shows the valid combinations for controlling the run numbers.
    # N = number of accumulated MD files to produce (e.g. Run Range Starts is a list of N integers).
    #                              _______________
    # ____________________________|_1_|_2_|_3_|_4_|
    # | 'Run Range Starts'        | N | N | N | 1 |
    # | 'Run Range Ends'          | N | - | - | - |
    # | 'Number of Runs to Merge' | - | N | 1 | 1 |
    # ---------------------------------------------
    #
    # If file_run_number is between range_starts and range_ends return the range and MD file number (0...N-1),
    # as a tuple or return -1, -1, -1 if the run number is not in a range asked for
    def get_file_number(self, file_run_number, range_starts, range_ends):
        for i in range(0, len(range_starts)):
	     if ((file_run_number >= range_starts[i]) and (file_run_number <= range_ends[i])):
	         return i, range_starts[i], range_ends[i]
        return -1, -1, -1

    # If number_of_runs_to_merge is given return the run range and MD file number (0...N-1) as a tuple,
    # or return -1, -1, -1 if the range is not as asked for
    def get_file_number_alternative(self, file_run_number, range_starts, number_of_runs_to_merge):
        if (len(range_starts) == 1 and len(number_of_runs_to_merge) == 1):
            if (file_run_number < range_starts[0]):
                return -1, -1, -1
            file_number = file_run_number//number_of_runs_to_merge[0] - range_starts[0]//number_of_runs_to_merge[0] 
            return file_number + 1, file_number * number_of_runs_to_merge[0], (file_number + 1) * number_of_runs_to_merge[0]
        elif (len(range_starts) > 1 and len(number_of_runs_to_merge) == 1):
            for i in range(0, len(range_starts)):
                 if (file_run_number >= range_starts[i] and file_run_number < range_starts[i] + number_of_runs_to_merge[0]):
                     return i, range_starts[i], range_starts[i] + number_of_runs_to_merge[0] - 1
            return -1, -1, -1
        elif (len(range_starts) == len(number_of_runs_to_merge)):
            for i in range(0, len(range_starts)):
                 if (file_run_number >= range_starts[i] and file_run_number < range_starts[i] + number_of_runs_to_merge[i]):
                     return i, range_starts[i], range_starts[i] + number_of_runs_to_merge[i] - 1
            return -1, -1, -1
                
        raise Exception("Size of range_starts, ", len(range_starts), " and number_of_runs_to_merge, ", 
                        len(number_of_runs_to_merge), " are incompatible")

    # If MD:Filenames is set return the appropriate filename, else return a filename such as 123_456_SQW.nxs
    def get_file_name(self, file_number, start, end):
        file_names = self._wvs.advanced_vars.get('MD:Filenames')
        if (file_names is None or len(file_names) == 0):
            return str(start) + '_' + str(end) + '_SQW.nxs'
        else:
            return file_names[file_number]

    # For psi allowed combinations are shown below
    # N = number of accumulated MD files to produce (e.g. Psi Starts is a list of N integers).
    #                     ___________
    # ___________________|_1_|_2_|_3_|
    # | 'Psi Starts'     | N | N | 1 |
    # | 'Psi Increments' | 1 | N | 1 |
    # --------------------------------
    #
    def get_psi(self, file_number, file_run_number, start, end):
        psi_starts = self._wvs.advanced_vars.get('MD:Psi Starts')
        psi_increments = self._wvs.advanced_vars.get('MD:Psi Increments')

        if (len(psi_starts) == 1 and len(psi_increments) == 1):
            return psi_starts[0] + (file_run_number - start) * psi_increments[0]
        elif (len(psi_starts) == len(psi_increments)):
            return psi_starts[file_number] + (file_run_number - start) * psi_increments[file_number]
        elif (len(psi_starts) > 1 and len(psi_increments) == 1):
            return psi_starts[file_number] + (file_run_number - start) * psi_increments[0]
        else:
            raise Exception("Size of psi_starts, ", len(psi_starts), " and psi_increments, ", 
                        len(psi_increments), " are incompatible")




