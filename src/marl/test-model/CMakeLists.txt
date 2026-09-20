add_library(marl_test_lib test.cpp)
target_link_libraries(marl_test_lib PUBLIC agent_lib environment_lib)

# Standalone evaluation executable
add_executable(eval_model run_eval.cpp)
target_link_libraries(eval_model PRIVATE marl_test_lib pthread)