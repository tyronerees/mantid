#pylint: disable=W0403,C0103,R0901,R0904,R0913
import numpy
import sys

import NTableWidget as tableBase


class PeakIntegrationTableWidget(tableBase.NTableWidget):
    """
    Extended table widget for studying peak integration of scan on various Pts.
    """

    # UB peak information table
    Table_Setup = [('Pt', 'int'),
                   ('Raw', 'float'),
                   ('Masked', 'float'),
                   ('Selected', 'checkbox')]

    def __init__(self, parent):
        """
        :param parent:
        """
        tableBase.NTableWidget.__init__(self, parent)

        return

    def append_pt(self, pt_number, raw_signal, masked_signal):
        """
        out_ws, target_frame, exp_no, scan_no, None
        :param info_tuple:
        :return: 2-tuple as boolean and error message
        """
        # TODO/NOW - Doc and check
        # assert ...

        status, msg = self.append_row([pt_number, raw_signal, masked_signal])
        if status is False:
            msg = 'Unable to append row to peak integration table due to %s' % msg

        return status, msg

    def setup(self):
        """
        Init setup
        :return:
        """
        self.init_setup(self.Table_Setup)

        self._statusColName = 'Selected'

        return

    def set_integrated_values(self, pt_vec, intensity_vec):
        """

        :param pt_vec:
        :param intensity_vec:
        :return:
        """
        # check
        assert len(pt_vec) == len(intensity_vec)

        # common value
        hkl = '0, 0, 0'
        q = '0, 0, 0'
        signal = -1

        num_rows = len(pt_vec)
        for i_row in xrange(num_rows):
            pt = int(pt_vec[i_row])
            intensity = intensity_vec[i_row]
            item_list = [pt, hkl, q, signal, intensity]
            self.append_row(item_list)

        return

    def set_q(self, row_index, vec_q):
        """
        Set up Q value
        :param row_index:
        :param vec_q: a list or array with size 3
        """
        assert len(vec_q) == 3

        # locate
        index_q_x = self.Table_Setup.index(('Q_x', 'float'))
        for j in xrange(3):
            col_index = j + index_q_x
            self.update_cell_value(row_index, col_index, vec_q[j])

        return


class UBMatrixTable(tableBase.NTableWidget):
    """
    Extended table for UB matrix
    """
    def __init__(self, parent):
        """

        :param parent:
        :return:
        """
        tableBase.NTableWidget.__init__(self, parent)

        # Matrix
        self._matrix = numpy.ndarray((3, 3), float)
        for i in xrange(3):
            for j in xrange(3):
                self._matrix[i][j] = 0.

        return

    def _set_to_table(self):
        """
        Set values in holder '_matrix' to TableWidget
        :return:
        """
        for i_row in xrange(3):
            for j_col in xrange(3):
                self.update_cell_value(i_row, j_col, self._matrix[i_row][j_col])

        return

    def get_matrix(self):
        """
        Get the copy of the matrix
        Guarantees: return a 3 x 3 ndarray
        :return:
        """
        # print '[DB] MatrixTable: _Matrix = ', self._matrix
        return self._matrix.copy()

    def set_from_list(self, element_array):
        """
        Set table value including holder and QTable from a 1D numpy array
        :param element_array:
        :return:
        """
        # Check
        assert isinstance(element_array, list)
        assert len(element_array) == 9

        # Set value
        i_array = 0
        for i in xrange(3):
            for j in xrange(3):
                self._matrix[i][j] = element_array[i_array]
                i_array += 1

        # Set to table
        self._set_to_table()

        return

    def set_from_matrix(self, matrix):
        """
        Set value to both holder and QTable from a numpy 3 x 3 matrix
        :param matrix:
        :return:
        """
        # Check
        assert isinstance(matrix, numpy.ndarray), 'Input matrix must be numpy.ndarray, but not %s' % str(type(matrix))
        assert matrix.shape == (3, 3)

        for i in xrange(3):
            for j in xrange(3):
                self._matrix[i][j] = matrix[i][j]

        self._set_to_table()

        return

    def setup(self):
        """
        Init setup
        :return:
        """
        # self.init_size(3, 3)

        for i in xrange(3):
            for j in xrange(3):
                self.set_value_cell(i, j)

        self._set_to_table()

        return


# UB peak information table
UB_Peak_Table_Setup = [('Scan', 'int'),
                       ('Pt', 'int'),
                       ('H', 'float'),
                       ('K', 'float'),
                       ('L', 'float'),
                       ('Q_x', 'float'),
                       ('Q_y', 'float'),
                       ('Q_z', 'float'),
                       ('Selected', 'checkbox'),
                       ('m1', 'float'),
                       ('Error', 'float')]


