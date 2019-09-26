library(fastICA)
library(qvalue)

source("dva.R")


pkyr.idx <- which(!(is.na(PhenoTypesKIRC.lv$race)));
exp.m <- exp.m[,pkyr.idx];

PhenoTypesList<-list(Cancer=PhenoTypesKIRC.lv$cancer[pkyr.idx],gender=PhenoTypesKIRC.lv$gender[pkyr.idx],age= PhenoTypesKIRC.lv$age[pkyr.idx], race=PhenoTypesKIRC.lv$race[pkyr.idx])
rowmexp = rowMeans(log(exp.m+1)) 
keepIndex=which(rowmexp > 1)
dataOri.m = exp.m[keepIndex,]
data.m<-log(dataOri.m +1)
#data.m = exp.m[keepIndex,] 

POI<-PhenoTypesList$Cancer
CF<-PhenoTypesList$age
dvobj<-dva(data.m,POI)