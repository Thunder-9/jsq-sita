
p_k<-function(dist,deb,fin,lambda){
	if(deb==fin){
		return(0.0)
	}
	card=length(dist[deb:fin])
	m=mean(dist[deb:fin])
	sig=var(dist[deb:fin])
	lamb=lambda*(card/length(dist))
	p=lamb*m
	if(p>1){
		return(1000000)
	}
	res=p+((1+(sig/m^2))/2)*(p^2/(1-p))
	return(res)
}


inter<-function(dist,deb,fin,lambda,ninter,it,interc){
	i=floor((fin-deb)*interc)
	if(it>log2(length(dist[deb:fin]))){
		return (dist[i])
	}
	inter1=p_k(dist,deb,i,lambda)
	inter2=p_k(dist,i+1,fin,lambda)
	if(inter1==(inter2*(ninter-1))){
		return(dist[i])
	}
	if(inter1<inter2){
		return(inter(dist,deb,fin,lambda,ninter,it+1,interc+(ninter^(it+1))))
	}
	if(inter1>inter2){
		return(inter(dist,deb,fin,lambda,ninter,it+1,interc-(ninter^(it+1))))
	}
}


f<-function(dist,x){
	return(Position(function(x){x==intervalle[u-1]},dist,right=TRUE,nomatch=0))
}

args<-commandArgs(trailingOnly=TRUE)
N=as.numeric(args[1])
lambda=as.numeric(args[2])
data="dist.data"
R=unlist(read.table(file=data,row.names=c(),sep="\n"),use.names=FALSE)
R<-sort(R,decreasing=FALSE)
print(R)
intervalle<-0
print(1/N)
intervalle[1]=inter(R,1,length(R),lambda,1/N,1,1/N)
if(N>2){
for(u in 2:(N-1)){
	print(1/(N-u+1))
 	pos=f(R,intervalle[u-1])+1
 	print(pos)
 	print(intervalle	)
 	intervalle[u]=inter(R[pos:length(R)],1,length(R[pos:length(R)]),lambda,1/(N-u+1),1,1/(N-u+1))
 }
}
print(intervalle)