class UBMatrixPeakTable(tableBase.NTableWidget):
    """
    Extended table for peaks used to calculate UB matrix
    """
    def __init__(self, parent):
        """

        :param parent:
        :return:
        """
        tableBase.NTableWidget.__init__(self, parent)

        return

    def get_exp_info(self, row_index):
        """
        Get experiment information from a row
        :param row_index:
        :return: scan number, pt number
        """
        assert isinstance(row_index, int)

        scan_number = self.get_cell_value(row_index, 0)
        assert isinstance(scan_number, int)
        pt_number = self.get_cell_value(row_index, 1)
        assert isinstance(pt_number, int)

        return scan_number, pt_number

    def get_hkl(self, row_index):
        """
        Get reflection's miller index
        :param row_index:
        :return:
        """
        assert isinstance(row_index, int)

        m_h = self.get_cell_value(row_index, 2)
        m_k = self.get_cell_value(row_index, 3)
        m_l = self.get_cell_value(row_index, 4)

        assert isinstance(m_h, float)
        assert isinstance(m_k, float)
        assert isinstance(m_l, float)

        return m_h, m_k, m_l

    def get_scan_pt(self, row_number):
        """
        Get Scan and Pt from a row
        :param row_number:
        :return:
        """
        scan_number = self.get_cell_value(row_number, 0)
        pt_number = self.get_cell_value(row_number, 1)

        return scan_number, pt_number

    def is_selected(self, row_index):
        """ Check whether a row is selected.
        :param row_index:
        :return:
        """
        if row_index < 0 or row_index >= self.rowCount():
            raise IndexError('Input row number %d is out of range [0, %d)' % (row_index, self.rowCount()))

        col_index = UB_Peak_Table_Setup.index(('Selected', 'checkbox'))

        return self.get_cell_value(row_index, col_index)

    def setup(self):
        """
        Init setup
        :return:
        """
        self.init_setup(UB_Peak_Table_Setup)
        self._statusColName = 'Selected'

        return

    def set_hkl(self, i_row, hkl, error=None):
        """
        Set HKL to table
        :param i_row:
        :param hkl:
        :param error: error of HKL
        """
        # Check
        assert isinstance(i_row, int)
        assert isinstance(hkl, list)

        i_col_h = UB_Peak_Table_Setup.index(('H', 'float'))
        i_col_k = UB_Peak_Table_Setup.index(('K', 'float'))
        i_col_l = UB_Peak_Table_Setup.index(('L', 'float'))

        self.update_cell_value(i_row, i_col_h, hkl[0])
        self.update_cell_value(i_row, i_col_k, hkl[1])
        self.update_cell_value(i_row, i_col_l, hkl[2])

        if error is not None:
            i_col_error = UB_Peak_Table_Setup.index(('Error', 'float'))
            self.update_cell_value(i_row, i_col_error, error)

        return

    def update_hkl(self, i_row, h, k, l):
        """ Update HKL value
        :param i_row: index of the row to have HKL updated
        :param h:
        :param k:
        :param l:
        """
        assert isinstance(i_row, int)

        self.update_cell_value(i_row, 2, h)
        self.update_cell_value(i_row, 3, k)
        self.update_cell_value(i_row, 4, l)

        return


