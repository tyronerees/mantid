""" MERLIN reduction script which also creates Merged MD files """ 

import time
import os

### Only required for running locally ###
#import sys
#sys.path.insert(0,'/opt/Mantid/scripts/Inelastic')
#sys.path.insert(0,'/home/whb43145/autoreduction_test_production')

from ReductionWrapper import *

try:
    import reduce_vars as web_var
except:
    web_var = None


class ReduceMERLIN(ReductionWrapper):
    @MainProperties
    def def_main_properties(self, file_run_number=None):
        """ Define main properties used in reduction """ 
        prop = {}
        if file_run_number is not None:
            prop['sample_run'] = file_run_number
        else:
            prop['sample_run'] = 22413

        prop['wb_run'] = 23012
        prop['incident_energy'] = 15
        prop['energy_bins'] = [-0.1,0.00175,0.95]
       
        # Absolute units reduction properties.
        prop['monovan_run'] = 22932
        prop['sample_mass'] = 10
        prop['sample_rmm'] = 435.96 
        prop['bleed'] = False
        return prop

    @AdvancedProperties
    def def_advanced_properties(self):
        """ Define advancded (optional) properties """
        prop = {};
        prop['map_file'] = 'one2one_125.map'
        prop['monovan_mapfile'] = 'rings_125.map'
        prop['hard_mask_file'] = 'Bjorn_mask.msk'
        prop['det_cal_file'] = 'det_corr_125.dat'
        prop['data_file_ext'] = '.nxs'
        prop['save_format'] = 'nxspe'

        # MD accumulation properties - prefix 'MD:' stops these parameters being
        # passed through to the initial reduction step.
        prop['MD:Accumulate to MD file'] = True
        prop['MD:Run Range Starts'] = [22413]
        prop['MD:Run Range Ends'] = [22932]
        prop['MD:Number of Runs to Merge'] = []
        prop['MD:Psi Starts'] = [0]
        prop['MD:Psi Increments'] = [2]
        prop['MD:UB Matrix'] = [1.4165, 1.4165, 1.4165] # a, b, c
        prop['MD:Filenames'] = None
        prop['MD:Minimum Extents'] = '-3, -5, -4, -5.0'
        prop['MD:Maximum Extents'] = '5, 2, 4, 30.0'
      
        return prop

    @iliad
    def main(self, input_file = None, output_directory = None, output_file = None):
        # run reduction, write auxiliary script to add something here.
        outWS = ReductionWrapper.reduce(input_file, output_directory)
        #SaveNexus(outWS, Filename = output_file)

        # When run from web service, instead return additional path for web server to copy data to
        return outWS

    def __init__(self, web_var = None):
        """ sets properties defaults for the instrument with Name"""
        ReductionWrapper.__init__(self,'MER',web_var)

# Create a file with the filename and path, with a '.lock' extension, to indicate file in use,
# unless this already exists, in which case do nothing and return False
def lock_obtained(fname):
    if not os.path.isfile(fname + '.lock'):
        lock_file = open(fname + '.lock', 'w')
        lock_file.close()
        return True
    else:
        return False

# Remove lock file, exception will be thrown if this does not exist
def release_lock(fname):
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
def get_file_number(file_run_number, range_starts, range_ends):
    for i in range(0, len(range_starts)):
         if ((file_run_number >= range_starts[i]) and (file_run_number <= range_ends[i])):
             return i, range_starts[i], range_ends[i]
    return -1, -1, -1

# If number_of_runs_to_merge is given return the run range and MD file number (0...N-1) as a tuple,
# or return -1, -1, -1 if the range is not as asked for
def get_file_number_alternative(file_run_number, range_starts, number_of_runs_to_merge):
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
def get_file_name(file_number, start, end):
    file_names = web_var.advanced_vars.get('MD:Filenames')
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
def get_psi(file_number, file_run_number, start, end):
    psi_starts = web_var.advanced_vars.get('MD:Psi Starts')
    psi_increments = web_var.advanced_vars.get('MD:Psi Increments')

    if (len(psi_starts) == 1 and len(psi_increments) == 1):
        return psi_starts[0] + (file_run_number - start) * psi_increments[0]
    elif (len(psi_starts) == len(psi_increments)):
        return psi_starts[file_number] + (file_run_number - start) * psi_increments[file_number]
    elif (len(psi_starts) > 1 and len(psi_increments) == 1):
        return psi_starts[file_number] + (file_run_number - start) * psi_increments[0]
    else:
        raise Exception("Size of psi_starts, ", len(psi_starts), " and psi_increments, ", 
                    len(psi_increments), " are incompatible")


