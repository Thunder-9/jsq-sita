args<-commandArgs(trailingOnly=TRUE)
filetest=args[1]
#filetest = "simulation_JSQ_n10_N10_lambda7.0_mu1.0.data"
data=read.table(filetest)
attach(data)

dates=V1
nclients=V2
cumul = 0.0
for(i in 1:(length(dates)-1)) {
	cumul=cumul+(dates[i+1]-dates[i])*nclients[i]
}

cumul=cumul/max(dates)

print(cumul);