dva<-function(data.m,pheno.v,imTh=0.90){
  lm.o <- lm(t(data.m) ~ pheno.v)
  res.m <- t(lm.o$res)
  model <- model.matrix(~1 + pheno.v)
  rmt.o <- prcomp(res.m)
  A<-summary(rmt.o)
  ncomp <- length(which(A$importance[3,]<imTh))
  fICA.o <- fastICA(res.m, n.comp = ncomp)
  tmp.m <- t(fICA.o$A)
  dv.m <- tmp.m
  sd <- 1/sqrt(ncol(data.m) - 3)
  for (k in 1:ncol(tmp.m)) {
    cor.v <- as.vector(cor(t(data.m), tmp.m[, k]))
    z.v <- 0.5 * log((1 + cor.v)/(1 - cor.v))
    pv.v <- 2 * pnorm(abs(z.v), 0, sd, lower.tail = FALSE)
    tmp.s <- sort(pv.v, decreasing = FALSE, index.return = TRUE)
    qv.o <- qvalue(pv.v)
    nsig <- length(which(qv.o$qvalues < 0.05))
    if(dim(data.m)[1] < 500){
      nsig<-dim(data.m)[1] 
    }else{ 
      if(nsig < 500){
        nsig<-500
      }
    }
    red.m <- data.m[tmp.s$ix[1:nsig], ]
    fICA.o <- fastICA(red.m, n.comp = ncomp)
    cor.v <- abs(cor(tmp.m[, k], t(fICA.o$A)))
    kmax <- which.max(cor.v)
    dv.m[, k] <- t(fICA.o$A)[, kmax]
    print(paste("Built decision variable ", k, sep = ""))
  }
  return(list(n.dv = ncomp,dv = dv.m))
}