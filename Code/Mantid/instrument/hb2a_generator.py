# Version 2.0:
# 1. Read Gap file
# 3. Improve usability
# TODO
# 2. Output in a proper manner
# END-TODO

# Version 1.0
# Use 'rotangle' in sample log to control 2theta_0
# The relative positions of detectors then can be fixed. 

import sys

class HB2AGenerator:
    """ A class to generate an HB2A instrument file
    """
    def __init__(self):
        """ Init
        """
        self._srcXMLFile = None  # source instrument file
        self._gapFile = None  # text file show the gap instrumetn

        return

    def importDetInfo(self, numdet, detgaplist, detradius):
        """  Import detectors' gap information
        """
        # Check
        if numdet != len(detgaplist):
            raise NotImplementedError("Number of detector must be same as number of gaps.")

        self._numDets = numdet
        self._detPosList = []

        pos = 0.
        for igap in xrange(numdet):
            gap = detgaplist[igap]
            pos += gap
            self._detPosList.append( (pos, detradius) )
        # ENDFOR

        for ipos in xrange(len(self._detPosList)):
            print "%d:  %.5f, %.5f" % (ipos, self._detPosList[ipos][0], self._detPosList[ipos][1])

        return


    def setSourceDistance(self, srcdist):
        """ Set radius of detecrtors
        """
        self._srcDistance = srcdist

        return

    def generateInstrumentFile(self, ofilename):
        """ 
        """
        # Generate a string for xml file
        header = self._genHeader()
        default = self._genDefaultInfo()
        source = self._genSource()
        sample = self._genSample()
        detectors = self._genDetectors()
        tail = self._genTail()
        
        # print header, default, source, sample, detectors, tail
        # Export to file
        ofile = open(ofilename, 'w')
        ofile.write(header)
        ofile.write(default)
        ofile.write(source)
        ofile.write(sample)
        ofile.write(detectors)
        ofile.write(tail)
        ofile.close()

        return


    def loadInstrumentFile(self, filename):
        """ Load source instrument file 
        """
        raise NotImplementedError("Not sure what to do with this!")

        return


    def _genHeader(self):
        """ Header of the file
        """
        header =  '<?xml version=\"1.0\" encoding="utf-8"?>\n'
        header += '<!-- For help on the notation used to specify an Instrument Definition File see http://www.mantidproject.org/IDF -->\n'
        header += '<instrument  xmlns="http://www.mantidproject.org/IDF/1.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" '
        header += 'xsi:schemaLocation="http://www.mantidproject.org/IDF/1.0 http://schema.mantidproject.org/IDF/1.0/IDFSchema.xsd" ' 
        header += 'name="HB2A" valid-from="1900-01-31 23:59:59" valid-to="2100-01-31 23:59:59" last-modified="2014-12-28 14:18:50">\n'

        return header


    def _genDefaultInfo(self):
        """
        """          
        default = "" 
        default += '  <defaults>                                                                   \n'
        default += '    <length unit="meter" />                                                    \n'
        default += '    <angle unit="degree" />                                                    \n'
        default += '    <reference-frame>                                                          \n'
        default += '      <!-- The z-axis is set parallel to and in the direction of the beam. the \n'
        default += '             y-axis points up and the coordinate system is right handed. -->   \n'
        default += '      <along-beam axis="z" />                                                  \n'
        default += '      <pointing-up axis="y" />                                                 \n'
        default += '      <handedness val="right" />                                               \n'
        default += '    </reference-frame>                                                         \n'
        default += '  </defaults>                                                                  \n'

        return default


    def _genSource(self):
        """
        """
        text = ""
        text += '  <!-- Source position -->                  \n'
        text += '  <component type="moderator">              \n'
        text += '    <location z="-%.5f" />                  \n' % (self._srcDistance)
        text += '  </component>                              \n'
        text += '  <type name="moderator" is="Source"></type>\n'

        return text


    def _genSample(self):
        """
        """
        text = ""

        text += '  <!-- Sample position -->                      \n'
        text += '  <component type="sample-position">            \n'
        text += '    <location y="0.0" x="0.0" z="0.0" />        \n'
        text += '  </component>                                  \n'
        text += '  <type name="sample-position" is="SamplePos" />\n'

        return text


    def _genDetectors(self):
        """
        """
        rotangle = 15.0

        text = ""

        # General information
        text += '  <!-- Detector list def -->                    \n'
        text += '  <idlist idname="detectors">                   \n'
        text += '    <id start="1" end="44" />                   \n'
        text += '  </idlist>                                     \n'
        text += '  <component type="detectors" idlist="detectors">\n'
        text += '    <location />                                 \n'
        text += '  </component>                                   \n'

        # Detector banks/all detectors; containing component 'bank_uniq'
        text += '  <!-- Detector Banks -->                       \n'
        text += '  <type name="detectors">                       \n'
        text += '    <component type="bank_uniq">                \n'
        text += '    <location>                                  \n'
        text += '      <parameter name="r-position">             \n'
        text += '        <value val="0"/>                        \n'
        text += '      </parameter>                              \n'
        text += '      <parameter name="t-position">             \n'
        text += '        <logfile id="rotangle"  eq="%f+value"/> \n' % (rotangle)  
        text += '        <!--  <value val="90"/> -->             \n'
        text += '      </parameter>                              \n'
        text += '      <parameter name="p-position">             \n'
        text += '        <value val="0"/>                        \n'
        text += '      </parameter>                              \n'
        text += '      <parameter name="rotx">                   \n'
        text += '        <value val="0"/>                        \n'
        text += '      </parameter>                              \n'
        text += '      <parameter name="roty">                   \n'
        text += '        <logfile id="rotangle"  eq="0.0+value"/>\n'
        text += '        <!--   <value val="90"/> -->            \n'
        text += '      </parameter>                              \n'
        text += '      <parameter name="rotz">                   \n'
        text += '        <value val="0"/>                        \n'
        text += '      </parameter>                              \n'
        text += '    </location>                                 \n'
        text += '    </component>                                \n'
        text += '  </type>                                       \n'

        # Define bank unique, containing standard_tube
        text += '  <!-- Definition of the unique existent bank (made of tubes) -->\n'
        text += '  <type name="bank_uniq">                                        \n'
        text += '    <component type="standard_tube">                             \n'
       
        for itube in xrange(1, 45):
            #tt = float(itube-1)*2.0
            tt = self._detPosList[itube-1][0]
            r  = self._detPosList[itube-1][1]
            text += '      <location r="%f" t="%f" name="tube_%d" />     \n' % (r, tt, itube)
        text += '    </component>                                                 \n'
        text += '  </type>                                                        \n'

        # Define standard tube; containing standard_pixel
        text += '  <!-- Definition of standard_tube -->                 \n'
        text += '  <type name="standard_tube" outline="yes">            \n'
        text += '    <component type="standard_pixel">                  \n'
        text += '      <location y="0.00" />                            \n'
        text += '    </component>                                       \n'
        text += '  </type>                                              \n'

        # Define standard pixel, the unit component
        text += '  <type name="standard_pixel" is="detector">           \n'
        text += '    <cylinder id="shape">                              \n'
        text += '      <centre-of-bottom-base x="0.0" y="0.0" z="0.0" />\n'
        text += '      <axis x="0.0" y="1.0" z="0.0" />                 \n'
        text += '      <radius val="0.00127" />                         \n'
        text += '      <height val=".0114341328125" />                  \n'
        text += '    </cylinder>                                        \n'
        text += '    <algebra val="shape" />                            \n'
        text += '  </type>                                              \n'

        return text


    def _genTail(self):
        """
        """
        tail = '</instrument>'

        return tail


