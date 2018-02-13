#include "Event.h"
#include "Histogram1D.h"
#include "Histogram2D.h"
#include "IOPrintf.h"
#include "OfflineSorting.h"
#include "Parameters.h"
#include "ParticleRange.h"
#include "SiriusRoutine.h"

#include <iostream>
#include <cmath>
#include <cstdlib>
#include <sstream>


#define NDEBUG 1
#include "debug.h"

//! must be 0 to disable making DE:E matrices for each strip of all detectors
// Makes plots of type m_e_de_b0f0...b7f7
#define MAKE_INDIVIDUAL_E_DE_PLOTS 1

//! must be 0 to disable making time evolution plots
// This is to check, for example, if the elastic peak or a gamma peak 
// (which is more likely) moves during the experiment
#define MAKE_TIME_EVOLUTION_PLOTS 1

//! must be 0 to disable making CACTUS time-energy plots
// Makes plots of type m_nai_e_t_0...27
#define MAKE_CACTUS_TIME_ENERGY_PLOTS 1

//! must be 0 to disable checking particles
// If this is on, it checks that 
#define APPLY_PARTICLE_GATE 1

//! must be 1 to enable looking for fission timing parameters and so on
// If this is on, it checks that 
#define USE_FISSION_PARAMETERS 1
 
 // ########################################################################

 //! User sorting routine.

 class UserXY : public SiriusRoutine {
 public:
     UserXY();
     bool Sort(const Event& event);
     void CreateSpectra();
     bool Command(const std::string& cmd);
     void GiveNames();
     bool CheckNaIpromptGate (float);
     bool CheckNaIbgGate (float);

#if USE_FISSION_PARAMETERS>0
     bool CheckPPACpromptGate (float);
     bool CheckPPACbgGate (float);
#endif /* USE_FISSION_PARAMETERS */
     bool IsPPACChannel (int);
     int GetPPACChannel (int);


     
 private:
     Histogram2Dp m_back, m_front, m_e_de_strip[8], m_e_de, m_e_de_thick, 
                  m_e_de_fiss, m_e_de_nofiss, m_e_de_fiss_promptFiss, m_e_de_fiss_bg;
 
 #if defined(MAKE_INDIVIDUAL_E_DE_PLOTS) && (MAKE_INDIVIDUAL_E_DE_PLOTS>0)
     //8x8 arrays because of the 8 front x 8 backdetectors of SiRi
     Histogram2Dp m_e_de_individual[8][8];
     Histogram1Dp h_ede_individual[8][8];
 #endif /* MAKE_INDIVIDUAL_E_DE_PLOTS */

     Histogram2Dp m_nai_t, m_nai_e;
    //m_nai_t er dermed en matrise med tid på x-aksen og NaI-detektornummer på y-aksen
    //m_nai_e er matrise med gammaenergi på x-aksen og NaI-detektornummer på y-aksen
     Histogram2Dp m_alfna, m_alfna_bg;
    //ALFNA og ALFNABAKGRUNN defineres
     Histogram2Dp m_alfna_nofiss,   m_alfna_fiss, m_alfna_fiss_promptFiss;
     Histogram2Dp m_alfna_bg_nofiss,              m_alfna_bg_fiss_promptFiss, m_alfna_bg_fiss_bg;

     Histogram1Dp h_na_n, h_thick, h_ede, h_ede_r[8], h_ex_r[8], h_de_n, h_e_n;
     Histogram1Dp h_ex, h_ex_nofiss, h_ex_fiss_promptFiss, h_ex_fiss_bg, h_ex_fiss;
   
 #if defined(MAKE_CACTUS_TIME_ENERGY_PLOTS) && (MAKE_CACTUS_TIME_ENERGY_PLOTS>0)
     Histogram2Dp m_nai_e_t[28], m_nai_e_t_all, m_nai_e_t_c,         // CACTUS_Time_Energy_Plots
                  m_siri_e_t[8], m_siri_e_t_all, m_siri_e_t_c,       // SIRI_Time_Energy_Plots
                  m_ppac_e_t[4], m_ppac_e_t_all, m_ppac_e_t_c;      // PPAC_Time_Energy_Plots
     // for some reason needed, otherwise histograms in the following line are not ploted
     Histogram2Dp m_dummy1,m_dummy2;
     Histogram2Dp m_nai_e_t_all_fiss_promptFiss, m_nai_e_t_c_fiss_promptFiss;
     Histogram2Dp m_nai_e_t_all_fiss_bg, m_nai_e_t_c_fiss_bg;

#endif /* MAKE_CACTUS_TIME_ENERGY_PLOTS */      

 #if defined(MAKE_TIME_EVOLUTION_PLOTS) && (MAKE_TIME_EVOLUTION_PLOTS>0)
     Histogram2Dp m_nai_t_evol[28], m_nai_e_evol[28];
     Histogram2Dp m_e_evol[8], m_de_evol[8][8], m_ede_evol[8], m_ex_evol;
 #endif /* MAKE_TIME_EVOLUTION_PLOTS */
  
 private:
     //! Correction of CACTUS time for CACTUS energy.
     Parameter tnai_corr_enai;
   
     //! Correction of CACTUS time for SiRi back detector energy.
     Parameter tnai_corr_esi;

#if USE_FISSION_PARAMETERS>0
     //! Correction of CACTUS time for SiRi back detector energy.
     Parameter tppac_corr_esi;
#endif /* USE_FISSION_PARAMETERS */ 

     //! Polynomials to calculate excitation energy from SiRi energy (E+DE).
     /*! Contains 8*3 coefficients. */
     Parameter ex_from_ede;
   
     //! Polynomials to make an empirical correction to the calculated excitation energy.
     /*! Contains 8*2 coefficients. */
     Parameter ex_corr_exp;
 
     //! Two rectangles to cut away SiRi noise/electrons.
     /*! Contains E-minimum 1, DE-minimum 1, E-minimum 2, DE-minimum 2. */
     Parameter ede_rect;
 
     //! Thickness centroid and second-order polynomial for SiRi E dependent thickness gate width.
     /*! Contains centroid, constant width, E-scaled width. */
     Parameter thick_range;

     //! The particle range data from zrange.
     ParticleRange particlerange;


    // Struct to hold the time gates
    struct TimeGate
    {
    float lower_prompt;
    float higher_prompt;
    float lower_bg;
    float higher_bg;
    };

     //! Time gates for the NaI detectors, e.g. for making the ALFNA matrices 
     Parameter nai_time_cuts;
    TimeGate NaITimeCut;

#if USE_FISSION_PARAMETERS>0
     //! Time gates for the ppacs.
     Parameter ppac_time_cuts;
     TimeGate PPACTimeCut;
     
     //! Time gates for the ppacs.
     Parameter fission_excitation_energy_min;

     //! Total efficiency of the PPACs in 4Pi
     Parameter ppac_efficiency;
#endif /* USE_FISSION_PARAMETERS */     

     //! Channel number of the PPACs
     // In case no PPACs are used, just define the channels as "-1" in the batch file,
     // then, as the channel id's are positive numbers, no channel will be identified as PPAC
     Parameter channel_PPAC;



     //! Apply energy corrections to CACTUS time.
     /*! \return corrected CACTUS time. */
     float tNaI(float t,    /*!< Uncorrected CACTUS time. */
                float Enai, /*!< Calibrated CACTUS energy in keV. */
                float Esi   /*!< Calibrated SiRi back energy in keV. */);
 
     //Declaration of function to correct PPAC time
     float tPpac(float t, /*!<Uncorrected CACTUS time.*/
                 float Esi /*!Calibrated SiRi back energy in keV.*/);
 
     float range(float E /*!< particle energy in keV */)
        { return particlerange.GetRange( (int)E ); }

};
 
