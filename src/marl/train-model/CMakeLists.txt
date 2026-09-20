add_library(marl_train_lib train.cpp)
target_link_libraries(marl_train_lib PUBLIC agent_lib environment_lib)

# Standalone headless training executable
add_executable(train_model run_train.cpp)
target_link_libraries(train_model PRIVATE marl_train_lib pthread)