def parseGapFile(gapfilename):
    """ Parse a standard incomplete column gap file
    Rule: always load the last column
    """
    # Import
    gfile = open(gapfilename, "r")
    lines = gfile.readlines()
    gfile.close()

    # Convert to 
    detgaplist = []
    for line in lines:
        cline = line.strip()
        if len(cline) == 0:
            continue

        terms = cline.split()
        gap = float(terms[-1])

        detgaplist.append(gap)
    # ENDFOR

    return detgaplist


def main(argv):
    """ Main
    """
    if len(argv) < 3:
        print "Input: %s [Gap file name] [Output IDF name] [Source Pos] [Det Radius]" % (argv[0])
        return

    gapfilename = argv[1]
    outidfname = argv[2]

    if len(argv) < 5:
        print "[Warning] Source position and detector radius are not given.  Source position is set to 2 meters; Detectors' radius are set to 2 meters. "
        sourcedistance = 2.
        detrad = 2.
    else:
        sourcedistance = float(argv[3])
        detrad = float(argv[4])

    detgaplist = parseGapFile(gapfilename)

    generator = HB2AGenerator()
    generator.importDetInfo(len(detgaplist), detgaplist, detrad)
    generator.setSourceDistance(sourcedistance)
    generator.generateInstrumentFile(outidfname)

    return



if __name__ == "__main__":
   main(sys.argv) 