// ########################################################################

bool UserXY::Command(const std::string& cmd)
{
    std::istringstream icmd(cmd.c_str());

    std::string name;
    icmd >> name;
    
    if( name == "rangefile" ) {
        std::string filename;
        icmd >> filename;
        particlerange.Read( filename );
        return true;
    }
    return SiriusRoutine::Command(cmd);
}

// ########################################################################
 
 UserXY::UserXY():
    tnai_corr_enai ( GetParameters(), "tnai_corr_enai", 4   ),
    tnai_corr_esi  ( GetParameters(), "tnai_corr_esi", 4    ),
    ex_from_ede    ( GetParameters(), "ex_from_ede", 8*3    ),
    ex_corr_exp    ( GetParameters(), "ex_corr_exp", 8*2    ),
    ede_rect       ( GetParameters(), "ede_rect", 4         ),
    thick_range    ( GetParameters(), "thick_range", 3      ),
    nai_time_cuts  ( GetParameters(), "nai_time_cuts", 2*2  ),
    channel_PPAC   ( GetParameters(), "channel_PPAC", 4     )
#if USE_FISSION_PARAMETERS>0
    , ppac_time_cuts ( GetParameters(), "ppac_time_cuts", 2*2 ),
    tppac_corr_esi ( GetParameters(), "tppac_corr_esi", 4   ),
    fission_excitation_energy_min ( GetParameters(), "fission_excitation_energy_min", 1 ),
    ppac_efficiency ( GetParameters(), "ppac_efficiency", 1 )
