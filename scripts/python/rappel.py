import pyatan

h0=pyatan.load("rappel.kif","",1)

v=rappel("Test",10)

for i in v:
	print i

h1=pyatan.load("appel.kif","",1)

print appel("TRUE","FALSE")

