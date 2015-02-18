""" MERLIN reduction script which also creates Merged MD files """ 

### Only required for running locally ###
#import sys
#sys.path.insert(0,'/home/whb43145/autoreduction_test')

from Direct.MDReductionWrapper import *

try:
    import reduce_vars as web_var
except:
    web_var = None


class ReduceMERLIN(MDReductionWrapper):
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

        # When run from web service, instead return additional path for web server to copy data to
        return outWS

    def __init__(self, web_var = None):
        """ sets properties defaults for the instrument with Name"""
        super(ReduceMERLIN, self).__init__('MER',web_var)


# This is called by the autoreduction script itself
def reduce(input_file, output_dir):
    # Define any extra directories needed for maps files etc. (not needed for running through autoreduction server)

    ### Only required for running locally ###
    #maps_dir = '/home/whb43145/autoreduction_test'

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
        rd.convert_and_merge(output_filename, output_dir, file_run_number)

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