#endif /* USE_FISSION_PARAMETERS */
 {
     ede_rect.Set( "500 250 30 500" );
     thick_range.Set( "130  13 0" );
}


// ########################################################################
 
 void UserXY::CreateSpectra()
 {
//    const int max_e = 20000, max_de = 10000;
    //Changed the maximum energy (x axis) and maximum delta energy (y axis) into something sensible for this plot
    //Don´t have to zoome like crazy everytime I make a particle spectrum :)
    const int max_e = 17000, max_de = 6000;
     
     // [a.u.], becuase it's timing bins, not ns
     // m_nai_t = Mat( "m_nai_t", "t(NaI) matrix", 500, 0,  500, "? [a.u.]",     32,0,32, "det. id.");
     m_nai_e = Mat( "m_nai_e", "E(NaI) matrix", 2000, 0, 15000, "E(NaI) [keV]", 32,0,32, "det. id.");


 // #if defined(MAKE_CACTUS_TIME_ENERGY_PLOTS) && (MAKE_CACTUS_TIME_ENERGY_PLOTS>0)
 //     // maximum energy of the gammadetectors (x axis) is 12000 keV
 //     // HOWEVER, this maximum value is not the same for all plots 
 //     // - for some it has been sat individually (not as a variable)
 //     const int max_enai = 12000;       
    
 //     for(int n=0; n<28; ++n ) {
 //     m_nai_e_t[n] = Mat( ioprintf("m_nai_e_t_%02d", n), ioprintf("t : E NaI %d", n),
 //                            500, 0, max_enai, "E(NaI) [keV]", 500, 0, 500, "t(NaI) [a.u.]" );
 //     // m_nai_e_t_fiss[n] = Mat( ioprintf("m_nai_e_t_fiss_%02d", n), ioprintf("t : E NaI %d", n, "coinc. PPAC"),
 //     //                        500, 0, max_enai, "E(NaI) [keV]", 500, 0, 500, "t(NaI) [a.u.]" );
 //     // m_nai_e_t_fiss_bg[n] = Mat( ioprintf("m_nai_e_t_fiss_bg_%02d", n), ioprintf("t : E NaI %d", n, "coinc. PPAC bg."),
 //     //                        500, 0, max_enai, "E(NaI) [keV]", 500, 0, 500, "t(NaI) [a.u.]" );
 //     }

 //     m_nai_e_t_all = Mat( "m_nai_e_t", "t : E NaI all together",
 //                         500, 0, max_enai, "E(NaI) [keV]", 500, 0, 500, "t(NaI) [a.u.]" );
 // #endif /* MAKE_CACTUS_TIME_ENERGY_PLOTS */

 // #if defined(MAKE_TIME_EVOLUTION_PLOTS) && (MAKE_TIME_EVOLUTION_PLOTS>0)
 //     // time evolution plots
 //     const int MT = 4*24*3600;
 //     const int NT = 4*24;
 //     for(int n=0; n<28; ++n ) {
 //         m_nai_t_evol[n] = Mat( ioprintf("m_nai_t_evol_%02d", n), ioprintf("time : t NaI %d", n),
 //                             500, 0, 500, "t(NaI) [a.u.]", NT, 0, MT, "wall clock time [s]" );
 //         m_nai_e_evol[n] = Mat( ioprintf("m_nai_e_evol_%02d", n), ioprintf("time : e NaI %d", n),
 //                             500, -1000, max_enai-1000, "e(NaI) [keV]", NT, 0, MT, "wall clock time [s]" );
 //     }
     
 // #endif /* MAKE_TIME_EVOLUTION_PLOTS */
}

// ########################################################################

static float _rando = 0;
static float calib(unsigned int raw, float gain, float shift)
{
    return shift + (raw+_rando) * gain;
}

// ########################################################################

float UserXY::tNaI(float t, float Enai, float Esi)
{
    const float c = tnai_corr_enai[0] + tnai_corr_enai[1]/(Enai+tnai_corr_enai[2]) + tnai_corr_enai[3]*Enai;
    const float d = tnai_corr_esi [0] + tnai_corr_esi [1]/(Esi +tnai_corr_esi [2]) + tnai_corr_esi [3]*Esi;
     return t - c - d;
 }
 

