library(ggplot2) 
library(reshape2)
library(sva)
library(grid)
library(fastICA)
library(qvalue)


source("dva.R")
source("MetricsOfCompar.R")

pkyr.idx <- which(!(is.na(PhenoTypesLSCC.lv$race)));
exp.m <- exp.m[,pkyr.idx];

PhenoTypesList<-list(Cancer=PhenoTypesLSCC.lv$cancer[pkyr.idx],gender=PhenoTypesLSCC.lv$gender[pkyr.idx],age= PhenoTypesLSCC.lv$age[pkyr.idx], race=PhenoTypesLSCC.lv$race[pkyr.idx])
rowmexp = rowMeans(log(exp.m+1)) 
keepIndex=which(rowmexp > 1)
dataOri.m = exp.m[keepIndex,]
data.m<-log(dataOri.m +1)
#data.m = exp.m[keepIndex,] 

POI<-PhenoTypesList$Cancer
CF<-PhenoTypesList$age
dvobj<-dva(data.m,POI)
realDVA<-MetricsOfCompar(data.m,PhenoTypesList,dvobj$dv)

dva1 <- data.frame(dvobj$dv[,realDVA$sortcorreIX[1,3]],CF)  ###
summary(dva1)
colnames(dva1)<-c('dva','age')
p<-ggplot(data=dva1, aes(x=factor(age),y=dva))
p1<-p+ geom_boxplot(aes(colour=age), outlier.colour='orangered',outlier.shape=NA,alpha=1, fill="#FF9999")
dva1 <-p1+labs(x="Age",y="DVA",title=paste("DVA1 ( R =",round(realDVA$sortcorre[1,3],3),", F =",round(realDVA$sortFstati[1,3],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13))
#+scale_y_continuous(limits = c(-0.025,0.05))

dva2 <- data.frame(dvobj$dv[,realDVA$sortcorreIX[2,3]],CF)
summary(dva2)
colnames(dva2)<-c('dva','age')
p<-ggplot(data=dva2, aes(x=factor(age),y=dva,fill=age))
p2<-p+ geom_boxplot(aes(colour=age),outlier.colour='orangered', outlier.shape=NA,alpha=1,fill="#000099")
dva2<-p2+labs(x="Age",y="DVA",title=paste("DVA2 ( R =",round(realDVA$sortcorre[2,3],3),", F =",round(realDVA$sortFstati[2,3],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13))

mod = model.matrix(~as.factor(Cancer), data=data.frame(PhenoTypesList))
mod0 = model.matrix(~1,data=data.frame(PhenoTypesList))
#Scheme 2
#n.sv = num.sv(data.m,mod,method="be")
#svobj = sva(data.m,mod,mod0,n.sv=n.sv)
#Scheme 2
svobj<-svaseq(dataOri.m,mod)

realSVA<-MetricsOfCompar(data.m,PhenoTypesList,svobj$sv)

sva1 <- data.frame(svobj$sv[,realSVA$sortcorreIX[1,3]],CF)
colnames(sva1)<-c('sva','age')
p<-ggplot(data=sva1, aes(x=factor(age),y=sva))
p1<-p+ geom_boxplot(aes(colour=age),outlier.colour='orangered', outlier.shape=NA,alpha=1,fill="#999999")
sva1 <-p1+labs(x="Age",y="SVA",title=paste("SVA1 ( R =",round(realSVA$sortcorre[1,3],3),", F =",round(realSVA$sortFstati[1,3],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13))
sva1

sva2 <- data.frame(svobj$sv[,realSVA$sortcorreIX[2,3]],CF)
colnames(sva2)<-c('sva','age')
p<-ggplot(data=sva2, aes(x=factor(age),y=sva))
p1<-p+ geom_boxplot(aes(colour=age),outlier.colour='orangered', outlier.shape=NA,alpha=1,fill="#56B4E9")
sva2 <-p1+labs(x="Age",y="SVA",title=paste("SVA2 ( R =",round(realSVA$sortcorre[2,3],3),", F =",round(realSVA$sortFstati[2,3],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13))


grid.newpage()
pushViewport(viewport(layout = grid.layout(2,2)))
vplayout <-function(x,y)
  viewport(layout.pos.row = x,layout.pos.col = y)
print(dva1,vp=vplayout(1,1))
print(dva2,vp=vplayout(1,2))
print(sva1,vp=vplayout(2,1))
print(sva2,vp=vplayout(2,2))