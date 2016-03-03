/* -*- c++ -*-
 *
 *  Created on: 17.03.2010
 *      Author: Alexander Bürger
 */

#ifndef PARTICLERANGE_H_
#define PARTICLERANGE_H_

#include <string>
#include <vector>

//! Class for reading and interpolating range data from 'zrange'.
class ParticleRange {
public:
    //! Initialize values of Emin, Emax, Estep.
    ParticleRange();

    //! Read an output file of zrange.
    /*! These files have three columns: particle energy [MeV], range
     *  [um], and range uncertainty [um]. The zrange data are
     *  interpolated in Estep steps between Emin and Emax.
     */
    void Read(const std::string& filename /*!< The name of the file to read. */);

    //! Get the range for a given particle energy.
    /*! If the energy is too high, 1m is returned. If it is to low, 0
     * is returned.
     * 
     * \return The range for this particle energy.
     */
    float GetRange(int energy /*!< Particle energy in keV.*/) const;

private:
    //! Minimum energy for the interpolated values.
    int Emin;

    //! Energy step for the interpolation.
    int Estep;

    //! Interpolated values, in steps of Estep starting from Emin.
    /*! This list may not go up to Emax if the zrange output file does
     *  not contain enough values.
     */
    std::vector<float> values;
};

#endif /* PARTICLERANGE_H_ */
