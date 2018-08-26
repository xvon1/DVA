library(ggplot2) 
library(reshape2)
library(sva)
library(grid)
library(fastICA)
library(qvalue)

source("dva.R")
source("MetricsOfCompar.R")

pkyr.idx <- which(!(is.na(PhenoTypesTCGA_GBM.lv$Race)));
data.m <- bmiqbeta.m[,pkyr.idx];
PhenoTypesList1<-list(Sex=PhenoTypesTCGA_GBM.lv$Sex[pkyr.idx],Race=PhenoTypesTCGA_GBM.lv$Race[pkyr.idx],age= PhenoTypesTCGA_GBM.lv$age[pkyr.idx],Cancer=PhenoTypesTCGA_GBM.lv$Cancer[pkyr.idx])
pkyr2.idx <- which(!(is.na(PhenoTypesList1$age)));
data.m <- data.m[,pkyr2.idx];
PhenoTypesList <-list(Sex=PhenoTypesList1$Sex[pkyr2.idx],Race=PhenoTypesList1$Race[pkyr2.idx],age= PhenoTypesList1$age[pkyr2.idx],Cancer=PhenoTypesList1$Cancer[pkyr2.idx])

POI<-PhenoTypesList$Cancer    #differ with real1.R Cancer
CF<-PhenoTypesList$Sex
dvobj<-dva(data.m,POI)
realDVA<-MetricsOfCompar(data.m,PhenoTypesList,dvobj$dv)

dva1 <- data.frame(dvobj$dv[,realDVA$sortcorreIX[1,1]],CF)  ###
summary(dva1)
colnames(dva1)<-c('dva','sex')
p<-ggplot(data=dva1, aes(x=factor(sex),y=dva))
p1<-p+ geom_boxplot(outlier.colour='orangered',outlier.shape=NA,alpha=1, aes(fill=factor(sex)))
dva1 <-p1+labs(x="Sex",y="DVA",title=paste("DVA1 ( R =",round(realDVA$sortcorre[1,1],2),", F =",round(realDVA$sortFstati[1,1],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13), axis.title= element_text(size=14))
#+scale_y_continuous(limits = c(-0.025,0.05))

dva2 <- data.frame(dvobj$dv[,realDVA$sortcorreIX[2,1]],CF)
summary(dva2)
colnames(dva2)<-c('dva','sex')
p<-ggplot(data=dva2, aes(x=factor(sex),y=dva,fill=sex))
p2<-p+ geom_boxplot(outlier.colour='orangered', outlier.shape=NA,alpha=1,aes(fill=factor(sex)))
dva2<-p2+labs(x="Sex",y="DVA",title=paste("DVA2 ( R =",round(realDVA$sortcorre[2,1],2),", F =",round(realDVA$sortFstati[2,1],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13), axis.title= element_text(size=14))

mod = model.matrix(~as.factor(Cancer), data=data.frame(PhenoTypesList))
mod0 = model.matrix(~1,data=data.frame(PhenoTypesList))
n.sv = num.sv(data.m,mod,method="leek")
svobj = sva(data.m,mod,mod0,n.sv=n.sv)
realSVA<-MetricsOfCompar(data.m,PhenoTypesList,svobj$sv)

sva1 <- data.frame(svobj$sv[,realSVA$sortcorreIX[1,1]],CF)
colnames(sva1)<-c('sva','sex')
p<-ggplot(data=sva1, aes(x=factor(sex),y=sva))
p1<-p+ geom_boxplot(outlier.colour='orangered', outlier.shape=NA,alpha=1,aes(fill=factor(sex)))
sva1 <-p1+labs(x="Sex",y="SVA",title=paste("SVA1 ( R =",round(realSVA$sortcorre[1,1],2),", F =",round(realSVA$sortFstati[1,1],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13), axis.title= element_text(size=14))
sva1

sva2 <- data.frame(svobj$sv[,realSVA$sortcorreIX[2,1]],CF)
colnames(sva2)<-c('sva','sex')
p<-ggplot(data=sva2, aes(x=factor(sex),y=sva))
p1<-p+ geom_boxplot(outlier.colour='orangered', outlier.shape=NA,alpha=1,aes(fill=factor(sex)))
sva2 <-p1+labs(x="Sex",y="SVA",title=paste("SVA2 ( R =",round(realSVA$sortcorre[2,1],2),", F =",round(realSVA$sortFstati[2,1],3),")"))+theme(plot.title=element_text(hjust=0.5))+theme(legend.position='none')+
  scale_x_discrete(breaks = c("40","50","60","70","80","90"))+theme(axis.text=element_text(size=13), axis.title= element_text(size=14))


grid.newpage()
pushViewport(viewport(layout = grid.layout(2,2)))
vplayout <-function(x,y)
  viewport(layout.pos.row = x,layout.pos.col = y)
print(dva1,vp=vplayout(1,1))
print(dva2,vp=vplayout(1,2))
print(sva1,vp=vplayout(2,1))
print(sva2,vp=vplayout(2,2))