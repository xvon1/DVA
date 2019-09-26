## DVA function
Decision Variable Analysis
Package:dva
Title: Discovering potential key features of genome wide profiling data using Decision Variable Analysis
Version: 0.0.0.1000
Authors: Feng Xie, Cheng Li, Xuan Li, Andrew E. Teschendorff, Yungang He*, Zhen Yang*
Depends: R (>= 3.4.4)
License: GNU General Public License(GPL)
Encoding: UTF-8
LazyLoad:yes
Description: Omics-data are commonly contaminated by sources of unwanted variation results from platforms, batches or other types of biological factors. The data can be considered as a combination of variation derived from phenotype of interest (POI) and other confounding factors. We propose a novel feature selection method called decision variable analysis (DVA) to deconvolve these factors. The decision variables (DVs) inferred by our algorithm have high correlations with the confounding factors(CFs), which can be used as covariates in modeling or regression analysis.
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