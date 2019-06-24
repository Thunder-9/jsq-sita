t=c(0.25,0.5 ,0.6,0.4   , 3   )
p=c(0.25,0.25,0.25,0.15,0.1)

print(sum(p))

unif=runif(20,min=0,max=1)
i=1
print(unif)
gen<-0
s=0.0
while(i<=length(unif)){
	s=0.0
	for(u in 1:(length(p))){
		s=s+p[u]
		if(s>unif[i]){
			gen[i]=t[u]
			break
		}
	}
	i=i+1

}

print(gen)

write.table(gen,file="dist.data",col.names=FALSE,row.names=FALSE,append=FALSE,sep=" ")