// ########################################################################
#if USE_FISSION_PARAMETERS>0
//Function to correct the time of the PPAC
float UserXY::tPpac(float t, float Esi)
{  
    const float a = tppac_corr_esi [0] + tppac_corr_esi [1]/(Esi +tppac_corr_esi [2]) + tppac_corr_esi [3]*Esi;
    return t - a;
}
#endif /* USE_FISSION_PARAMETERS */
// ########################################################################
//Function to give names to the various time cuts
void UserXY::GiveNames ()
 {
NaITimeCut.lower_prompt = nai_time_cuts[0];
NaITimeCut.higher_prompt = nai_time_cuts[1];
NaITimeCut.lower_bg = nai_time_cuts[2];
NaITimeCut.higher_bg = nai_time_cuts[3];

#if USE_FISSION_PARAMETERS>0
PPACTimeCut.lower_prompt = nai_time_cuts[0];
PPACTimeCut.higher_prompt = nai_time_cuts[1];
PPACTimeCut.lower_bg = nai_time_cuts[2];
PPACTimeCut.higher_bg = nai_time_cuts[3];
#endif /* USE_FISSION_PARAMETERS */
 }
 // ########################################################################
 // Check if a variable is in the prompt/bg gate of NaI/PPACs
bool UserXY::CheckNaIpromptGate(float floattime)
{ 
bool NaIpromptGate = floattime > NaITimeCut.lower_prompt && floattime < NaITimeCut.higher_prompt;
return NaIpromptGate;
}
 // #########
bool UserXY::CheckNaIbgGate(float floattime)
{ 
bool NaIbgGate = floattime > NaITimeCut.lower_bg && floattime < NaITimeCut.higher_bg;
return NaIbgGate;
}
 // #########
#if USE_FISSION_PARAMETERS>0
 bool UserXY::CheckPPACpromptGate(float floattime)
    { 
    bool PPACpromptGate = floattime > PPACTimeCut.lower_prompt && floattime < PPACTimeCut.higher_prompt;
    return PPACpromptGate;
    }
     // #########
 bool UserXY::CheckPPACbgGate(float floattime)
    { 
    bool PPACbgGate = floattime > PPACTimeCut.lower_bg && floattime < PPACTimeCut.higher_bg;
    return PPACbgGate;
    }
 #endif /* USE_FISSION_PARAMETERS */
// ########################################################################
// Checks whether a channel is a PPAC channel
 bool UserXY::IsPPACChannel(int channel)
    { 
    bool PPACChannel =    channel == channel_PPAC[0] || channel == channel_PPAC[1] 
                       || channel == channel_PPAC[2] || channel == channel_PPAC[3];
    return PPACChannel;
    }

// Get which PPAC channel
 int UserXY::GetPPACChannel(int nai_channel)
    {
        for (int i = 0 ; i < 4 ; ++i)
            if (nai_channel == channel_PPAC[i])
                return i;
        std::cerr << "NaI-channel  " << nai_channel << "is NOT a PPAC!!!" << std::endl;
        return -1;
    }
// ########################################################################



bool UserXY::Sort(const Event& event)
{
    GiveNames(); // give names to the time cut parameters 

    // begin the sorting

    _rando = drand48() - 0.5;
     
    // ......................................................//
    // SORTING ROUTINE FOR GAMMA-RAY Spectra only 
    // (need to comment out all the rest below until the next funciton then...)
    // ......................................................//
    const int nChannelsGamma = 32; // Number of channels of the gamma-deteactors
     for( int i=0; i<nChannelsGamma; i++ ) {
        // for( int i=0; i<event.n_na; i++ ) {
        const int id = event.na[i].chn;

        if( event.na[i].adc <= 0 )
            continue;

        const float na_e = calib( (int)event.na[i].adc, gain_na[id], shift_na[id] );
        const int   na_e_int = (int)na_e;
        // const float na_t = calib( (int)event.na[i].tdc/8, gain_tna[id], shift_tna[id] ); 
        // const int   na_t_int = (int)na_t;

        m_nai_e->Fill( na_e_int, id );

        // m_nai_e_t[id] ->Fill( na_e_int,  na_t_int );
        // m_nai_e_t_all ->Fill( na_e_int,  na_t_int );


        // #if defined(MAKE_TIME_EVOLUTION_PLOTS) && (MAKE_TIME_EVOLUTION_PLOTS>0)
        // float weight = 1;
        // const int timediff = Timediff(event);
        // m_nai_e_evol[id]->Fill( na_e_int, timediff, weight );
        // m_nai_t_evol[id]->Fill( na_t_int,   timediff ); // changed from na_t_c to na_t_int, as particles don't exsist
        // #endif /* MAKE_TIME_EVOLUTION_PLOTS */
    }
    
    return true;
}

// ########################################################################
// ########################################################################
// ########################################################################

int main(int argc, char* argv[])
{
    return OfflineSorting::Run(new UserXY(), argc, argv );
}