class ProcessTableWidget(tableBase.NTableWidget):
    """
    Extended table for peaks used to calculate UB matrix
    """
    TableSetup = [('Scan', 'int'),
                  ('Number Pt', 'int'),
                  ('Status', 'str'),
                  ('Total Counts', 'float'),
                  ('Intensity', 'float'),
                  ('Select', 'checkbox'),
                  ('Merged Workspace', 'str'),
                  ('Group Name', 'str')]

    def __init__(self, parent):
        """

        :param parent:
        :return:
        """
        tableBase.NTableWidget.__init__(self, parent)

        return

    def add_new_merged_data(self, exp_number, scan_number, ws_name, ws_group):
        """
        Append a new row with merged data
        :param exp_number:
        :param scan_number:
        :param ws_name:
        :param ws_group:
        :return:
        """
        # check
        assert isinstance(exp_number, int)
        assert isinstance(scan_number, int)
        assert isinstance(ws_name, str)

        # construct a row
        new_row = [scan_number, -1, 'done', -1, -1, False, ws_name, ws_group]
        self.append_row(new_row)

        return

    def append_scans(self, scans):
        """ Append rows for merge in future
        :param scans:
        :return:
        """
        # Check
        assert isinstance(scans, list)

        # set value as default
        num_pt = None
        red_status = 'In Queue'
        counts = None
        intensity = None
        select = False
        merged_ws = None
        group_name = None

        # Append rows
        for scan in scans:
            # set the list
            row_value_list = [scan, num_pt, red_status, counts, intensity, select, merged_ws, group_name]
            status, err = self.append_row(row_value_list)
            if status is False:
                raise RuntimeError(err)

        return

    def get_rows_by_state(self, target_state):
        """ Get the rows' indexes by status' value (state)
        Requirements: target_state is a string
        Guarantees: a list of integers as row indexes are returned for all rows with state as target_state
        :param target_state:
        :return:
        """
        # Get column index
        status_col_index = self._myColumnNameList.index('Status')

        # Check
        assert isinstance(target_state, str)

        # Loop around to check
        return_list = list()
        num_rows = self.rowCount()
        for i_row in xrange(num_rows):
            status_i = self.get_cell_value(i_row, status_col_index)
            if status_i == target_state:
                return_list.append(i_row)
        # END-FOR (i_row)

        return return_list

    def get_merged_status(self, row_number):
        """ Get the status whether it is merged
        :param row_number:
        :return: boolean
        """
        # check
        assert isinstance(row_number, int)
        assert 0 <= row_number < self.rowCount()

        # get value
        merge_status_col_index = self._myColumnNameList.index('status')
        status_str = self.get_cell_value(row_number, merge_status_col_index)

        if status_str.lower() == 'done':
            return True

        return False

    def get_merged_ws_name(self, i_row):
        """
        Get ...
        :param i_row:
        :return:
        """
        j_col_merged = self.TableSetup.index(('Merged Workspace', 'str'))

        return self.get_cell_value(i_row, j_col_merged)

    def get_scan_list(self):
        """ Get list of selected scans to merge from table
        :return: list of 2-tuples (scan number, row number)
        """
        scan_list = list()
        num_rows = self.rowCount()
        col_select_index = self._myColumnNameList.index('Select')
        col_scan_index = self._myColumnNameList.index('Scan')

        for i_row in xrange(num_rows):
            if self.get_cell_value(i_row, col_select_index) is True:
                scan_num = self.get_cell_value(i_row, col_scan_index)
                scan_list.append((scan_num, i_row))

        return scan_list

    def get_scan_number(self, row_number):
        """ Get scan number of a row
        Guarantees: get scan number of a row
        :param row_number:
        :return:
        """
        col_index = ProcessTableWidget.TableSetup.index(('Scan', 'int'))

        return self.get_cell_value(row_number, col_index)

    def setup(self):
        """
        Init setup
        :return:
        """
        self.init_setup(self.TableSetup)
        self._statusColName = 'Select'

        return

    def set_scan_pt(self, scan_no, pt_list):
        """
        :param scan_no:
        :param pt_list:
        :return:
        """
        # Check
        assert isinstance(scan_no, int)

        num_rows = self.rowCount()
        set_done = False
        for i_row in xrange(num_rows):
            tmp_scan_no = self.get_cell_value(i_row, 0)
            if scan_no == tmp_scan_no:
                self.update_cell_value(i_row, 1, len(pt_list))
                set_done = True
                break
        # END-FOR

        if set_done is False:
            return 'Unable to find scan %d in table.' % scan_no

        return ''

    def set_peak_intensity(self, row_number, scan_number, peak_intensity):
        """ Set peak intensity to a row or scan
        Requirement: Either row number or scan number must be given
        Guarantees: peak intensity is set
        :param row_number:
        :param scan_number:
        :param peak_intensity:
        :return:
        """
        # check requirements
        assert row_number is not None and scan_number is not None, 'Row number and scan number cannot be defined ' \
                                                                   'simultaneously.'
        assert row_number is None and scan_number is None, 'Row number and scan number cannot be left empty ' \
                                                           'simultaneously.'
        assert isinstance(peak_intensity, float)

        if row_number is None:
            # row number is not defined.  go through table for scan number
            row_number = -1
            num_rows = self.rowCount()
            scan_col_index = ProcessTableWidget.TableSetup.index(('Scan', 'int'))
            for row_index in xrange(num_rows):
                scan_i = self.get_cell_value(row_index, scan_col_index)
                if scan_i == scan_number:
                    row_number = row_index
                    break
            # check
            if row_number < 0:
                raise RuntimeError('Scan %d cannot be found in the table.' % scan_number)
        # END-IF

        intensity_col_index = ProcessTableWidget.TableSetup.index(('Intensity', 'float'))
        return self.update_cell_value(row_number, intensity_col_index, peak_intensity)

    def set_pt_by_row(self, row_number, pt_list):
        """
        :param row_number:
        :param pt_list:
        :return:
        """
        # Check
        assert isinstance(row_number, int)
        assert isinstance(pt_list, list)

        j_pt = self.get_column_index('Number Pt')
        self.update_cell_value(row_number, j_pt, len(pt_list))

        return ''

    def set_status(self, scan_no, status):
        """
        Set the status for merging scan to QTable
        :param scan_no: scan number
        :param status:
        :return:
        """
        # Check
        assert isinstance(scan_no, int)

        num_rows = self.rowCount()
        set_done = False
        for i_row in xrange(num_rows):
            tmp_scan_no = self.get_cell_value(i_row, 0)
            if scan_no == tmp_scan_no:
                self.update_cell_value(i_row, 2, status)
                set_done = True
                break
        # END-FOR

        if set_done is False:
            return 'Unable to find scan %d in table.' % scan_no

        return ''

    def set_status_by_row(self, row_number, status):
        """
        Set status to a specified row according to row number
        :param row_number:
        :param status:
        :return:
        """
        # Check
        assert isinstance(row_number, int)
        assert isinstance(status, str)

        # Set
        i_status = self.TableSetup.index(('Status', 'str'))
        self.update_cell_value(row_number, i_status, status)

        return

    def set_ws_names(self, scan_num, merged_md_name, ws_group_name):
        """
        Set the output workspace and workspace group's names to QTable
        :param scan_num:
        :param merged_md_name:
        :param ws_group_name:
        :return:
        """
        # Check
        assert isinstance(scan_num, int)
        assert isinstance(merged_md_name, str) or merged_md_name is None
        assert isinstance(ws_group_name, str) or ws_group_name is None

        num_rows = self.rowCount()
        set_done = False
        for i_row in xrange(num_rows):
            tmp_scan_no = self.get_cell_value(i_row, 0)
            if scan_num == tmp_scan_no:
                if merged_md_name is not None:
                    self.update_cell_value(i_row, 3, merged_md_name)
                if ws_group_name is not None:
                    self.update_cell_value(i_row, 4, ws_group_name)
                set_done = True
                break
        # END-FOR

        if set_done is False:
            return 'Unable to find scan %d in table.' % scan_num

        return

    def set_ws_names_by_row(self, row_number, merged_md_name, ws_group_name):
        """
        Set the workspaces' names to this table
        :param row_number:
        :param merged_md_name:
        :param ws_group_name:
        :return:
        """
        # Check
        assert isinstance(row_number, int)
        assert isinstance(merged_md_name, str) or merged_md_name is None
        assert isinstance(ws_group_name, str) or ws_group_name is None

        j_ws_name = self.get_column_index('Merged Workspace')
        j_group_name = self.get_column_index('Group Name')

        if merged_md_name is not None:
            self.update_cell_value(row_number, j_ws_name, merged_md_name)
        if ws_group_name is not None:
            self.update_cell_value(row_number, j_group_name, ws_group_name)

        return


