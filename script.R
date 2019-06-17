args<-commandArgs(trailingOnly=TRUE)
filetest=args[1]
#filetest = "simulation_JSQ_n10_N10_lambda7.0_mu1.0.data"
data=read.table(filetest)
attach(data)

deb=V1
f=V2
file=V3

time=V2-V1

print(mean(time))