#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_DLIB_EXT=dll
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/1093845578/shaderGenerator.o \
	${OBJECTDIR}/_ext/1093845578/shaderLoader.o \
	${OBJECTDIR}/_ext/1626136874/glee.o \
	${OBJECTDIR}/_ext/1626136874/random.o \
	${OBJECTDIR}/_ext/1626136874/sort.o \
	${OBJECTDIR}/_ext/1728301206/debug.o \
	${OBJECTDIR}/_ext/1728301206/loader.o \
	${OBJECTDIR}/_ext/1728301206/main.o \
	${OBJECTDIR}/_ext/1728301206/physics.o \
	${OBJECTDIR}/_ext/1728301206/renderer.o


# C Compiler Flags
CFLAGS=-m64 -Wno-write-strings

# CC Compiler Flags
CCFLAGS=-m64 -Wno-write-strings
CXXFLAGS=-m64 -Wno-write-strings

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../Externals/SDL2-2.0.0/lib -lSDL2 -lSDL2main -lopengl32 -lglu32

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1093845578/shaderGenerator.o: ../Source/Shader/shaderGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1093845578
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1093845578/shaderGenerator.o ../Source/Shader/shaderGenerator.cpp

${OBJECTDIR}/_ext/1093845578/shaderLoader.o: ../Source/Shader/shaderLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1093845578
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1093845578/shaderLoader.o ../Source/Shader/shaderLoader.cpp

${OBJECTDIR}/_ext/1626136874/glee.o: ../Source/Tools/glee.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/glee.o ../Source/Tools/glee.cc

${OBJECTDIR}/_ext/1626136874/random.o: ../Source/Tools/random.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/random.o ../Source/Tools/random.cc

${OBJECTDIR}/_ext/1626136874/sort.o: ../Source/Tools/sort.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/sort.o ../Source/Tools/sort.cc

${OBJECTDIR}/_ext/1728301206/debug.o: ../Source/debug.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/debug.o ../Source/debug.cpp

${OBJECTDIR}/_ext/1728301206/loader.o: ../Source/loader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/loader.o ../Source/loader.cpp

${OBJECTDIR}/_ext/1728301206/main.o: ../Source/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/main.o ../Source/main.cpp

${OBJECTDIR}/_ext/1728301206/physics.o: ../Source/physics.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/physics.o ../Source/physics.cpp

${OBJECTDIR}/_ext/1728301206/renderer.o: ../Source/renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -g -DM_DEBUG -DWIN32 -D_NDEBUG -D_WINDOWS -I../Source -I../Source/Tools -I../Source/Shader -I../Externals/SDL2-2.0.0/include/SDL2 -std=c++11 -Wno-write-strings -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/renderer.o ../Source/renderer.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
