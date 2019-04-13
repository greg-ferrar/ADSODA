# ADSODA

CC	 = gcc
C++FLAGS = -DCAVE -g
CFLAGS = -DCAVE -g
LDFLAGS = -w -lcave_ogl -lglut -lsocket -lGL -lGLU -lXmu -lXext -lXi -lX11 -lm
INCLUDE = -I/afs/ncsa.uiuc.edu/projects/MATH428/illiMath/glut -I/afs/ncsa/packages/cave/IRIX_5.2/include
LIBS     = -L/afs/ncsa.uiuc.edu/projects/MATH428/illiMath/glut -L/afs/ncsa/packages/cave/IRIX_5.2/lib

OBJS =	amatrix.o vector.o \
	solve.o solid.o face.o halfspace.o \
	light.o draw.o space.o demo.o util.o initdemo.o options.o

all: ADSODA.x

cattery: ADSODA.so ADSODA2.so ADSODA3.so ADSODA4.so ADSODA5.so

noosh: ADSODA.x

ADSODA.so: noosh.o $(OBJS)
	ld -o $@ -shared -exported_symbol ADSODA_dsp,ADSODA_ini,ADSODA_ppr,ADSODA_ffn noosh.o $(OBJS)

ADSODA2.so: noosh2.o $(OBJS)
	ld -o $@ -shared -exported_symbol ADSODA2_dsp,ADSODA2_ini,ADSODA2_ppr,ADSODA2_ffn noosh2.o $(OBJS)

ADSODA3.so: noosh3.o $(OBJS)
	ld -o $@ -shared -exported_symbol ADSODA3_dsp,ADSODA3_ini,ADSODA3_ppr,ADSODA3_ffn noosh3.o $(OBJS)

ADSODA4.so: noosh4.o $(OBJS)
	ld -o $@ -shared -exported_symbol ADSODA4_dsp,ADSODA4_ini,ADSODA4_ppr,ADSODA4_ffn noosh4.o $(OBJS)

ADSODA5.so: noosh5.o $(OBJS)
	ld -o $@ -shared -exported_symbol ADSODA5_dsp,ADSODA5_ini,ADSODA5_ppr,ADSODA5_ffn noosh5.o $(OBJS)

ADSODA.x: $(OBJS) noosh.o
	$(CC) -o $@ $(OBJS) noosh.o $(LIBS) $(LDFLAGS) 

clean:
	rm $(OBJS) ADSODA.x ADSODA.so

noosh.o: noosh.cpp
	$(CC) -c $(C++FLAGS) -o $@ noosh.cpp $(INCLUDE)

noosh2.o : noosh.cpp
	$(CC) -c -DADSODA_dsp=ADSODA2_dsp -DADSODA_ini=ADSODA2_ini -DADSODA_ppr=ADSODA2_ppr -DADSODA_ffn=ADSODA2_ffn $(C++FLAGS) -o $@ noosh.cpp $(INCLUDE)

noosh3.o : noosh.cpp
	$(CC) -c -DADSODA_dsp=ADSODA3_dsp -DADSODA_ini=ADSODA3_ini -DADSODA_ppr=ADSODA3_ppr -DADSODA_ffn=ADSODA3_ffn $(C++FLAGS) -o $@ noosh.cpp $(INCLUDE)

noosh4.o : noosh.cpp
	$(CC) -c -DADSODA_dsp=ADSODA4_dsp -DADSODA_ini=ADSODA4_ini -DADSODA_ppr=ADSODA4_ppr -DADSODA_ffn=ADSODA4_ffn $(C++FLAGS) -o $@ noosh.cpp $(INCLUDE)

noosh5.o : noosh.cpp
	$(CC) -c -DADSODA_dsp=ADSODA5_dsp -DADSODA_ini=ADSODA5_ini -DADSODA_ppr=ADSODA5_ppr -DADSODA_ffn=ADSODA5_ffn $(C++FLAGS) -o $@ noosh.cpp $(INCLUDE)

cattery.o: cattery.cpp
	$(CC) -c $(C++FLAGS) -o $@ cattery.cpp $(INCLUDE)

amatrix.o: amatrix.cpp
	$(CC) -c $(C++FLAGS) -o $@ amatrix.cpp $(INCLUDE)

options.o: options.cpp
	$(CC) -c $(C++FLAGS) -o $@ options.cpp $(INCLUDE)

vector.o: vector.cpp
	$(CC) -c $(C++FLAGS) -o $@ vector.cpp $(INCLUDE)

main.o: main.cpp
	$(CC) -c $(C++FLAGS) -o $@ main.cpp $(INCLUDE)

solve.o: solve.cpp
	$(CC) -c $(C++FLAGS) -o $@ solve.cpp $(INCLUDE)

solid.o: solid.cpp
	$(CC) -c $(C++FLAGS) -o $@ solid.cpp $(INCLUDE)

face.o: face.cpp
	$(CC) -c $(C++FLAGS) -o $@ face.cpp $(INCLUDE)

halfspace.o: halfspace.cpp
	$(CC) -c $(C++FLAGS) -o $@ halfspace.cpp $(INCLUDE)

light.o: light.cpp
	$(CC) -c $(C++FLAGS) -o $@ light.cpp $(INCLUDE)

draw.o: draw.cpp
	$(CC) -c $(C++FLAGS) -o $@ draw.cpp $(INCLUDE)
 
space.o: space.cpp
	$(CC) -c $(C++FLAGS) -o $@ space.cpp $(INCLUDE)

demo.o: demo.cpp
	$(CC) -c $(C++FLAGS) -o $@ demo.cpp $(INCLUDE)

util.o: util.cpp
	$(CC) -c $(C++FLAGS) -o $@ util.cpp $(INCLUDE)

initdemo.o: initdemo.cpp
	$(CC) -c $(C++FLAGS) -o $@ initdemo.cpp $(INCLUDE)


