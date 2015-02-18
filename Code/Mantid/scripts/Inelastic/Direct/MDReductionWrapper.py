import os
import time

from ReductionWrapper import *

class MDReductionWrapper(ReductionWrapper):
    """ A similar class to ReductionWrapper, but for reduction followed by accumulation
        to a merged MD file
    """ 
    def __init__(self, instrumentName, web_var = None):
        super(MDReductionWrapper, self).__init__(instrumentName, web_var = None)

    def lock_obtained(self, fname):
        """Create a file with the filename and path, with a '.lock' extension, to indicate
           file in use, unless this already exists, in which case do nothing and return False
        """
        if not os.path.isfile(fname + '.lock'):
            lock_file = open(fname + '.lock', 'w')
            lock_file.close()
            return True
        else:
            return False

    def release_lock(self, fname):
        """Remove lock file, exception will be thrown if this does not exist
        """
        os.remove(fname + '.lock')

    def get_file_number(self, file_run_number, range_starts, range_ends):
        """ The table below shows the valid combinations for controlling the run numbers.
            N = number of accumulated MD files to produce (e.g. Run Range Starts is a list of N integers).
                                          _______________
             ____________________________|_1_|_2_|_3_|_4_|
             | 'Run Range Starts'        | N | N | N | 1 |
             | 'Run Range Ends'          | N | - | - | - |
             | 'Number of Runs to Merge' | - | N | 1 | 1 |
             ---------------------------------------------
        
            If file_run_number is between range_starts and range_ends return the range and MD file number (0...N-1),
            as a tuple or return -1, -1, -1 if the run number is not in a range asked for
        """

        for i in range(0, len(range_starts)):
	     if ((file_run_number >= range_starts[i]) and (file_run_number <= range_ends[i])):
	         return i, range_starts[i], range_ends[i]
        return -1, -1, -1

    def get_file_number_alternative(self, file_run_number, range_starts, number_of_runs_to_merge):
        """ If number_of_runs_to_merge is given return the run range and MD file number (0...N-1) as a tuple,
            or return -1, -1, -1 if the range is not as asked for
        """
        if (len(range_starts) == 1 and len(number_of_runs_to_merge) == 1):
            if (file_run_number < range_starts[0]):
                return -1, -1, -1
            file_number = file_run_number//number_of_runs_to_merge[0] - range_starts[0]//number_of_runs_to_merge[0] 
            return file_number, file_number * number_of_runs_to_merge[0] + 1, (file_number + 1) * number_of_runs_to_merge[0]
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

    def get_file_name(self, file_names, file_number, start, end):
        """ If MD:Filenames is set return the appropriate filename, else return a filename such as 123_456_SQW.nxs
        """
        if (file_names is None or len(file_names) == 0):
            return str(start) + '_' + str(end) + '_SQW.nxs'
        else:
            return file_names[file_number]

    def get_psi(self, psi_starts, psi_increments, file_number, file_run_number, start):
        """ For psi allowed combinations are shown below
            N = number of accumulated MD files to produce (e.g. Psi Starts is a list of N integers).
                                ___________
            ___________________|_1_|_2_|_3_|
            | 'Psi Starts'     | N | N | 1 |
            | 'Psi Increments' | 1 | N | 1 |
            --------------------------------
        """

        if (len(psi_starts) == 1 and len(psi_increments) == 1):
            return psi_starts[0] + (file_run_number - start) * psi_increments[0]
        elif (len(psi_starts) == len(psi_increments)):
            return psi_starts[file_number] + (file_run_number - start) * psi_increments[file_number]
        elif (len(psi_starts) > 1 and len(psi_increments) == 1):
            return psi_starts[file_number] + (file_run_number - start) * psi_increments[0]
        else:
            raise Exception("Size of psi_starts, ", len(psi_starts), " and psi_increments, ", 
                        len(psi_increments), " are incompatible")

    def convert_and_merge(self, input_file, input_dir, file_run_number):
        """ Does the conversion process to a filebacked MD file, first working out the file 
            names required
        """
        # Process ranges to merge into something more useful
        range_starts = self._wvs.advanced_vars.get('MD:Run Range Starts')
        range_ends = self._wvs.advanced_vars.get('MD:Run Range Ends')
        number_of_runs_to_merge = self._wvs.advanced_vars.get('MD:Number of Runs to Merge')

        if (len(range_ends) == 0):
            file_number, start, end = self.get_file_number_alternative(file_run_number, range_starts, number_of_runs_to_merge)
        else:
            file_number, start, end = self.get_file_number(file_run_number, range_starts, range_ends)

        merged_filename = input_dir + '/../' + \
          self.get_file_name(self._wvs.advanced_vars.get('MD:Filenames'), file_number, start, end)

        psi = self.get_psi(self._wvs.advanced_vars.get('MD:Psi Starts'), \
                           self._wvs.advanced_vars.get('MD:Psi Increments'), \
                           file_number, file_run_number, start)

        ub_matrix = self._wvs.advanced_vars.get('MD:UB Matrix')

        # Convert to an MD workspace and merge into the big file
        loaded_ws = Load(Filename = input_file)

        # UB matrix contains lattice and the beam direction  
        SetUB(Workspace = loaded_ws, a = ub_matrix[0], b = ub_matrix[1], c = ub_matrix[2])
       
        # Add crystal rotation logging
        # str(float(psi)) psi and can be saved as a number
        AddSampleLog(Workspace = loaded_ws, LogName = 'Psi', LogText = str(float(psi)), LogType = 'Number')

        # Set crystal rotation
        SetGoniometer(Workspace = loaded_ws, Axis0 = 'Psi, 0, 1, 0, 1')

        # Convert to MD
        # If these values need to be changed they can be set in def_advanced_properties
        # on the reduction class
        pars = dict();
        pars['InputWorkspace']='loaded_ws'
        pars['QDimensions']='Q3D'
        pars['dEAnalysisMode']='Direct'
        pars['Q3DFrames']='HKL'
        pars['QConversionScales']='HKL'
        pars['PreprocDetectorsWS']='preprDetMantid'
        pars['MinValues']=self._wvs.advanced_vars.get('MD:Minimum Extents')
        pars['MaxValues']=self._wvs.advanced_vars.get('MD:Maximum Extents')
        pars['SplitInto']=50
        pars['MaxRecursionDepth']=1
        pars['MinRecursionDepth']=1
        pars['OutputWorkspace'] = 'merged_ws'

        # Get the lock on the output file. Can obtain this whether or not the file exists yet.
        # If lock is not obtained sleep for 10s. TODO: should this timeout?
        # In case of problems will need to delete the lock file manually.
        while(not self.lock_obtained(merged_filename)):
            print "Waiting for lock on ", merged_filename
            time.sleep(10)

        # Load the output file if it exists, create it otherwise
        if (os.path.exists(merged_filename)):
            merged_ws = LoadMD(Filename = merged_filename, FileBackEnd = True)
            pars['OverwriteExisting'] = False
        else:
            pars['OverwriteExisting'] = True

        merged_ws = ConvertToMD(**pars)

        # Save the files
        if pars.get('OverwriteExisting'):
            SaveMD(merged_ws, Filename = merged_filename, MakeFileBacked = True)        
        else:
            SaveMD(merged_ws, Filename = merged_filename, UpdateFileBackEnd = True)

        # Release the file lock now.
        self.release_lock(merged_filename)


