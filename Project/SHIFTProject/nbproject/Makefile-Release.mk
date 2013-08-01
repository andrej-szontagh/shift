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
CND_CONF=Release
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
	${OBJECTDIR}/_ext/1728301206/loaderImage.o \
	${OBJECTDIR}/_ext/1728301206/loaderWorld.o \
	${OBJECTDIR}/_ext/1728301206/main.o \
	${OBJECTDIR}/_ext/1728301206/physics.o \
	${OBJECTDIR}/_ext/1728301206/renderer.o \
	${OBJECTDIR}/_ext/1728301206/rendererClip.o \
	${OBJECTDIR}/_ext/1728301206/rendererCore.o \
	${OBJECTDIR}/_ext/1728301206/rendererDraw.o \
	${OBJECTDIR}/_ext/1728301206/rendererGui.o \
	${OBJECTDIR}/_ext/1728301206/rendererLoader.o \
	${OBJECTDIR}/_ext/1728301206/rendererProcess.o \
	${OBJECTDIR}/_ext/1728301206/rendererRoutines.o \
	${OBJECTDIR}/_ext/1728301206/rendererSort.o \
	${OBJECTDIR}/_ext/1728301206/rendererSpecificFoliage.o \
	${OBJECTDIR}/_ext/1728301206/rendererSpecificGrass.o \
	${OBJECTDIR}/_ext/1728301206/rendererSpecificSolid.o \
	${OBJECTDIR}/_ext/1728301206/rendererSpecificTerrain.o \
	${OBJECTDIR}/_ext/1728301206/rendererSun.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=
CXXFLAGS=

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject.exe

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject.exe: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/shiftproject ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1093845578/shaderGenerator.o: ../Source/Shader/shaderGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1093845578
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1093845578/shaderGenerator.o ../Source/Shader/shaderGenerator.cpp

${OBJECTDIR}/_ext/1093845578/shaderLoader.o: ../Source/Shader/shaderLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1093845578
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1093845578/shaderLoader.o ../Source/Shader/shaderLoader.cpp

${OBJECTDIR}/_ext/1626136874/glee.o: ../Source/Tools/glee.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/glee.o ../Source/Tools/glee.cc

${OBJECTDIR}/_ext/1626136874/random.o: ../Source/Tools/random.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/random.o ../Source/Tools/random.cc

${OBJECTDIR}/_ext/1626136874/sort.o: ../Source/Tools/sort.cc 
	${MKDIR} -p ${OBJECTDIR}/_ext/1626136874
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1626136874/sort.o ../Source/Tools/sort.cc

${OBJECTDIR}/_ext/1728301206/debug.o: ../Source/debug.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/debug.o ../Source/debug.cpp

${OBJECTDIR}/_ext/1728301206/loader.o: ../Source/loader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/loader.o ../Source/loader.cpp

${OBJECTDIR}/_ext/1728301206/loaderImage.o: ../Source/loaderImage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/loaderImage.o ../Source/loaderImage.cpp

${OBJECTDIR}/_ext/1728301206/loaderWorld.o: ../Source/loaderWorld.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/loaderWorld.o ../Source/loaderWorld.cpp

${OBJECTDIR}/_ext/1728301206/main.o: ../Source/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/main.o ../Source/main.cpp

${OBJECTDIR}/_ext/1728301206/physics.o: ../Source/physics.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/physics.o ../Source/physics.cpp

${OBJECTDIR}/_ext/1728301206/renderer.o: ../Source/renderer.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/renderer.o ../Source/renderer.cpp

${OBJECTDIR}/_ext/1728301206/rendererClip.o: ../Source/rendererClip.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererClip.o ../Source/rendererClip.cpp

${OBJECTDIR}/_ext/1728301206/rendererCore.o: ../Source/rendererCore.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererCore.o ../Source/rendererCore.cpp

${OBJECTDIR}/_ext/1728301206/rendererDraw.o: ../Source/rendererDraw.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererDraw.o ../Source/rendererDraw.cpp

${OBJECTDIR}/_ext/1728301206/rendererGui.o: ../Source/rendererGui.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererGui.o ../Source/rendererGui.cpp

${OBJECTDIR}/_ext/1728301206/rendererLoader.o: ../Source/rendererLoader.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererLoader.o ../Source/rendererLoader.cpp

${OBJECTDIR}/_ext/1728301206/rendererProcess.o: ../Source/rendererProcess.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererProcess.o ../Source/rendererProcess.cpp

${OBJECTDIR}/_ext/1728301206/rendererRoutines.o: ../Source/rendererRoutines.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererRoutines.o ../Source/rendererRoutines.cpp

${OBJECTDIR}/_ext/1728301206/rendererSort.o: ../Source/rendererSort.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSort.o ../Source/rendererSort.cpp

${OBJECTDIR}/_ext/1728301206/rendererSpecificFoliage.o: ../Source/rendererSpecificFoliage.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSpecificFoliage.o ../Source/rendererSpecificFoliage.cpp

${OBJECTDIR}/_ext/1728301206/rendererSpecificGrass.o: ../Source/rendererSpecificGrass.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSpecificGrass.o ../Source/rendererSpecificGrass.cpp

${OBJECTDIR}/_ext/1728301206/rendererSpecificSolid.o: ../Source/rendererSpecificSolid.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSpecificSolid.o ../Source/rendererSpecificSolid.cpp

${OBJECTDIR}/_ext/1728301206/rendererSpecificTerrain.o: ../Source/rendererSpecificTerrain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSpecificTerrain.o ../Source/rendererSpecificTerrain.cpp

${OBJECTDIR}/_ext/1728301206/rendererSun.o: ../Source/rendererSun.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1728301206
	${RM} $@.d
	$(COMPILE.cc) -O2 -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/1728301206/rendererSun.o ../Source/rendererSun.cpp

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
