cmake_minimum_required(VERSION 2.8.9)

project(PorousScaffolds)

#-----------------------------------------------------------------------------
# Extension meta-information
set(EXTENSION_HOMEPAGE "http://slicer.org/slicerWiki/index.php/Documentation/Nightly/Extensions/PorousScaffolds")
set(EXTENSION_CATEGORY "Mesh Generation")
set(EXTENSION_CONTRIBUTORS "Fenghe TANG, Dr. Xiaojun CHEN (SJTU)")
set(EXTENSION_DESCRIPTION "This is an extension module to generate porous scaffolds based on a model.")
set(EXTENSION_ICONURL "http://wiki.slicer.org/slicerWiki/index.php/File:PorousScaffolds.png")
set(EXTENSION_SCREENSHOTURLS "http://wiki.slicer.org/slicerWiki/index.php/File:PorousScaffolds_ScreenShot.png")

#-----------------------------------------------------------------------------
# Extension dependencies
find_package(Slicer REQUIRED)
include(${Slicer_USE_FILE})

#-----------------------------------------------------------------------------
# Extension modules
add_subdirectory(PorousScaffolds)
## NEXT_MODULE

#-----------------------------------------------------------------------------
include(${Slicer_EXTENSION_CPACK})
