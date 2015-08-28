.. algorithm::

.. summary::

.. alias::

.. properties::

Description
-----------

Calculates phonon densities of states, Raman and IR spectrum from the
output of CASTEP code obtained in the form of *.phonon* and *.castep* files.

The PeakWidth property may be passed a function containg the variable "energy"
(e.g. *0.1*energy*) to set the FWHM of the peak as a function of the energy
(centre point of the peak). This can be useful for comparison with experimental
data by allowing the peak width to change according to the resolution of the
instrument.

If the IonTable spectrum type is used then the output workspace will be
a table workspace containing each ion that is present in a *.phonon* file.

If the BondTable spectrum type is used then the output workspace will be
a table workspace containing details of the bonds defined in the *.castep*
file.

S(Q, w) Mode
~~~~~~~~~~~~

In TOSCA S(Q, w) mode the :math:`S(Q, \omega)` is calculated as follows [1]_:

.. math::

  S(Q, n\omega) = \sum_{j}
                  \sigma_{j}
                  \left<
                  \frac{(Q \cdot U_{j})^{2n}}{n!}
                  exp\left(-(Q \cdot U_{j})^{2}\right)
                  \right>

where :math:`j` denotes an atom in the sample, :math:`\sigma` is the incoherent
scattering cross section of atom :math:`j` and :math:`n` is the number of
overtones (with 1 being the fundamental frequencies only).

The displacement of an atom :math:`j`, :math:`U_{j}`, is given by the following
equation:

.. math::

  U^{2} = \frac{16.759}{\mu n \nu}

where :math:`\mu` is the mass of atom :math:`j` and :math:`\nu` is the frequency
of the vibration of atom :math:`j` in :math:`cm^{-1}`.

The instrument dependant momentum distribution, :math:`Q`, is given by the
following equation for the TOSCA instrument:

.. math::

  Q^{2} \approx \frac{1}{2} \left(\frac{\nu}{8.066}\right)

Usage
-----

.. include:: ../usagedata-note.txt

**Example - loading data from phonon & castep files:**

.. testcode:: ExSimulatedDensityOfStatesSimple

    # Loading the same data from a castep and phonon file
    phonon_ws = SimulatedDensityOfStates(PHONONFile='squaricn.phonon')
    castep_ws = SimulatedDensityOfStates(CASTEPFile='squaricn.castep')

    print CheckWorkspacesMatch(phonon_ws, castep_ws)

Output:

.. testoutput:: ExSimulatedDensityOfStatesSimple

    Success!

**Example - loading partial contributions of ions:**

.. testcode:: ExSimulatedDensityOfStatesPartial

    squaricn = SimulatedDensityOfStates(PHONONFile='squaricn.phonon',
                                        Ions=['H', 'C', 'O'])

    for name in squaricn.getNames():
      print name

Output:

.. testoutput:: ExSimulatedDensityOfStatesPartial

    squaricn_H
    squaricn_C
    squaricn_O

**Example - loading summed partial contributions of ions:**

.. testcode:: ExSimulatedDensityOfStatesPartialSummed

    sum_ws = SimulatedDensityOfStates(PHONONFile='squaricn.phonon',
                                      Ions=['H', 'C', 'O'],
                                      SumContributions=True)
    total_ws = SimulatedDensityOfStates(PHONONFile='squaricn.phonon')

    print CheckWorkspacesMatch(total_ws, sum_ws, Tolerance=1e-12)

Output:

.. testoutput:: ExSimulatedDensityOfStatesPartialSummed

    Success!

**Example - Getting the list of ions in a phonon file:**

.. testcode:: ExSimulatedDensityOfStatesIonTable

    ion_ws = SimulatedDensityOfStates(PHONONFile='squaricn.phonon',
                                      SpectrumType='IonTable')
    print ','.join(ion_ws.column('Species'))

Output:

.. testoutput:: ExSimulatedDensityOfStatesIonTable

    H, H, H, H, C, C, C, C, C, C, C, C, O, O, O, O, O, O, O, O

References
----------

.. [1] D. Champion, J. Tomkinson, and G. Kearley. “a-CLIMAX: a new INS analysis tool”. `Applied Physics A: Materials Science & Processing 74 (Dec. 2002), s1302–s1304 <http://dx.doi.org/10.1007/s003390101223>`__

.. categories::

.. sourcelink::
  :cpp: None
  :h: None
