
#install.packages("dplyr")
library(dplyr)
args<-commandArgs(trailingOnly=TRUE)
N=as.numeric(args[1])

data="pareto-r.data"
R=unlist(read.table(file=data,row.names=c(),sep="\n"),use.names=FALSE)

#print(max(R))

R<-sort(R,decreasing=FALSE)
m=sum(R)
m=m[length(m)]
m=m/N
print(m)
#print(R)






#m=sum(R)/N
inter<-0
i_inter=1
i_prec=1
sum=R[1]
#print(m)



#version cumule




for(i in 1:(length(R)-1)){
	if(((sum+R[i+1])>m)){
		inter[i_inter]=R[i]
		sum=0.0
		i_inter=i_inter+1
		sum=0.0
	
	}else{
		sum=sum+R[i]
	}


}
inter[N-1]=R[length(R)]



	print(inter)



write.table(inter,file="inter.data",col.names=FALSE,row.names=FALSE,append=FALSE,sep=" ")