class ScanSurveyTable(tableBase.NTableWidget):
    """
    Extended table widget for peak integration
    """
    Table_Setup = [('Scan', 'int'),
                   ('Max Counts Pt', 'int'),
                   ('Max Counts', 'float'),
                   ('H', 'float'),
                   ('K', 'float'),
                   ('L', 'float'),
                   ('Q-range', 'float'),
                   ('Selected', 'checkbox')]

    def __init__(self, parent):
        """
        :param parent:
        """
        tableBase.NTableWidget.__init__(self, parent)

        self._myScanSummaryList = list()

        self._currStartScan = 0
        self._currEndScan = sys.maxint
        self._currMinCounts = 0.
        self._currMaxCounts = sys.float_info.max

        return

    def filter_and_sort(self, start_scan, end_scan, min_counts, max_counts,
                        sort_by_column, sort_order):
        """
        Filter the survey table and sort
        Note: it might not be efficient here because the table will be refreshed twice
        :param start_scan:
        :param end_scan:
        :param min_counts:
        :param max_counts:
        :param sort_by_column:
        :param sort_order: 0 for ascending, 1 for descending
        :return:
        """
        # check
        assert isinstance(start_scan, int) and isinstance(end_scan, int) and end_scan >= start_scan
        assert isinstance(min_counts, float) and isinstance(max_counts, float) and min_counts < max_counts
        assert isinstance(sort_by_column, str), \
            'sort_by_column requires a string but not %s.' % str(type(sort_by_column))
        assert isinstance(sort_order, int), \
            'sort_order requires an integer but not %s.' % str(type(sort_order))

        # get column index to sort
        col_index = self.get_column_index(column_name=sort_by_column)

        # filter on the back end row contents list first
        self.filter_rows(start_scan, end_scan, min_counts, max_counts)

        # order
        self.sort_by_column(col_index, sort_order)

        return

    def filter_rows(self, start_scan, end_scan, min_counts, max_counts):
        """
        Filter by scan number, detector counts on self._myScanSummaryList
        and reset the table via the latest result
        :param start_scan:
        :param end_scan:
        :param min_counts:
        :param max_counts:
        :return:
        """
        # check whether it can be skipped
        if start_scan == self._currStartScan and end_scan == self._currEndScan \
                and min_counts == self._currMinCounts and max_counts == self._currMaxCounts:
            # same filter set up, return
            return

        # clear the table
        self.remove_all_rows()

        # go through all rows in the original list and then reconstruct
        for index in xrange(len(self._myScanSummaryList)):
            sum_item = self._myScanSummaryList[index]
            # check
            assert isinstance(sum_item, list)
            assert len(sum_item) == len(self._myColumnNameList) - 1
            # check with filters: original order is counts, scan, Pt., ...
            scan_number = sum_item[1]
            if scan_number < start_scan or scan_number > end_scan:
                # print 'Scan %d is out of range.' % scan_number
                continue
            counts = sum_item[0]
            if counts < min_counts or counts > max_counts:
                # print 'Scan %d Counts %f is out of range.' % (scan_number, counts)
                continue

            # modify for appending to table
            row_items = sum_item[:]
            counts = row_items.pop(0)
            row_items.insert(2, counts)
            row_items.append(False)

            # append to table
            self.append_row(row_items)
        # END-FOR (index)

        # Update
        self._currStartScan = start_scan
        self._currEndScan = end_scan
        self._currMinCounts = min_counts
        self._currMaxCounts = max_counts

        return

    def get_scan_numbers(self, row_index_list):
        """
        Get scan numbers with specified rows
        :param row_index_list:
        :return:
        """
        scan_list = list()
        scan_col_index = self.Table_Setup.index(('Scan', 'int'))
        for row_index in row_index_list:
            scan_number_i = self.get_cell_value(row_index, scan_col_index)
            scan_list.append(scan_number_i)
        scan_list.sort()

        return scan_list

    def get_selected_run_surveyed(self):
        """
        Purpose: Get selected pt number and run number that is set as selected
        Requirements: there must be one and only one run that is selected
        Guarantees: a 2-tuple for integer for return as scan number and Pt. number
        :return: a 2-tuple of integer
        """
        # get the selected row indexes and check
        row_index_list = self.get_selected_rows(True)
        assert len(row_index_list) == 1, 'There must be exactly one run that is selected. Now' \
                                         'there are %d runs that are selected' % len(row_index_list)

        # get scan and pt
        row_index = row_index_list[0]
        scan_number = self.get_cell_value(row_index, 0)
        pt_number = self.get_cell_value(row_index, 1)

        return scan_number, pt_number

    def show_reflections(self, num_rows):
        """
        :param num_rows:
        :return:
        """
        assert isinstance(num_rows, int)
        assert num_rows > 0
        assert len(self._myScanSummaryList) > 0

        for i_ref in xrange(min(num_rows, len(self._myScanSummaryList))):
            # get counts
            scan_summary = self._myScanSummaryList[i_ref]
            # check
            assert isinstance(scan_summary, list)
            assert len(scan_summary) == len(self._myColumnNameList) - 1
            # modify for appending to table
            row_items = scan_summary[:]
            max_count = row_items.pop(0)
            row_items.insert(2, max_count)
            row_items.append(False)
            # append
            self.append_row(row_items)
        # END-FOR

        return

    def set_survey_result(self, scan_summary_list):
        """

        :param scan_summary_list:
        :return:
        """
        # check
        assert isinstance(scan_summary_list, list)

        # Sort and set to class variable
        scan_summary_list.sort(reverse=True)
        self._myScanSummaryList = scan_summary_list

        return

    def setup(self):
        """
        Init setup
        :return:
        """
        self.init_setup(ScanSurveyTable.Table_Setup)
        self.set_status_column_name('Selected')

        return

    def reset(self):
        """ Reset the inner survey summary table
        :return:
        """
        self._myScanSummaryList = list()
