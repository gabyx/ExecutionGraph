# Find various components of the Angular Framework 
# The following variables are set if Meta is found.
#  Angular_CLI_EXECUTABLE        - The path to the Angular CLI executable, otherwise Angular_CLI_EXECUTABLE-NOTFOUND

include(FindPackageHandleStandardArgs)

find_program(ANGULAR_CLI_EXECUTABLE "ng")

find_package_handle_standard_args(Angular DEFAULT_MSG ANGULAR_CLI_EXECUTABLE)
mark_as_advanced(ANGULAR_CLI_EXECUTABLE)