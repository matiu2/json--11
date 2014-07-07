# Pulls in and compiles the bandit source from it's git repository
include(ExternalProject)

ExternalProject_Add(
    bandit
    PREFIX bandit
    GIT_REPOSITORY https://github.com/joakimkarlsson/bandit.git
    UPDATE_COMMAND echo # No need to update all the time once you have it
    CONFIGURE_COMMAND echo # We just need the source
    BUILD_COMMAND echo # We just need the source
    INSTALL_COMMAND echo # We just need the source
)

set(BANDIT_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bandit/src/bandit")
