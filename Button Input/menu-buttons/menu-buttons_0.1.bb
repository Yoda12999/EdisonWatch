DESCRIPTION = "Menu button driver support for EdisonWatch"
SECTION = "base"
LICENSE = "GPL-2.0"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/GPL-2.0;md5=801f80980d171dd6425610833a22dbe6"

inherit module

PR = "r0"
PV = "0.1"

SRC_URI = "file://menu_buttons.c"
SRC_URI += "file://Makefile"

S = "${WORKDIR}"
