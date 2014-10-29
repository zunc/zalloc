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
CND_PLATFORM=GNU-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Release
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/inc/buddy/buddy.o \
	${OBJECTDIR}/inc/buddy_alloc.o \
	${OBJECTDIR}/inc/nv_alloc.o \
	${OBJECTDIR}/inc/nvmalloc/nvmalloc.o \
	${OBJECTDIR}/inc/ram_alloc.o \
	${OBJECTDIR}/inc/slab/slab.o \
	${OBJECTDIR}/inc/slab_alloc.o \
	${OBJECTDIR}/src/handler.o \
	${OBJECTDIR}/src/main.o


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
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zalloc

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zalloc: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.c} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zalloc ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/inc/buddy/buddy.o: inc/buddy/buddy.c 
	${MKDIR} -p ${OBJECTDIR}/inc/buddy
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/buddy/buddy.o inc/buddy/buddy.c

${OBJECTDIR}/inc/buddy_alloc.o: inc/buddy_alloc.c 
	${MKDIR} -p ${OBJECTDIR}/inc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/buddy_alloc.o inc/buddy_alloc.c

${OBJECTDIR}/inc/nv_alloc.o: inc/nv_alloc.c 
	${MKDIR} -p ${OBJECTDIR}/inc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/nv_alloc.o inc/nv_alloc.c

${OBJECTDIR}/inc/nvmalloc/nvmalloc.o: inc/nvmalloc/nvmalloc.c 
	${MKDIR} -p ${OBJECTDIR}/inc/nvmalloc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/nvmalloc/nvmalloc.o inc/nvmalloc/nvmalloc.c

${OBJECTDIR}/inc/ram_alloc.o: inc/ram_alloc.c 
	${MKDIR} -p ${OBJECTDIR}/inc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/ram_alloc.o inc/ram_alloc.c

${OBJECTDIR}/inc/slab/slab.o: inc/slab/slab.c 
	${MKDIR} -p ${OBJECTDIR}/inc/slab
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/slab/slab.o inc/slab/slab.c

${OBJECTDIR}/inc/slab_alloc.o: inc/slab_alloc.c 
	${MKDIR} -p ${OBJECTDIR}/inc
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/inc/slab_alloc.o inc/slab_alloc.c

${OBJECTDIR}/src/handler.o: src/handler.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/handler.o src/handler.c

${OBJECTDIR}/src/main.o: src/main.c 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.c) -O2 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/main.o src/main.c

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/zalloc

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
