The sorting routine

This repository contains a default sorting routine that can be used with data collected at the OCL. Please feel free to adopt it to your purposes and notify us/load up branches with your suggestions.

**Files**:
* `user_sort.cpp` - the sorting routine
In case you don't have fission detectors, you might want to create less spectra. Set the "#define USE_FISSION_PARAMETERS 1" accordingly.

* `/lib` - libraries

* `<Yourfile>.batch` - the file in which all parameters are listed. Before you start using it for your experiment, go through all parameters and set them according to your experiment!
At some point of time you will also want to change:
 * data files
 * the gainshifts file
 * output file name (with the root matrices)
 * the export of "alfna" matrix to mama matrices
I tried to create a rather plain file called `Pu239_dp_rather_default.batch` which you can use as a start. Please remember to fill in the kinematics from rkinz/qkinz of your reaction (see the file).


* `gainshifts_plain.dat` - gains and shifts for the detectors. Change the values to align and calibrate the detectors. It can be a little cumbersome, as the read-out is very sensitive to additional lines/spaces and so on. Always cross-check the parameters that are used by sorting.

* `Makefile` - the makefile. Assumes that the sorting routine is called `user_sort.cpp`

* `.gitignore` - ignore most files that are not listed here (e.g. the output files)


**Basic usage**:

* First, fork your own copy of this repository! Then you can use git as a version controll of changes/adoptions for your experiment
(you might *watch* the original repository to get updates on changes - or even keep the fork synced:
https://help.github.com/articles/fork-a-repo/ )

* Clone your repository
`git clone https://github.com/YOUR-USERNAME/usersort`

* Usage:

```
make very-clean
make
./sorting <Yourfile>.batch
```
**Example Output**:
here is there you can check that the parameters are read correctly. Note that here I don't use the plain gainshifts file, but by own data.

```
fabiobz@fabiobz-Latitude-E5540 ~/Desktop/Masterthesis/239Pu $ ./sorting Pu239_dp_032_unif.batch 
Parameter 'ede_rect': 500 250 30 500 (4 values)
Parameter 'thick_range': 130 13 0 (3 values)
Parameter 'gain_e': 4.30756 4.29565 4.27608 4.25879 ... (64 values)
Parameter 'gain_de': 1.46561 1.40355 1.45859 1.38638 ... (64 values)
Parameter 'gain_ge': 1 1 1 1 ... (6 values)
Parameter 'gain_na': 3.94115 3.97515 3.73442 3.58838 ... (32 values)
Parameter 'shift_e': 558.78 583.89 629 668.87 ... (64 values)
Parameter 'shift_de': 4.91 56.33 49.51 56.33 ... (64 values)
Parameter 'shift_ge': 0 0 0 0 ... (6 values)
Parameter 'shift_na': -84.94 -98.16 -59.16 -45.87 ... (32 values)
Parameter 'shift_tge': 0 0 0 0 ... (6 values)
Parameter 'shift_tna': 27.5 30.5 11.5 -24.5 ... (32 values)
Parameter 'gain_tge': 1 1 1 1 ... (6 values)
Parameter 'gain_tna': 1 1 1 1 ... (32 values)
Parameter 'tnai_corr_enai': -5.52368 18201.9 210.387 -0.00125989 (4 values)
Parameter 'tnai_corr_esi': 50.4471 -475514 4506.25 -0.00185164 (4 values)
Parameter 'tppac_corr_esi': 50.4471 -475514 4506.25 -0.00185164 (4 values)
ParticleRange: 7923 interpolation points
Parameter 'ede_rect': 500 250 30 500 (4 values)
Parameter 'thick_range': 140 20 0 (3 values)
Parameter 'ex_from_ede': 15528 -0.96287 -1.3943e-06 15521 ... (24 values)
Parameter 'ex_corr_exp': 0 1 0 1 ... (16 values)
Parameter 'nai_time_cuts': 197 207 281 291 (4 values)
Parameter 'channel_PPAC': 4 12 30 31 (4 values)
Parameter 'ppac_time_cuts': 190 200 310 320 (4 values)
Parameter 'fission_excitation_energy_min': 5000 (1 values)
Parameter 'ppac_efficiency': 0.48 (1 values)
Set data directory to '../239Pu/datafilesLocal'
data: reading file '../239Pu/datafilesLocal/sirius-20140617-142011-big-000.data' buffers [0,end[.
964/0 14.4025 295.51 bufs/s ^C      
data: reading file '../239Pu/datafilesLocal/sirius-20140622-080050.data' buffers [0,end[.
8192/0 13.8173 584.289 bufs/s         
export as ROOT file into 'offline_Pu239_dp_029_unif_all.root'
resetting all histograms
```
