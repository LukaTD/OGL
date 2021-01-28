lines = open("house.obj")

verts = []
texCoords = [] 
norms = []
for line in lines:
	tab = line.split(" ")

	if len(tab) == 0: 
		continue

	if tab[0] == "v":
		verts += [(float(tab[1]),float(tab[2]),float(tab[3]))]
	elif tab[0] == "vt":
		texCoords += [(float(tab[1]),float(tab[2]))]
	elif tab[0] == "vn":
		norms += [(float(tab[1]),float(tab[2]),float(tab[3]))]