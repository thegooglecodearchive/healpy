PKG:=cxxsupport

SD:=$(SRCROOT)/$(PKG)
OD:=$(BLDROOT)/$(PKG)

FULL_INCLUDE+= -I$(SD)

HDR_$(PKG):=$(SD)/*.h
LIB_$(PKG):=$(LIBDIR)/libcxxsupport.a
OBJ:=cxxutils.o fitshandle.o rotmatrix.o trafos.o ls_image.o error_handling.o wigner.o
OBJ:=$(OBJ:%=$(OD)/%)

ODEP:=$(HDR_$(PKG)) $(HDR_libfftpack) $(HDR_c_utils)

$(OBJ): $(ODEP) | $(OD)_mkdir
$(LIB_$(PKG)): $(OBJ)

$(OD)/fitshandle.o: $(HDR_libcfitsio)
$(OD)/ls_image.o: $(SD)/font_data.inc

all_hdr+=$(HDR_$(PKG))
all_lib+=$(LIB_$(PKG))
