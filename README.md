## DVA function
Decision Variable Analysis
Package:dva
Title: Decision variables to be discovered in modelling high-dimensional omics data for cancer studies
Version: 0.0.0.1000
Authors: Feng Xie, Cheng Li, Jie Xie, et al.
Depends: R (>= 3.4.4)
License: GNU General Public License(GPL)
Encoding: UTF-8
LazyLoad:yes
Description: High-dimensional omics data are often contaminated by sources of unwanted variations caused by platforms, batches, or other external factors. These interferences and noise can obscure critical signals related to cancer. Contaminated data are modeled as a combination of variables derived from the phenotype of interest (POI) and confounding factors. To identify these variables, a novel method called Decision Variable Analysis (DVA) is proposed. The novelty of DVA is to iteratively extract independent decisive variables for modeling the data. Specifically, a priori knowledge introduced as the definite variable linked with POI is removed from data through a residual operation. The number of variables is estimated from the residual matrix based on the zero gradient of singular values, rather than relying on random matrix theory or principal components analysis, which can produce unreliable results when the number of features exceeds the number of samples. Applications of DVA to both synthetic and real data demonstrate superior performance in identifying variables compared to conventional approaches. Improvements offered by DVA are illustrated across high-dimensional omics datasets, particularly those with smaller sample sizes relative to the number of features on different platforms. The results indicate that DVA is an effective method for dissecting sources of variation in high-dimensional data with disturbances.
Usage: dva(data.m, pheno.v, imTh=0.50)
Arguments:
   data.m denotes data matrix in which rows label features and columns label samples. It is assumed that number of features is much larger than number of samples.
   pheno.v denotes numeric vector of POI(e.g., cancer status) that length equal to number of columns of data matrix. 
   imTh denotes importance threshold of information content in data. The range of values for 'imTh' is from 0 to 1. imTh = 0 means no information in data and imTh = 1 means no noise, no measurement error or uncertainties in data.
Return value:
   n.dv means the number of selected DVs.
   dv denotes the matrix of selected DVs.
   
Examples:
## load in real data in the data subfolder 
load();
POI<-PhenoTypesList$Cancer
dvobj<-dva(data.m,POI) 
   
## scripts/code
Scripts and test code are available in the R subfolder.

## data
The real example data is available in the data subfolder.