def convert_and_merge(input_file, output_file, psi, ub_matrix, file_run_number):
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
    pars['MinValues']=web_var.advanced_vars.get('MD:Minimum Extents')
    pars['MaxValues']=web_var.advanced_vars.get('MD:Maximum Extents')
    pars['SplitInto']=50
    pars['MaxRecursionDepth']=1
    pars['MinRecursionDepth']=1
    pars['OutputWorkspace'] = 'merged_ws'

    # Get the lock on the output file. Can obtain this whether or not the file exists yet.
    # If lock is not obtained sleep for 10s. TODO: should this timeout?
    # In case of problems will need to delete the lock file manually.
    while(not lock_obtained(output_file)):
        print "Waiting for lock on ", output_file
        time.sleep(10)

    # Load the output file if it exists, create it otherwise
    if (os.path.exists(output_file)):
        merged_ws = LoadMD(Filename = output_file, FileBackEnd = True)
        pars['OverwriteExisting'] = False
    else:
        pars['OverwriteExisting'] = True

    merged_ws = ConvertToMD(**pars)

    # Save the file
    print output_file
    if pars.get('OverwriteExisting'):
        SaveMD(merged_ws, Filename = output_file, MakeFileBacked = True)        
    else:
        SaveMD(merged_ws, Filename = output_file, UpdateFileBackEnd = True)

    # Release the file lock now.
    release_lock(output_file)

# This is called by the autoreduction script itself
def reduce(input_file, output_dir):
    # Define any extra directories needed for maps files etc. (not needed for running through autoreduction server)

    ### Only required for running locally ###
    #maps_dir = '/home/whb43145/autoreduction_test_russell/maps'

    input_path, input_filename = os.path.split(input_file)

    # Merlin data starts in the format: 'MERXXXXX.raw', where XXXXXX is the run number
    file_run_number = int(input_filename.split('MER')[1].split('.')[0])

    ### Only required for running locally ###
    #config.setDataSearchDirs('{0};{1};{2}'.format(input_path,output_dir,maps_dir))
    config['defaultsave.directory'] = output_dir.encode('ascii','replace') # folder to save resulting spe/nxspe files

    rd = ReduceMERLIN(web_var)
    rd._run_from_web = False
    rd.def_advanced_properties()
    rd.def_main_properties(file_run_number)

    if not rd._run_from_web:
        run_dir=os.path.dirname(os.path.realpath(__file__))
        file = os.path.join(run_dir,'reduce_vars.py')
        rd.save_web_variables(file);

    output_filename = output_dir + '/' + rd.reducer.prop_man.save_file_name + '.nxspe'

    outWS = rd.reduce(input_file, output_dir)

    if (web_var.advanced_vars.get('MD:Accumulate to MD file')):
        # Process ranges to merge into something more useful
        range_starts = web_var.advanced_vars.get('MD:Run Range Starts')
        range_ends = web_var.advanced_vars.get('MD:Run Range Ends')
        number_of_runs_to_merge = web_var.advanced_vars.get('MD:Number of Runs to Merge')

        if (len(range_ends) == 0):
            file_number, start, end = get_file_number_alternative(file_run_number, range_starts, number_of_runs_to_merge)
        else:
            file_number, start, end = get_file_number(file_run_number, range_starts, range_ends)

        merged_filename = output_dir + '/../' + get_file_name(file_number, start, end)

        psi = get_psi(file_number, file_run_number, start, end)

        ub_matrix = web_var.advanced_vars.get('MD:UB Matrix')
 
        convert_and_merge(output_filename, merged_filename, psi, ub_matrix, file_run_number)

    return None # Can make this an additional save directory


def main(*args, **kwargs):
    if not kwargs and sys.argv: #If called from command line
        if len(sys.argv) == 3 and '=' not in sys.argv:
            # with two simple inputs
            kwargs = { 'data' : sys.argv[1], 'output':sys.argv[2]}
        else:
            # With key value inputs
            kwargs = dict(x.split('=', 1) for x in sys.argv[1:])
    if not kwargs and 'data' not in kwargs and 'output' not in kwargs:
        raise ValueError("Data and Output paths must be supplied")
    
    # If additional storage location are required, return them as a list of accessible directory paths
    additional_save_location = reduce(kwargs['data'], kwargs['output'])
    return additional_save_location


if __name__=="__main__":
    main()


